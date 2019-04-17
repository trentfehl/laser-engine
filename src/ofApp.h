#pragma once

#include "ofMain.h"
#include "ofxLaserManager.h"
#include "ofxLaserDacEtherdream.h"
#include "ofxGui.h"
#include "ofxEigen.h"
#include "ofxBPMDetector.h"

class ofApp : public ofBaseApp{

   public:
	void setup();
	void update();
	void draw();
	void exit();
	void setupParameters();
	void setupControlPoints();
	void updateControlPoints();

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
	struct ControlPoint {
	    glm::vec3 p; // Position.
	    glm::vec3 h; // Heading.
	    int direction;
	};
	vector<ControlPoint> points;
	vector<float> t;
	vector<float> u;
	int m; // Square of points.size()
	int p; // Power.
};
