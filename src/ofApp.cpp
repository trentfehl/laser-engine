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
    if( volHistory.size() >= 400 ){
	    volHistory.erase(volHistory.begin(), volHistory.begin()+1);
    }

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

    ofPolyline line;

    for (auto point : points) {
        line.addVertex(point.x, point.y, 0);
    }

    line.close();
    line.scale(laserRadius*0.25, laserRadius*0.25);
    line.translate(origin);
    laser.drawPoly(line, color);

    ofPolyline circle;
    circle.arc(ofPoint(0,0,0),1,1,0,360);
    circle.scale(laserRadius*0.25, laserRadius*0.25);
    circle.translate(origin);
    laser.drawPoly(circle, ofColor(255,0,0));

    // sends points to the DAC
    laser.send();

    laser.drawUI();
    cgui.draw();
}

//--------------------------------------------------------------
void ofApp::updateControlPoints(){

    for (std::size_t i=0, max=points.size(); i!=max; i++) {

        float point_step = 0.01;

        float dist = sqrt(pow(points[i].x, 2) + pow(points[i].y, 2));

        if (dist >= 1) { 
            point_step = 0.07;

            ofVec3f normal = {
                points[i].x,
                points[i].y,
                0,
            };

            ofVec3f para_to_tangent = normal.dot(points[i].h) * normal;
            ofVec3f perp_to_tangent = points[i].h - para_to_tangent;
                
            // Invert parallel component for reflection.
            points[i].h = perp_to_tangent - para_to_tangent;

            if (points[i].h[0] > 0) {
                points[i].direction = -1;
                points[i].angle = atan(points[i].h[1]/points[i].h[0]) * 180 / M_PI;
            } 
            else if (points[i].h[0] < 0) {
                points[i].direction = 1;
                points[i].angle = atan(points[i].h[1]/points[i].h[0]) * 180 / M_PI;
            } else {
                // Don't calculate angle if X component is zero.
                continue; 
            }

            points[i].angle += rand() % 2 + 0;
        }

        points[i].x += cos(points[i].angle*M_PI/180) * point_step * points[i].direction;
        points[i].y += sin(points[i].angle*M_PI/180) * point_step * points[i].direction;
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

    // samples are "interleaved"
    int numCounted = 0;	

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
        float r = (rand() % 99 + 0)*0.01;
        float theta = rand() % 360 + 0;
        points[i].x = r*cos(theta*M_PI/180); 
        points[i].y = r*sin(theta*M_PI/180); 

        // Initialize heading.
        points[i].h = {
            cos(rand() % 360 + 0),
            sin(rand() % 360 + 0),
            0,
        };

        if (points[i].h[1]*points[i].h[0] > 0) {
            points[i].direction = 1;
            points[i].angle = atan(points[i].h[1]/points[i].h[0]) * 180 / M_PI;
        } 
        if (points[i].h[1]*points[i].h[0] < 0) {
            points[i].direction = -1;
            points[i].angle = atan(points[i].h[1]/points[i].h[0]) * 180 / M_PI;
        }
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
	t[i] = sqrt(pow(points[i].x - points[i-1].x, 2) +  
                    pow(points[i].y - points[i-1].y, 2));
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
