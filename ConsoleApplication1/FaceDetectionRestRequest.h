#include<cpprest\http_client.h>

using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency::streams;

#pragma once
class FaceDetectionRestRequest
{
public:
	/* use to send local request by default */
	FaceDetectionRestRequest();
	/* use to construct request that send local/remote request */
	FaceDetectionRestRequest(bool send_raw_image);
	void ConstructRequestHeader(const wchar_t *subscription_key);
	void ConstructRequestBody(const wchar_t* image_path);
	int SendDetectionRequest(const wchar_t* key, const wchar_t* image);
	/* set the confidence score */
	void setSmileConfidenceScore(double v);
	void setGenderValue(std::string str);
	void setAgeValue(double a);
	void setGlassesValue(std::string str);

	/* get the values after detection */
	double getSmileConfidenceScore();
	std::string getGenderValue();
	double getAgeValue();
	std::string getGlassesValue();

	http_client* getClient();
	http_request* getRequest();
	~FaceDetectionRestRequest();
private:
	http_client* client;
	http_request* request;
	bool send_raw_image = true;

	double smileScore = 0.0f;
	std::string genderValue = "";
	double age = 0.0f;
	std::string glassesValue = "";
};

