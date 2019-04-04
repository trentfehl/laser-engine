#include "ofApp.h"
#include <math.h>
#include <cmath>
#include <algorithm>

//--------------------------------------------------------------
void ofApp::setup(){
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
    rotation = 0; // Radians
    rotation_vector.set(0, 0, 1);
    factor_r = 2;
    R = round(laserRadius*0.80);
    d = round(laserRadius*0.80);
    k = 5;

    settings.sampleRate = 44100;
    settings.bufferSize = 256;
    settings.numBuffers = 4;
    settings.numOutputChannels = 2;
    settings.numInputChannels = 2;
    ofSoundStreamSetup(settings);

    fft = ofxFft::create(settings.bufferSize, OF_FFT_WINDOW_HAMMING);
    drawBins.resize(fft->getBinSize());
    middleBins.resize(fft->getBinSize());
    audioBins.resize(fft->getBinSize());
}

//--------------------------------------------------------------
void ofApp::update(){
    float deltaTime = ofClamp(ofGetLastFrameTime(), 0, 0.2);
    elapsedTime+=deltaTime;

    if(keyIsDown['c']) {
	polyLines.clear();
    }

    // Rotations
    if(keyIsDown['x'] && keyIsDown[OF_KEY_UP]) {
	rotation_vector.set(1, 0, 0);
        rotation++;
    }
    if(keyIsDown['x'] && keyIsDown[OF_KEY_DOWN]) {
	rotation_vector.set(1, 0, 0);
        rotation--;
    }

    if(keyIsDown['y'] && keyIsDown[OF_KEY_UP]) {
	rotation_vector.set(0, 1, 0);
        rotation++;
    }
    if(keyIsDown['y'] && keyIsDown[OF_KEY_DOWN]) {
	rotation_vector.set(0, 1, 0);
        rotation--;
    }

    if(keyIsDown['z'] && keyIsDown[OF_KEY_UP]) {
	rotation_vector.set(0, 0, 1);
        rotation++;
    }
    if(keyIsDown['z'] && keyIsDown[OF_KEY_DOWN]) {
	rotation_vector.set(0, 0, 1);
        rotation--;
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
    // drawRose();
    soundMutex.lock();
    drawBins = middleBins;
    soundMutex.unlock();

    // sends points to the DAC
    laser.send();

    laser.drawUI();
    cgui.draw();
}

//--------------------------------------------------------------
void ofApp::drawHypotrochoid(){
    ofPolyline line;
    ofVec3f vec;

    r = round(factor_r*R*0.1);

    float i = 0;
    while (i < 2*M_PI*(r/__gcd(r,R))) { 
	float x = (R - r)*cos(i) + d*cos((R-r)*i/r);
	float y = (R - r)*sin(i) + d*sin((R-r)*i/r);
        vec = ofVec3f(x,y,0).rotateRad(rotation*0.01*M_PI, rotation_vector);
	line.addVertex(vec + origin);
	i+=step*0.001*M_PI;
    }
    line.close(); // close the shape

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
        vec = ofVec3f(x,y,0).rotateRad(rotation*0.01*M_PI, rotation_vector);
	line.addVertex(vec + origin);
	i+=step*0.001*M_PI;
    }
    line.close(); // close the shape

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
    float maxValue = 0;
    for(int i = 0; i < inBuffer.size; i++) {
	    if(abs(input[i]) > maxValue) {
		    maxValue = abs(input[i]);
	    }
    }
    for(int i = 0; i < inBuffer.size; i++) {
	    input[i] /= maxValue;
    }
    
    float* curFft = fft->getAmplitude();
    memcpy(&audioBins[0], curFft, sizeof(float) * fft->getBinSize());
    
    maxValue = 0;
    for(int i = 0; i < fft->getBinSize(); i++) {
	    if(abs(audioBins[i]) > maxValue) {
		    maxValue = abs(audioBins[i]);
	    }
    }
    for(int i = 0; i < fft->getBinSize(); i++) {
	    audioBins[i] /= maxValue;
    }
    
    soundMutex.lock();
    middleBins = audioBins;
    soundMutex.unlock();
}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer &outBuffer) {
	for(int i = 0; i < outBuffer.size(); i += 2) {
            // Stuff
	}
}

//--------------------------------------------------------------
void ofApp::exit(){ 
    laser.saveSettings();
    dac.close();
}
