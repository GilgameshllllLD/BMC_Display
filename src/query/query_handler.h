#ifndef _BMCDISPLAYAPP_APP_QUERY_QUERYHANDLER_H_
#define _BMCDISPLAYAPP_APP_QUERY_QUERYHANDLER_H_

#include <ds/app/event_client.h>
#include <ds/data/resource_list.h>
#include <ds/thread/serial_runnable.h>
#include <ds/ui/sprite/sprite_engine.h>
// NOTE: Placing this include at the top gets a conflict
// with cinder. Need to look into that.
#include <ds/network/node_watcher.h>
#include "query/story_query.h"
#include "model/all_data.h"


namespace bmc {
	class Globals;

	/**
	 * \class bmc::QueryHandler
	 * \brief Handle app events that deal with querying for data.
	 */
	class QueryHandler {
	public:
		QueryHandler(ds::ui::SpriteEngine&, AllData&, Globals& g, Globals::APPType type);

		void								runInitialQueries();
		void								update();
	private:

		void								onAppEvent(const ds::Event&);
		void								onStoryQuery(StoryQuery&);

		ds::EventClient						mEventClient;
		Globals&							mGlobals;
		AllData&							mAllData;

		ds::SerialRunnable<StoryQuery>		mStoryQuery;

		ds::NodeWatcher						mNodeWatcher;
		Poco::Timestamp::TimeVal			mLastMessageTime;
		bool								mNeedtoRefresh;
		MqttWatcher							mQttwatcherWelcome;
		MqttWatcher							mQttwatcherRio;
		MqttWatcher							mQttwatcherTrans;
		Globals::APPType					mType;

		// CACHING
		ds::ResourceList					mResources;
	};

} // !namespace bmc

#endif // !_BMCDISPLAYAPP_APP_QUERY_QUERYHANDLER_H_