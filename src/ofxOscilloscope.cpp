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
}


void ofxOscilloscope::autoCalibrate() {
	
}

void ofxOscilloscope::draw(float x,float y) {
	draw(x, y, width, height);
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
	glColor4f(1, 1, 1, 0.5);
	for(int i = -5; i < 5; i++) {
		float yy = i;
		yy *= verticalGain;
		yy += verticalPosition;
		yy = y + ofMap(yy, -1, 1, h, 0);
		ofLine(x, yy, x + w, yy);
	}
	ofPopStyle();
	glDisable(GL_SCISSOR_TEST);
}

void ofxOscilloscope::analyze(float *buffer, int bufferSize) {
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