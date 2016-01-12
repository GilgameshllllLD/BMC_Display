#include "Story_model.h" 

#include "templatevideo_model.h"

namespace ds {
namespace model {
namespace {
const int							EMPTY_INT = 0;
const unsigned int					EMPTY_UINT = 0;
const float							EMPTY_FLOAT = 0.0f;
const std::wstring					EMPTY_WSTRING;
const ds::Resource					EMPTY_RESOURCE;
const TemplateVideoRef EMPTY_TEMPLATEVIDEOREF;

}

/**
* \class ds::model::Data
*/
class StoryRef::Data {
public:
	Data()
	: mFirstLine(EMPTY_WSTRING)
	, mId(EMPTY_UINT)
	, mResourceId(EMPTY_INT)
	, mSecondLine(EMPTY_WSTRING)
	, mTemplateVideoId(EMPTY_INT)
	, mTemplateVideoRef(EMPTY_TEMPLATEVIDEOREF)
	, mThirdLine(EMPTY_WSTRING)
{}

std::wstring mFirstLine;
int mResourceId;
unsigned int mId;
std::wstring mSecondLine;
int mTemplateVideoId;
std::wstring mThirdLine;
TemplateVideoRef mTemplateVideoRef;


};

StoryRef::StoryRef(){}


const std::wstring& StoryRef::getFirstLine() const {
	if(!mData) return EMPTY_WSTRING; 
	return mData->mFirstLine; 
}
const int& StoryRef::getResourceId() const {
	if(!mData) return EMPTY_INT; 
	return mData->mResourceId; 
}
const unsigned int& StoryRef::getId() const {
	if(!mData) return EMPTY_UINT; 
	return mData->mId; 
}
const std::wstring& StoryRef::getSecondLine() const {
	if(!mData) return EMPTY_WSTRING; 
	return mData->mSecondLine; 
}
const int& StoryRef::getTemplateVideoId() const {
	if(!mData) return EMPTY_INT; 
	return mData->mTemplateVideoId; 
}
const std::wstring& StoryRef::getThirdLine() const {
	if(!mData) return EMPTY_WSTRING; 
	return mData->mThirdLine; 
}
const TemplateVideoRef& StoryRef::getTemplateVideoRef() const {
	if(!mData) return EMPTY_TEMPLATEVIDEOREF; 
	return mData->mTemplateVideoRef; 
}


StoryRef& StoryRef::setFirstLine(const std::wstring& FirstLine){
	if(!mData) mData.reset(new Data()); 
	if(mData) mData->mFirstLine = FirstLine; 
	return *this; 
}
StoryRef& StoryRef::setResourceId(const int& ResourceId){
	if(!mData) mData.reset(new Data()); 
	if(mData) mData->mResourceId = ResourceId; 
	return *this; 
}
StoryRef& StoryRef::setId(const unsigned int& Id){
	if(!mData) mData.reset(new Data()); 
	if(mData) mData->mId = Id; 
	return *this; 
}
StoryRef& StoryRef::setSecondLine(const std::wstring& SecondLine){
	if(!mData) mData.reset(new Data()); 
	if(mData) mData->mSecondLine = SecondLine; 
	return *this; 
}
StoryRef& StoryRef::setTemplateVideoId(const int& TemplateVideoId){
	if(!mData) mData.reset(new Data()); 
	if(mData) mData->mTemplateVideoId = TemplateVideoId; 
	return *this; 
}
StoryRef& StoryRef::setThirdLine(const std::wstring& ThirdLine){
	if(!mData) mData.reset(new Data()); 
	if(mData) mData->mThirdLine = ThirdLine; 
	return *this; 
}
StoryRef& StoryRef::setTemplateVideoRef(const TemplateVideoRef& TemplateVideoRef){
	if(!mData) mData.reset(new Data()); 
	if(mData) mData->mTemplateVideoRef = TemplateVideoRef; 
	return *this; 
}



} // namespace model
} // namespace ds