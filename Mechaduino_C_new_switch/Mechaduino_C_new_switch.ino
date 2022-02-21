
/*
  -------------------------------------------------------------
  Mechaduino 0.1 & 0.2 Firmware  v0.1.5
  SAM21D18 (Arduino Zero compatible), AS5047 encoder, A4954 driver

  All Mechaduino related materials are released under the
  Creative Commons Attribution Share-Alike 4.0 License
  https://creativecommons.org/licenses/by-sa/4.0/

  Many thanks to all contributors!
  --------------------------------------------------------------
  
  Controlled via a SerialUSB terminal at 115200 baud.

  Implemented serial commands are:

 s  -  step
 d  -  dir
 p  -  print [step number] , [encoder reading]

 c  -  calibration routine
 e  -  check encoder diagnositics
 q  -  parameter query

 x  -  position mode
 v  -  velocity mode
 t  -  torque mode

 y  -  enable control loop
 n  -  disable control loop
 r  -  enter new setpoint

 j  -  step response
 k  -  edit controller gains -- note, these edits are stored in volatile memory and will be reset if power is cycled
 g  -  generate sine commutation table
 m  -  print main menu


  ...see serialCheck() in Utils for more details

*/

#include "Utils.h"
#include "Parameters.h"
#include "State.h"
#include "analogFastWrite.h"

// Global variables for operation of Mechaduino C:

float Distance_C = 1000; // this placeholder value will be replaced by the output of wave data math
int Velocity_C = 1500; // this placeholder value will be replaced by the output of wave data math

//////////////////////////////////////
/////////////////SETUP////////////////
//////////////////////////////////////

void setup()        // This code runs once at startup
{                         
   
  digitalWrite(ledPin,HIGH);        // turn LED on 
  setupPins();                      // configure pins
  setupTCInterrupts();              // configure controller interrupt

  SerialUSB.begin(115200);          
  delay(3000);                      // This delay seems to make it easier to establish a connection when the Mechaduino is configured to start in closed loop mode.  
  serialMenu();                     // Prints menu to serial monitor
  setupSPI();                       // Sets up SPI for communicating with encoder
  digitalWrite(ledPin,LOW);         // turn LED off 
  
  // spot check some of the lookup table to decide if it has been filled in
  if (lookup[0] == 0 && lookup[128] == 0 && lookup[1024] == 0)
    SerialUSB.println("WARNING: Lookup table is empty! Run calibration");

  // Uncomment the below lines as needed for your application.
  // Leave commented for initial calibration and tuning.
  
  //    configureStepDir();           // Configures setpoint to be controlled by step/dir interface
  //    configureEnablePin();         // Active low, for use wath RAMPS 1.4 or similar
  enableTCInterrupts();         // uncomment this line to start in closed loop 
  mode = 'x';                   // start in position mode

  pinMode(2, INPUT); // set input pin for limit switch 
  // Limit switch is high when up, or not in contact with carriage, and low when pressed down

  // rotate rail stepper up towards limit switch at top of track
  while (r > -5000.0){ 
  /*************************************************************************
  the number in the line above needs to be adjusted based on track length
  *************************************************************************/
    
    // check to see if limit switch is up or HIGH at each step
    if (digitalRead(2) == HIGH){ 
        r -= 0.1; 
        delayMicroseconds(100);
    }
    
    // if limit switch is pressed down by carriage, then quit out of while loop
    else {
        break; 
    }
  }
  delay(1000);

  // return carriage towards middle of linear rail.
  while (r < 100.0){ 
    /*************************************************************************
    the number in the line above needs to be adjusted based on Distance variable
    *************************************************************************/
    r += 0.1;
    delayMicroseconds(100);
  }
  delay(1000);      
  
}

//////////////////////////////////////
/////////////////LOOP/////////////////
//////////////////////////////////////


void loop()                 // main loop
{

  // variability code if desired
  /*
  float randDistance;
  int randSpeed;
  randDistance = random(500, 700);
  randSpeed = random(1200, 2200);
  */

  // move carriage upwards on linear rail
  moveRel(-Distance_C, Velocity_C, 400);
  delay (10);

  // move carriage downwards on linear rail
  moveRel(Distance_C, Velocity_C, 400);
  delay (10);


  //serialCheck();              //must have this execute in loop for serial commands to function

  //r=0.1125*step_count;      //Don't use this anymore. Step interrupts enabled above by "configureStepDir()", adjust step size ("stepangle")in parameters.cpp

}
