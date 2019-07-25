#pragma once

#include "ofMain.h"
#include "ofxLaserManager.h"
#include "ofxLaserDacEtherdream.h"
#include "ofxGui.h"
#include "ofxFft.h"

class ofApp : public ofBaseApp{

   public:
	bool keyIsDown[255];  
  
	void setup();
	void update();
	void draw();
	void exit();

	void keyPressed(int key);
	void keyReleased(int key);

	ofxLaser::Manager laser;
	ofxLaser::DacEtherdream dac;

	std::vector<ofPolyline> polyLines;
	ofxPanel cgui;
	ofParameter<ofColor> color;

	float elapsedTime; 
	int laserRadius;
	int laserWidth;
	int laserHeight;
	ofVec3f origin;

	int step;
	int x_inc;
	int y_inc;
	int z_inc;
	ofQuaternion quat;

	// Hypotrocoid
	void drawHypotrochoid();
	int r;
	int R;
	int d;
	int factor_r;

	// Rose
	void drawRose();
	int k;

	// Audio
	void audioIn(ofSoundBuffer &inBuffer)
	void audioOut(ofSoundBuffer &outBuffer)
	ofSoundStreamSettings settings;
	settings.sampleRate = (size_t)  sampleRate;
	settings.bufferSize = (size_t)  bufferSize;
	settings.numBuffers = (size_t)  nBuffers;
	settings.numOutputChannels = (size_t) outputChannels;
	settings.numInputChannels = (size_t)  inputChannels;

	ofxFft* fft;
	ofMutex soundMutex;
};
