#include "stdafx.h"
#include<cpprest\filestream.h>
#include<iostream>
#include<string>
#include "EmotionRestRequest.h"

/*


Reference:
1) console tools: https://westus.dev.cognitive.microsoft.com/docs/services/5639d931ca73072154c1ce89/operations/563b31ea778daf121cc3a5fa/console
*/

std::string emotion_base_host = "https://westus.api.cognitive.microsoft.com/emotion/v1.0/recognize";

EmotionRestRequest::EmotionRestRequest()
{
	this->client = new http_client(conversions::to_string_t(emotion_base_host));
	this->request = new http_request(methods::POST);
}

EmotionRestRequest::EmotionRestRequest(bool send_raw_file)
{
	this->client = new http_client(conversions::to_string_t(emotion_base_host));
	this->request = new http_request(methods::POST);
	this->send_raw_image = send_raw_file;
}


void EmotionRestRequest::ConstructRequestHeader(const wchar_t * subscription_key)
{
	if (this->send_raw_image) {
		this->request->headers().add(L"Content-Type", L"application/octet-stream");
	}
	else {
		this->request->headers().add(L"Content-Type", L"application/json");
	}
	this->request->headers().add(L"ocp-apim-subscription-key", subscription_key);
}

void EmotionRestRequest::ConstructRequestBody(const wchar_t * image_path)
{
	if (this->send_raw_image) {
		concurrency::streams::istream input_stream = concurrency::streams::file_stream<uint8_t>::open_istream(conversions::to_string_t(image_path)).get();
		this->request->set_body(input_stream, input_stream.streambuf().size());
	}
	else {
		/* set remote url path, NOTE about parameters for json::values::string */
		json::value jvalue;
		jvalue[L"url"] = json::value::string(conversions::to_string_t(image_path));
		this->request->set_body(jvalue);
	}
}

http_client* EmotionRestRequest::getClient()
{
	return this->client;
}

http_request* EmotionRestRequest::getRequest()
{
	return this->request;
}

int EmotionRestRequest::sendEmotionRequest(const wchar_t * key, const wchar_t *image)
{
	auto fileStream = std::make_shared<ostream>();

	// Open stream to output file.
	pplx::task<void> requestTask = fstream::open_ostream(U("emotion.html")).then([=](ostream outFile)
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

		/********************************************************************************************
		 * Here we assume the the request image only contain one face, so we only delete first/end []
		 * Then we parse the first face rectangle and its score
		 * This api supports multiple faces, but its not necessary since our preprocessing have only
		 * one image/face. (The same goes for face detection/computer vision API, only one face!)
		 ******************************************************************************************/
		std::string json_str = str.substr(1, str.size() - 2);
		std::cout << "Response string after formating and pruning: " << json_str << std::endl;

		json::value obj = json::value::parse(conversions::to_string_t(json_str));


		if (obj.has_field(L"faceRectangle"))
		{
			json::value jv = obj.at(L"faceRectangle");
			int height = jv.at(L"height").as_number().to_int32();
			int left = jv.at(L"left").as_number().to_int32();
			int top = jv.at(L"top").as_number().to_int32();
			int width = jv.at(L"width").as_number().to_int32();
			this->setFaceRectangle(width, height, left, top);
		}

		if (obj.has_field(L"scores"))
		{
			json::value jv = obj.at(L"scores");
			double anger_s = jv.at(L"anger").as_number().to_double();
			double contempt_s = jv.at(L"contempt").as_number().to_double();
			double disgust_s = jv.at(L"disgust").as_number().to_double();
			double fear_s = jv.at(L"fear").as_number().to_double();
			double happy_s = jv.at(L"happiness").as_number().to_double();
			double neutral_s = jv.at(L"neutral").as_number().to_double();
			double sad_s = jv.at(L"sadness").as_number().to_double();
			double surprise_s = jv.at(L"surprise").as_number().to_double();
			this->setEmotionScores(anger_s, contempt_s, disgust_s, fear_s, happy_s, neutral_s, sad_s, surprise_s);
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

EmotionRestRequest::~EmotionRestRequest()
{
	delete client;
	delete request;
}

void EmotionRestRequest::setFaceRectangle(int w, int h, int l, int t)
{
	this->img_width = w;
	this->img_height = h;
	this->img_left = l;
	this->img_top = t;
}

void EmotionRestRequest::setEmotionScores(double anger, double cont, double dis, double fear, double hap, double neural, double sadness, double surprise)
{
	this->anger_score = anger;
	this->contempt_score = cont;
	this->disgust_score = dis;
	this->fear_score = fear;
	this->happiness_score = hap;
	this->neutral_score = neural;
	this->sadness_score = sadness;
	this->surprise_score = surprise;
}

double EmotionRestRequest::getAngerScore()
{
	return this->anger_score;
}

double EmotionRestRequest::getContemptScore()
{
	return this->contempt_score;
}

double EmotionRestRequest::getDisgustScore()
{
	return this->disgust_score;
}

double EmotionRestRequest::getFearScore()
{
	return this->fear_score;
}

double EmotionRestRequest::getHappinessScore()
{
	return this->happiness_score;
}

double EmotionRestRequest::getNeuralScore()
{
	return this->neutral_score;
}

double EmotionRestRequest::getSadnessScore()
{
	return this->sadness_score;
}

double EmotionRestRequest::getSurpriseScore()
{
	return this->surprise_score;
}