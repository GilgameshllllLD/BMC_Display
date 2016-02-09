#include "mqtt_watcher.h"

#include "mosquittopp.h"

#include <ds/cfg/settings.h>
#include <ds/debug/debug_defines.h>
#include <ds/ui/sprite/sprite_engine.h>
#include "ds/debug/logger.h"

#include <future>

namespace bmc
{

	namespace {
		class MqttSingleton
		{
		public:
			void static initilize_once() { static MqttSingleton S; }
			~MqttSingleton() { mosqpp::lib_cleanup(); }

		private:
			MqttSingleton() { mosqpp::lib_init(); };
		};
		const static ds::BitMask   MQTT_LOG = ds::Logger::newModule("mqtt");

		// a fast way of clearing a queue
		static void _clear_queue(MqttWatcher::MessageQueue& q)
		{
			if (!q.empty()) MqttWatcher::MessageQueue().swap(q);
		}
	} //!namespace

	/**
	 * \class telstra::MqttWatcher
	 */
	MqttWatcher::MqttWatcher(
		ds::ui::SpriteEngine& e,
		const std::string& host /*= "test.mosquitto.org"*/,
		const std::string& topic_inbound /*= "/ds_test_mqtt_inbound"*/,
		const std::string& topic_outband /*= "/ds_test_mqtt_outbound"*/,
		float refresh_rate /*= 0.1f*/,
		int port /*= 1883*/)
		: ds::AutoUpdate(e)
		, mLoop(e, host, topic_inbound, topic_outband, refresh_rate, port)
		, mLastMessageTime(0.0f)
	{
		MqttSingleton::initilize_once();
	}

	MqttWatcher::~MqttWatcher()
	{
		// does not need locking. it's atomic
		mLoop.mAbort = true;
	}

	void MqttWatcher::addListener(const std::function<void(const MessageQueue&)>& fn)
	{
		if (!fn) return;
		mListeners.push_back(fn);
	}

	void MqttWatcher::update(const ds::UpdateParams &)
	{
		if (!mLoop.mConnected)
		{
			Poco::Timestamp::TimeVal nowwy = Poco::Timestamp().epochMicroseconds();
			auto delty = (float)(nowwy - mLastMessageTime) / 1000000.0f;
			if (delty > 30.0f)
			{
				runLoop();
				mLastMessageTime = Poco::Timestamp().epochMicroseconds();
			}
			else
			{
				/*static std::once_flag _warn_once;
				std::call_once(_warn_once, []{
				DS_LOG_ERROR_M("MQTT watcher permanently died after attempting to connect to server " << retry_count << " time(s).", MQTT_LOG);
				});*/
			}
		}

		_clear_queue(mMsgInbound);

		if (!mLoop.mLoopInbound.empty())
		{
			std::lock_guard<std::mutex>	_lock(mLoop.mInboundMutex);
			if (!mLoop.mLoopInbound.empty()) //double check
			{
				mLoop.mLoopInbound.swap(mMsgInbound);
			}
		}

		if (!mMsgOutbound.empty())
		{
			std::lock_guard<std::mutex>	_lock(mLoop.mOutboundMutex);
			while (!mMsgOutbound.empty())
			{
				mLoop.mLoopOutbound.push(mMsgOutbound.front());
				mMsgOutbound.pop();
			}
		}

		if (!mMsgInbound.empty())
		{
			for (const std::function<void(const MessageQueue&)>& cb : mListeners)
			{
				cb(mMsgInbound);
			}
		}
	}

	void MqttWatcher::push(const std::string& str)
	{
		mMsgOutbound.push(str);
	}

	void MqttWatcher::runLoop()
	{
		if (!mLoop.mConnected)
		{
			DS_LOG_INFO_M("Attempting to connect to the MQTT server...", MQTT_LOG);
			std::async(std::launch::async, [this]{ mLoop.run(); });
			mLoop.mConnected = true;
		}
	}

	/**
	 * \class telstra::MqttWatcher::Loop
	 */

	MqttWatcher::Loop::Loop(
		ds::ui::SpriteEngine&,
		const std::string& host,
		const std::string& topic_inbound,
		const std::string& topic_outband,
		float refresh_rate,
		int port)
		: mAbort(false)
		, mHost(host)
		, mPort(port)
		, mTopicInbound(topic_inbound)
		, mTopicOutbound(topic_outband)
		, mFirstTimeMessage(true)
		, mRefreshRateMs(static_cast<int>(refresh_rate * 1000))
	{}

	namespace {
		class MosquittoReceiver final : public mosqpp::mosquittopp
		{
		public:
			void on_connect(int rc) override { mConnectAction(rc); }
			void on_message(const struct mosquitto_message *message) override { mMessageAction(std::string((char*)message->payload, message->payloadlen)); }
			void setConnectAction(const std::function<void(int)>& fn) { mConnectAction = fn; }
			void setMessageAction(const std::function<void(const std::string&)>& fn) { mMessageAction = fn; }

		private:
			std::function<void(int)>				mConnectAction{ [](int){} };
			std::function<void(const std::string&)>	mMessageAction{ [](const std::string&){} };
		};
	}

	void MqttWatcher::Loop::run()
	{
		mAbort = false;
		MosquittoReceiver mqtt_isnt;

		mqtt_isnt.setConnectAction([this](int code) {
			DS_LOG_INFO_M("MQTT server connected: " << code, MQTT_LOG);
		});

		mqtt_isnt.setMessageAction([this](const std::string& m) {
			std::cout << "MQTT watcher received: " << m << std::endl, MQTT_LOG;
			if (!mAbort)
			{
				std::lock_guard<std::mutex>	_lock(mInboundMutex);
				mLoopInbound.push(m);
			}
		});

		auto err_no = mqtt_isnt.connect(mHost.c_str(), mPort);
		if (err_no != MOSQ_ERR_SUCCESS && mFirstTimeMessage)
		{
			DS_LOG_ERROR_M("Unable to connect to the MQTT server. Error number is: " << err_no << ". Error string is: " << mosqpp::strerror(err_no), MQTT_LOG);
			mAbort = true;
		}

		err_no = mqtt_isnt.subscribe(nullptr, mTopicInbound.c_str());
		if (err_no != MOSQ_ERR_SUCCESS && mFirstTimeMessage)
		{
			DS_LOG_ERROR_M("Unable to subscribe to the MQTT topic (" << mTopicInbound << "). Error number is: " << err_no << ". Error string is: " << mosqpp::strerror(err_no), MQTT_LOG);
			mAbort = true;
		}

		while (!mAbort && mqtt_isnt.loop() == MOSQ_ERR_SUCCESS)
		{
			if (!mLoopOutbound.empty())
			{
				std::lock_guard<std::mutex>	_lock(mOutboundMutex);
				while (!mLoopOutbound.empty())
				{
					mqtt_isnt.publish(nullptr, mTopicOutbound.c_str(), mLoopOutbound.front().size(), mLoopOutbound.front().data());
					mLoopOutbound.pop();
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(mRefreshRateMs));
		}

		mConnected = false;
		if (mFirstTimeMessage)
			std::cout << "MQTT watcher returned.";

		mFirstTimeMessage = false;
	}

} //!telstra