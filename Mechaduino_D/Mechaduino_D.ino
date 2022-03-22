
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

  
  //  Here are the initial variables for the moveRel function.
  //  The moveRel function defines the number of steps as a float, and the velocity
  //  as an int, so that convention has been followed. 
  //  there is also an acceleration int, but we will tune this manually.

  float distance = 180;
  int velocity = 100;

  // The new distance and velocity from the serialCheck routine
  // are moved here after they are received.  They will be processed
  // in the loop after the serialCheck routine returns.

  float newDist;
  int  newVel;

//////////////////////////////////////
/////////////////SETUP////////////////
//////////////////////////////////////

void setup()        // This code runs once at startup
{  
  newDist = distance;                       
  newVel = velocity;
   
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

}
  


//////////////////////////////////////
/////////////////LOOP/////////////////
//////////////////////////////////////


void loop()                 // main loop
{

  serialCheck();              //must have this execute in loop for serial commands to function

  // If a new distance or velocity was received.
  if (!((distance == newDist) && (velocity == newVel))) {

    if ((newDist - distance) != 0) {
      moveRel((newDist - distance), newVel, 20); // Move the motor to the new bottom point.
    }

    distance = newDist; // Update the distance value with the new distance value.
    velocity = newVel; // Update the velocity value with the new velocity value. 
  }

  //r=0.1125*step_count;      //Don't use this anymore. Step interrupts enabled above by "configureStepDir()", adjust step size ("stepangle")in parameters.cpp

  // below is an implimentation of the 'trapezoidal speed trajectory' function that
  // is built in to the Mechaduino firmware, and we will use to run the steppers
  
  moveRel(distance,velocity,30);
  delay(500);
  moveRel(-distance,velocity,20);
  delay(500);

}
