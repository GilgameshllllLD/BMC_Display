#include "globals.h"

#include <Poco/String.h>

#include <ds/app/engine/engine_cfg.h>
#include <ds/app/environment.h>
#include <ds/cfg/settings.h>

#include "app_defs.h"

namespace bmc {

	/**
	 * \class bmc::Globals
	 */
	Globals::Globals(ds::ui::SpriteEngine& e, const AllData& d)
		: mEngine(e)
		, mAllData(d)
		, mAnimationDuration(0.35f)
	{
	}

	const float Globals::getAnimDur(){
		return mAnimationDuration;
	}

	void Globals::initialize(){
		mAnimationDuration = getSettingsLayout().getFloat("animation:duration", 0, mAnimationDuration);
	}

	const ds::cfg::Settings& Globals::getSettings(const std::string& name) const {
		return mEngine.getEngineCfg().getSettings(name);
	}

	const ds::cfg::Settings& Globals::getSettingsLayout() const {
		return mEngine.getEngineCfg().getSettings(SETTINGS_LAYOUT);
	}


	const ds::cfg::Text& Globals::getText(const std::string& name) const {
		return mEngine.getEngineCfg().getText(name);

	}

	void Globals::setupMqttWatcher()
	{
		if (mMqttWatcher) return;

	}

	void Globals::getMqttWatcher()
	{
		/*if (mMqttWatcher)
			return mMqttWatcher.get()
		else*/
			//return nullptr;
	}

	void Globals::consumeMqttMessage(const std::string& msg)
	{

	}

	void Globals::processMqttAmbientRequest()
	{

	}

	void Globals::processMqttEngageRequest(int visiting_group_id)
	{

	}

} // !namespace bmc
