README
=====

This project uses Microsoft REST [SDK](https://casablanca.codeplex.com/) and Microsoft Cognitive [Service](https://www.microsoft.com/cognitive-services/en-us/) to do *face detection*, *computer vision analysis* and *emotion detection*

Now supports:

* Face API
	* age, gender, glasses, smile
* Vision API
	* image captioning, description, tags, categorizations
* Emotion API
	* anger, surprise, contempt, neutral, sadness, happiness detection.
	
for more details, check the documents folder.

##Reference
1. API Console debug: https://westus.dev.cognitive.microsoft.com/docs/services/56f91f2d778daf23d8ec6739/operations/56f91f2e778daf14a499e1fa/console
2. Cognitive Service: https://www.microsoft.com/cognitive-services/en-us/documentation
3. C++ REST SDK(github): https://github.com/Microsoft/cpprestsdk
4. Add cpprestsdk to VS project(for less bugs, vision >= VS2015): https://github.com/Microsoft/cpprestsdk/wiki/How-to-use-the-C---Rest-SDK-NuGet-package
