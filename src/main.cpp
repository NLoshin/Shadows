// Include libraries
#include <Arduino.h>
#include "Adafruit_NeoPixel.h"
#include "Adafruit_PWMServoDriver.h"

#define SCENECOUNT 9



// Servo motors initialization with Adafruit PWM library
Adafruit_PWMServoDriver servo = Adafruit_PWMServoDriver();

// Structure for position variables
struct position {uint8_t x; uint8_t y;};

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
  float coefY;
  float coefNoise;
  float coefPixels;
  float coefTime;
};

// Class for show scenes
class sceneClass {
  private:
  // PRIVATE VARIABLES=======
  uint8_t id = 0;

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


      matrix.drawPixel(w,  0, matrix.Color(255, 255, 255));
      matrix.drawPixel(w,  9, matrix.Color(255, 255, 255));
    // TODO: Storm function
  }
   matrix.fillCircle(0,  9, 3, matrix.Color(255, 255, 255));
   if (id != SCENECOUNT-1) matrix.fillCircle(29, 9, 3, matrix.Color(255, 255, 255));
  
  // Function for circuit frame
  void circuit(sceneStruct scene) {
    // TODO: Circuit function
  }

      }
    }
  }

    }
  }

  // Function for check distance from UltraSonic
  bool checkDist() {
    digitalWrite(TRIGPIN,1);
    delayMicroseconds(10);
    digitalWrite(TRIGPIN,0);
    
    int distance = pulseIn(ECHOPIN, 1, 3000) / 58;
    return distance > 0;
  }

  // MAIN CODE===============
  public:
  // Initializate scene structures
  void init(sceneStruct sceneConfig[SCENECOUNT]) {
    for(int i; i<SCENECOUNT; i++) {
      scenes[i] = sceneConfig[i];
      if (scenes[i].startTime < 1000) scenes[i].startTime *= 1000;
      if (scenes[i].endTime   < 1000) scenes[i].endTime   *= 1000;
      int time = millis(); 
      scenes[i].startTime = scenes[i].startTime + time;
      scenes[i].endTime   = scenes[i].endTime   + time;
      scenes[i].time = scenes[i].endTime - scenes[i].startTime;
      scenes[i].coefX = 1.0 * (scenes[i].end.x - scenes[i].start.x) / scenes[i].time;
      scenes[i].coefZ = 1.0 * (scenes[i].end.y - scenes[i].start.y) / scenes[i].time;
    }
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
    // if (checkDist()) {
    if (long(millis() - scenes[id].endTime) > 0) {
      id++;
      delay(3000);
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
  ////// DISPLAY SETUP //////
  matrix.begin();
  // matrix.clear();
  matrix.setBrightness(255);
  ////// CLASS SETUP //////
    {0x42}};

    {34, 49,   0b0000010, {{0, 0, 0}, {255, 0, 0}, {}}},
    {52, 57,   0b0100010, {{0, 0, 0}, {255, 0, 0}, {}}, 3, {14, 4, 1}, {255, 50, 0}},

    {60, 62,   0b0100100, {},3},
    {65, 69,   0b0100100, {}, 3},
    {72, 75,   0b0100100, {}, 3, {14, 4, 1}, {255, 255, 0}},

    {78, 101,  0b0000001, {{255, 0, 0}, {0, 0, 0}, {255, 0, 0}}},
    {104, 120, 0b0100100, {}, 3}
  };                                                                                                                                                                                                               
  scenes.init(sceneConfig);
  // scenes.rainbowText();
  // matrix.show();
  ///// SERVOS SETUP /////
  servo.begin();

  ////// AUDIO MODEL SETUP //////
  // Serial.begin(115200);
  // player.volume(10);
  // player.play();
  ///// ENGINES SETUP /////
  pinMode(ENGINE1, OUTPUT);
  pinMode(ENGINE2, OUTPUT);
}

void loop() {
  scenes.update();
}