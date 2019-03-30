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
  unsigned int startTime;
  unsigned int endTime;
  uint8_t Rad;
  position start; // X, Y
  position end;   // X, Y
  int noise[3];   // Rad, freq, count
  uint8_t pixels[5][2];
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
  static const uint8_t sceneCount = 3; ///////////////////////////////////////////////
  bool scene;
  sceneStruct scenes[sceneCount] = {};
  uint8_t id = 0;

  // Function for shadow moving
  position mainMove(sceneStruct scene) {
    float x; float y;
    do {
      x = scene.coefX * (millis() - scene.startTime) + scene.start.x;
      y = scene.coefY * (millis() - scene.startTime) + scene.start.y;
      if (x != int(x) && y != int(y)) {
        int color = (round(x * 100) + round(y * 100)) / 2 % 100;
        color = color / 100.0 * 255;
        matrix.fillCircle(int(x)+1, int(y)+1, scene.Rad, matrix.Color(color, color, color));
        matrix.fillCircle(int(x), int(y),     scene.Rad, matrix.Color(255-color, 255-color, 255-color));
      }
      else if (x != int(x)) {
        int color = round(x * 100) % 100;
        color = color / 100.0 * 255;
        matrix.fillCircle(int(x)+1, int(y), scene.Rad, matrix.Color(color, color, color));
        matrix.fillCircle(int(x), int(y),   scene.Rad, matrix.Color(255-color, 255-color, 255-color));
      }
      else if (y != int(y)) {
        int color = round(y * 100) % 100;
        color = color / 100.0 * 255;
        matrix.fillCircle(int(x), int(y)+1, scene.Rad, matrix.Color(color, color, color));
        matrix.fillCircle(int(x), int(y),   scene.Rad, matrix.Color(255-color, 255-color, 255-color));
      }
      else matrix.fillCircle(int(x), int(y), scene.Rad, matrix.Color(255, 255, 255));
      Serial.print(x);
      Serial.print('\t');
      Serial.println(y);
    } while (x > W && y > H);  // FIXME: 400000 pos
      return {x, y};
  }

  // Function for display the storm 
  void noise(sceneStruct scene) { 
    uint8_t i;
    position xy = mainMove(scene);
    uint8_t color = millis() % scene.noise[2] * scene.coefNoise;
    while (i < scene.noise[2]) {
      uint8_t a; uint8_t b;
      do { 
        a = random(H); b = random(W);
      } while (false);
      // } while ((pow(xy.x - a, 2) + pow(xy.y - b, 2)) > scene.noise[0]); //  TODO: THis shit
      matrix.drawPixel(a, b, matrix.Color(color, color, color));
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
      scenes[i].time = scenes[i].endTime - scenes[i].startTime;
      if (scenes[i].startTime < 1000) scenes[i].startTime *= 1000;
      if (scenes[i].endTime   < 1000) scenes[i].endTime   *= 1000;
      scenes[i].startTime = scenes[i].startTime + millis();
      scenes[i].endTime   = scenes[i].endTime   + millis();
      scenes[i].coefX = 1.0 * (scenes[i].end.x - scenes[i].start.x) / scenes[i].time;
      scenes[i].coefY = 1.0 * (scenes[i].end.y - scenes[i].start.y) / scenes[i].time;
      scenes[i].coefNoise  = scenes[i].noise[1] / 255;
      scenes[i].coefTime = scenes[i].time / 255;
    }
  }

  // Function for update matrix
  void update() {
    matrix.clear();
    mainMove(scenes[id]);
    noise (scenes[id]);
    fills (scenes[id]);
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
  //StartT ,EndT,  X0,Y0,   X1,Y1,  R,freq,count
    {1000, 5000,   1, {4, 4}, {8, 8},  {3, 500, 2}, {{0, 0}, {0, 1}, {0, 2}}, true},
    {5000, 10000 , 1, {8, 8}, {8, 2},  {3, 500, 2}, true},
    {10000, 50000, 1, {8, 2}, {0, 2},  {3, 500, 2}, true}
  };                         
  Serial.println("Init");                                                                                                                                                                                           
  scenes.init(sceneConfig);

  ////// AUDIO MODEL SETUP //////
  Serial1.begin(115200);
  player.volume(10);
  player.play();
}

void loop() {
  scenes.update();
}