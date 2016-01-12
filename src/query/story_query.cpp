#include "story_query.h"
#include "model/generated/templatevideo_model.h"

#include <map>
#include <sstream>
#include <ds/debug/logger.h>
#include <ds/query/query_client.h>

#include <ds/app/environment.h>

namespace bmc {

/**
 * \class bmc::StoryQuery
 */
StoryQuery::StoryQuery() {
}

void StoryQuery::run() {
	mOutput.mStories.clear();
	try {
		query(mOutput);
	} catch (std::exception const&) {
	}
}

void StoryQuery::query(AllStories& output) {

	const ds::Resource::Id				cms(ds::Resource::Id::CMS_TYPE, 0);
	std::stringstream					buf;
	ds::query::Result					r;
	//-----------------------0------------------------1----------2-----------3----------4----------5----------6-------------7-----
	buf << "SELECT playlistelement.id, playlistelement.name,elementtypeid,resourceid,textline1,textline2,textline3,templatevideoid FROM playlistelement  INNER JOIN playlist ON playlist.playlistid = playlistelement.playlistid and playlist.playlistid = 1";
	if (!ds::query::Client::query(cms.getDatabasePath(), buf.str(), r, ds::query::Client::INCLUDE_COLUMN_NAMES_F)){
		DS_LOG_WARNING(" error querying playlist");
	}
	if (r.rowsAreEmpty()){
		DS_LOG_WARNING("No rows returned querying playlist.");
		return;
	}

	std::vector<ds::model::StoryRef> storyGroup;
	ds::query::Result::RowIterator it(r);
	while (it.hasValue())
	{
		ds::model::StoryRef singleStory;
		singleStory.setId(it.getInt(0));
		singleStory.setResourceId(it.getInt(3));
		singleStory.setFirstLine(it.getWString(4));
		singleStory.setSecondLine(it.getWString(5));
		singleStory.setThirdLine(it.getWString(6));
		singleStory.setTemplateVideoId(it.getInt(7));

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

		++it;
		mOutput.mStories.push_back(singleStory);
	}
	auto i = 0;
}

} // !namespace bmc
