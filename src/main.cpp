// Include libraries
#include <Arduino.h>
#include "Adafruit_NeoPixel.h"
#include "Adafruit_PWMServoDriver.h"

#define SCENECOUNT 9


#define SERVOCOUNT 3
#define Y 1

#define angleCoef 23 // 4096 / 180

// Servo motors initialization with Adafruit PWM library
Adafruit_PWMServoDriver servo = Adafruit_PWMServoDriver();

// Structure for position variables
struct position {byte x; byte y;};

// Structure for servos
struct servoStruct {
  byte pwmPin;
  int SAFEZONES[3][2];
  // Automatic variables 
  Adafruit_PWMServoDriver pwm;
};

// Structure for scenes
struct sceneStruct {
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
  float coefX;
  float coefZ;
  float coefNoise;
  float coefPixels;
};

// Class for show scenes
class sceneClass {
  private:
  // PRIVATE VARIABLES=======
  byte id = 0;

  const long shadowsText[10] = {
    0b111101001001100110001111010001,
    0b100001001010010101001001010001,
    0b100001001010010100101001010001,
    0b100001001010010100101001010101,
    0b111101111011110100101001010101,
    0b111101111011110100101001010101,
    0b000101001010010100101001010101,
    0b000101001010010100101001010101,
    0b000101001010010101001001010101,
    0b111101001010010110001111011111
  };

  // Function for display the storm
  void storm(sceneStruct scene) { 
    // TODO: Storm function
  }
  
  // Function for circuit frame
  void circuit(sceneStruct scene) {
    // TODO: Circuit function
  }

      }
    }
  }

  // Function for move servos
  void updateServos(sceneStruct scene) {
    }
  }

  // MAIN CODE===============
  public:
  sceneStruct scenes[SCENECOUNT];
  servoStruct servos[SERVOCOUNT];
  // Initializate scene structures
  void init(sceneStruct sceneStructs[SCENECOUNT], servoStruct servoStructs[SERVOCOUNT]) {
    for(int i=0; i<SCENECOUNT; i++) {
      scenes[i] = sceneStructs[i];
      if (scenes[i].startTime < 1000) scenes[i].startTime *= 1000;
      if (scenes[i].endTime   < 1000) scenes[i].endTime   *= 1000;
      unsigned int time = millis(); 
      scenes[i].startTime = scenes[i].startTime + time;
      scenes[i].endTime   = scenes[i].endTime   + time;
      scenes[i].time = scenes[i].endTime - scenes[i].startTime;
      scenes[i].coefX = 1.0 * (scenes[i].end.x - scenes[i].start.x) / scenes[i].time;
      scenes[i].coefZ = 1.0 * (scenes[i].end.y - scenes[i].start.y) / scenes[i].time;
    }
    for (int i=0; i < SERVOCOUNT; i++) {
      servos[i] = servoStructs[i];
      servos[i].pwm = Adafruit_PWMServoDriver(servos[i].pwmPin);
      servos[i].pwm.begin();
      servos[i].pwm.setPWMFreq(60);
      for (int j=0; j < sizeof(servos[i].SAFEZONES); j++) {
        for (int l=0; l < 2; l++) {
          servos[i].SAFEZONES[j][l] = servos[i].SAFEZONES[j][l] * angleCoef; 
        }
      }
    }
  }

  // Function for update matrix
  void update() {
    if bitRead(scenes[id].protect, 0) storm(scenes[id]);
    // if bitRead(scenes[id].protect, 3) updateServos(scenes[id]);
    
    Serial.print("Update:\t");
    Serial.println(id);

    if (false) { // Book signal
      id++;
      // player.next();
    }
  }
};

//=======================================================
//====================== MAIN CODE ======================
//=======================================================

sceneClass scenes;
void setup() {
  delay(5000);
  Serial.begin(9600);
  ////// CLASS SETUP //////
  servoStruct servoStructs[SERVOCOUNT] {
    {0x40},
    {0x41},
    {0x42}};

  sceneStruct sceneStructs[SCENECOUNT] = {
    {0,   20, 0b10},
    {23,  31, 0b10},

    {34,  49, 0b10},
    {52,  57, 0b10},

    {60,  62, 0b10},
    {65,  69, 0b10},
    {72,  75, 0b10},

    {78,  101, 0b10},
    {104, 120, 0b10}
  };                                                                                                                                                                                                               
  scenes.init(sceneStructs, servoStructs);

  ///// SERVOS SETUP /////
  servo.begin();
  servo.setPWMFreq(60);
  delay(500);

  ///// ENGINES SETUP /////
  pinMode(ENGINE1, OUTPUT);
  pinMode(ENGINE2, OUTPUT);
}

void loop() {
  scenes.update();
}