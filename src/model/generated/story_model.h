#pragma once
#ifndef DS_MODEL_AUTOGENERATED_Story
#define DS_MODEL_AUTOGENERATED_Story

#include <ds/data/resource.h>
#include <memory>
#include <vector>
#include <cinder/Vector.h>



namespace ds {
namespace model{


class TemplateVideoRef;


/**
* \class ds::model::StoryRef
*			Auto-generated data model for Story
*/
class StoryRef {
public:

	StoryRef();

	const std::wstring& getFirstLine() const;
	const int& getResourceId() const;
	const unsigned int& getId() const;
	const std::wstring& getSecondLine() const;
	const int& getTemplateVideoId() const;
	const std::wstring& getThirdLine() const;
	const TemplateVideoRef& getTemplateVideoRef() const;


	StoryRef& setFirstLine(const std::wstring& FirstLine);
	StoryRef& setResourceId(const int& ResourceId);
	StoryRef& setId(const unsigned int& Id);
	StoryRef& setSecondLine(const std::wstring& SecondLine);
	StoryRef& setTemplateVideoId(const int& TemplateVideoId);
	StoryRef& setThirdLine(const std::wstring& ThirdLine);
	StoryRef& setTemplateVideoRef(const TemplateVideoRef& TemplateVideoRef);



private:
	class Data;
	std::shared_ptr<Data>	mData;
};

} // namespace model
} // namespace ds

#endif
