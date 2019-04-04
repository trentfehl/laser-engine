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

	void audioIn(ofSoundBuffer &inBuffer);
	void audioOut(ofSoundBuffer &outBuffer);

	ofxLaser::Manager laser;
	ofxLaser::DacEtherdream dac;

	std::vector<ofPolyline> polyLines;
	ofxPanel cgui;
	ofParameter<ofColor> color;

	ofSoundStreamSettings settings;
	ofxFft* fft;
	ofMutex soundMutex;
	vector<float> drawBins, middleBins, audioBins;

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
};
