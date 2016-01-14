#pragma once
#ifndef _BMCDISPLAYAPP_APP_UI_STORY_STORY_ITEM_H_
#define _BMCDISPLAYAPP_APP_UI_STORY_STORY_ITEM_H_


#include <ds/ui/sprite/sprite.h>
#include <ds/app/event_client.h>
#include <ds/ui/sprite/text.h>
#include <ds/ui/sprite/multiline_text.h>
#include <ds/ui/sprite/image.h>
#include <ds/ui/sprite/video.h>
#include <ds/ui/layout/layout_sprite.h>

#include "model/generated/story_model.h"
#include "model/generated/templatevideo_model.h"

namespace bmc {

	class Globals;

	/**
	* \class bmc::StoryView
	*			A sample view
	*/
	class StoryItem final : public ds::ui::Sprite  {
	public:
		StoryItem(Globals& g, ds::model::StoryRef model);
		typedef enum { IMAGE = 0, VIDEO, CUSTOM} storyType;

		void								setdata();
		void								layout();
		void								run();
		storyType							getType(){ return mType; };
		float								getVideoTime();

	private:
		virtual void						updateServer(const ds::UpdateParams& p);

		typedef ds::ui::Sprite				inherited;
		Globals&							mGlobals;
		ds::model::StoryRef					mModel;

		ds::ui::Image*						mBackgroundImage;
		ds::ui::Video*						mBackgroundVideo;
		ds::ui::Text*						mFirstLine;
		ds::ui::Text*						mSecondLine;
		ds::ui::Text*						mThirdLine;
		float								mStartTime;
		float								mEndTime;
		storyType							mType;
	};

} // namespace bmc

#endif
