#ifndef _BMCDISPLAYAPP_APP_MODEL_ALLSTORIES_H_
#define _BMCDISPLAYAPP_APP_MODEL_ALLSTORIES_H_

#include "model/generated/story_model.h"

namespace bmc {

/**
* \class bmc::AllData
* This is kind of silly, but If we expand anything I want this here
*/
class AllStories {
public:

	AllStories(){};

	std::vector<ds::model::StoryRef>	mStories;

};

} // !namespace bmc

#endif // !_BMCDISPLAYAPP_APP_MODEL_ALLSTORIES_H_