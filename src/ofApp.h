#pragma once

#include "ofMain.h"
#include "ofxLaserManager.h"
#include "ofxLaserDacEtherdream.h"
#include "ofxGui.h"

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
	int rotation;
	ofVec3f rotation_vector;

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
