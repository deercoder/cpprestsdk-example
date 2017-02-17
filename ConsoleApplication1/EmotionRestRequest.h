#include<cpprest\http_client.h>

using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency::streams;
#pragma once
class EmotionRestRequest
{
public:
	EmotionRestRequest();
	EmotionRestRequest(bool send_raw_file);
	~EmotionRestRequest();
	int sendEmotionRequest(const wchar_t * key, const wchar_t *image);

	void ConstructRequestHeader(const wchar_t *subscription_key);
	void ConstructRequestBody(const wchar_t* image_path);

	/* parse for face rectangle, for the emotion detection */
	void setFaceRectangle(int w, int h, int l, int t);

	/* set scores and output */
	void setEmotionScores(double anger, double cont, double dis, double fear, double hap, double neural, double sadness, double surprise);
	double getAngerScore();
	double getContemptScore();
	double getDisgustScore();
	double getFearScore();
	double getHappinessScore();
	double getNeuralScore();
	double getSadnessScore();
	double getSurpriseScore();

	http_client* getClient();
	http_request* getRequest();
private:
	http_client* client;
	http_request* request;
	bool send_raw_image = true;

	/* score for each emotions, there are 8 default/fixed emotions, output score for each emotion */
	double anger_score;
	double contempt_score;
	double disgust_score;
	double fear_score;
	double happiness_score;
	double neutral_score;
	double sadness_score;
	double surprise_score;

	/* face rectangle corresponding to the emotion */
	int img_height;
	int img_width;
	int img_top;
	int img_left;
};

