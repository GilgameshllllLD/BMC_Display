#include "story_view.h"

#include <Poco/LocalDateTime.h>

#include <ds/app/environment.h>
#include <ds/ui/sprite/sprite_engine.h>
#include <ds/debug/logger.h>

#include "app/app_defs.h"
#include "app/globals.h"
#include "events/app_events.h"

namespace bmc {

	StoryView::StoryView(Globals& g)
		: inherited(g.mEngine)
		, mGlobals(g)
		, mCurrentItem(nullptr)
		, mCurrentItemIndex(0)
	{
		hide();
		setOpacity(0.0f);
		setCenter(0.5f, 0.5f);
		setSize(304.0f, 102.0f);
		setPosition(mEngine.getWorldWidth() / 2, mEngine.getWorldHeight() / 2);

		setData();
		layout();
		animateOn();
	}


	void StoryView::setData() {
		// update view to match new content
		mItemGroup.clear();

		for (auto it = mGlobals.mAllData.mAllStories.mStories.begin(); it < mGlobals.mAllData.mAllStories.mStories.end(); it++)
		{
			auto singleItem = new bmc::StoryItem(mGlobals, (*it));
			if (singleItem)
				mItemGroup.push_back(singleItem);
		}

		if (mItemGroup.size() == 0)
			return;

		if (mItemGroup.size() > 0)
		{
			mCurrentItem = mItemGroup[mCurrentItemIndex];;
			addChildPtr(mCurrentItem);
		}

	}

	void StoryView::layout(){

		if (mCurrentItem)
			mCurrentItem->run();
		setIdleTime(mCurrentItem->getType());
	}


	void StoryView::animateOn(){
		show();
		tweenOpacity(1.0f, mGlobals.getSettingsLayout().getFloat("story_view:anim_time", 0, 0.35f));
	}

	void StoryView::animateOff(){
		tweenOpacity(0.0f, mGlobals.getSettingsLayout().getFloat("story_view:anim_time", 0, 0.35f), 0.0f, ci::EaseNone(), [this]{hide(); });
	}

	void StoryView::updateServer(const ds::UpdateParams& p){
		inherited::updateServer(p);
	}

	void StoryView::nextItems()
	{
		mCurrentItem->tweenOpacity(0.0f, mGlobals.getAnimDur(), 0.0f, ci::EaseInOutQuad(), [this](){
			mCurrentItem->release();

			if (mCurrentItemIndex != mItemGroup.size() - 1)
				mCurrentItemIndex++;
			else
			{
				mCurrentItemIndex = 0;
				setData();
			}

			mCurrentItem = mItemGroup[mCurrentItemIndex];
			if (mCurrentItem)
			{
				addChildPtr(mCurrentItem);
				mCurrentItem->run();
				setIdleTime(mCurrentItem->getType());
			}

		});
	}

	void StoryView::setIdleTime(StoryItem::storyType type)
	{
		auto imageDuringTime = mGlobals.getSettingsLayout().getFloat("back_image:duration", 0, 7.0f);

		if (type == StoryItem::IMAGE)
			callAfterDelay([this]{nextItems(); }, imageDuringTime);
		else if (type == StoryItem::VIDEO)
		{

			if (mCurrentItem->getVideoTime() > mGlobals.getAnimDur())
				callAfterDelay([this]{nextItems(); }, mCurrentItem->getVideoTime() - mGlobals.getAnimDur());
		}
		else if (type == StoryItem::CUSTOM)
		{
			if (mCurrentItem->getVideoTime() > mGlobals.getAnimDur())
				callAfterDelay([this]{nextItems(); }, mCurrentItem->getVideoTime() - mGlobals.getAnimDur());
		}
	}

} // namespace bmc
