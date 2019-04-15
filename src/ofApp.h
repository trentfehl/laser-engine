#pragma once

#include "ofMain.h"
#include "ofxLaserManager.h"
#include "ofxLaserDacEtherdream.h"
#include "ofxGui.h"
#include "ofxEigen.h"

class ofApp : public ofBaseApp{

   public:
	bool keyIsDown[255];  
  
	void setup();
	void update();
	void draw();
	void exit();
	void setupSound();
	void setupParameters();
	void setupControlPoints();
	void updateControlPoints();

	void keyPressed(int key);
	void keyReleased(int key);

	void audioIn(ofSoundBuffer &inBuffer);
	void audioOut(ofSoundBuffer &outBuffer);

	ofxLaser::Manager laser;
	ofxLaser::DacEtherdream dac;

	std::vector<ofPolyline> polyLines;
	ofParameter<ofColor> color;
	ofxPanel cgui;

	ofSoundStream soundStream;
	ofSoundStreamSettings settings;

	vector<float> left;
	vector<float> right;
	vector<float> volHistory;
	
	int bufferCounter;
	int drawCounter;
	
	float smoothedVol;
	float scaledVol;

	float elapsedTime; 
	int laserWidth;
	int laserHeight;
	int laserRadius;
	ofVec3f origin;

	int step;
	ofQuaternion quat;

	struct ControlPoint {
	    float x;
	    float y;
	    float angle;
	    float direction;
	    ofVec3f h; // Heading.
	};
	vector<ControlPoint> points;
	vector<float> t;
	vector<float> u;
	int m; // Square of points.size()
	int p; // Power.
};
