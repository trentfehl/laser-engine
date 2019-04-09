#include "ofApp.h"
#include <math.h>
#include <cmath>
#include <algorithm>

//--------------------------------------------------------------
void ofApp::setup(){
    // Setup sound.
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

    // Rendering.
    step = 10; // * M_PI
    quat.set(0, 0, 0, 1);
    factor_r = 2;
    R = round(laserRadius*0.80);
    d = round(laserRadius*0.80);
    k = 5;
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

    if(keyIsDown['c']) {
	polyLines.clear();
    }

    // Rotations
    if(keyIsDown['x'] && keyIsDown[OF_KEY_UP]) {
        x_inc++;
        x_inc = min(x_inc, 100);
	quat.set(x_inc*0.01, 0, 0, 1);
    }
    if(keyIsDown['x'] && keyIsDown[OF_KEY_DOWN]) {
        x_inc--;
        x_inc = max(x_inc, 0);
	quat.set(x_inc*0.01, 0, 0, 1);
    }

    if(keyIsDown['y'] && keyIsDown[OF_KEY_UP]) {
        y_inc++;
        y_inc = min(y_inc, 100);
	quat.set(0, y_inc*0.01, 0, 1);
    }
    if(keyIsDown['y'] && keyIsDown[OF_KEY_DOWN]) {
        y_inc--;
        y_inc = max(y_inc, 0);
	quat.set(0, y_inc*0.01, 0, 1);
    }

    if(keyIsDown['z'] && keyIsDown[OF_KEY_UP]) {
        z_inc++;
        z_inc = min(z_inc, 100);
	quat.set(0, 0, z_inc*0.01, 1);
    }
    if(keyIsDown['z'] && keyIsDown[OF_KEY_DOWN]) {
        z_inc--;
        z_inc = max(z_inc, 0);
	quat.set(0, 0, z_inc*0.01, 1);
    }

    // Modify step size
    if(keyIsDown['s'] && keyIsDown[OF_KEY_UP]) {  
        step++;
        step = min(step, 100);
        std::cout << "step: " << step << std::endl;
    }  
    if(keyIsDown['s'] && keyIsDown[OF_KEY_DOWN]) {  
        step--;
        step = max(step, 1);
        std::cout << "step: " << step << std::endl;
    }  

    // Modify 'k' of Rose
    if(keyIsDown['k'] && keyIsDown[OF_KEY_UP]) {  
        k++;
        k = min(k, 100);
        std::cout << "k: " << k << std::endl;
    }  
    if(keyIsDown['k'] && keyIsDown[OF_KEY_DOWN]) {  
        k--;
        k = max(k, 1);
        std::cout << "k: " << k << std::endl;
    }  

    // Modify 'factor_r' of Hypotrochoid
    if(keyIsDown['r'] && keyIsDown[OF_KEY_UP]) {  
        factor_r++;
        factor_r = min(factor_r, 800);
        std::cout << "factor_r: " << factor_r << std::endl;
    }  
    if(keyIsDown['r'] && keyIsDown[OF_KEY_DOWN]) {  
        factor_r--;
        factor_r = max(factor_r, 1);
        std::cout << "factor_r: " << factor_r << std::endl;
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
    
    // drawHypotrochoid();
    drawRose();

    // sends points to the DAC
    laser.send();

    laser.drawUI();
    cgui.draw();
}

//--------------------------------------------------------------
void ofApp::drawHypotrochoid(){
    ofPolyline line;

    r = round(factor_r*R*0.1);

    float i = 0;
    while (i < 2*M_PI*(r/__gcd(r,R))) { 
	float x = (R - r)*cos(i) + d*cos((R-r)*i/r);
	float y = (R - r)*sin(i) + d*sin((R-r)*i/r);
	line.addVertex(ofVec3f(x,y,0));
	i+=step*0.001*M_PI;
    }
    line.close(); // close the shape
    line.scale(scaledVol, scaledVol);
    line.rotateQuat(quat);
    line.translate(origin);

    laser.drawPoly(line, color);
}

//--------------------------------------------------------------
void ofApp::drawRose(){
    ofPolyline line;
    ofVec3f vec;

    float i = 0;
    while (i < 2*M_PI) { 
	float x = cos(k*i)*cos(i)*laserRadius;
	float y = cos(k*i)*sin(i)*laserRadius;
	line.addVertex(ofVec3f(x,y,0));
	i+=step*0.001*M_PI;
    }
    line.close(); // close the shape
    line.scale(scaledVol, scaledVol);
    line.rotateQuat(quat);
    line.translate(origin);

    laser.drawPoly(line, color);
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
void ofApp::exit(){ 
    laser.saveSettings();
    dac.close();
}
