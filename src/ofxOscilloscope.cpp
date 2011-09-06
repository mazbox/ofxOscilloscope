/**
 *  ofxOscilloscope.cpp
 *
 *  Created by Marek Bereza on 05/09/2011.
 */

#include "ofxOscilloscope.h"

void ofxOscilloscope::setup(float samplerate) {
	this->samplerate = samplerate;
	this->width = 320;
	this->height = 240;
	this->verticalGain = 1;
	this->verticalPosition = 0;
	this->timebase = 0.01;
	this->triggerLevel = 0;
	this->triggerDirection = UP;
	this->triggerHoldOff = 0;
	this->samplePos = -1;
	this->lastSample = 0;
	this->paused = false;
	this->automode = 0;
}


void ofxOscilloscope::autoCalibrate() {
	automode = 1;
}

void ofxOscilloscope::draw(float x,float y) {
	draw(x, y, width, height);
}

void ofxOscilloscope::drawGrid(float x, float y, float w, float h) {
	drawVerticalLines(x, y, w, h);
	drawHorizontalLines(x, y, w, h);
}


void ofxOscilloscope::drawHorizontalLines(float x, float y, float w, float h) {
	float ms = timebase * 1000;
	float step = 10;
	
	if(ms<5) {
		step = 0.5;
		
	} else if(ms<8) {
		step = 1;
	} else if(ms<16) {
		step = 2;
	} else if(ms<35) {
		step = 5;
	}
	for(float i = 0; i < ms; i += step) {
		glColor4f(1,1,1,0.25);
		float xval = i;
		xval = ofMap(xval, 0, ms, x, x+width);
		ofLine(xval, y, xval, y+height);
		ofSetHexColor(0xBBFFBB);
		if(i!=0) ofDrawBitmapString(ofToString(i)+"ms", xval+4, y+height-4);
	}
}

void ofxOscilloscope::drawVerticalLines(float x, float y, float w, float h) {
	float scale = 1.f/verticalGain;
	int jump = 1;
	if(scale>4) {
		jump = 2;
	}
	if(scale>8) {
		jump = 3;
	}
	
	// 1 unit lines
	for(int i = -scale-1; i < scale+1; i++) {
		glColor4f(1, 1, 1, 0.5);
		float yval = i;
		yval *= verticalGain;
		yval += verticalPosition;
		yval = ofMap(yval, -1, 1, h, 0);
		ofLine(x, y + yval, x + width, y + yval);
		if(i%jump==0) {
			ofSetHexColor(0xBBFFBB);
			ofDrawBitmapString(ofToString(i) + "v", x+5, y + yval-2);
		}
	}
	// 0.5 unit lines
	for(int i = -scale-1; i < scale+1; i+= 1) {
		glColor4f(1, 1, 1, 0.2);
		float yval = i;
		yval += 0.5;
		yval *= verticalGain;
		yval += verticalPosition;
		yval = ofMap(yval, -1, 1, h, 0);
		ofLine(x, y + yval, x + width, y + yval);
		if(scale<2) {
			ofSetHexColor(0xBBFFBB);
			ofDrawBitmapString(ofToString(i + 0.5) + "v", x+5, y + yval-2);
		}
	}
	
	// 0.25 unit lines
	if(scale<2) {
		for(int i = -scale-1; i < scale+1; i++) {
		
			glColor4f(1, 1, 1, 0.1);
			float yval = i;
			yval += 0.25;
			yval *= verticalGain;
			yval += verticalPosition;
			yval = ofMap(yval, -1, 1, h, 0);
			ofLine(x, y + yval, x + width, y + yval);
			
			yval = i;
			yval += 0.75;
			yval *= verticalGain;
			yval += verticalPosition;
			yval = ofMap(yval, -1, 1, h, 0);
			ofLine(x, y + yval, x + width, y + yval);


		}
	}
}
void ofxOscilloscope::draw(float x,float y,float w, float h) {
	glEnable(GL_SCISSOR_TEST);
	ofEnableAlphaBlending();
	glScissor(x, ofGetHeight() - y - h, width, height);
	this->width = w;
	this->height = h;
	ofPushStyle();
	ofNoFill();
	ofSetHexColor(0xFFFFFF);
	ofRect(x+1, y+1, w-1, h-1);
	glBegin(GL_LINE_STRIP);
	int length = 0;
	inputMutex.lock();
	length = sampleSize;
	inputMutex.unlock();
	float waveIncrement = 44100.f * timebase / w;
	for(int i = 0; i < w; i++) {
		float wPos = i * waveIncrement;
		if(wPos>=length-1) wPos = length - 2;
		
		float yval = sample[(int)wPos];
		float nextY = sample[(int)ceil(wPos)];
		float lerpAmt = wPos - floor(wPos);
		yval = nextY*lerpAmt + yval*(1.f - lerpAmt);
		yval *= verticalGain;
		yval += verticalPosition;
		glVertex2f(i+x, y+ofMap(yval, -1, 1, h, 0));
	}
	glEnd();

	drawGrid(x, y, w, h);
	ofPopStyle();
	glDisable(GL_SCISSOR_TEST);
}

void ofxOscilloscope::analyze(float *buffer, int bufferSize) {
	if(paused) return;
	
	if(automode) {
		if(automode==1) {
			autoMin = FLT_MAX;
			autoMax = FLT_MIN;
		}
	
		automode++;
		for(int i = 0; i < bufferSize; i++) {
			if(autoMin>buffer[i]) autoMin = buffer[i];
			if(autoMax<buffer[i]) autoMax = buffer[i];
		}
		if(automode>5) {
			automode = 0;
			verticalGain = 0.5/(autoMax - autoMin);
			verticalPosition = autoMin;
		}
		return;
	}
	inputMutex.lock();
	sampleSize = (timebase * samplerate);
	
	if(sampleSize>MAX_SAMPLE_SIZE) {
		sampleSize = MAX_SAMPLE_SIZE;
	}
	inputMutex.unlock();
	
	for(int i = 0; i < bufferSize; i++) {
		if(samplePos==-1) { // not sampling
			
			// if we have a crossing, trigger
			if(triggerDirection==UP) {
				if(lastSample<=triggerLevel && buffer[i]>=triggerLevel) {
					samplePos = 0;
				}
			} else if(triggerDirection==DOWN) {
				if(lastSample>=triggerLevel && buffer[i]<=triggerLevel) {
					samplePos = 0;
				}
			} else {
				if((lastSample>=triggerLevel && buffer[i]<=triggerLevel) || (lastSample<=triggerLevel && buffer[i]>=triggerLevel)) {
					samplePos = 0;
				}
			}
		}
			
		if(samplePos>-1) {
			sampleTemp[samplePos] = buffer[i];
			samplePos++;
			// the buffer is full
			if(samplePos>=sampleSize) {
				memcpy(sampleOut, sampleTemp, sizeof(float)*sampleSize);
				samplePos = -1;
			}
		}
		
		lastSample = buffer[i];
	}
	
	// here, you either continue triggering, or look for a trigger,
	// when you finish triggering, lock and copy
	inputMutex.lock();
	memcpy(sample, sampleTemp, sizeof(float)*sampleSize); 
	inputMutex.unlock();
}

float ofxOscilloscope::getWidth() {
	return width;
}

float ofxOscilloscope::getHeight() {
	return height;
}
void ofxOscilloscope::togglePaused() {
	paused ^= true;
}