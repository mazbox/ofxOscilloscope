#include "testApp.h"
#include "Drawable.h"

//--------------------------------------------------------------
void testApp::setup() {
	
	oscilloscope.setup(44100);

	soundStream.setup(this, 0, 1, 44100, 1024, 1);
	ofSetVerticalSync(true);
	ofSetFrameRate(30);
	ofSetWindowTitle("ofxOscilloscope");
}


//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	oscilloscope.draw(20, 20, 640, 480);

}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	
	
}

//--------------------------------------------------------------
void testApp::keyReleased  (int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}


//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

float oscilloscopeBuffer[1024];
float phase = 0;
//--------------------------------------------------------------
void testApp::audioIn(float * output, int bufferSize, int nChannels){
/*
	float inc = 2*PI/(44100/100);
	for(int i = 0; i < bufferSize; i++) {
		output[i*2] = sin(phase) + sin(phase*2)*0.5 + sin(phase*4)*0.25 + sin(phase*8)*0.125;
		phase += inc;
		output[i*2+1] = output[i*2];
	}*/
	
	for(int i = 0; i < bufferSize; i++) {
		oscilloscopeBuffer[i] = output[i];
	}
	
	oscilloscope.analyze(oscilloscopeBuffer, bufferSize);
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
