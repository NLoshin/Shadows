// Include librariesя
#include <Arduino.h>
#include "SPI.h"
#include "Adafruit_NeoMatrix.h"
#include "DFRobotDFPlayerMini.h"

// Define Matrix Parameters
#define W 30
#define H 10
#define PIN 2

// Matrix initialization with AdaFruit NeoMatrix
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(W, H, PIN, 
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
  NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB + NEO_KHZ800);

// Audio Player initialization with DFRobotPlayer
DFRobotDFPlayerMini player;

// Structure for position variables
struct position {uint8_t x; uint8_t y;};

// Structure for scenes
struct sceneStruct {
  unsigned long startTime;
  unsigned long endTime;
  position start; // X, Y
  position end;   // X, Y
  int noise[3];   // Rad, freq, count
  float coefX;
  float coefY;
  float coefNoise;
};

// Class for work with matrix and motors
class sceneClass {
  private:
  // PRIVATE VARIABLES=======
  static const uint8_t sceneCount = 3; ///////////////////////////////////////////////
  bool scene;
  sceneStruct scenes[sceneCount] = {};
  uint8_t id = 0;

  // Function for shadow moving
  position newPos(sceneStruct scene) {
    float x = scene.coefX * (millis() - scene.startTime) + scene.start.x;
    float y = scene.coefY * (millis() - scene.startTime) + scene.start.y;
    if (x != int(x)) {
      int color = round(x * 100) % 100;
      color = color / 100.0 * 255;
      matrix.drawPixel(int(x)+1, int(y), matrix.Color(color, color, color));
    }
    if (y != int(y)) {
      int color = round(y * 100) % 100;
      color = color / 100.0 * 255;
      matrix.drawPixel(int(x), int(y)+1, matrix.Color(color, color, color));
    }

    if (x != int(x) && y != int(y)) {
      int color = (round(x * 100) + round(y * 100)) / 2 % 100;
      color = color / 100.0 * 255;
      matrix.drawPixel(int(x), int(y), matrix.Color(255-color, 255-color, 255-color));
      matrix.drawPixel(int(x)+1, int(y)+1, matrix.Color(color, color, color));
    }
    else matrix.drawPixel(int(x), int(y), matrix.Color(255, 255, 255));
    return {x, y};
  }

  // Function for display the storm 
  void noise(int id, sceneStruct scene) { 
    uint8_t i;
    position xy = newPos(scene);
    uint8_t color = millis() % scene.noise[2] * scene.coefNoise;
    while (i < scene.noise[2]) {
      uint8_t a; uint8_t b;
      do { 
        a = random(H); b = random(W);
      } while ((pow(xy.x - a, 2) + pow(xy.y - b, 2)) > scene.noise[0]);
      matrix.drawPixel(int(a), int(b), matrix.Color(color, color, color));
    }
  }


  // MAIN CODE===============
  public:
  // Scene count value
  // Initializate classes function
  void init(sceneStruct sceneConfig[sceneCount]) {
    for(int i; i<sceneCount; i++) {
      scenes[i] = sceneConfig[i];
      if (scenes[i].startTime < 1000) scenes[i].startTime *= 1000;
      if (scenes[i].endTime   < 1000) scenes[i].endTime   *= 1000;
      scenes[i].startTime = scenes[i].startTime + millis();
      scenes[i].endTime   = scenes[i].endTime   + millis();
      scenes[i].coefX = 1.0 * (scenes[i].end.x - scenes[i].start.x) / (scenes[i].endTime - scenes[i].startTime);
      scenes[i].coefY = 1.0 * (scenes[i].end.y - scenes[i].start.y) / (scenes[i].endTime - scenes[i].startTime);
      scenes[i].coefNoise = scenes[i].noise[1] / 255;
    }
  }

  // Function for update matrix
  void update() {
    newPos(scenes[id]);
    matrix.show();
    if (long(millis() - scenes[id].endTime) > 0) id++;
  }
};

//=======================================================
//====================== MAIN CODE ======================
//=======================================================

sceneClass scenes;
void setup() {
  delay(5000);
  ////// DISPLAY SETUP //////
  matrix.begin();
  matrix.clear();
  matrix.setBrightness(100);
  matrix.show();

  sceneStruct sceneConfig[3] = { 
  //StartT ,EndT,  X0,Y0,   X1,Y1,   R,freq,count
    {1000, 15000,  {4, 4},  {20, 8}, {3, 500, 4}},
    {15000, 40000, {20, 8}, {3, 3},  {3, 500, 4}},
    {40000, 50000, {20, 8}, {3, 3},  {3, 500, 4}}
  };
  scenes.init(sceneConfig);

  ////// AUDIO MODEL SETUP //////
  Serial1.begin(115200);
  player.volume(10);
  player.play();
}

void loop() {
  scenes.update();
}