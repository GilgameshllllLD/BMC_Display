#pragma once
#ifndef _BMCDISPLAYAPP_APP_UI_STORY_STORY_VIEW_H_
#define _BMCDISPLAYAPP_APP_UI_STORY_STORY_VIEW_H_


#include <ds/ui/sprite/sprite.h>
#include <ds/app/event_client.h>
#include <ds/ui/sprite/text.h>

#include "model/generated/story_model.h"
#include "story_item.h"

namespace bmc {

class Globals;

/**
* \class bmc::StoryView
*			A sample view
*/
class StoryView final : public ds::ui::Sprite  {
public:
	StoryView(Globals& g);

private:
	virtual void						updateServer(const ds::UpdateParams& p);

	void								animateOn();
	void								animateOff();

	void								setData();

	void								layout();
	void								nextItems();
	void								setIdleTime(StoryItem::storyType);
	bool								engageCheck(std::wstring startTime, std::wstring endTime);
	std::wstring						fillZero(int tempValue);
	typedef ds::ui::Sprite				inherited;
	Globals&							mGlobals;

	std::vector<StoryItem*>				mItemGroup;

	StoryItem*							mCurrentItem;
	int									mCurrentItemIndex;



};
} // namespace bmc

#endif
