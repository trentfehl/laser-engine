// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
	void rotate();

	void keyPressed(int key);

	ofxLaser::Manager laser;
	ofxLaser::DacEtherdream dac;

	ofParameter<ofColor> color;
	ofxPanel cgui;

	ofSoundStream soundStream;
	ofSoundStreamSettings settings;
	ofxBPMDetector bpmDetector;
	void audioReceived(float *input, int bufferSize, int nChannels);
	float bpm;

	float elapsedTime; 
	int laserRadius;
	int laserDiameter;
	glm::vec3 origin;

	// Spline rotation.
	bool rotateSpline;
	bool showRotationAxis;
	float theta;
	int theta_step;
	int theta_duration;
	float phi;
	int phi_step;
	int phi_duration;
	float angle;
	glm::vec3 axis;
	ofQuaternion quat;

	bool showPolygon;
	bool showBoundary;
	bool showSpline;
	SplineEval splineEval;

	struct polyLine {
	    ofPolyline line;
	    ofColor line_color;
	};
	std::vector<polyLine> polylines;
};
