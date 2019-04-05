// Include librariesя
#include <Arduino.h>
#include "SPI.h"
#include "Adafruit_NeoMatrix.h"
#include "DFRobotDFPlayerMini.h"

// Define Matrix Parameters
#define W 30
#define H 10
#define PIN 10
#define sceneCount 9

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
  int noise[3];   // Rad, freq, count
  uint8_t Rad;
  position start; // X, Y
  position end;   // X, Y
  bool fill;
  unsigned long time;
  float coefX;
  float coefY;
  float coefNoise;
  float coefPixels;
  float coefTime;
};

// Class for work with matrix and motors
class sceneClass {
  private:
  // PRIVATE VARIABLES=======
  sceneStruct scenes[sceneCount] = {};
  uint8_t id = 0;

  // Function for shadow moving
  void moveCircle(sceneStruct scene) {
    float x; float y;
    do {
      x = scene.coefX * (millis() - scene.startTime) + scene.start.x;
      y = scene.coefY * (millis() - scene.startTime) + scene.start.y;
      if (x != int(x) && y != int(y)) {
        int color = (round(x * 100) + round(y * 100)) / 2 % 100;
        color = color / 100.0 * 255;
        if (color < 125) {
          matrix.fillCircle(int(x)+1, int(y)+1, scene.Rad, matrix.Color(color, color, color));
          matrix.fillCircle(int(x), int(y),     scene.Rad, matrix.Color(255-color, 255-color, 255-color));
        }
        else {
          matrix.fillCircle(int(x), int(y),     scene.Rad, matrix.Color(255-color, 255-color, 255-color));
          matrix.fillCircle(int(x)+1, int(y)+1, scene.Rad, matrix.Color(color, color, color));
        }
      }
      else if (x != int(x)) {
        int color = round(x * 100) % 100;
        color = color / 100.0 * 255;
        if (color < 125) {
          matrix.fillCircle(int(x)+1, int(y), scene.Rad, matrix.Color(color, color, color));
          matrix.fillCircle(int(x), int(y),   scene.Rad, matrix.Color(255-color, 255-color, 255-color));
        }
        else {
          matrix.fillCircle(int(x), int(y),   scene.Rad, matrix.Color(255-color, 255-color, 255-color));
          matrix.fillCircle(int(x)+1, int(y), scene.Rad, matrix.Color(color, color, color));
        }
      }
      else if (y != int(y)) {
        int color = round(y * 100) % 100;
        color = color / 100.0 * 255;
        if (color < 125) {
          matrix.fillCircle(int(x), int(y)+1, scene.Rad, matrix.Color(color, color, color));
          matrix.fillCircle(int(x), int(y),   scene.Rad, matrix.Color(255-color, 255-color, 255-color));
        }
        else {
          matrix.fillCircle(int(x), int(y),   scene.Rad, matrix.Color(255-color, 255-color, 255-color));
          matrix.fillCircle(int(x), int(y)+1, scene.Rad, matrix.Color(color, color, color));
        }
      }
      else matrix.fillCircle(int(x), int(y), scene.Rad, matrix.Color(255, 255, 255));
    } while (x > W && y > H);  // FIXME: 400000 pos
  }

  // Function for display the storm
  void noise(sceneStruct scene) { 
    uint8_t i=0;
    while (i < scene.noise[2]) {
      uint8_t a; uint8_t b;
      a = random(W); b = random(H);
      // } while ((pow(xy.x - a, 2) + pow(xy.y - b, 2)) > scene.noise[0]); //  TODO: THis shit
      matrix.drawPixel(a, b, matrix.Color(120, 120, 120));
      i++;
    }
  }

  // Function for display array of pixels
  void fills(sceneStruct scene) {
    for (uint8_t i; i < sizeof(scene.pixels); i++) {
      uint8_t color = (scene.endTime - millis()) * scenes[i].coefTime;
      Serial.println(color);
      matrix.drawPixel(scene.pixels[i][0], scene.pixels[i][1], matrix.Color(color, color, color));
      if (scene.fill) matrix.fillScreen(matrix.Color(color, color, color));
      Serial.println(i);
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
      scenes[i].time = scenes[i].endTime - scenes[i].startTime;
      scenes[i].coefX = 1.0 * (scenes[i].end.x - scenes[i].start.x) / scenes[i].time;
      scenes[i].coefY = 1.0 * (scenes[i].end.y - scenes[i].start.y) / scenes[i].time;
      scenes[i].coefTime = 1.0 * scenes[i].time / 255;
    }
  }

  // Function for update matrix
  void update() {
    matrix.clear();
    noise (scenes[id]);
    // fills (scenes[id]);
    matrix.show();
    if (long(millis() - scenes[id].endTime) > 0) id++;
  }
};

//=======================================================
//====================== MAIN CODE ======================
//=======================================================

sceneClass scenes;
void setup() {
  delay(3000);
  Serial.begin(9600);
  ////// DISPLAY SETUP //////
  matrix.begin();
  matrix.clear();
  matrix.setBrightness(100);
  matrix.show();

  sceneStruct sceneConfig[sceneCount] = { 
  //StartT ,EndT,  R, X0,Y0,   X1,Y1,  R,freq,count
    {0,     3000},
    {3000,  6000,  {3, 2000, 2}, 2, {0, 4}, {29, 4}},

    {6000, 9000,   {3, 2000, 2}},
    {9000, 12000,  {3, 2000, 2}},

    {12000, 15000, {3, 2000, 2}},
    {15000, 18000, {3, 2000, 2}},
    {18000, 21000, {3, 2000, 2}},

    {21000, 24000, {3, 2000, 2}},
    {24000, 27000, {3, 2000, 2}}
  };                                                                                                                                                                                                               
  scenes.init(sceneConfig);

  ////// AUDIO MODEL SETUP //////
  // Serial.begin(115200);
  // player.volume(10);
  // player.play();
}

void loop() {
  matrix.clear();
  scenes.update();
  matrix.show();
}