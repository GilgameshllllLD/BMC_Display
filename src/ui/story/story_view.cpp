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

		for (auto it = mGlobals.mAllData.mAllStories.mStorieLists.begin(); it < mGlobals.mAllData.mAllStories.mStorieLists.end(); it++)
		{
			if (engageCheck((*it).getListStartTime(), (*it).getListEndTime()))
			{
				for (auto i = 0; i < (*it).getStoryRef().size(); i++)
				{
					auto singleItem = new bmc::StoryItem(mGlobals, (*it).getStoryRef()[i]);
					if (singleItem)
						mItemGroup.push_back(singleItem);
				}
			}
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
		{
			mCurrentItem->run();
			setIdleTime(mCurrentItem->getType());
		}
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

	bool StoryView::engageCheck(std::wstring startTime, std::wstring endTime)
	{
		if (startTime == L"0" && endTime == L"0")
			return true;
		double sTime, eTime;
		std::wstringstream sStr(startTime);
		std::wstringstream eStr(endTime);
		sStr >> sTime;
		eStr >> eTime;
		if (eStr.fail() || sStr.fail())
			return false;
		else
		{
			auto localTime = Poco::LocalDateTime();
			double localTimeValue;
			std::wstringstream lStr(L"");
			lStr << localTime.year();
			lStr << fillZero(localTime.month());
			lStr << fillZero(localTime.day());
			lStr << fillZero(localTime.hour());
			lStr << fillZero(localTime.minute());
			lStr << fillZero(localTime.second());
			lStr >> localTimeValue;
			if (localTimeValue > sTime && localTimeValue < eTime)
				return true;
			return false;
		}
	}

	std::wstring StoryView::fillZero(int tempValue)
	{
		std::wstring temsStr(L"");
		temsStr += std::to_wstring(tempValue);
		if (temsStr.size() == 1)
		{
			temsStr.clear();
			temsStr += L"0";
			temsStr += std::to_wstring(tempValue);
		}
		return temsStr;
	}

} // namespace bmc
