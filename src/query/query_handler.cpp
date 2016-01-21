#include "query_handler.h"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <cinder/Json.h>
#include <ds/app/event_notifier.h>
#include <ds/debug/logger.h>
#include "app/app_defs.h"
#include "app/globals.h"
#include <Poco/LocalDateTime.h>

namespace bmc {

	/**
	 * \class bmc::QueryHandler
	 */
	QueryHandler::QueryHandler(ds::ui::SpriteEngine& se, AllData &ad, Globals& g, Globals::APPType type)
		: mEventClient(se.getNotifier(), [this](const ds::Event* e){if (e) onAppEvent(*e); })
		, mAllData(ad)
		, mNodeWatcher(se)
		, mNeedtoRefresh(false)
		, mGlobals(g)
		, mType(type)
		, mStoryQuery(se, [type](){return new StoryQuery(type); })
		, mQttwatcherWelcome(se, "bmc.downstreamsandbox.com", "playlist/welcome/preview", "toolbox/playlist/welcome/preview/complete")
		, mQttwatcherRio(se, "bmc.downstreamsandbox.com", "playlist/riogrande/preview", "toolbox/playlist/riogrande/preview/complete")
		, mQttwatcherTrans(se, "bmc.downstreamsandbox.com", "playlist/transformation/preview", "toolbox/playlist/transformation/preview/complete")
	{
		// Initialize data
		mStoryQuery.setReplyHandler([this](StoryQuery& q){this->onStoryQuery(q); });

		mNodeWatcher.add([this](const ds::NodeWatcher::Message& msg){
			mNeedtoRefresh = true;
			mLastMessageTime = Poco::Timestamp().epochMicroseconds();
		});
		mQttwatcherWelcome.addListener([this](const MqttWatcher::MessageQueue& m)
		{
			if (mType == Globals::APPType::WELCOME)
			{
				std::stringstream tempStr(m.back());
				tempStr >> mGlobals.mPreviewListId;
				mGlobals.mActivePreview = true;
			}
		});
		mQttwatcherRio.addListener([this](const MqttWatcher::MessageQueue& m)
		{
			if (mType == Globals::APPType::RIO)
			{
				std::stringstream tempStr(m.back());
				tempStr >> mGlobals.mPreviewListId;
				mGlobals.mActivePreview = true;
			}
		});
		mQttwatcherTrans.addListener([this](const MqttWatcher::MessageQueue& m)
		{
			if (mType == Globals::APPType::TRANSFORMATION)
			{
				std::stringstream tempStr(m.back());
				tempStr >> mGlobals.mPreviewListId;
				mGlobals.mActivePreview = true;
			}
		});

	}

	void QueryHandler::runInitialQueries(){
		mStoryQuery.start(nullptr, true);
	}

	void QueryHandler::onAppEvent(const ds::Event& _e) {
	}

	void QueryHandler::onStoryQuery(StoryQuery& q) {
		mAllData.mAllStories = q.mOutput;
	}


	void QueryHandler::update(){
		auto i = Poco::LocalDateTime();

		if (mNeedtoRefresh)
		{
			Poco::Timestamp::TimeVal nowwy = Poco::Timestamp().epochMicroseconds();
			auto delty = (float)(nowwy - mLastMessageTime) / 1000000.0f;
			if (delty > mGlobals.getSettingsLayout().getFloat("cms:refresh_wait_time", 0, 1.0f))
			{
				mNeedtoRefresh = false;
				mStoryQuery.start();
			}
		}
		if (mGlobals.mCompletePreview)
		{
			auto finishStr = mGlobals.getSettingsLayout().getText("mqtt:finish:string", 0, "");
			if (mType == Globals::APPType::WELCOME)
				mQttwatcherWelcome.push(finishStr);
			else if (mType == Globals::APPType::RIO)
				mQttwatcherRio.push(finishStr);
			else if (mType == Globals::APPType::TRANSFORMATION)
				mQttwatcherTrans.push(finishStr);
			mGlobals.mCompletePreview = false;
		}
	}

} // !namespace bmc
