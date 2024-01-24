// Program developed by Corey Leydig
// This program offers an API that allows devices to control up to two stepper motors for linear actuator 
// applications where limit switches used to define bounds are connected directly to the driver.
// This driver uses the pololu high powered stepper motor driver which includes capibilities for stall
// detection anf back EMF measuring for both stepper motors. Link to their library is below:
// library's documentation for information about how to connect the driver:
//   http://pololu.github.io/high-power-stepper-driver
// The HighPowerStepperDriver library must be installed

// For calculating the amount of steps needed to complete one revolution:
// see the function setStepMode(); whatever value inputted will define the number of microsteps used to complete
// one step. One step can be found by looking at the specifications of the stepper motor used. Use the equation below:
// 360/(step/microstep)
// For example: The NEMA 23 stepper motor has a step value of 1.8 (This is the amount of degrees that the motor rotates
// in one step) The microstep setting is set to 32 (This values is controlled via the setStepMode() function)
// 360/(1.8/32) = 6400; I must run the step function 6400 times to rotate one full rotation
// 

#define F_CPU 16000000L // Clock Frequency
// #define fast 0.006
// #define slow 0.05

#include <avr/io.h>
#include <util/delay.h>
#include <SPI.h>
#include <HighPowerStepperDriver.h>

const int stepA = 2;
const int dirA = 3;
const int csPinA = 4;
const int sleepA = 5;
const int faultA = 6;
const int stallA = 7;
const int stepB = 8;
const int dirB = 9;
const int csPinB = 10;
const int sleepB = 11;
const int faultB = 12;
const int stallB = 13;
const int BEMFA = A0; 
const int BEMFB = A1;
const float stepDistance = 0.1;
const int inLimitA = A3;
const int inLimitB = A5;
const int outLimitA = A2;
const int outLimitB = A4;
const double stepRes = 128; // 0.01 in 
String inst;
String data;
const bool useLimits = true;


// assign motor values to motor arrays
int motorA[] = {stepA,dirA,stallA,inLimitA,outLimitA,sleepA};
int motorB[] = {stepB,dirB,stallB,inLimitB,outLimitB,sleepB};

// Keep track of the current position at all times for both 

int posA = 0;
int posB = 0;

// This period is the length of the delay between steps, which controls the
// stepper motor's speed.  You can increase the delay to make the stepper motor
// go slower.  If you decrease the delay, the stepper motor will go faster, but
// there is a limit to how fast it can go before it starts missing steps.

HighPowerStepperDriver sdA;
HighPowerStepperDriver sdB;

void setup()
{

  Serial.begin(115200);
  SPI.begin();
  sdA.setChipSelectPin(csPinA);
  sdB.setChipSelectPin(csPinB);
  
  // Setup Pins
  pinMode(faultA, INPUT);
  pinMode(stallA, INPUT);
  pinMode(sleepA, OUTPUT);
  pinMode(faultB, INPUT);
  pinMode(stallB, INPUT);
  pinMode(sleepB, OUTPUT);
  pinMode(inLimitA, INPUT);
  pinMode(inLimitB, INPUT);
  pinMode(outLimitA, INPUT);
  pinMode(outLimitB, INPUT);

  // Drive the STEP and DIR pins low initially.
  pinMode(stepA, OUTPUT);
  digitalWrite(stepA, LOW);
  pinMode(dirA, OUTPUT);
  digitalWrite(dirA, LOW);

  pinMode(stepB, OUTPUT);
  digitalWrite(stepB, LOW);
  pinMode(dirB, OUTPUT);
  digitalWrite(dirB, LOW);

  // Give the driver some time to power up.
  delay(1);

  // Reset the driver to its default settings and clear latched status
  // conditions.
  sdA.resetSettings();
  sdA.clearStatus();
  sdB.resetSettings();
  sdB.clearStatus();

  // Select auto mixed decay.  TI's DRV8711 documentation recommends this mode
  // for most applications, and we find that it usually works well.
  sdA.setDecayMode(HPSDDecayMode::AutoMixed);
  sdB.setDecayMode(HPSDDecayMode::AutoMixed);  

  // Set the current limit. You should change the number here to an appropriate
  // value for your particular system.
  sdA.setCurrentMilliamps36v4(2400);
  sdB.setCurrentMilliamps36v4(2400);

  // Set the number of microsteps that correspond to one full step.
  sdA.setStepMode(HPSDStepMode::MicroStep64);
  sdB.setStepMode(HPSDStepMode::MicroStep64);
  
  // Enable the motor outputs.
  sdA.enableDriver();
  sdB.enableDriver();
}

void loop()
{
 
  while(true) {
    
    if (Serial.available() > 0) {  
      inst = "";
      data = "";
      inst = Serial.readStringUntil('\n');
      data = Serial.readStringUntil('\n') ;   
      break; 
    }
  }
  
  //////////////////////////////////////////////////////
  ///////////// First instruction: goToPosition ////////
  //////////////////////////////////////////////////////

  /// Test Variables ///

  // String v1 = "goToPosition";
  // String v2 = "56.76,motorA,2000";
  
  //////////////////////


  if (inst == "goToPosition"){
    
    // Split the incomming data into seperate parts //
    char dataChar[data.length()+1];
    char *directions[data.length()];
    char *ptr = NULL;    
    data.toCharArray(dataChar, data.length()+1);
    byte index = 0;
    ptr = strtok(dataChar, " ,");
    while (ptr != NULL) {
      directions[index] = ptr;
      index++;
      ptr = strtok(NULL, ",");      
    }
    //Serial.println(directions[0]);
    ////////////////////////////////////////////////
    // Convert all char to respected values //

    String NP = directions[0];
    String NP2 = directions[1];
    double newPosA = NP.toDouble();
    double newPosB = NP2.toDouble();
    String mA = directions[1]; 
      //Serial.println("Works");
    Serial.print("movement,");
    moveToPosition(newPosA, newPosB, motorA, motorB, &posA, &posB, stepRes); 
    float posADisplay = float(posA)/100;
    float posBDisplay = float(posB)/100;  
     
    Serial.print(posADisplay);
    Serial.print(",");       
    Serial.print(posBDisplay);      
    Serial.println(",");
    
  } 

  //////////////////////////////////////////////////////
  ///////////// Second instruction: Calibrate    ///////
  //////////////////////////////////////////////////////

  else if (inst == "calibrate") {
    Serial.print("calibrate,");
    calibrate(motorA,motorB);
    posA = 0;
    posB = 0;
    Serial.println("calibrateComplete,");
  }   

  //////////////////////////////////////////////////////
  ///////////// Fourth instruction: getPosition ////////
  //////////////////////////////////////////////////////

  else if (inst == "getPosition"){
    if (data == "motorA"){
      float posADisplay = float(posA)/100;
      Serial.print("motorA,");
      Serial.print(posADisplay);
      Serial.println(",");
    }  
    else if (data == "motorB") {
      float posBDisplay = float(posB)/100;
      Serial.print("motorB,");
      Serial.print(posBDisplay);
      Serial.println(",");
      
    }
    else {
      Serial.println("Invalid Choice");
    }    
  }  

  //////////////////////////////////////////////////////
  ///////////// Fifth instruction: Measure Bounds //////
  //////////////////////////////////////////////////////

  else if (inst == "measureBounds"){
    if (data == "motorA"){
      Serial.print("measureA,");
      Serial.print(measureBounds(motorA, stepRes));
      Serial.println(",");
    }
    else if (data == "motorB"){
      Serial.print("measureB,");
      Serial.print(measureBounds(motorB, stepRes));
      Serial.println(",");
    }
    else {
      Serial.println("Invalid Choice");
    }
  }
          
}

