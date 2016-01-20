#include "story_query.h"
#include "model/generated/templatevideo_model.h"
#include <Poco/LocalDateTime.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTimeParser.h>
#include <map>
#include <sstream>
#include <ds/debug/logger.h>
#include <ds/query/query_client.h>

#include <ds/app/environment.h>

namespace bmc {

	/**
	 * \class bmc::StoryQuery
	 */
	StoryQuery::StoryQuery(Globals::APPType  type)
		:mType(type)
		, mQueryType(REGULAR)
		, mPreviewId(-1)
	{
	}

	void StoryQuery::run() {
		mOutput.mStorieLists.clear();
		try {
			if (mQueryType == REGULAR)
				query(mOutput);
			else
				prelistQuery(mPreviewId);
		}
		catch (std::exception const&) {
		}
	}

	void StoryQuery::query(AllStories& output) {

		const ds::Resource::Id				cms(ds::Resource::Id::CMS_TYPE, 0);
		std::stringstream					buf;
		ds::query::Result					r;
		std::vector<ds::model::StoryListRef>
			listGroup;

		buf.str("");
		buf << "Select playlistid from playlist where isdefault = 1 and typeid =" << int(mType);

		if (!ds::query::Client::query(cms.getDatabasePath(), buf.str(), r, ds::query::Client::INCLUDE_COLUMN_NAMES_F)){
			DS_LOG_WARNING(" error querying default playlist");
		}
		if (r.rowsAreEmpty()){
			DS_LOG_WARNING("No rows returned querying default playlist.");
			return;
		}
		ds::query::Result::RowIterator it(r);
		while (it.hasValue())
		{
			ds::model::StoryListRef singleList;
			singleList.setId(it.getInt(0));
			singleList.setListStartTime(L"0");
			singleList.setListEndTime(L"0");
			listGroup.push_back(singleList);
			++it;
		}

		std::stringstream tempStirng;
		//load engagement play items
		if (mType == Globals::APPType::WELCOME)
			tempStirng.str("welcomeplaylistid");
		else if (mType == Globals::APPType::RIO)
			tempStirng.str("riograndeplaylistid");
		else if (mType == Globals::APPType::TRANSFORMATION)
			tempStirng.str("transformationplaylistid");

		buf.str("");
		ds::query::Result				listR;
		auto localTime = Poco::LocalDateTime();
		auto msg = Poco::DateTimeFormatter::format(localTime, "%Y-%m-%d %H:%M:%S");
		buf << "SELECT engagementid, startdate,enddate," << tempStirng.str() << " From engagement where " << tempStirng.str() << " >0" << " and '" << msg << "' < enddate";
		if (!ds::query::Client::query(cms.getDatabasePath(), buf.str(), listR, ds::query::Client::INCLUDE_COLUMN_NAMES_F)){
			DS_LOG_WARNING(" error querying engagement list");
		}
		if (listR.rowsAreEmpty()){
			DS_LOG_WARNING("No rows returned querying engagement list.");
		}
		ds::query::Result::RowIterator listIt(listR);
		while (listIt.hasValue())
		{
			if (listIt.getInt(3) == listGroup[0].getId())
			{
				++listIt;
				continue;
			}

			ds::model::StoryListRef singleList;
			singleList.setListStartTime(collectNumbers(listIt.getString(1)));
			singleList.setListEndTime(collectNumbers(listIt.getString(2)));
			singleList.setId(listIt.getInt(3));
			listGroup.push_back(singleList);
			++listIt;
		}

		if (listGroup.size() == 0)
			return;

		for (auto i = 0; i < listGroup.size(); i++)
		{
			buf.str("");
			buf << "SELECT id,resourceid,textline1,textline2,textline3,templatevideoid FROM playlistelement where playlistid = " << listGroup[i].getId() << " ORDER by sort_order";
			ds::query::Result					itemR;
			if (!ds::query::Client::query(cms.getDatabasePath(), buf.str(), itemR, ds::query::Client::INCLUDE_COLUMN_NAMES_F)){
				DS_LOG_WARNING(" error querying playlist item");
			}
			if (itemR.rowsAreEmpty()){
				DS_LOG_WARNING("No rows returned querying playlist item.");
				return;
			}

			std::vector<ds::model::StoryRef> storyGroup;
			ds::query::Result::RowIterator itemIt(itemR);
			while (itemIt.hasValue())
			{
				ds::model::StoryRef singleStory;
				singleStory.setId(itemIt.getInt(0));
				singleStory.setResourceId(itemIt.getInt(1));
				singleStory.setFirstLine(itemIt.getWString(2));
				singleStory.setSecondLine(itemIt.getWString(3));
				singleStory.setThirdLine(itemIt.getWString(4));
				singleStory.setTemplateVideoId(itemIt.getInt(5));

				if (singleStory.getTemplateVideoId() > 0)
				{
					buf.str("");
					ds::query::Result			videoR;
					buf << "select  id,resourceid, timecodestartoverlay,timecodeendoverlay from templatevideo where id =" << singleStory.getTemplateVideoId();
					if (!ds::query::Client::query(cms.getDatabasePath(), buf.str(), videoR, ds::query::Client::INCLUDE_COLUMN_NAMES_F)){
						DS_LOG_WARNING(" error querying Template Video");
					}
					if (videoR.rowsAreEmpty()){
						DS_LOG_WARNING("No rows returned querying Template Video.");
					}
					ds::query::Result::RowIterator videoIt(videoR);
					ds::model::TemplateVideoRef templateVideo;
					while (videoIt.hasValue())
					{
						templateVideo.setId(videoIt.getInt(0));
						templateVideo.setResourceId(videoIt.getInt(1));
						templateVideo.setStartTime(videoIt.getInt(2));
						templateVideo.setEndTime(videoIt.getInt(3));
						++videoIt;
					}
					singleStory.setTemplateVideoRef(templateVideo);
				}
				storyGroup.push_back(singleStory);
				++itemIt;
			}
			listGroup[i].setStoryRef(storyGroup);
			mOutput.mStorieLists.push_back(listGroup[i]);
		}
	}

	std::wstring StoryQuery::collectNumbers(std::string text)
	{
		std::wstringstream finalStr;
		finalStr.str(L"");
		auto length = text.length();
		auto i = 0;
		while (i < length)
		{
			if (isdigit(text[i]))
			{
				finalStr << text[i];
			}
			i++;
		}

		return finalStr.str();
	}

	void StoryQuery::prelistQuery(int playlistId)
	{
		if (playlistId == -1)
			return;

		std::stringstream tempStirng;
		//load engagement play items
		if (mType == Globals::APPType::WELCOME)
			tempStirng.str("welcomeplaylistid");
		else if (mType == Globals::APPType::RIO)
			tempStirng.str("riograndeplaylistid");
		else if (mType == Globals::APPType::TRANSFORMATION)
			tempStirng.str("transformationplaylistid");

		ds::model::StoryListRef singlePlayList;
		singlePlayList.setId(playlistId);

		const ds::Resource::Id				cms(ds::Resource::Id::CMS_TYPE, 0);
		std::stringstream					buf;
		buf.str("");
		buf << "SELECT id,resourceid,textline1,textline2,textline3,templatevideoid FROM playlistelement where playlistid = " << singlePlayList.getId() << " ORDER by sort_order";
		ds::query::Result					itemR;
		if (!ds::query::Client::query(cms.getDatabasePath(), buf.str(), itemR, ds::query::Client::INCLUDE_COLUMN_NAMES_F)){
			DS_LOG_WARNING(" error querying preview playlist item");
		}
		if (itemR.rowsAreEmpty()){
			DS_LOG_WARNING("No rows returned querying preview playlist item.");
			return;
		}

		std::vector<ds::model::StoryRef> storyGroup;
		ds::query::Result::RowIterator itemIt(itemR);
		while (itemIt.hasValue())
		{
			ds::model::StoryRef singleStory;
			singleStory.setId(itemIt.getInt(0));
			singleStory.setResourceId(itemIt.getInt(1));
			singleStory.setFirstLine(itemIt.getWString(2));
			singleStory.setSecondLine(itemIt.getWString(3));
			singleStory.setThirdLine(itemIt.getWString(4));
			singleStory.setTemplateVideoId(itemIt.getInt(5));

			if (singleStory.getTemplateVideoId() > 0)
			{
				buf.str("");
				ds::query::Result			videoR;
				buf << "select  id,resourceid, timecodestartoverlay,timecodeendoverlay from templatevideo where id =" << singleStory.getTemplateVideoId();
				if (!ds::query::Client::query(cms.getDatabasePath(), buf.str(), videoR, ds::query::Client::INCLUDE_COLUMN_NAMES_F)){
					DS_LOG_WARNING(" error querying Template Video");
				}
				if (videoR.rowsAreEmpty()){
					DS_LOG_WARNING("No rows returned querying Template Video.");
				}
				ds::query::Result::RowIterator videoIt(videoR);
				ds::model::TemplateVideoRef templateVideo;
				while (videoIt.hasValue())
				{
					templateVideo.setId(videoIt.getInt(0));
					templateVideo.setResourceId(videoIt.getInt(1));
					templateVideo.setStartTime(videoIt.getInt(2));
					templateVideo.setEndTime(videoIt.getInt(3));
					++videoIt;
				}
				singleStory.setTemplateVideoRef(templateVideo);
			}
			storyGroup.push_back(singleStory);
			++itemIt;
		}
		singlePlayList.setStoryRef(storyGroup);
		mOutput.mPreviewLists = singlePlayList;

	}

} // !namespace bmc
