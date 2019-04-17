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
    laserWidth = 800;
    laserHeight = 800;
    laser.setup(laserWidth, laserHeight);

    laserRadius = std::min(laserWidth/2, laserHeight/2);
    origin.set(laserWidth/2, laserHeight/2, 0);

    // Setup DAC
    laser.addProjector(dac);
    string dacIP = "192.168.1.234";
    dac.setup(dacIP);
	
    // Setup GUI
    laser.initGui();
    cgui.setup("color panel", "colors.xml", ofGetWidth()-240, 700 );
    cgui.add(color.set("color", ofColor(190, 0, 190), ofColor(0), ofColor(255)));

    setupSound();
    setupControlPoints();
    setupParameters();
}

//--------------------------------------------------------------
void ofApp::update(){
    float deltaTime = ofClamp(ofGetLastFrameTime(), 0, 0.2);
    elapsedTime+=deltaTime;

    //lets scale the vol up to a 0-1 range 
    scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);

    //lets record the volume into an array
    volHistory.push_back(scaledVol);

    //if we are bigger the the size we want to record - lets drop the oldest value
    if(volHistory.size() >= 400)
        volHistory.erase(volHistory.begin(), volHistory.begin()+1);

    // prepares laser manager to receive new points
    laser.update();
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofBackground(0);
    ofNoFill();
    ofSetLineWidth(1);
    ofDrawRectangle(0,0,laserWidth, laserHeight);
    
    updateControlPoints();

    // Show polygon of control points.
    ofPolyline line;
    for (auto point : points)
        line.addVertex(point.p[0], point.p[1], point.p[2]);
    line.close();
    line.scale(laserRadius*0.25, laserRadius*0.25);
    line.translate(origin);
    laser.drawPoly(line, color);

    // Show container for the points.
    ofPolyline circle;
    circle.arc(ofPoint(0,0,0),1,1,0,360);
    circle.scale(laserRadius*0.25, laserRadius*0.25);
    circle.translate(origin);
    laser.drawPoly(circle, ofColor(255,0,0));

    // Sends points to the DAC
    laser.send();

    laser.drawUI();
    cgui.draw();
}

//--------------------------------------------------------------
void ofApp::updateControlPoints(){

    for (std::size_t i=0, max=points.size(); i!=max; i++) {

        float step_size = 0.01;
        float heading_offset = (rand() % 10 + 1) * 0.001;

        // If point reaches edge, reflect it.
        if (glm::length(points[i].p) >= 1) { 
            step_size = 0.07;

            glm::vec3 normal = {
                points[i].p[0],
                points[i].p[1],
                points[i].p[2],
            };

            points[i].h = glm::reflect(points[i].h, normal);

            // if (points[i].h[0] > 0) {
            //     points[i].direction = -1;
            // } 
            // else if (points[i].h[0] < 0) {
            //     points[i].direction = 1;
            // }
        }

        float step = points[i].direction * step_size;
        points[i].p[0] += glm::dot(points[i].h, {1 - heading_offset,0,0}) * step;
        points[i].p[1] += glm::dot(points[i].h, {0,1 - heading_offset,0}) * step;
        points[i].p[2] += glm::dot(points[i].h, {0,0,1 - heading_offset}) * step;
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    keyIsDown[key] = true;  
} 

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {  
    keyIsDown[key] = false;  
}  

//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer &inBuffer) {
    float curVol = 0.0;
    int numCounted = 0;	

    // Samples are "interleaved".
    for (size_t i = 0; i < inBuffer.getNumFrames(); i++){
	    left[i]	= inBuffer[i*2]*0.5;
	    right[i]	= inBuffer[i*2+1]*0.5;

	    curVol += left[i] * left[i];
	    curVol += right[i] * right[i];
	    numCounted+=2;
    }

    // calculate RMS
    curVol /= (float)numCounted;
    curVol = sqrt( curVol );

    smoothedVol *= 0.93;
    smoothedVol += 0.07 * curVol;

    bufferCounter++;
}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer &outBuffer) {
	// Stuff
}

//--------------------------------------------------------------
void ofApp::setupSound() {
    soundStream.printDeviceList();

    int bufferSize = 256;

    left.assign(bufferSize, 0.0);
    right.assign(bufferSize, 0.0);
    volHistory.assign(400, 0.0);

    bufferCounter = 0;
    drawCounter	= 0;
    smoothedVol = 0.0;
    scaledVol = 0.0;

    ofSoundStreamSettings settings;

    auto devices = soundStream.getMatchingDevices("default");
    if(!devices.empty()){
	settings.setInDevice(devices[0]);
    }

    settings.setInListener(this);
    settings.sampleRate = 44100;
    settings.numOutputChannels = 2;
    settings.numInputChannels = 2;
    settings.bufferSize = bufferSize;
    soundStream.setup(settings);
}

//--------------------------------------------------------------
void ofApp::setupControlPoints(){ 
    // Set number of control points.
    points.resize(5);

    // Initialize random seed.
    srand (time(NULL));

    // Initialize points randomly inside unit circle.
    for (std::size_t i=0, max=points.size(); i!=max; i++) {
        // Initialize unit vector position.
        points[i].p = {
            rand() % 99 + 0,
            rand() % 99 + 0,
            rand() % 99 + 0,
        };
        points[i].p = glm::normalize(points[i].p);

        // Initialize unit vector heading.
        points[i].h = {
            rand() % 99 + 0,
            rand() % 99 + 0,
            rand() % 99 + 0,
        };
        points[i].h = glm::normalize(points[i].h);

        points[i].direction = 1;
    }
}

//--------------------------------------------------------------
void ofApp::setupParameters(){
   // With equal_length method and fixed lengths between knots, only
   // solve for parameters once.
   // http://demonstrations.wolfram.com/GlobalBSplineCurveInterpolation/

   float d = 0.0f;
   t.resize(points.size());

   for (int i=1; i<points.size(); i++) {
	t[i] = glm::length(points[i].p);
        d += t[i];
   }

   t[0] = 0.0f;
   t[1] = 1.0f;

   for (int i=1; i<(t.size()-1); i++) {
       t[i] = t[i-1] + t[i]/d;
   }

   p = 3; // Power.

   m = pow(points.size(), 2);
   u.resize(m);

   for (int j=0; j<=p; j++) {
       u[j] = 0.0f;
   }
   for (int j=(p+1); j<(m-p); j++) {
       for (int i=0; i<(j-p); i++) {
           u[j] += t[i];
       }
       u[j] /= p;
   }
   for (int j=(m-p); j<m; j++) {
       u[j] = 1.0f;
   }
}

//--------------------------------------------------------------
void ofApp::exit(){ 
    laser.saveSettings();
    dac.close();
}
