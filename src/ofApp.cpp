#include "ofApp.h"
#include <math.h>
#include <cmath>
#include <algorithm>

//--------------------------------------------------------------
void ofApp::setup(){
    // Size	
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
    cgui.add(color.set("color", ofColor(0, 255, 0), ofColor(0), ofColor(255)));

    // Config
    step = 10; // * M_PI
    quat.set(0, 0, 0, 1);

    // Hypotrochoid setup.
    factor_r = 2;
    R = round(laserRadius*0.80);
    d = round(laserRadius*0.80);

    // Rose setup.
    k = 5;
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
void ofApp::exit(){ 
    laser.saveSettings();
    dac.close();
}
