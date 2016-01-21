#ifndef _BMCDISPLAYAPP_APP_GLOBALS_
#define _BMCDISPLAYAPP_APP_GLOBALS_

#include <ds/cfg/cfg_text.h>
#include <ds/cfg/settings.h>
#include <ds/cfg/cfg_nine_patch.h>
#include <ds/ui/sprite/sprite_engine.h>

#include "model/all_data.h"
#include "query/mqtt_watcher.h"

namespace ds {
	namespace ui {
		class SpriteEngine;
	} // namespace ui
} // namespace ds

namespace bmc {

	/**
	 * \class bmc::Globals
	 * \brief Global data for the app.
	 */
	class Globals {
		class MqttWatcher;
	public:
		Globals(ds::ui::SpriteEngine&, const AllData& d);
		typedef enum { WELCOME = 1, RIO, TRANSFORMATION } APPType;

		ds::ui::SpriteEngine&			mEngine;

		const AllData&					mAllData;

		const float						getAnimDur();

		void							initialize();

		//Shortcuts
		const ds::cfg::Text&			getText(const std::string& name) const;
		const ds::cfg::Settings&		getSettingsLayout() const;
		const ds::cfg::Settings&		getSettings(const std::string& name) const;
		bool							mActivePreview;
		bool							mCompletePreview;
		int								mPreviewListId;

	private:

		float							mAnimationDuration;
		std::shared_ptr<MqttWatcher>	mMqttWatcher;
	};

} // !namespace bmc

#endif // !_BMCDISPLAYAPP_APP_GLOBALS_