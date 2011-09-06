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
 *  ofxOscilloscopeGui.h, created by Marek Bereza on 05/09/2011.
 */


#include "Gui.h"
#include "ofxOscilloscope.h"
#include "Drawable.h"

class ofxOscilloscopeGui: public ofxOscilloscope, public xmlgui::Gui, public xmlgui::Listener {

public:
	void setup(float samplerate) {

		Gui::loadFromXml("oscilloscopegui.xml");
		addListener(this);
		xmlgui::Drawable *c = (xmlgui::Drawable*)getControlById("scope");
		
		c->drawable = this;
		
		
		
		getControlById("verticalGain")->pointToValue(&verticalGain);
		getControlById("verticalPos")->pointToValue(&verticalPosition);
		getControlById("triggerLevel")->pointToValue(&triggerLevel);
		getControlById("triggerHoldOff")->pointToValue(&triggerHoldOff);
		getControlById("timebase")->pointToValue(&timebase);
		getControlById("pause")->pointToValue(&paused);
		ofBackground(34, 34, 34);
		
		ofxOscilloscope::setup(samplerate);
	}
	
	void controlChanged(xmlgui::Control *control) {
		//	printf("%s\n", control->id.c_str());
		if(control->id=="triggerDirection") {
			//		printf("%d\n", control->getInt());
			if(control->getInt()==0) {
				triggerDirection = ofxOscilloscope::UP;
			} else if(control->getInt()==1) {
				triggerDirection = ofxOscilloscope::NONE;
			} else if(control->getInt()==2) {
				triggerDirection = ofxOscilloscope::DOWN;
			}
		} else if(control->id=="autoCalibrate") {
			autoCalibrate();
		}
	}
	
	
};