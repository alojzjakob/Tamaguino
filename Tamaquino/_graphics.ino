#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

// walking
int walkPos=0;
int walkXPos=0;
bool walkAnimReverse=false;
bool walkRight=false;
int walkDirOffset=2;

// ground
int grassXPos=0;
float treesXPos=-20;

// sky
float couldsXPos=0;
const int sunRadius=3;
bool sunOrMoon = false;
const int moonShadow=2;
float sunXPos=-2*sunRadius;

// clouds
const int cloud1Width=32;
float cloud1XPos=display.width()+cloud1Width;

// notification
int notificationBlink=0;

void setupGraphics() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
}

void resetDisplay() {
  display.clearDisplay();
  display.setCursor(0,0);
}

void drawSunMoon() {
  sunXPos += 0.1;
  if(sunXPos > display.width()+2*sunRadius){
    sunXPos = -2 * sunRadius;
    sunOrMoon = !sunOrMoon;
  }
  
  if(sleeping){
    sunOrMoon = true;
  }
  
  if(!sunOrMoon){
    display.fillCircle(sunXPos, 2 * sunRadius,sunRadius,WHITE);
  }else{
    display.fillCircle(sunXPos, 2 * sunRadius,sunRadius,WHITE);
    display.fillCircle(sunXPos - moonShadow, 2 * sunRadius,sunRadius,BLACK);

    if(round(cloud1XPos) % 5 == 0){
      for(int i=0;i<6;i++){
        stars[i][0]=random(0,display.width());
        stars[i][1]=random(0,10);
      }
    }else{
      for(int i=0;i<6;i++){
        
        display.drawPixel(stars[i][0],stars[i][1],WHITE);
      }
    }
  }
}

void drawCloud() {
  cloud1XPos-=0.3;
  if(cloud1XPos<-cloud1Width){
    cloud1XPos=display.width()+cloud1Width;
  }
  display.drawBitmap(cloud1XPos, 5, cloud2 , cloud1Width, 5, WHITE);
}

void drawMountains() {
  display.drawBitmap(0, 7, mountains , 128, 16, WHITE);
}

void drawDino() {
  if(!sleeping){
    display.drawBitmap(walkXPos, 26, dinoWalk[walkPos+walkDirOffset] , 48, 24, WHITE);
  }else{
    display.drawBitmap(walkXPos, 29, dinoWalk[walkPos+walkDirOffset] , 48, 24, WHITE);
    if(walkRight){
      if(round(cloud1XPos) % 3 ==0){
        display.setCursor(walkXPos+48,36);
        display.print(F("Z"));
      }else{
        display.setCursor(walkXPos+46,38);
        display.print(F("z"));
      }
    }else{
      if(round(cloud1XPos) % 3 ==0){
        display.setCursor(walkXPos-4,36);
        display.print(F("Z"));
      }else{
        display.setCursor(walkXPos-2,38);
        display.print(F("z"));
      }
    }
  }

  if(walkRight){
    if(!sleeping){
      walkXPos+=1;
      grassXPos+=2;
      treesXPos+=0.5;
    }
    if(walkXPos>80){walkRight=false; walkDirOffset=3;}
  } else{
    if(!sleeping){
      walkXPos-=1;
      grassXPos-=2;
      treesXPos-=0.5;
    }
    if(walkXPos<0){walkRight=true; walkDirOffset=0;}  
  }

  if(!sleeping){
    if(walkAnimReverse){
      --walkPos;
      if(walkPos==-1){walkPos=0; walkAnimReverse=false;}
    }else{
      ++walkPos;
      if(walkPos==3){walkPos=2; walkAnimReverse=true;}
    }
  }
}

void drawGround() {
  for(int i=0;i<2*display.width()/4;i++){
    display.drawBitmap(-walkXPos+i*8, 50, grass , 8, 6, WHITE);
  }
}

void drawPoops() {
  for(int i=0; i<3; i++){
    if(poopCoordinates[i]>0){
      display.drawBitmap(-walkXPos+poopCoordinates[i], 44, poop , 16, 6, WHITE);
    }
  }
}

void drawGrass() {
  for(int i=0;i<2*display.width()/16;i++){
    display.drawBitmap(-grassXPos+i*32, 56, grass_front , 32, 8, WHITE);
  }
}

void drawTrees() {
  display.drawBitmap(-treesXPos, 23, trees , 112, 20, WHITE);
}

void drawEatAnimation() {
  display.fillRect(0,0,display.width(),display.height(),BLACK);
  for(int j=0;j<3;j++){
    for(int i=0; i<4; i++){
      display.clearDisplay();

      display.drawBitmap(50,40,steak,24,24,WHITE);
      if(j>0) display.fillCircle(76,59,13,BLACK);
      if(j==2) display.fillCircle(60,63,13,BLACK);

      display.drawBitmap(80,24,eating[i],48,40,WHITE);
      delay(150);
      display.display();
    }
  }
}

void drawDoctorAnimation() {
  for(int i=0;i<5;i++){
    display.clearDisplay();
    if(i%2!=0){
      display.fillRect(32,23,64,16,WHITE);
      display.fillRect(56,0,16,64,WHITE);
    }
    display.display();
    delay(300);
  }
}

void drawNotification() {
  ++notificationBlink;
  if(notificationBlink==10){
    notificationBlink=0;
  }
  if(notificationBlink!=1){
    display.drawRect(117,28,11,11,WHITE);
    display.setTextColor(WHITE);
    digitalWrite(13,LOW);
  }else{
    display.fillRect(117,28,11,11,WHITE);
    display.setTextColor(BLACK);
    digitalWrite(13,HIGH);
  }
  display.setCursor(120,30);
  display.println(F("!"));
  if(dead){
      digitalWrite(13,LOW);
  }
}

void drawDead() {
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextColor(WHITE);
  display.println(F("Pet died...\n\nPress button 1\nto restart"));
}

void show() {
  display.display();
}