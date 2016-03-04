#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <atomic>
#include <string>
#include <sstream>
#include "GPIO.h"

struct IO LEDPin;

// function for on close event or other signels
void ctrl_C_Handler(int s){
	cout << endl << "Caught signal " << s << endl;
	
	LEDPin.setLow();
	LEDPin.makeInput();
	
	exit(1); 
}
  
int main (void){
	// handler for on close event or other signels
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = ctrl_C_Handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);
	
    LEDPin.init(20); 
	LEDPin.makeOutput();
	LEDPin.setLow();

	while (1){
		LEDPin.setHigh();
		usleep(1000000);// one second
	
		LEDPin.setLow();
		usleep(1000000);// one second
	
	}

    return 0;
}
