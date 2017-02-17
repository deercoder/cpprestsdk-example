#include "stdafx.h"
#include<cpprest\filestream.h>
#include<iostream>
#include<string>
#include "FaceDetectionRestRequest.h"


utility::char_t *base_host = U("https://westus.api.cognitive.microsoft.com/face/v1.0/detect?returnFaceId=true&returnFaceLandmarks=true&returnFaceAttributes=age,gender,smile,glasses");

/**
 * constructor for HTTP client and request, send FaceDetection Request, with pre-defined API and interface
 * check: 
*/
FaceDetectionRestRequest::FaceDetectionRestRequest()
{
	this->client = new http_client(base_host);
	this->request = new http_request(methods::POST);
}

FaceDetectionRestRequest::FaceDetectionRestRequest(bool send_raw_file)
{
	this->client = new http_client(base_host);
	this->request = new http_request(methods::POST);
	this->send_raw_image = send_raw_file;
}

void FaceDetectionRestRequest::ConstructRequestHeader(const wchar_t * subscription_key)
{
	if (this->send_raw_image) {
		this->request->headers().add(L"Content-Type", L"application/octet-stream");
	}
	else {
		this->request->headers().add(L"Content-Type", L"application/json");
	}
	this->request->headers().add(L"ocp-apim-subscription-key", subscription_key);
}

void FaceDetectionRestRequest::ConstructRequestBody(const wchar_t * image_path) 
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

http_client* FaceDetectionRestRequest::getClient()
{
	return this->client;
}

http_request* FaceDetectionRestRequest::getRequest()
{
	return this->request;
}

int FaceDetectionRestRequest::SendDetectionRequest(const wchar_t * key, const wchar_t *image)
{
	auto fileStream = std::make_shared<ostream>();

	// Open stream to output file.
	pplx::task<void> requestTask = fstream::open_ostream(U("results.html")).then([=](ostream outFile)
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
		std::string json_str = str.substr(1, str.size() - 2);
		std::cout << "Response string after formating and pruning: " << json_str << std::endl;

		// after delete '[]', reform into json object to parse
		//json::value jv = json::value::value(conversions::to_string_t(json_str));
		//bool flag = jv.has_field(L"faceId");
		//bool flag2 = response.extract_json().get().has_field(L"faceId");
		//bool flag3 = response.extract_json().get().has_field(L"faceRectangle");
		/// flag4 is true, means that it's null. why??? becuase the response is treated as a string,
		/// not a json(), so extract_json() is null
		//bool flag4 = response.extract_json().get().is_null();
		//std::cout << "," << flag2 << "," << flag4 << std::endl;
		//if (flag) {
		//	std::cout << conversions::to_utf8string(jv.at(L"faceId").as_string()) << std::endl;
		//}
		json::value obj = json::value::parse(conversions::to_string_t(json_str));

		bool flag5 = obj.has_field(L"faceId");
		utility::string_t aaat = obj.at(L"faceId").as_string();

		/* set the fields if there is key value */
		if (obj.has_field(L"faceAttributes"))
		{
			json::object faceRect = obj.at(L"faceAttributes").as_object();
			this->setSmileConfidenceScore(faceRect.at(L"smile").as_number().to_double());
			this->setGenderValue(conversions::to_utf8string(faceRect.at(L"gender").as_string()));
			this->setAgeValue(faceRect.at(L"age").as_number().to_double());
			this->setGlassesValue(conversions::to_utf8string(faceRect.at(L"glasses").as_string()));
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

void FaceDetectionRestRequest::setAgeValue(double v)
{
	this->age = v;
}

void FaceDetectionRestRequest::setSmileConfidenceScore(double s)
{
	this->smileScore = s;
}

void FaceDetectionRestRequest::setGenderValue(std::string a)
{
	this->genderValue = a;
}

void FaceDetectionRestRequest::setGlassesValue(std::string a)
{
	this->glassesValue = a;
}

double FaceDetectionRestRequest::getAgeValue()
{
	return this->age;
}

double FaceDetectionRestRequest::getSmileConfidenceScore()
{
	return this->smileScore;
}

std::string FaceDetectionRestRequest::getGenderValue()
{
	return this->genderValue;
}

std::string FaceDetectionRestRequest::getGlassesValue()
{
	return this->glassesValue;
}

FaceDetectionRestRequest::~FaceDetectionRestRequest()
{
	delete client;
	delete request;
}
