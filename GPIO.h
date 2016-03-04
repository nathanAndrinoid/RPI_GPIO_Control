#ifndef GPIO_H
#define GPIO_H

#include <fstream>
#include <unistd.h>
#include <string>
#include <iostream>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define BCM2708_PERI_BASE	0x3F000000
#define GPIO_BASE (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */
#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))

using namespace std;

struct IO {
	int gpionum; // GPIO number associated with the instance of an object
    int regValue;
    int  mem_fd;
    void *gpio_map;
    volatile unsigned *gpio;
    volatile unsigned *SETAddress;
    volatile unsigned *CLRAddress;
    volatile unsigned *READAddress;
    
	// when declaring a new IO struct you give an input parameter of the GPIO pin it will control
    void init(int gnum){
		gpionum = gnum;
		regValue = 1<<gnum;
		setupMap();
		// *gpio = 4096;
		SETAddress = gpio+7;
		CLRAddress = gpio+10;
		READAddress = gpio+13;
    }
	
    void makeInput(){ // Set GPIO Direction in
		INP_GPIO(gpionum);
    }
    
    void makeOutput(){ // Set GPIO Direction out
		// In order to make it output you needed to make it input first
		// for reliability.
		INP_GPIO(gpionum);
		OUT_GPIO(gpionum);
    }

	void loopFast(){
		while (1){
			*SETAddress = regValue;
			*CLRAddress = regValue;
		}
	}
    
	void setBit(int bit){
		if (bit)
			*SETAddress = regValue;
		else
			*CLRAddress = regValue;
	}
	
    void setHigh(){ // 24 ns time complete
    	*SETAddress = regValue;
    }
    
    void setLow(){ // 24 ns time complete
    	*CLRAddress = regValue;
    }
    
    int getValue(){ // about 104 ns to complete (probably not anymore)
    	return (int)(((*READAddress) & regValue) != 0);
    }
    
    int getPinNum(){ // return the GPIO number associated with the instance of an object
		return gpionum;
    }
    
    void setupMap(void){
		int mem_fd;
	
		/* open /dev/mem */
		if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
			cout << "can't open /dev/mem" << endl;
			exit(-1);
		}
	 
		/* mmap GPIO */
		gpio_map = mmap(
			NULL,             //Any adddress in our space will do
			BLOCK_SIZE,       //Map length
			PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
			MAP_SHARED,       //Shared with other processes
			mem_fd,           //File to map
			GPIO_BASE         //Offset to GPIO peripheral
		);
	 
		close(mem_fd); //No need to keep mem_fd open after mmap
	 
		if (gpio_map == MAP_FAILED) {
			printf("mmap error %d\n", (int)gpio_map);//errno also set!
			exit(-1);
		}
	 
		cout << "GPIO = " << (*((volatile unsigned *)gpio_map)) << endl;

		// Always use volatile pointer!
		gpio = (volatile unsigned *)gpio_map;
	}
};

#endif
