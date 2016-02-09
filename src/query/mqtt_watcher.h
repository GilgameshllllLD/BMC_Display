#ifndef BMCDISPLAYAPP_APP_QUERY_MQTT_WATCHER_H_
#define BMCDISPLAYAPP_APP_QUERY_MQTT_WATCHER_H_

#include <queue>
#include <mutex>
#include <atomic>

#include <ds/app/auto_update.h>

namespace bmc {

/**
 * \class ds::MqttWatcher
 * \brief Feed clients information about changes in MQTT.
 */
class MqttWatcher final : public ds::AutoUpdate
{
public:
	typedef std::queue < std::string > MessageQueue;

public:
	// Standard MQTT location
	MqttWatcher(	ds::ui::SpriteEngine&,
					const std::string& host = "test.mosquitto.org",
					const std::string& topic_inbound = "/ds_test_mqtt_inbound",
					const std::string& topic_outband = "/ds_test_mqtt_outbound",
					float refresh_rate = 0.1f,
					int port = 1883);

	virtual ~MqttWatcher();

	void							addListener(const std::function<void(const MessageQueue&)>&);
	void							push(const std::string&);

protected:
	virtual void					update(const ds::UpdateParams &) override;

private:
	class Loop
	{
	public:
		std::mutex					mInboundMutex;
		std::mutex					mOutboundMutex;
		std::atomic<bool>			mAbort;
		std::atomic<bool>			mConnected{ false };
		MessageQueue				mLoopInbound;
		MessageQueue				mLoopOutbound;

	public:
		Loop(	ds::ui::SpriteEngine&,
				const std::string& host,
				const std::string& topic_inbound,
				const std::string& topic_outband,
				float refresh_rate,
				int port);

		virtual void				run();

	private:
		const std::string			mHost;
		const std::string			mTopicInbound;
		const std::string			mTopicOutbound;
		const int					mPort;
		const int					mRefreshRateMs;	// in milliseconds
		bool						mFirstTimeMessage;
	};

	MessageQueue					mMsgInbound;
	MessageQueue					mMsgOutbound;
	std::vector < std::function<void(const MessageQueue&)> >
									mListeners;
	Loop							mLoop;

	Poco::Timestamp::TimeVal		mLastMessageTime;

private:
	void							runLoop();
};

} //!namespace bmc

#endif //!BMCDISPLAYAPP_APP_QUERY_MQTT_WATCHER_H_
