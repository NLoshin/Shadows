// Include libraries
#include <Arduino.h>
#include "Adafruit_NeoPixel.h"
#include "Adafruit_PWMServoDriver.h"

#define SCENECOUNT 9


// Define PWM parameters
#define SERVOCOUNT 4
#define Y 1

#define angleCoef 4096 / 180
#define colorCoef 4096 / 100

// Structure for position variables
struct position {byte x; byte y;};

// Structure for servos
struct servoStruct {
  position SAFEZONES[3];
  // Automatic variables 
  Adafruit_PWMServoDriver pwm;
  uint16_t length[3];
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

  void initScenes(sceneStruct sceneStructs[SCENECOUNT]) {
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
  }

  void initServos(servoStruct servoStructs[SERVOCOUNT]) {
    for (int i=0; i < SERVOCOUNT; i++) {
      servos[i] = servoStructs[i];
      // INIT PWM
      servos[i].pwm = Adafruit_PWMServoDriver(0x40+i);
      servos[i].pwm.begin();  
      servos[i].pwm.setPWMFreq(60);

      // INIT VARIABLES
      for (byte j; j < 3; j++) {
        servos[i].length[j] = abs(servos[i].SAFEZONES[j].x - servos[i].SAFEZONES[j].y);
      }
    }
  }

  // Function for display the storm
  void storm(sceneStruct scene) { 
    // TODO: Storm function
  }
  
  // Function for circuit frame
  void circuit(sceneStruct scene) {
    // TODO: Circuit function
  }

  // Function for color dioids
  void colorDiods(Adafruit_PWMServoDriver pwm, byte colors[3]) {
    for (uint8_t color=0; color < 3; color++) {
      for (uint8_t pin=0; pin < sizeof(RGBPINS[color]); pin++) {
        pwm.setPWM(RGBPINS[color][pin], 0, colors[color]);
      }
    }
  }

  // Function for move servos
  void updateServos(sceneStruct scene) {
    for (int i=0; i < SERVOCOUNT; i++) {
      colorDiods(servos[i].pwm, scenes[i].color);
    }
  }

  // MAIN CODE===============
  public:
  sceneStruct scenes[SCENECOUNT];
  servoStruct servos[SERVOCOUNT];
  
  // Initializate scene structures
  void init(sceneStruct sceneStructs[SCENECOUNT], servoStruct servoStructs[SERVOCOUNT]) {
    initScenes(sceneStructs);
    initServos(servoStructs);
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
    {},
    {},
    {},
    {}
  };

  sceneStruct sceneStructs[SCENECOUNT] = {
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
  scenes.init(sceneStructs, servoStructs);
}

void loop() {
  scenes.update();
}