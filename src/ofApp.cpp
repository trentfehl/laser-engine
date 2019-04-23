#include "ofApp.h"
#include <math.h>
#include <cmath>
#include <algorithm>
#include <stdlib.h>
#include <time.h>

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(40);

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
}

//--------------------------------------------------------------
void ofApp::update(){
    float deltaTime = ofClamp(ofGetLastFrameTime(), 0, 0.2);
    elapsedTime+=deltaTime;

    float bpm = bpmDetector.getBPM();

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
    
    if (!splineEval.isThreadRunning()) {
        splineEval.updateControlPoints();
        splineEval.startThread();
    }

    if (showPolygon) {
        ofPolyline polygon;
        for (SplineEval::Control c : splineEval.controls)
            polygon.addVertex(c.point[0], c.point[1], c.point[2]);
        polygon.close();
        polygon.scale(scale_x, scale_y);
        polygon.translate(origin);
        polylines.push_back({polygon, ofColor(0,255,0)});
    }

    if (showBoundary) {
        ofPolyline circle;
        circle.arc(ofPoint(0,0,0),1,1,0,360);
        circle.scale(scale_x, scale_y);
        circle.translate(origin);
        polylines.push_back({circle, ofColor(255,0,0)});
    }

    if (showSpline) {
        ofPolyline spline;

        splineEval.lock();
        for (SplineEval::Result r : splineEval.results)
            spline.addVertex(r.point[0], r.point[1], r.point[2]);
        splineEval.unlock();

        spline.close();
        spline.scale(scale_x, scale_y);
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
    if (key == 'p') {
        showPolygon = !showPolygon;
    }

    // Show boundary for the points.
    if (key == 'b') {
        showBoundary = !showBoundary;
    }

    // Show spline for the points.
    if (key == 's') {
        showSpline = !showSpline;
    }
} 

//--------------------------------------------------------------
void ofApp::audioReceived(float *input, int bufferSize, int nChannels){
    bpmDetector.processFrame(input, bufferSize, nChannels);
}

//--------------------------------------------------------------
void ofApp::exit(){ 
    laser.saveSettings();
    dac.close();
}
