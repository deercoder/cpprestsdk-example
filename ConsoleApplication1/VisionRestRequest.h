#include<cpprest\http_client.h>

using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency::streams;
#pragma once
class VisionRestRequest
{
public:
	/* use to send local request by default */
	VisionRestRequest(int flag);
	/* use to construct request that send local/remote request */
	VisionRestRequest(int flag, bool send_raw_image);
	void ConstructRequestHeader(const wchar_t *subscription_key);
	void ConstructRequestBody(const wchar_t* image_path);
	int SendAnalysisRequest(const wchar_t* key, const wchar_t* image);

	/* set image properties from the request, cognitive service can fetch width, height and format information */
	void setImageWidth(double a);
	void setImageHeight(double a);
	void setImageFormat(std::string form);

	/* image captions information (Description request) */
	void setImageCaptionsInfo(std::string tags, std::string caps, std::string caps_conf);
	std::string getImageCaptionInfo();
	std::string getImageCaptionConfidenceInfo();
	std::string getImageCaptionTagsInfo();

	/* image tags information (Tag request) */
	void setImageTagsInfo(std::string tags);
	void setImageTagsConfidenceInfo(std::string conf);
	std::string getImageTags();
	std::string getImageTagsConfidenceInfo();

	/* image category names */
	void setCategoryNames(std::string s);
	void setCategoryScores(std::string s);
	std::string getCategoryNames();
	std::string getCategoryScores();

	/*
	from 0 to 6, the corresponding type is as follows:
	{ "Categories", "ImageType", "Faces", "Adult", "Color", "Tags", "Description" };
	*/
	void setRequestType(int flag);


	http_client* getClient();
	http_request* getRequest();
	~VisionRestRequest();
private:
	http_client* client;
	http_request* request;
	bool send_raw_image = true;

	/// memeber variables to store the responses(parsed results)
	double img_width;
	double img_height;
	std::string img_format;

	/// information extracted from the `Description` request, it contains many tags and one caption/description
	std::string image_captions_tags;  ///NOTE, for these tags, there is no confidence in the `Description` request.
	std::string image_captions;
	std::string image_caption_confs;

	/// information about the tags, there are only 5 tags, this is from `Tag` request
	std::string image_tags;
	std::string image_tags_confidence;

	/// information about the category names
	std::string cat_names;
	std::string cat_scores;
};

