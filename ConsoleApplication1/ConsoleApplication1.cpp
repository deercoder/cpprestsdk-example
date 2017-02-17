// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FaceDetectionRestRequest.h"
#include "VisionRestRequest.h"
#include "EmotionRestRequest.h"

int main(int argc, char* argv[])
{
	FaceDetectionRestRequest* fq = new FaceDetectionRestRequest();
	fq->SendDetectionRequest(L"XXXXXXXXXXXXXXXXXXXXXXXX", L"C:\\Users\\ChangLiu\\Documents\\Visual Studio 2015\\Projects\\ConsoleApplication1\\Debug\\NF_200001_001.jpg");

	std::cout << "******************************** Face Detection Test Case *****************************" << std::endl;
	std::cout << "smile: " << fq->getSmileConfidenceScore() << std::endl;
	std::cout << "gender: " << fq->getGenderValue() << std::endl;
	std::cout << "age: " << fq->getAgeValue() << std::endl;
	std::cout << "glasses: " << fq->getGlassesValue() << std::endl;

	/*
	from 0 to 6, the corresponding type is as follows:
	{ "Categories", "ImageType", "Faces", "Adult", "Color", "Tags", "Description" };
	*/
	VisionRestRequest *vq = new VisionRestRequest(6);
	vq->SendAnalysisRequest(L"XXXXXXXXXXXXXXXXXXXXXXXX", L"C:\\Users\\ChangLiu\\Documents\\Visual Studio 2015\\Projects\\ConsoleApplication1\\16_abraham_lincoln.jpg");
	std::cout << "******************************** Computer Vision Test Case *****************************" << std::endl;
	std::cout << vq->getImageCaptionInfo() << std::endl;
	std::cout << vq->getImageCaptionConfidenceInfo() << std::endl;
	std::cout << vq->getImageCaptionTagsInfo() << std::endl;
	//std::cout << vq->getImageTags() << std::endl;
	//std::cout << vq->getImageTagsConfidenceInfo() << std::endl;
	std::cout << vq->getCategoryNames() << std::endl;
	std::cout << vq->getCategoryScores() << std::endl;


	std::cout << "******************************** Emotion Test Case *****************************" << std::endl;
	EmotionRestRequest *eq = new EmotionRestRequest();
	eq->sendEmotionRequest(L"XXXXXXXXXXXXXXXXXXXXXXXX", L"C:\\Users\\ChangLiu\\Documents\\Visual Studio 2015\\Projects\\ConsoleApplication1\\16_abraham_lincoln.jpg");
	std::cout << "Anger score: " << eq->getAngerScore() << std::endl;
	std::cout << "Contempt score: " << eq->getContemptScore() << std::endl;
	std::cout << "Disgust score: " << eq->getDisgustScore() << std::endl;
	std::cout << "Fear score: " << eq->getFearScore() << std::endl;
	std::cout << "Happiness score: " << eq->getHappinessScore() << std::endl;
	std::cout << "Neutral score: " << eq->getNeuralScore() << std::endl;
	std::cout << "Sadness score: " << eq->getSadnessScore() << std::endl;
	std::cout << "Surprise score: " << eq->getSurpriseScore() << std::endl;

	return 0;
}
