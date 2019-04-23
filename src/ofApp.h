#pragma once

#include "ofMain.h"
#include "ofxLaserManager.h"
#include "ofxLaserDacEtherdream.h"
#include "ofxGui.h"
#include "ofxBPMDetector.h"
#include "splineEval.h"

class ofApp : public ofBaseApp{

   public:
	void setup();
	void update();
	void draw();
	void exit();

	void keyPressed(int key);

	ofxLaser::Manager laser;
	ofxLaser::DacEtherdream dac;

	ofParameter<ofColor> color;
	ofxPanel cgui;

	ofSoundStream soundStream;
	ofSoundStreamSettings settings;
	ofxBPMDetector bpmDetector;
	void audioReceived(float *input, int bufferSize, int nChannels);

	float elapsedTime; 
	int laserRadius;
	int laserDiameter;
	glm::vec3 origin;

	int step;
	ofQuaternion quat;

	bool showPolygon;
	bool showBoundary;
	bool showSpline;
	SplineEval splineEval;
};


