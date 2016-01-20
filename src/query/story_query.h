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
		typedef enum { REGULAR = 1, PREVIEW } QUERYTYPE;
		StoryQuery(Globals::APPType type);

		virtual void			run();

		AllStories				mOutput;
		QUERYTYPE				mQueryType;
		int						mPreviewId;
	private:
		void					query(AllStories& output);
		void					prelistQuery(int playlistId);
		std::wstring			collectNumbers(std::string text);
		Globals::APPType		mType;
	};

} // !namespace bmc

#endif //!_BMCDISPLAYAPP_APP_QUERY_INDUSTRY_QUERY_H_