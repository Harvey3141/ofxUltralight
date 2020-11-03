﻿#pragma once

#include <ofMain.h>
#include <Ultralight/Ultralight.h>
#include <JavaScriptCore/JavaScript.h>
#include <AppCore/Platform.h>



using namespace ultralight;

// Define our custom Logger class
class MyLogger : public Logger {
public:
	MyLogger() {}
	virtual ~MyLogger() {}

	///
	/// Called when the library wants to print a message to the log.
	///
	virtual void LogMessage(LogLevel log_level, const String16& message) override {
		printf("%s\n", String(message).utf8().data());
	}
};


class ofxUltralight {

public:
		void setup(int width, int height, string url);
		void setup(int width, int height, ofVec2f t_offset, string url);
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void mouseScrolled(int x, int y, float scrollX, float scrollY);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		Config config;
		shared_ptr<GPUDriver> gpu_driver;
		RefPtr<Renderer> renderer;
		RefPtr<View> view;

		ofVec2f offset;
		ofTexture oeTexture;
};