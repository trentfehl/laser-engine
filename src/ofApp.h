#pragma once

#include "ofMain.h"
#include "ofxLaserManager.h"
#include "ofxLaserDacEtherdream.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

    public:
	void setup();
	void update();
	void draw();
	void exit();

	// Geometry
	void drawHypotrochoid();

	void keyPressed(int key);

	ofxLaser::Manager laser;
	ofxLaser::DacEtherdream dac;

	std::vector<ofPolyline> polyLines;
	ofxPanel cgui;
	ofParameter<ofColor> color;

	float elapsedTime; 
	int laserWidth;
	int laserHeight;

	int r;
	int R;
	int d;

	int factor_r;
};
