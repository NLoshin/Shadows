// Include libraries
#include <Arduino.h>
#include "Adafruit_NeoPixel.h"
#include "Adafruit_PWMServoDriver.h"

// Define Matrix Parameters
#define W 30
#define H 10
#define MATRIXPIN 10
#define SCENECOUNT 9

// Define Sensors Parameters
#define ECHOPIN 4
#define TRIGPIN 5


// Servo motors initialization with Adafruit PWM library
Adafruit_PWMServoDriver servo = Adafruit_PWMServoDriver();

// Structure for position variables
struct position {uint8_t x; uint8_t y;};

// Structure for scenes
struct sceneStruct {
  unsigned long startTime;
  unsigned long endTime;
  uint8_t protect;
  unsigned int colors[3][3];
  uint8_t noiseCount;
  unsigned int singlePixPos[3];
  unsigned int singlePixColor[3];
  uint8_t Rad;
  position start; // X, Y
  position end;   // X, Y
  // Automatic variables
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
  sceneStruct scenes[SCENECOUNT] = {};
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

  // Function for move circle
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
    } while (x > W && y > H);  // FIX: 400000 pos
  }

  // Function for display the storm
  void storm(sceneStruct scene) { 
    uint8_t i=0;
    while (i < scene.noiseCount) {
      uint8_t a; uint8_t b;
      a = random(W); b = random(H);
      matrix.drawPixel(a, b, matrix.Color(120, 120, 120));
      i++;
    }
  }

  // Function for fill screen
  void fill(sceneStruct scene) {
    matrix.clear();
    uint8_t color = (millis() - scene.startTime) / scene.coefTime + 50;
    if (millis() - scene.startTime > scene.time/2) color = 255 - color - 100;
    matrix.fillScreen(matrix.Color(color, color, color));
  }

  // Function for circuit matrix
  void circuit() {
    for(uint8_t w=0;w<30;w++) {
      matrix.drawPixel(w,  0, matrix.Color(255, 255, 255));
      matrix.drawPixel(w,  9, matrix.Color(255, 255, 255));
    }
    for(uint8_t h=0;h<10;h++) {
      matrix.drawPixel(0,  h, matrix.Color(255, 255, 255));
      matrix.drawPixel(29, h, matrix.Color(255, 255, 255));
    }  
  }

  // Function for display circles in bottom angles
  void angles() {
   matrix.fillCircle(0,  9, 3, matrix.Color(255, 255, 255));
   if (id != SCENECOUNT-1) matrix.fillCircle(29, 9, 3, matrix.Color(255, 255, 255));
  }

  // Function for vertical gradient matrix
  void Vgradient(sceneStruct scene) {
    for(uint8_t h = 0; h <= H; h++) {
      uint8_t R = scene.colors[0][0] + (scene.colors[1][0] - scene.colors[0][0]) * 1.0 * h/H * (millis() - scene.startTime) / scene.coefTime;
      uint8_t G = scene.colors[0][1] + (scene.colors[1][1] - scene.colors[0][1]) * 1.0 * h/H * (millis() - scene.startTime) / scene.coefTime;
      uint8_t B = scene.colors[0][2] + (scene.colors[1][2] - scene.colors[0][2]) * 1.0 * h/H * (millis() - scene.startTime) / scene.coefTime;
      uint16_t color = matrix.Color(R, G, B);
      for(uint8_t w = 0; w <= W; w++) {
        matrix.drawPixel(w, h, matrix.Color(R, G, B));
      }
    }
  }

  // Function for triple horizontal gradient
  void H3gradient(sceneStruct scene) {
    for (int column =0; column < 10; column++) {
      for (int row =0; row < 30; row++) {
        float timeCoef = (millis() - scene.startTime) / scene.coefTime;
        if (row <=15) matrix.drawPixel(row, column, matrix.Color((255-row*17)*timeCoef, 0, 0)); // CHECK: *timeCoef
        else matrix.drawPixel(row, column, matrix.Color((row*17)*timeCoef, 0, 0));
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
      scenes[i].coefY = 1.0 * (scenes[i].end.y - scenes[i].start.y) / scenes[i].time;
      scenes[i].coefTime = 1.0 * scenes[i].time / 255;
      scenes[i].singlePixColor[0] = matrix.Color(scenes[i].singlePixColor[0], scenes[i].singlePixColor[1], scenes[i].singlePixColor[2]);
    }
  }

  // Function for display rainbow "SHADOWS" text
  void rainbowText() {
    for (int column =0; column < 10; column++) {
      for (int row =0; row < 30; row++) {
        if (bitRead(shadowsText[column], 29-row)) {
          if (row <=15) matrix.drawPixel(row, column, matrix.Color(255-row*17, row*17, 0));
          else          matrix.drawPixel(row, column, matrix.Color(0, 255-row*17, row*17));
        }
      }
    }
  }

  // Function for update matrix
  void update() {
    if bitRead(scenes[id].protect, 0) H3gradient(scenes[id]);
    if bitRead(scenes[id].protect, 1) Vgradient(scenes[id]);
    if bitRead(scenes[id].protect, 2) angles();
    if bitRead(scenes[id].protect, 3) fill(scenes[id]);//circuit();
    if bitRead(scenes[id].protect, 4) fill(scenes[id]);
    if bitRead(scenes[id].protect, 5) storm(scenes[id]);
    if bitRead(scenes[id].protect, 6) moveCircle(scenes[id]);
    matrix.fillCircle(scenes[id].singlePixPos[0], scenes[id].singlePixPos[1], scenes[id].singlePixPos[2], scenes[id].singlePixColor[0]);

    Serial.print("Update:\t");
    Serial.println(id);
    // if (checkDist()) {
    if (long(millis() - scenes[id].endTime) > 0) {
      id++;
      matrix.clear();
      matrix.show();
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
  matrix.show();
  sceneStruct sceneConfig[SCENECOUNT] = {
    {0,  20,   0b0010000},
    {23, 31,   0b1100000, {}, 3, {}, {}, 2, {0, 4}, {29, 4}},

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

  ////// AUDIO MODEL SETUP //////
  // Serial.begin(115200);
  // player.volume(10);
  // player.play();
  ///// SENSORS SETUP /////
  pinMode(ECHOPIN, INPUT);
  pinMode(TRIGPIN, OUTPUT);
}

void loop() {
  matrix.clear();
  scenes.update();
  matrix.show();
}