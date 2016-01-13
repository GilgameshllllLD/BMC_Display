#include "story_item.h"

#include <Poco/LocalDateTime.h>

#include <ds/app/environment.h>
#include <ds/ui/sprite/sprite_engine.h>
#include <ds/debug/logger.h>

#include "app/app_defs.h"
#include "app/globals.h"
#include "events/app_events.h"

#include <ds/ui/interface_xml/interface_xml_importer.h>

namespace bmc {

	StoryItem::StoryItem(Globals& g, ds::model::StoryRef model)
		: inherited(g.mEngine)
		, mGlobals(g)
		, mBackgroundImage(nullptr)
		, mBackgroundVideo(nullptr)
		, mFirstLine(nullptr)
		, mSecondLine(nullptr)
		, mThirdLine(nullptr)
		, mModel(model)
		, mStartTime(0.0f)
		, mEndTime(0.0f)
	{
		std::map<std::string, ds::ui::Sprite*>	spriteMap;
		ds::ui::XmlImporter::loadXMLto(this, ds::Environment::expand("%APP%/data/layouts/story_item.xml"), spriteMap);

		mFirstLine = dynamic_cast<ds::ui::MultilineText*>(spriteMap["first_line"]);
		mSecondLine = dynamic_cast<ds::ui::MultilineText*>(spriteMap["second_line"]);
		mThirdLine = dynamic_cast<ds::ui::MultilineText*>(spriteMap["third_line"]);

		mBackgroundImage = new ds::ui::Image(mEngine, ds::ui::Image::IMG_CACHE_F);
		mBackgroundImage->setOpacity(0.0f);
		addChildPtr(mBackgroundImage);

		mBackgroundVideo = new ds::ui::Video(mEngine);
		mBackgroundVideo->setAutoStart(false);
		mBackgroundVideo->setOpacity(0.0f);
		mBackgroundVideo->setVideoCompleteCallback([this]()
		{
			mBackgroundVideo->seekTime(0.0f);
		});
		addChildPtr(mBackgroundVideo);

		if (mModel.getResourceId() > 0)
		{
			ds::Resource r;
			r.query(mModel.getResourceId());
			{
				if (r.getType() == ds::Resource::IMAGE_TYPE)
					mType = IMAGE;
				else if (r.getType() == ds::Resource::VIDEO_TYPE)
					mType = VIDEO;
			}
		}
		else
		{
			if (mModel.getFirstLine() != L"" || mModel.getSecondLine() != L"" || mModel.getThirdLine() != L"")
				mType = CUSTOM;
		}

		setdata();
	}

	void StoryItem::setdata()
	{
		if (mType == IMAGE)
			mBackgroundImage->setImageResource(mModel.getResourceId(), ds::ui::Image::IMG_CACHE_F);
		else if (mType == VIDEO)
			mBackgroundVideo->setResourceId(mModel.getResourceId());
		else if (mType == CUSTOM)
		{
			mBackgroundVideo->setResourceId(mModel.getTemplateVideoRef().getResourceId());
			mStartTime = mModel.getTemplateVideoRef().getStartTime();
			mEndTime = mModel.getTemplateVideoRef().getEndTime();

			if (mFirstLine && mModel.getFirstLine() != L"")
				mFirstLine->setText(mModel.getFirstLine());

			if (mSecondLine && mModel.getSecondLine() != L"")
			{
				mSecondLine->setText(mModel.getSecondLine());
				auto initFontsize = 5.0f;
				mSecondLine->setFontSize(initFontsize);
				auto maxHeight = mGlobals.getSettingsLayout().getFloat("second_line:max_height", 0, 37.0f);
				auto maxWidth = mGlobals.getSettingsLayout().getFloat("second_line:max_width", 0, 284.0f);
				while (mSecondLine->getWidth() < maxWidth && mSecondLine->getHeight() < maxHeight)
				{
					initFontsize += 0.1f;
					mSecondLine->setFontSize(initFontsize);
				}
			}

			if (mThirdLine && mModel.getThirdLine() != L"")
				mThirdLine->setText(mModel.getThirdLine());
		}
	}

	void StoryItem::updateServer(const ds::UpdateParams& p)
	{
		inherited::updateServer(p);
	}

	void StoryItem::layout()
	{
		auto showTime = mGlobals.getSettingsLayout().getFloat("back_image:duration", 0);
		mBackgroundImage->tweenOpacity(1.0f, mGlobals.getAnimDur(), 0.0f);
		callAfterDelay([this]
		{
			mBackgroundImage->tweenOpacity(0.0f, mGlobals.getAnimDur(), 0.0f, ci::EaseInOutQuad(), [this]()
			{
				mBackgroundVideo->tweenOpacity(1.0f, mGlobals.getAnimDur(), 0.0f, ci::EaseInOutQuad(), [this]()
				{
					mBackgroundVideo->play();
				});
			});
		}, mGlobals.getAnimDur() + showTime);
	}

	void StoryItem::run()
	{
		if (mType == IMAGE && mBackgroundImage)
			mBackgroundImage->tweenOpacity(1.0f, mGlobals.getAnimDur(), 0.0f);
		else if (mType == VIDEO && mBackgroundVideo)
		{
			mBackgroundVideo->tweenOpacity(1.0f, mGlobals.getAnimDur(), 0.0f);
			mBackgroundVideo->play();
		}
		else if (mType == CUSTOM && mFirstLine && mSecondLine && mThirdLine)
		{
			mBackgroundVideo->tweenOpacity(1.0f, mGlobals.getAnimDur(), 0.0f);
			mBackgroundVideo->play();
			mBackgroundVideo->sendToBack();
			mFirstLine->tweenOpacity(1.0f, mGlobals.getAnimDur(), mStartTime);
			mSecondLine->tweenOpacity(1.0f, mGlobals.getAnimDur(), mStartTime);
			mThirdLine->tweenOpacity(1.0f, mGlobals.getAnimDur(), mStartTime);
			if (mEndTime > 0.0f)
			{
				callAfterDelay([this]()
				{
					mFirstLine->tweenOpacity(0.0f, mGlobals.getAnimDur());
					mSecondLine->tweenOpacity(0.0f, mGlobals.getAnimDur());
					mThirdLine->tweenOpacity(0.0f, mGlobals.getAnimDur());
				}, mEndTime);
			}
		}
	}

}
