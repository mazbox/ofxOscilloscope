/**     ___           ___           ___                         ___           ___     
 *     /__/\         /  /\         /  /\         _____         /  /\         /__/|    
 *    |  |::\       /  /::\       /  /::|       /  /::\       /  /::\       |  |:|    
 *    |  |:|:\     /  /:/\:\     /  /:/:|      /  /:/\:\     /  /:/\:\      |  |:|    
 *  __|__|:|\:\   /  /:/~/::\   /  /:/|:|__   /  /:/~/::\   /  /:/  \:\   __|__|:|    
 * /__/::::| \:\ /__/:/ /:/\:\ /__/:/ |:| /\ /__/:/ /:/\:| /__/:/ \__\:\ /__/::::\____
 * \  \:\~~\__\/ \  \:\/:/__\/ \__\/  |:|/:/ \  \:\/:/~/:/ \  \:\ /  /:/    ~\~~\::::/
 *  \  \:\        \  \::/          |  |:/:/   \  \::/ /:/   \  \:\  /:/      |~~|:|~~ 
 *   \  \:\        \  \:\          |  |::/     \  \:\/:/     \  \:\/:/       |  |:|   
 *    \  \:\        \  \:\         |  |:/       \  \::/       \  \::/        |  |:|   
 *     \__\/         \__\/         |__|/         \__\/         \__\/         |__|/   
 *
 *  Description: 
 *				 
 *  ofxOscilloscope.h, created by Marek Bereza on 05/09/2011.
 */

#pragma once

#include "ofMain.h"

class ofxOscilloscope: public ofBaseDraws {
public:
	const static int MAX_SAMPLE_SIZE = 11025;
	
	enum TriggerDirection {
		NONE,
		UP,
		DOWN
	};
	
	void setup(float samplerate);
	
	void draw(float x,float y);
	void draw(float x,float y,float w, float h);

	float getWidth();
	float getHeight();
	
	float width;
	float height;
	
	void analyze(float *buffer, int bufferSize);
	
	void autoCalibrate();
	
	float samplerate;
	// how high the wave should be
	float verticalGain;
	
	// vertical offset
	float verticalPosition;
	
	// the size of the y axis in time
	float timebase;
	
	// when to trigger the sample
	float triggerLevel;
	
	// whether to trigger on a rise fall or either
	TriggerDirection triggerDirection;
	
	// minimum time between triggers
	float triggerHoldOff;

	// pauses the current signal
	void togglePaused();
	
protected:
	bool paused;
private:
	void drawGrid(float x, float y, float w, float h);
	void drawVerticalLines(float x, float y, float w, float h);	
	void drawHorizontalLines(float x, float y, float w, float h);	
	// the sample to display
	float sample[MAX_SAMPLE_SIZE];
	
	// these are non-threadsafe buffers
	float sampleOut[MAX_SAMPLE_SIZE];
	float sampleTemp[MAX_SAMPLE_SIZE];
	
	int sampleSize;
	

	// this is the position we're at
	// in the sample when triggering.
	// -1 denote that we're not sampling.
	int samplePos;
	
	// this is the last sample that 
	// passed through the system 
	// (not an array, but just the 
	// very last value.)
	float lastSample;

	ofMutex inputMutex;
	
	int automode;
	float autoMin;
	float autoMax;
};


