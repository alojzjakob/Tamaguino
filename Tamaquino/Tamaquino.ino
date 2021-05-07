/* Tamaquino
 based on Tamaguino by Alojz Jakob <http://jakobdesign.com>
 greatly simplified by Q42 <https://www.q42.nl/>

 ********** TAMAQUINO ***********
 * Tamagotchi clone for Arduino *
 ********************************
 
*/

#include <SPI.h>
#include <Wire.h>

#define DISPLAY_WIDTH 128

const int button1Pin = 9;
const int button2Pin = 8; 
const int button3Pin = 7;
const unsigned long dayNightCycleSeconds = 240;
const int dayStartTime = 800;
// Calculate the millis offset so our program always starts at dawn instead of midnight
const unsigned long millisOffset = dayStartTime / 2400.0 * dayNightCycleSeconds * 1000;

/* ------- PET STATS ------- */
unsigned long previousUpdateTime = 0;

float energy = 100;
float happiness = 100;

bool dead = false;

bool sleeping = false;

float poopometer = 0;
int poopCoordinates[3] = { 0,0,0 };

void setup() {
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(button3Pin, INPUT_PULLUP);

  pinMode(13, OUTPUT);

  // Make sure our random function does not do the same thing every time
  randomSeed(analogRead(0));

  setupGraphics();

  Serial.begin(115200);
}

void loop() {

  if (dead) {
    drawDead();
    show();

    if(digitalRead(button1Pin) == LOW){
      // Reset (low-level command to go to first line of code)
      asm volatile ("  jmp 0");
    }

    return;
  }

  // Calculate time from 0 to 2400
  int time = ((millis() + millisOffset) % (dayNightCycleSeconds * 1000)) * (2400.0 / (dayNightCycleSeconds * 1000.0));

  // Update stats once per second
  if (previousUpdateTime + 1000 < millis()) {
    if (energy > 0) {
      energy -= sleeping ? 0.0009 : 0.0045;
    }

    if (happiness > 0) {
      happiness -= sleeping ? 0.0016 : 0.0024;

      if (energy < 20) {
        happiness -= sleeping ? 0.0016 : 0.0024;
      }

      if (countPoops() > 0 && !sleeping) {
        happiness -= 0.0024;
      }
    }

    poopometer += sleeping ? 0.001 : 0.005;

    if(poopometer >= 100 && countPoops() < 3){
      int poopNumber = countPoops();
      poopCoordinates[poopNumber]=random(20,DISPLAY_WIDTH+32);
      poopometer = 0;
    }

    if(energy <= 0 && happiness <= 0){
      dead=true;
    }

    /* ------- DEBUG ------- */
    Serial.print(energy);
    Serial.print('\t');
    Serial.print(happiness);
    Serial.print('\t');
    Serial.print(poopometer);
    Serial.println();

    previousUpdateTime = millis();
  }
  
  /* ------- BUTTON 1 - Food ------- */
  if(digitalRead(button1Pin) == LOW){
    drawEatAnimation();

    energy += 20;
    if (energy > 100) {
      energy = 100;
    }
    
    poopometer += 0.1 * energy;
  }
  /* ------- BUTTON 2 - Attention ------- */
  if(digitalRead(button2Pin) == LOW){
    happiness += 15;

    if (happiness > 100) {
      happiness = 100;
    }
  }
  /* ------- BUTTON 3 - Cleanup ------- */
  if(digitalRead(button3Pin) == LOW){
    resetPoops();
  }

  resetDisplay();

  drawSunMoon(time);

  drawCloud();

  drawMountains();

  drawDino();
  
  drawGround();
  
  drawPoops();
  
  drawGrass();
  
  drawTrees();

  if(energy <= 20 || countPoops() > 0 || happiness <= 20){
    drawNotification();
  } else {
    digitalWrite(13, LOW);
  }

  show();
}

int countPoops(){
  int poopsCnt = 0;
  for(int i=0; i<3; i++){
    if(poopCoordinates[i]>0){
      ++poopsCnt;
    }
  }
  return poopsCnt;
}

void resetPoops(){
  for(int i=0; i<3; i++){
    poopCoordinates[i]=0;
  }
}
