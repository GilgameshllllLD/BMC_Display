#ifndef _BMCDISPLAYAPP_APP_QUERY_INDUSTRY_QUERY_H_
#define _BMCDISPLAYAPP_APP_QUERY_INDUSTRY_QUERY_H_

#include <functional>
#include <Poco/Runnable.h>
#include <ds/query/query_result.h>

#include "model/all_stories.h"
#include "app/globals.h"

namespace bmc {
	class Globals;

/**
 * \class bmc::StoryQuery
 */
class StoryQuery : public Poco::Runnable {
public:
	StoryQuery(Globals::APPType type);

	virtual void			run();

	AllStories				mOutput;
private:
	void					query(AllStories& output);
	Globals::APPType		mType;
};

} // !namespace bmc

#endif //!_BMCDISPLAYAPP_APP_QUERY_INDUSTRY_QUERY_H_