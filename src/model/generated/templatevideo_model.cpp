#include "TemplateVideo_model.h" 


namespace ds {
namespace model {
namespace {
const int							EMPTY_INT = 0;
const unsigned int					EMPTY_UINT = 0;
const float							EMPTY_FLOAT = 0.0f;
const std::wstring					EMPTY_WSTRING;
const ds::Resource					EMPTY_RESOURCE;

}

/**
* \class ds::model::Data
*/
class TemplateVideoRef::Data {
public:
	Data()
	: mEndTime(EMPTY_INT)
	, mId(EMPTY_INT)
	, mResourceId(EMPTY_INT)
	, mStartTime(EMPTY_INT)
{}

int mId;
int mResourceId;
int mStartTime;
int mEndTime;


};

TemplateVideoRef::TemplateVideoRef(){}


const int& TemplateVideoRef::getId() const {
	if(!mData) return EMPTY_INT; 
	return mData->mId; 
}
const int& TemplateVideoRef::getResourceId() const {
	if(!mData) return EMPTY_INT; 
	return mData->mResourceId; 
}
const int& TemplateVideoRef::getStartTime() const {
	if(!mData) return EMPTY_INT; 
	return mData->mStartTime; 
}
const int& TemplateVideoRef::getEndTime() const {
	if(!mData) return EMPTY_INT; 
	return mData->mEndTime; 
}


TemplateVideoRef& TemplateVideoRef::setId(const int& Id){
	if(!mData) mData.reset(new Data()); 
	if(mData) mData->mId = Id; 
	return *this; 
}
TemplateVideoRef& TemplateVideoRef::setResourceId(const int& ResourceId){
	if(!mData) mData.reset(new Data()); 
	if(mData) mData->mResourceId = ResourceId; 
	return *this; 
}
TemplateVideoRef& TemplateVideoRef::setStartTime(const int& StartTime){
	if(!mData) mData.reset(new Data()); 
	if(mData) mData->mStartTime = StartTime; 
	return *this; 
}
TemplateVideoRef& TemplateVideoRef::setEndTime(const int& EndTime){
	if(!mData) mData.reset(new Data()); 
	if(mData) mData->mEndTime = EndTime; 
	return *this; 
}



} // namespace model
} // namespace ds