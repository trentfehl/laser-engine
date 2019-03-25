#include "ofApp.h"
#include <math.h>
#include <cmath>
#include <algorithm>

//--------------------------------------------------------------
void ofApp::setup(){
	
    laserWidth = 800;
    laserHeight = 800;
    laser.setup(laserWidth, laserHeight);

    laser.addProjector(dac);
    
    string dacIP = "192.168.1.234";

    dac.setup(dacIP);
	
    laser.initGui();
	 
    cgui.setup("color panel", "colors.xml", ofGetWidth()-240, 700 );
    cgui.add(color.set("color", ofColor(0, 255, 0), ofColor(0), ofColor(255)));

    // Hypotrochoid setup.
    factor_r = 2;
    R = round(laserWidth*0.20);
    d = round(laserWidth*0.20);
}

//--------------------------------------------------------------
void ofApp::update(){
    float deltaTime = ofClamp(ofGetLastFrameTime(), 0, 0.2);
    elapsedTime+=deltaTime;

    // prepares laser manager to receive new points
    laser.update();
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofBackground(0);
    ofNoFill();
    ofSetLineWidth(1);
    ofDrawRectangle(0,0,laserWidth, laserHeight);
    
    drawHypotrochoid();

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
	float x = (R - r)*cos(i) + d*cos((R-r)*i/r) + laserWidth/2;
	float y = (R - r)*sin(i) + d*sin((R-r)*i/r) + laserHeight/2;
	line.addVertex(ofVec3f(x,y,0));
	i+=0.05*M_PI;
    }
    line.close(); // close the shape

    laser.drawPoly(line, color);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key =='c') {
	polyLines.clear();
    } else if(key == OF_KEY_UP){
    } else if(key == OF_KEY_DOWN){
    } else if(key == OF_KEY_RIGHT){
        factor_r++;
        factor_r = min(factor_r, 800);
        std::cout << "factor_r: " << factor_r << std::endl;
    } else if(key == OF_KEY_LEFT){
        factor_r--;
        factor_r = max(factor_r, 1);
        std::cout << "factor_r: " << factor_r << std::endl;
    } else if(key == OF_KEY_PAGE_UP){
    } else if(key == OF_KEY_PAGE_DOWN){
    }

}

//--------------------------------------------------------------
void ofApp::exit(){ 
    laser.saveSettings();
    dac.close();
}
