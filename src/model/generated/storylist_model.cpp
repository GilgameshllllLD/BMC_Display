#include "StoryList_model.h" 


namespace ds {
namespace model {
namespace {
const int							EMPTY_INT = 0;
const unsigned int					EMPTY_UINT = 0;
const float							EMPTY_FLOAT = 0.0f;
const std::wstring					EMPTY_WSTRING;
const ds::Resource					EMPTY_RESOURCE;
const std::vector<StoryRef> EMPTY_STORYREF_VECTOR;

}

/**
* \class ds::model::Data
*/
class StoryListRef::Data {
public:
	Data()
	: mId(EMPTY_UINT)
	, mListEndTime(EMPTY_WSTRING)
	, mListStartTime(EMPTY_WSTRING)
	, mStoryRef(EMPTY_STORYREF_VECTOR)
{}

std::wstring mListStartTime;
std::wstring mListEndTime;
unsigned int mId;
std::vector<StoryRef> mStoryRef;


};

StoryListRef::StoryListRef(){}


const std::wstring& StoryListRef::getListStartTime() const {
	if(!mData) return EMPTY_WSTRING; 
	return mData->mListStartTime; 
}
const std::wstring& StoryListRef::getListEndTime() const {
	if(!mData) return EMPTY_WSTRING; 
	return mData->mListEndTime; 
}
const unsigned int& StoryListRef::getId() const {
	if(!mData) return EMPTY_UINT; 
	return mData->mId; 
}
const std::vector<StoryRef>& StoryListRef::getStoryRef() const {
	if(!mData) return EMPTY_STORYREF_VECTOR; 
	return mData->mStoryRef; 
}


StoryListRef& StoryListRef::setListStartTime(const std::wstring& ListStartTime){
	if(!mData) mData.reset(new Data()); 
	if(mData) mData->mListStartTime = ListStartTime; 
	return *this; 
}
StoryListRef& StoryListRef::setListEndTime(const std::wstring& ListEndTime){
	if(!mData) mData.reset(new Data()); 
	if(mData) mData->mListEndTime = ListEndTime; 
	return *this; 
}
StoryListRef& StoryListRef::setId(const unsigned int& Id){
	if(!mData) mData.reset(new Data()); 
	if(mData) mData->mId = Id; 
	return *this; 
}
StoryListRef& StoryListRef::setStoryRef(const std::vector<StoryRef>& StoryRef){
	if(!mData) mData.reset(new Data()); 
	if(mData) mData->mStoryRef = StoryRef; 
	return *this; 
}



} // namespace model
} // namespace ds