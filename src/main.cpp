// Include libraries
#include <Arduino.h>
#include "Adafruit_NeoPixel.h"
#include "Adafruit_PWMServoDriver.h"

#define SCENECOUNT 9


// Define PWM parameters
#define SERVOCOUNT 2
Adafruit_PWMServoDriver pwms[SERVOCOUNT] = {Adafruit_PWMServoDriver(), Adafruit_PWMServoDriver(0x41)};
#define Y 1

#define colorCoef 4096 / 100

// Structure for position variables
struct position {byte x; byte y;};

// Structure for servos
struct servoStruct {
  position SAFEZONES[3];
  // Automatic variables 
  uint16_t length[3];
};

// Structure for scenes
struct scenestruct {
  unsigned long startTime;
  unsigned long endTime;
  byte protect;
  position start;
  position end;
  byte Rad;
  byte color[3];
  byte noiseCount;
  // Automatic variables
  unsigned long time;
};

/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
// Scene ID
byte id = 0;

// Servo structs
servoStruct servos[SERVOCOUNT] {
  {}
};

// Scene structs
scenestruct scenes[SCENECOUNT] = {
  {0,   20, 0b10, {}, {}, 0, {100, 0, 0}},
  {23,  31, 0b10, {}, {}, 0, {0, 100, 0}},

  {34,  49, 0b10, {}, {}, 0, {0, 0, 100}},
  {52,  57, 0b10, {}, {}, 0, {100, 100, 0}},

  {60,  62, 0b10, {}, {}, 0, {100, 0, 100}},
  {65,  69, 0b10, {}, {}, 0, {0, 100, 100}},
  {72,  75, 0b10, {}, {}, 0, {100, 100, 100}},

  {78,  101, 0b10},
  {104, 120, 0b10}
};

// Initializate scene structures
void initStructs() {
  // Init servos
  for (int servo=0; servo < SERVOCOUNT; servo++) {
    // INIT PWM
    pwms[servo].begin();
    pwms[servo].setPWMFreq(50);
    Serial.println("Lets servooos");   

    // INIT VARIABLES
    for (byte j; j < 3; j++) {
      servos[servo].length[j] = abs(servos[servo].SAFEZONES[j].x - servos[servo].SAFEZONES[j].y);
    }
    delay(100);
  }

  // Init scenes
  unsigned int time = millis();
  for(int scene=0; scene<SCENECOUNT; scene++) {
    if (scenes[scene].startTime < 1000) scenes[scene].startTime *= 1000;
    if (scenes[scene].endTime   < 1000) scenes[scene].endTime   *= 1000;
    scenes[scene].startTime = scenes[scene].startTime + time;
    scenes[scene].endTime   = scenes[scene].endTime   + time;
    scenes[scene].time = scenes[scene].endTime - scenes[scene].startTime;
  }
}

// Function for display the storm
void storm(scenestruct scene) { 
  // TODO: Storm function
}

// Function for circuit frame
void circuit(scenestruct scene) {
  // TODO: Circuit function
}

// Function for color dioids
void colorDiods(byte id) {
  for (byte color=0; color < 3; color++) {
    Serial.print("Color");
    Serial.print('\t');
    Serial.println(colorCoef*scenes[id].color[color]);
    for (byte pin=4; pin < 8; pin++) {
      pwms[id].setPWM(pin+color*4, 0, scenes[id].color[color]);
    }
  }
}

// Function for move servos
void updateServos(scenestruct scene) {
  for (int servo=0; servo < SERVOCOUNT; servo++) {
    Serial.print("Servo");
    Serial.print('\t');
    Serial.println(servo);
    colorDiods(servo);
  }
}

// MAIN CODE===============
// Function for update matrix
void update() {
  Serial.print("Update:\t");
  Serial.println(id);

  if bitRead(scenes[id].protect, 0) storm(scenes[id]);
  if bitRead(scenes[id].protect, 1) updateServos(scenes[id]);

  if (!digitalRead(3)) { // Book signal
    id++;
    delay(3000);
    // player.next();
  }
}
/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////

//=======================================================
//====================== MAIN CODE ======================
//=======================================================
void setup() {
  delay(3000);
  pinMode(3, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("Lets go");
  ////// CLASS SETUP //////                                                                                                                                                                                                       
  initStructs();
  int time = millis();
  while (true) {
    Serial.println(round((millis() - time)/100));
    if (millis() - time > 1000) break;
  }
}

void loop() {
  update();
}