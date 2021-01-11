﻿#include "ofxUltralight.h"


void ofxUltralight::setup(int width, int height, string url) {
	setup(width, height, ofVec2f(0, 0), url);
	DOMready = false;
}

void ofxUltralight::setup(int width, int height, ofVec2f t_offset, string url) {
	//ofLogNotice(ofToDataPath("resources").c_str());
	offset = t_offset;

	config.resource_path = "../../../../addons/ofxUltralight/libs/resources";
	config.use_gpu_renderer = false;
	config.device_scale = 1.0;
	config.user_agent = "Mozilla/5.0 (Linux; Android 8.1.0; SM-G965F Build/OPM2.171019.029) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/7.2 Chrome/59.0.3071.125 Mobile Safari/537.36";

	auto& platform = Platform::instance();
	platform.set_font_loader(GetPlatformFontLoader());
	platform.set_config(config);
	platform.set_logger(new MyLogger());

	//gpu_driver = make_shared<GPUDriverGL>(1);
	//platform.set_gpu_driver(gpu_driver.get());

	renderer = Renderer::Create();
	view = renderer->CreateView(width, height, false, nullptr);
	ofLogVerbose("given string is '" + url + "'");
	string search = "http" +':' + '//';
	string search2 = "https" + ':' + '//';
	bool isURL = false;
	// there seem to be problem with colons, so some workaround. Feel free to fix it...
	if ( url.rfind("http", 0) == 0 && url.rfind("//", 6) ) {
		ofLogVerbose("starts with http:// ... set it as a URI");
		isURL = true;
	}
	if ( url.rfind("https", 0) == 0 && url.rfind("//", 7) ) {
		ofLogVerbose("starts with https:// ... set it as a URI");
		isURL = true;
	}

	if (isURL) {
		view->LoadURL(url.c_str());
	} else {
		ofLogVerbose("think it's a File - try to load it");
		ofBuffer buffer = ofBufferFromFile(url);
		string content;
		if (buffer.size()) {
			for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {

				content += *it;

			}
		}
		view->LoadHTML(content.c_str());
	}

	view->Focus();
	view->set_load_listener(this);
}

//--------------------------------------------------------------
void ofxUltralight::OnDOMReady(ultralight::View* caller, uint64_t frame_id, bool is_main_frame, const String& url) {
	ofLogVerbose( "######## DOM ready ##########" );
	DOMready = true;

	Ref<JSContext> locked_context = caller->LockJSContext();
	SetJSContext(locked_context.get());
	// do this directyl in you ofApp
	//JSObject global = JSGlobalObject();
	//global["callMessage"] = BindJSCallback(&ofxUltralight::Message);
}

//--------------------------------------------------------------
void ofxUltralight::update() {
	renderer->Update();
}

//--------------------------------------------------------------
void ofxUltralight::draw() {
	renderer->Render();

	///
/// Check if our Surface is dirty (pixels have changed).
///

		///
		/// Psuedo-code to upload Surface's bitmap to GPU texture.
		///

	///
	/// Cast it to a BitmapSurface.
	///
	BitmapSurface* bitmap_surface = (BitmapSurface*)(view->surface());
	if ( !bitmap_surface->dirty_bounds().IsEmpty()) {
		ofLogVerbose("dirty, so draw");

		///
	/// Get the underlying bitmap.
	///
		RefPtr<Bitmap> bitmap = bitmap_surface->bitmap();

		void* pixels = bitmap->LockPixels();

		/// Get the bitmap dimensions.
		uint32_t width = bitmap->width();
		uint32_t height = bitmap->height();
		uint32_t stride = bitmap->row_bytes();

		unsigned char* pixels2 = (unsigned char*)pixels;
		// swap R and B channels (does nnot work in ladData later, don't know why.)
		oeTexture.setSwizzle(GL_TEXTURE_SWIZZLE_R, GL_BLUE);
		oeTexture.setSwizzle(GL_TEXTURE_SWIZZLE_B, GL_RED);
		// load the pixels to ofTexture
		oeTexture.loadData(pixels2, width, height, GL_RGBA);

		/// Unlock the Bitmap when we are done.
		bitmap->UnlockPixels();

		/// Clear the dirty bounds.
		bitmap_surface->ClearDirtyBounds();

		// also do get JS stuff, as context may have changed

		  // Acquire the JS execution context for the current page.
		  //
		  // This call will lock the execution context for the current
		  // thread as long as the Ref<> is alive.
		  //
		
	}
	oeTexture.draw(offset.x, offset.y);


}

//--------------------------------------------------------------
void ofxUltralight::keyPressed(int key) {
	int text = key;
	// get the correct virtual key codes
	int vk = key;
	switch (key) {
		case OF_KEY_LEFT:
			vk = KeyCodes::GK_LEFT;
			break;
		case OF_KEY_RIGHT:
			vk = KeyCodes::GK_RIGHT;
			break;
		case OF_KEY_UP:
			vk = KeyCodes::GK_UP;
			break;
		case OF_KEY_DOWN:
			vk = KeyCodes::GK_DOWN;
			break;
		case OF_KEY_DEL:
			vk = KeyCodes::GK_DELETE;
			break;
		case OF_KEY_BACKSPACE:
			vk = KeyCodes::GK_BACK;
			break;
			// this is the 'dot' key. For some reason I had to hard-code this in order to make it work
		case 46:
			vk = KeyCodes::GK_DECIMAL;
			break;
		case -1:
			break;

		case OF_KEY_RETURN:
			vk = KeyCodes::GK_RETURN;
			return;
		case OF_KEY_LEFT_SHIFT:
			vk = KeyCodes::GK_LSHIFT;
			return;
		case OF_KEY_RIGHT_SHIFT:
			vk = KeyCodes::GK_RSHIFT;
			return;
		case OF_KEY_LEFT_CONTROL:
			vk = KeyCodes::GK_LCONTROL;
			return;
		case OF_KEY_RIGHT_CONTROL:
			vk = KeyCodes::GK_RCONTROL;
			return;
		case OF_KEY_LEFT_ALT:
			return;
		case OF_KEY_RIGHT_ALT:
			return;
		case OF_KEY_LEFT_COMMAND:
			return;
		case OF_KEY_RIGHT_COMMAND:
			return;
		default:
			break;
	}

	KeyEvent evt;
	evt.type = KeyEvent::kType_RawKeyDown;
	// You'll need to generate a key identifier from the virtual key code
	// when synthesizing events. This function is provided in KeyEvent.h
	evt.virtual_key_code = vk;
	evt.native_key_code = key;
	evt.text = ofToString( (char)text ).c_str();
	evt.unmodified_text = ofToString( (char)text ).c_str();
	//evt.modifiers = (char)OF_KEY_MODIFIER;
	GetKeyIdentifierFromVirtualKeyCode(evt.virtual_key_code, evt.key_identifier);
	view->FireKeyEvent(evt);

	KeyEvent evt2;
	evt2.type = KeyEvent::kType_Char;
	evt2.virtual_key_code = vk;
	evt2.native_key_code = key;
	evt2.text = ofToString( (char)text ).c_str();
	evt2.unmodified_text = ofToString( (char)text ).c_str();
	//evt2.modifiers = (char)OF_KEY_MODIFIER;
	evt2.unmodified_text = ofToString( (char)key ).c_str(); // If not available, set to same as evt.text

	view->FireKeyEvent(evt2);
}

//--------------------------------------------------------------
void ofxUltralight::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofxUltralight::mouseMoved(int x, int y) {
	MouseEvent evt;
	evt.type = MouseEvent::kType_MouseMoved;
	evt.x = x - offset.x;
	evt.y = y - offset.y;
	evt.button = MouseEvent::kButton_None;

	view->FireMouseEvent(evt);
}

//--------------------------------------------------------------
void ofxUltralight::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofxUltralight::mousePressed(int x, int y, int button) {
	//cout << "Klick Mouse at " << x - offset.x << " - " << y - offset.y ;
	MouseEvent evt;
	evt.type = MouseEvent::kType_MouseDown;
	evt.x = x - offset.x;
	evt.y = y - offset.y;
	evt.button = MouseEvent::kButton_Left;

	view->FireMouseEvent(evt);
}

//--------------------------------------------------------------
void ofxUltralight::mouseReleased(int x, int y, int button) {
	MouseEvent evt;
	evt.type = MouseEvent::kType_MouseUp;
	evt.x = x - offset.x;
	evt.y = y - offset.y;
	evt.button = MouseEvent::kButton_Left;

	view->FireMouseEvent(evt);
}

//--------------------------------------------------------------
void ofxUltralight::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofxUltralight::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofxUltralight::mouseScrolled(int x, int y, float scrollX, float scrollY) {
	ScrollEvent scroll_event;
	scroll_event.type = ScrollEvent::kType_ScrollByPixel;
	scroll_event.delta_x = scrollX * 30;
	scroll_event.delta_y = scrollY * 30;

	view->FireScrollEvent(scroll_event);
}

//--------------------------------------------------------------
void ofxUltralight::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofxUltralight::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofxUltralight::dragEvent(ofDragInfo dragInfo) {

}

//--------------------------------------------------------------
string ofxUltralight::getStringFromJSstr(JSString str) {
	auto length = JSStringGetLength(str);
	auto buffer = new char[length];
	JSStringGetUTF8CString(str, buffer, length);
	
	return (string)buffer;
}