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

#include "ofApp.h"
#include <math.h>
#include <cmath>
#include <algorithm>
#include <stdlib.h>
#include <time.h>

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(30);

    // Size window.
    laserRadius = 400;
    laserDiameter = laserRadius*2;
    laser.setup(laserDiameter, laserDiameter);
    origin = {laserRadius, laserRadius, 0};

    // Setup DAC.
    laser.addProjector(dac);
    string dacIP = "192.168.1.234";
    dac.setup(dacIP);

    // Setup sound.
    auto devices = soundStream.getMatchingDevices("default");
    if(!devices.empty()){
	settings.setInDevice(devices[0]);
    }

    settings.setInListener(this);
    settings.sampleRate = 44100;
    settings.numOutputChannels = 2;
    settings.numInputChannels = 2;
    settings.bufferSize = 256;
    soundStream.setup(settings);

    laser.initGui();
    cgui.setup("color panel", "colors.xml", ofGetWidth()-240, 700);
    cgui.add(color.set("color", ofColor(190, 0, 190), ofColor(0), ofColor(255)));

    splineEval.setupControlPoints();
    showSpline = true;
    rotateSpline = true;

    theta = rand() % 360 + 0;
    phi = rand() % 360 + 0;
    axis = glm::vec3(0, 0, 1);
}

//--------------------------------------------------------------
void ofApp::update(){
    float deltaTime = ofClamp(ofGetLastFrameTime(), 0, 0.2);
    elapsedTime+=deltaTime;

    bpm = bpmDetector.getBPM();

    // prepares laser manager to receive new points
    laser.update();
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofBackground(0);
    ofNoFill();
    ofSetLineWidth(1);
    ofDrawRectangle(0, 0, laserDiameter+240, laserDiameter);

    float scale_x = laserRadius*0.35;
    float scale_y = laserRadius*0.35;
    float scale_z = laserRadius*0.35;
    
    if (!splineEval.isThreadRunning()) {
        splineEval.updateControlPoints();
        splineEval.startThread();
    }

    if (rotateSpline) rotate();

    if (showPolygon) {
        ofPolyline polygon;
        for (SplineEval::Control c : splineEval.controls)
            polygon.addVertex(c.point[0], c.point[1], c.point[2]);
        polygon.close();
        polygon.scale(scale_x, scale_y, scale_z);
        polygon.rotateQuat(quat);
        polygon.translate(origin);
        polylines.push_back({polygon, ofColor(0,255,0)});
    }

    if (showBoundary) {
        ofPolyline circle;
        circle.arc(ofPoint(0,0,0),1,1,0,360,100);
        circle.scale(scale_x, scale_y, scale_z);
        circle.rotateQuat(quat);
        circle.translate(origin);
        polylines.push_back({circle, ofColor(255,0,0)});
    }

    if (showRotationAxis) {
        ofPolyline line;
        line.addVertex(ofPoint(0,0,0));
        line.addVertex(axis);
        line.scale(scale_x, scale_y, scale_z);
        line.rotateQuat(quat);
        line.translate(origin);
        polylines.push_back({line, ofColor(0,0,255)});
    }

    if (showSpline) {
        ofPolyline spline;

        splineEval.lock();
        for (SplineEval::Result r : splineEval.results)
            spline.addVertex(r.point[0], r.point[1], r.point[2]);
        splineEval.unlock();

        spline.close();
        spline.scale(scale_x, scale_y, scale_z);
        spline.rotateQuat(quat);
        spline.translate(origin);
        polylines.push_back({spline, color});
    }

    // Laser polylines.
    for(size_t i = 0; i<polylines.size(); i++) {
	    laser.drawPoly(polylines[i].line, polylines[i].line_color);
    }
    polylines.clear();

    // Sends points to the DAC
    laser.send();

    laser.drawUI();
    cgui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    // Show polygon of control points.
    if (key == 'p') showPolygon = !showPolygon;

    // Show boundary for the points.
    if (key == 'b') showBoundary = !showBoundary;

    // Show spline.
    if (key == 's') showSpline = !showSpline;

    // Rotate spline.
    if (key == 'r') rotateSpline = !rotateSpline;

    // Show rotational axis.
    if (key == 'a') showRotationAxis = !showRotationAxis;
} 

//--------------------------------------------------------------
void ofApp::audioReceived(float *input, int bufferSize, int nChannels){
    bpmDetector.processFrame(input, bufferSize, nChannels);
}

//--------------------------------------------------------------
void ofApp::rotate(){ 
    // Axis drift.
    if (theta_duration == 0) {
        theta_step = rand() % 21 + (-10);
        theta_duration = rand() % 200 + 60;
    }
    if (phi_duration == 0) {
        phi_step = rand() % 21 + (-10);
        phi_duration = rand() % 600 + 100;
    }

    theta += (theta_step * 0.005);
    phi += (phi_step * 0.005);

    axis[0] = sin(theta*M_PI/180)*cos(phi*M_PI/180);
    axis[1] = sin(theta*M_PI/180)*sin(phi*M_PI/180);
    axis[2] = cos(theta*M_PI/180);

    // Spline rotation about axis.
    quat[0] = axis[0] * sin(angle/2.0*M_PI/180);
    quat[1] = axis[1] * sin(angle/2.0*M_PI/180);
    quat[2] = axis[2] * sin(angle/2.0*M_PI/180);
    quat[3] = cos(angle/2.0*M_PI/180);

    angle += 0.1;
    angle = fmod(angle, 360);

    --phi_duration;
    --theta_duration;
}

//--------------------------------------------------------------
void ofApp::exit(){ 
    laser.saveSettings();
    dac.close();
}
