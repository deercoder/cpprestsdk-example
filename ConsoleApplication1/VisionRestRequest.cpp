#include "stdafx.h"
#include "VisionRestRequest.h"
#include<cpprest\filestream.h>
#include<iostream>
#include<string>


/**
https://westus.api.cognitive.microsoft.com/vision/v1.0/analyze[?visualFeatures][&details][&language]

visualFeatures can be: Categories, Tags, Faces, ImageType, Color, Adult
details can be: Celebrities
language can be: en, zh

Only one value can be set at a time.

Reference:
1) Console debug tools: https://westus.dev.cognitive.microsoft.com/docs/services/56f91f2d778daf23d8ec6739/operations/56f91f2e778daf14a499e1fa/console
2) API interface: https://westus.dev.cognitive.microsoft.com/docs/services/56f91f2d778daf23d8ec6739/operations/56f91f2e778daf14a499e1fa
*/
std::string vision_base_host = "https://westus.api.cognitive.microsoft.com/vision/v1.0/analyze?details=Celebrities&language=en&visualFeatures=";
const std::string const_vision_host = "https://westus.api.cognitive.microsoft.com/vision/v1.0/analyze?details=Celebrities&language=en&visualFeatures=";
std::string feature_options[7] = { "Categories", "ImageType", "Faces", "Adult", "Color", "Tags", "Description" };

VisionRestRequest::VisionRestRequest(int flag)
{
	vision_base_host = const_vision_host;
	this->setRequestType(flag);
	this->client = new http_client(conversions::to_string_t(vision_base_host));
	this->request = new http_request(methods::POST);
}

VisionRestRequest::VisionRestRequest(int flag, bool send_raw_file)
{
	vision_base_host = const_vision_host;
	this->setRequestType(flag);
	this->client = new http_client(conversions::to_string_t(vision_base_host));
	this->request = new http_request(methods::POST);
	this->send_raw_image = send_raw_file;
}

void VisionRestRequest::setRequestType(int flag)
{
	if (flag >= 0 && flag < 7)
	{
		vision_base_host = vision_base_host + feature_options[flag];
	}
	else
	{
		std::cout << "Wrong flag, exceeding the range!" << std::endl;
	}
	std::cout << "[Debug]: " << vision_base_host << std::endl;
}


void VisionRestRequest::ConstructRequestHeader(const wchar_t * subscription_key)
{
	if (this->send_raw_image) {
		this->request->headers().add(L"Content-Type", L"application/octet-stream");
	}
	else {
		this->request->headers().add(L"Content-Type", L"application/json");
	}
	this->request->headers().add(L"ocp-apim-subscription-key", subscription_key);
}

void VisionRestRequest::ConstructRequestBody(const wchar_t * image_path)
{
	if (this->send_raw_image) {
		concurrency::streams::istream input_stream = concurrency::streams::file_stream<uint8_t>::open_istream(conversions::to_string_t(image_path)).get();
		/*
		Must specify the length of the stream, otherwise it shows ERROR 400: image size too small or too large
		even though their API supports non-length parameters, check here:
		https://social.msdn.microsoft.com/Forums/en-US/6fb47e0d-fc9e-44f0-af3d-66887e10a72c/face-api-error-invalidimagesize-image-size-is-too-small-or-too-big-for-each-request?forum=mlapi
		*/
		this->request->set_body(input_stream, input_stream.streambuf().size());
	}
	else {
		/* set remote url path, NOTE about parameters for json::values::string */
		json::value jvalue;
		jvalue[L"url"] = json::value::string(conversions::to_string_t(image_path));
		this->request->set_body(jvalue);
	}
}

http_client* VisionRestRequest::getClient()
{
	return this->client;
}

http_request* VisionRestRequest::getRequest()
{
	return this->request;
}

int VisionRestRequest::SendAnalysisRequest(const wchar_t * key, const wchar_t *image)
{
	auto fileStream = std::make_shared<ostream>();

	// Open stream to output file.
	pplx::task<void> requestTask = fstream::open_ostream(U("results2.html")).then([=](ostream outFile)
	{
		*fileStream = outFile;
		this->ConstructRequestHeader(key);
		this->ConstructRequestBody(image);
		return this->getClient()->request(*this->getRequest());
	})
		// Handle response headers arriving.
		.then([=](http_response response)
	{
		printf("Received response status code:%u\n", response.status_code());
		utility::string_t tt = response.extract_string().get();
		std::string str(conversions::to_utf8string(tt));
		std::cout << "JSON string is: " << str << std::endl;

		json::value obj = json::value::parse(conversions::to_string_t(str));

		/* categories contains multiple elements, if there are, otherwise there is only one or empty,
		   so we should consider for all cases, not just one or two, use general iterations to represent
		 */
		if (obj.has_field(L"categories"))
		{
			json::array cats = obj.at(L"categories").as_array();
			std::string concat_cat_str = "";
			std::string concat_cat_score = "";
			for (size_t i = 0; i < cats.size(); i++)
			{
				json::value jv = cats[i];
				concat_cat_str += conversions::to_utf8string(jv.at(L"name").as_string());
				concat_cat_score += std::to_string(jv.at(L"score").as_number().to_double());
				if (i != cats.size() - 1)
				{
					concat_cat_str += ",";
					concat_cat_score += ",";
				}
			}
			this->setCategoryNames(concat_cat_str);
			this->setCategoryScores(concat_cat_score);
		}

		/* if request type is the `tags`, then parse the tags information and its score */
		if (obj.has_field(L"tags"))
		{
			json::array tags = obj.at(L"tags").as_array();
			std::string concat_tags = "";
			std::string concat_conf = "";
			for(size_t i = 0; i < tags.size(); i++)
			{
				json::value jv = tags[i];
				concat_tags += conversions::to_utf8string(jv.at(L"name").as_string());
				concat_conf += std::to_string(jv.at(L"confidence").as_number().to_double());
				if (i != tags.size() - 1)
				{
					concat_tags += ",";
					concat_conf += ",";
				}
				//std::cout << conversions::to_utf8string(jv.at(L"name").as_string()) << std::endl;
				//std::cout << jv.at(L"confidence").as_number().to_double() << std::endl;
			}
			this->setImageTagsInfo(concat_tags);
			this->setImageTagsConfidenceInfo(concat_conf);
		}

		/* if request type is the `description`, then parse the caption and its tags. (this tags have more than above tag request) */
		if (obj.has_field(L"description"))
		{
			json::object jb = obj.at(L"description").as_object();
			json::array des_tags = jb.at(L"tags").as_array();
			json::array des_catpions = jb.at(L"captions").as_array();

			std::string caption_tags = "";
			std::string caption = "";
			std::string caption_conf = "";

			for (size_t i = 0; i < des_tags.size(); i++)
			{
				json::value jv = des_tags[i];
				//std::cout << conversions::to_utf8string(jv.as_string()) << std::endl;
				caption_tags += conversions::to_utf8string(jv.as_string());

				if (i != des_tags.size() - 1)
				{
					caption_tags += ",";
				}
			}

			for (size_t j = 0; j < des_catpions.size(); j++)
			{
				json::value jv = des_catpions[j];
				//std::cout << conversions::to_utf8string(jv.at(L"text").as_string()) << std::endl;
				//std::cout << jv.at(L"confidence").as_number().to_double() << std::endl;
				caption += conversions::to_utf8string(jv.at(L"text").as_string());
				caption_conf += std::to_string(jv.at(L"confidence").as_number().to_double());

				if (j != des_catpions.size() - 1)
				{
					caption += ",";
					caption_conf += ",";
				}
			}
			
			this->setImageCaptionsInfo(caption_tags, caption, caption_conf);
		}


		if (obj.has_field(L"metadata")) 
		{
			json::object jb = obj.at(L"metadata").as_object();
			//std::cout << jb.at(L"width").as_number().to_double() << std::endl;
			//std::cout << jb.at(L"height").as_number().to_double() << std::endl;
			//std::cout << conversions::to_utf8string(jb.at(L"format").as_string()) << std::endl;

			this->setImageWidth(jb.at(L"width").as_number().to_double());
			this->setImageHeight(jb.at(L"height").as_number().to_double());
			this->setImageFormat(conversions::to_utf8string(jb.at(L"format").as_string()));
		}

		// Write response body into the file.
		return response.body().read_to_end(fileStream->streambuf());
	})

		// Close the file stream.
		.then([=](size_t)
	{
		return fileStream->close();
	});

	// Wait for all the outstanding I/O to complete and handle any exceptions
	try
	{
		requestTask.wait();
	}
	catch (const std::exception &e)
	{
		printf("Error exception:%s\n", e.what());
	}
	return 0;
}

void VisionRestRequest::setImageCaptionsInfo(std::string t, std::string c, std::string cf)
{
	this->image_captions_tags = t;
	this->image_captions = c;
	this->image_caption_confs = cf;
}

std::string VisionRestRequest::getImageCaptionInfo()
{
	return this->image_captions;
}

std::string VisionRestRequest::getImageCaptionConfidenceInfo()
{
	return this->image_caption_confs;
}

std::string VisionRestRequest::getImageCaptionTagsInfo()
{
	return this->image_captions_tags;
}

void VisionRestRequest::setImageTagsInfo(std::string a)
{
	this->image_tags = a;
}

std::string VisionRestRequest::getImageTags()
{
	return this->image_tags;
}

void VisionRestRequest::setImageTagsConfidenceInfo(std::string a)
{
	this->image_tags_confidence = a;
}

std::string VisionRestRequest::getImageTagsConfidenceInfo()
{
	return this->image_tags_confidence;
}

void VisionRestRequest::setImageWidth(double a)
{
	this->img_width = a;
}

void VisionRestRequest::setImageHeight(double b)
{
	this->img_height = b;
}

void VisionRestRequest::setImageFormat(std::string s)
{
	this->img_format = s;
}

void VisionRestRequest::setCategoryNames(std::string s)
{
	this->cat_names = s;
}

void VisionRestRequest::setCategoryScores(std::string s)
{
	this->cat_scores = s;
}

std::string VisionRestRequest::getCategoryNames()
{
	return this->cat_names;
}

std::string VisionRestRequest::getCategoryScores()
{
	return this->cat_scores;
}

VisionRestRequest::~VisionRestRequest()
{
	delete client;
	delete request;
}
