/*
  serialblink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/

//#define SERIAL_BLINK_DEBUG

#define WDBuffSize 7

float hiDelay;
float loDelay;

char SwH[WDBuffSize];
char SwP[WDBuffSize];

char *DataPtr;

int ReadingWaveData = false;

void ProcessWaveData(char WaveData) {

  float SwHf;
  float SwPf;

  if (WaveData == '\0') {
    memset(SwH, '\0', WDBuffSize);
    memset(SwP, '\0', WDBuffSize);
    DataPtr = SwH;
    return;
  }

  if (WaveData == ',') {
    DataPtr = SwP;
#ifdef SERIAL_BLINK_DEBUG
    SerialUSB.print(",\n");
#endif
    return;
  }

  if (WaveData == ';') {
    //Move the wave data to the proper place.

#ifdef SERIAL_BLINK_DEBUG
    SerialUSB.print(";\n");
#endif
    hiDelay = strtof(SwH, NULL);
    loDelay = strtof(SwP, NULL);
#ifdef SERIAL_BLINK_DEBUG
    SerialUSB.print(hiDelay, 2);
    SerialUSB.write('\n');
    SerialUSB.print(loDelay, 2);
    SerialUSB.write('\n');
#endif

    ReadingWaveData = false;
    return;
  }

  *DataPtr = WaveData;
  ++DataPtr;
#ifdef SERIAL_BLINK_DEBUG
  SerialUSB.print(SwH);
  SerialUSB.write('\n');
  SerialUSB.print(SwP);
  SerialUSB.write('\n');
#endif

}

void serialCheck() {        //Monitors serial for commands.  Must be called in routinely in loop for serial interface to work.

  char inChar;

  if (SerialUSB.available()) {

    inChar = (char)SerialUSB.read();

    if (ReadingWaveData) {
      ProcessWaveData(inChar);

      if (inChar == ';') {
        ReadingWaveData = false;
      }
      return;
    }

    switch (inChar) {

    case 'W':
#ifdef SERIAL_BLINK_DEBUG
      SerialUSB.print("W received\n");
#endif
      ReadingWaveData = true;
      ProcessWaveData('\0');
      break;

    case 'p':             //print
//      print_angle();
      break;

    case 's':             //step
//      oneStep();
//      print_angle();
      break;

    case 'd':             //dir
//      if (dir) {
//        dir = false;
//      } else {
//        dir = true;
//      }
      break;

    case 'w':                //old command
//      calibrate();           //cal routine
      break;

    case 'c':
//      calibrate();           //cal routine
      break;

    case 'e':
//      readEncoderDiagnostics();   //encoder error?
      break;

    case 'y':
//      r = (read_angle() + (360.0 * wrap_count));          // hold the current position
//      SerialUSB.print("New setpoint ");
//      SerialUSB.println(r, 2);
//      enableTCInterrupts();      //enable closed loop
      break;

    case 'n':
//      disableTCInterrupts();      //disable closed loop
//      analogFastWrite(VREF_2, 0);     //set phase currents to zero
//      analogFastWrite(VREF_1, 0);
      break;

    case 'r':             //new setpoint
//      SerialUSB.println("Enter setpoint:");
//      while (SerialUSB.available() == 0)  {}
//      r = SerialUSB.parseFloat();
//      SerialUSB.println(r);
      break;

    case 'x':
//      mode = 'x';           //position loop
      break;

    case 'v':
//      mode = 'v';           //velocity loop
      break;

    case 't':
//      mode = 't';           //torque loop
      break;

    case 'h':               //hybrid mode
//      mode = 'h';
      break;

    case 'q':
//      parameterQuery();     // prints copy-able parameters
      break;

    case 'a':             //anticogging
//      antiCoggingCal();
      break;

    case 'k':
//      parameterEditmain();
      break;

    case 'g':
//      sineGen();
      break;

    case 'm':
//      serialMenu();
      break;

    case 'j':
//      stepResponse();
      break;


    default:
      break;
    } 
  }
}

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  SerialUSB.begin(115200);          

  hiDelay = 1000;
  loDelay = 1000;
  
  SerialUSB.print("serialblink ready.\n");
}

// the loop function runs over and over again forever
void loop() {

  serialCheck();

  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay((uint32_t) hiDelay);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay((uint32_t) loDelay);                       // wait for a second
}


