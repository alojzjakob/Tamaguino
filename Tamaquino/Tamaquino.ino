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

/* ------- PET STATS ------- */

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
}

void loop() {

  if (dead) {
    drawDead();
    show();

    if(digitalRead(button1Pin)){
      // Reset (low-level command to go to first line of code)
      asm volatile ("  jmp 0");
    }

    return;
  }

  /* -------- MODIFY PET STATS -------- */
  if (energy > 0) {
    energy -= sleeping ? 0.00005 : 0.00025;
  }

  if (happiness > 0) {
    happiness -= sleeping ? 0.0002 : 0.0001;
  }

  poopometer += sleeping ? 0.00005 : 0.00025;

  if(poopometer >= 100){
    int poopNumber = countPoops();
    poopCoordinates[round(poopNumber)]=random(20,DISPLAY_WIDTH+32);
    poopometer=0;
  }

  if(energy <= 0 && happiness <= 0){
    dead=true;
  }
  
  /* ------- BUTTON 1 - Food ------- */
  if(digitalRead(button1Pin) == LOW){
    drawEatAnimation();

    if (energy < 100) {
      energy += 20;
    }
    
    poopometer += 0.05;
  }
  /* ------- BUTTON 2 - Attention ------- */
  if(digitalRead(button2Pin) == LOW){
    if (happiness < 100) {
      happiness += 15;
    }
  }
  /* ------- BUTTON 3 - Cleanup ------- */
  if(digitalRead(button3Pin) == LOW){
    resetPoops();
  }

  resetDisplay();

  drawSunMoon();

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
