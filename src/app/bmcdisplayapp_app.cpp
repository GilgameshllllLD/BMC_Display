#include "bmcdisplayapp_app.h"

#include <Poco/String.h>
#include <ds/app/environment.h>
#include <ds/debug/logger.h>
#include <ds/app/engine/engine.h>

#include <ds/ui/media/media_viewer.h>

#include <cinder/Rand.h>

#include "app/app_defs.h"
#include "app/globals.h"

#include "events/app_events.h"

#include "ui/story/story_view.h"

namespace bmc {

	ds::RootList getRootList(ds::Engine& engine, const Globals::APPType type)
	{
		ds::RootList rooty = ds::RootList();

		ds::cfg::Settings layoutSettings;
		ds::Environment::loadSettings("layout.xml", layoutSettings);

		return rooty;
	}

	BMCDisplayApp::BMCDisplayApp()
		: inherited(ds::RootList([this]()->ds::RootList
	{
		ds::cfg::Settings layoutSettings;

		ds::Environment::loadSettings("layout.xml", layoutSettings);
		mType = (Globals::APPType)layoutSettings.getInt("APP:TYPE", 0);
		return getRootList(mEngine, mType);

	}).ortho())
		, mGlobals(mEngine, mAllData)
		, mQueryHandler(mEngine, mAllData, mGlobals, mType)
		, mIdling(false)
		, mTouchDebug(mEngine)
	{
		/*fonts in use */
		mEngine.editFonts().install(ds::Environment::getAppFile("data/fonts/NotoSans-Bold.ttf"), "noto-bold");
		mEngine.editFonts().install(ds::Environment::getAppFile("data/fonts/FreigSanProBold.otf"), "FreigSanPro-bold");
		mEngine.editFonts().install(ds::Environment::getAppFile("data/fonts/FreigSanProBook.otf"), "FreigSanPro-book");
		mEngine.editFonts().install(ds::Environment::getAppFile("data/fonts/FreigSanProLig.otf"), "FreigSanPro-lig");

		enableCommonKeystrokes(true);
	}

	void BMCDisplayApp::setupServer(){


		/* Settings */
		mEngine.loadSettings(SETTINGS_LAYOUT, "layout.xml");
		mEngine.loadTextCfg("text.xml");

		mGlobals.initialize();
		mQueryHandler.runInitialQueries();

		const int numRoots = mEngine.getRootCount();
		int numPlacemats = 0;
		for (int i = 0; i < numRoots - 1; i++){
			// don't clear the last root, which is the debug draw
			if (mEngine.getRootBuilder(i).mDebugDraw) continue;

			ds::ui::Sprite& rooty = mEngine.getRootSprite(i);
			if (rooty.getPerspective()){
				const float clippFar = 10000.0f;
				const float fov = 60.0f;
				ds::PerspCameraParams p = mEngine.getPerspectiveCamera(i);
				p.mTarget = ci::Vec3f(mEngine.getWorldWidth() / 2.0f, mEngine.getWorldHeight() / 2.0f, 0.0f);
				p.mFarPlane = clippFar;
				p.mFov = fov;
				p.mPosition = ci::Vec3f(mEngine.getWorldWidth() / 2.0f, mEngine.getWorldHeight() / 2.0f, mEngine.getWorldWidth() / 2.0f);
				mEngine.setPerspectiveCamera(i, p);
			}
			else {
				mEngine.setOrthoViewPlanes(i, -10000.0f, 10000.0f);
			}

			rooty.clearChildren();
		}

		ds::ui::Sprite &rootSprite = mEngine.getRootSprite();
		rootSprite.setTransparent(false);
		rootSprite.setColor(ci::Color::black());

		// add sprites
		rootSprite.addChildPtr(new StoryView(mGlobals));
	}

	void BMCDisplayApp::update() {
		inherited::update();

		mQueryHandler.update();

		if (mEngine.isIdling() && !mIdling){
			//Start idling
			mIdling = true;
			mEngine.getNotifier().notify(IdleStartedEvent());
		}
		else if (!mEngine.isIdling() && mIdling){
			//Stop idling
			mIdling = false;
			mEngine.getNotifier().notify(IdleEndedEvent());
		}

	}

	void BMCDisplayApp::keyDown(ci::app::KeyEvent event){
		using ci::app::KeyEvent;
		inherited::keyDown(event);
		if (event.getChar() == KeyEvent::KEY_r){ // R = reload all configs and start over without quitting app
			setupServer();

			// Shows all enabled sprites with a label for class type
		}
		else if (event.getCode() == KeyEvent::KEY_f){

			const int numRoots = mEngine.getRootCount();
			int numPlacemats = 0;
			for (int i = 0; i < numRoots - 1; i++){
				mEngine.getRootSprite(i).forEachChild([this](ds::ui::Sprite& sprite){
					if (sprite.isEnabled()){
						sprite.setTransparent(false);
						sprite.setColor(ci::Color(ci::randFloat(), ci::randFloat(), ci::randFloat()));
						sprite.setOpacity(0.95f);

						ds::ui::Text* labelly = mGlobals.getText("media_viewer:title").create(mEngine, &sprite);
						labelly->setText(typeid(sprite).name());
						labelly->enable(false);
						labelly->setColor(ci::Color::black());
					}
					else {

						ds::ui::Text* texty = dynamic_cast<ds::ui::Text*>(&sprite);
						if (!texty || (texty && texty->getColor() != ci::Color::black())) sprite.setTransparent(true);
					}
				}, true);
			}
		}
	}

	void BMCDisplayApp::mouseDown(ci::app::MouseEvent e) {
		mTouchDebug.mouseDown(e);
	}

	void BMCDisplayApp::mouseDrag(ci::app::MouseEvent e) {
		mTouchDebug.mouseDrag(e);
	}

	void BMCDisplayApp::mouseUp(ci::app::MouseEvent e) {
		mTouchDebug.mouseUp(e);
	}

	void BMCDisplayApp::fileDrop(ci::app::FileDropEvent event){
		std::vector<std::string> paths;
		for (auto it = event.getFiles().begin(); it < event.getFiles().end(); ++it){
			//paths.push_back((*it).string());

			ds::ui::MediaViewer* mv = new ds::ui::MediaViewer(mEngine, (*it).string(), true);
			mv->initialize();
			mEngine.getRootSprite().addChildPtr(mv);
		}
	}

} // namespace bmc

// This line tells Cinder to actually create the application
CINDER_APP_BASIC(bmc::BMCDisplayApp, ci::app::RendererGl(ci::app::RendererGl::AA_MSAA_4))