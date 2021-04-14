/* Tamaguino
 by Alojz Jakob <http://jakobdesign.com>

 ********** TAMAGUINO ***********
 * Tamagotchi clone for Arduino *
 ********************************
 
*/

#include <SPI.h>
#include <Wire.h>

#define DISPLAY_WIDTH 128

const int button1Pin = 9;
const int button2Pin = 8; 
const int button3Pin = 7; 

// menus
bool menuOpened = false;
int menu=0;
int subMenu=1;
bool menuDepth=false;
bool justOpened=false;
#define MENUSIZE 8
#define STRING_SIZE 11

const char mainMenu[MENUSIZE][8][STRING_SIZE] PROGMEM = {
  {"food","apple","steak","water",NULL},
  {"game",NULL},
  {"sleep",NULL},
  {"clean",NULL},
  {"doctor",NULL},
  {"discipline",NULL},
  {"stats","hunger","happiness","health","discipline","weight","age",NULL},
  {"settings","sound",
    //"something",
    NULL
  },
};

/* ------- PET STATS ------- */

float hunger=100;
float happiness=100;
float health=100;
float discipline=100;
float weight=1;
float age=0;

int action=0;
int setting=0;

bool notification = false;
int notificationBlink=0;
bool dead=false;

bool sleeping=false;

float poopometer=0;
int poops [3] = {
  0,0,0,
};

void setup() {
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(button3Pin, INPUT_PULLUP);

  pinMode(13,OUTPUT);

  randomSeed(analogRead(0));

  setupGraphics();
}



void loop() {  
  if(!dead){
    /* -------- MODIFY PET STATS -------- */
    // TODO: different gradients regarding to age
    if(sleeping){
      hunger-=0.00005;
      poopometer+=0.00005;
      if(happiness-0.0001>0){
        happiness-=0.0001;
      }
      health-=0.00005+countPoops()*0.0001;
      if(discipline-0.0001>0){
        discipline-=0.0001;
      }
    }else{
      hunger-=0.00025;
      poopometer+=0.00025;
      if(happiness-0.0002>0){
        happiness-=0.0002;
      }
      health-=0.0001+countPoops()*0.0001;
      if(discipline-0.0002>0){
        discipline-=0.0002;
      }
      //discipline-=0.02;
    }
    age+=0.0000025;

    if(poopometer>=10){
      poopometer=countPoops();
      poops[round(poopometer)]=random(20,DISPLAY_WIDTH+32);
      poopometer=0;
    }
    
    if(hunger<=20 || countPoops()>0 || happiness<=20 || health<=20){
      notification=true;  
    }
    if(hunger>20 && countPoops()==0 && happiness>20 && health>20){
      notification=false;  
      digitalWrite(13,LOW);
    }

    if(hunger<=0 || health<=0 || happiness<=0){
      dead=true;
    }

    resetDisplay();  
  
    /* ------- BUTTON PRESS ACTIONS ------- */
    
    /* ------- BUTTON 1 - MENU ------- */
    if(digitalRead(button1Pin) == LOW){
      // MENU
    
      if(!menuOpened){
        menuOpened=true;
      }else{
        if(menuDepth){

          if((const char*)pgm_read_word(&(mainMenu[menu][subMenu+1]))==NULL){
            subMenu=1;
          }else{
            ++subMenu;
          }
          setting=100*(menu+1)+subMenu;
        }else{
          if(menu==MENUSIZE-1){
            menu=0;
          }else{
            ++menu;
          }
          
          if((const char*)pgm_read_word(&(mainMenu[menu][1]))!=NULL){
            subMenu=1;
            
            justOpened=true;
          }
          setting=100*(menu+1)+subMenu;
        }
      }

      delay(60);
    }
    /* ------- BUTTON 2 - SELECT ------- */
    if(digitalRead(button2Pin) == LOW){
      if(menuOpened){

        if(subMenu!=1 && (const char*)pgm_read_word(&(mainMenu[menu][1][0]))!=NULL){
          action=100*(menu+1)+subMenu; 
        }
        if(subMenu==1 && (const char*)pgm_read_word(&(mainMenu[menu][1][0]))==NULL){
          action=100*(menu+1)+subMenu;
        }
        if(subMenu==1 && (const char*)pgm_read_word(&(mainMenu[menu][1][0]))!=NULL && menuDepth){
          action=100*(menu+1)+subMenu; 
        }
        if((const char*)pgm_read_word(&(mainMenu[menu][1][0]))!=NULL){
          setting=100*(menu+1)+subMenu;
          menuDepth=true;
        }
        
      }else{
        action=NULL;
        
        menuOpened=true;
        menuDepth=true;
        subMenu=1;
        menu=6;
        action=701;
        setting=701;
      }
      justOpened=false;

      delay(60);
    }
    /* ------- BUTTON 3 - BACK ------- */
    if(digitalRead(button3Pin) == LOW){
      if(!menuDepth){
        menuOpened=false;
        menu=0;
        setting=0;
      }else{
        menuDepth=false;
        setting=100*(menu+1)+1;
      }
      action=NULL;
      subMenu=1;

      delay(60);
    }
  
  
  
    /* ------- SCENERY AND WALKING ------- */
  
    drawSunMoon();
  
    drawCloud();

    drawMountains();
  
    drawDino();
    
    drawGround();
    
    drawPoops();
    
    drawGrass();
    
    drawTrees();


      
    
  
  
    /* ------- MENUS AND ACTIONS ------- */
    //render menu
    if(menuOpened){
      drawMenu();
    }
  
    //do actions

    if(action>0){


      if((action==101 || action==102 || action==103) && !sleeping && random(1,(11-round(discipline/10)))==1 ){
        //95-100 discipline = 100% chance to feed
        //85-95 discipline = 50% chance
        //75-85 discipline = 33.33% chance
        //65-75 discipline = 25% chance
        //55-65 discipline = 20% chance
        //45-55 discipline = 16.67% chance
        //35-45 discipline = 14.28% chance
        //25-35 discipline = 12.5% chance
        //15-25 discipline = 12.5% chance
        //5-15 discipline = 10% chance
        //0-5 discipline = 9% chance

        //animate eating
        
        drawEatAnimation();
        
        switch(action){
          //apple
          case 101:
            if(hunger+10>100){
              hunger=100;
              weight+=0.1;
            }else{
              hunger+=10;
            }
            if(health+1<=100){
              health+=1;
            }
            poopometer+=0.02;
            break;
          //steak
          case 102:
            if(hunger+20>100){
              hunger=100;
              weight+=0.2;
            }else{
              hunger+=20;
              weight+=0.1;
            }
            if(health-1>0){
              health-=1;
            }
            poopometer+=0.05;
            break;
          //water
          case 103:
            if(hunger+5<=100){
              hunger+=5;
            }
            poopometer+=0.01;
            break;

        }
      }

      switch(action){
        case 301:
          //sleep
          sleeping=!sleeping;

          break;
        case 401:
          //bath
          resetPoops();
          break;
        case 501:
          //doctor
          if(health<60){
            health=100;
            drawDoctorAnimation();
          }
              
          break;
        case 601:
          //discipline
          if(action==601 && !sleeping){
            if(discipline+12<=100){
              discipline+=12;
            }else{
              discipline=100;
            }
            if(happiness-3>0){
              happiness-=3;
            }
            delay(150);
            drawWarnAnimation();
            
          }
          break;
      }
      action=0;
    }
  
    if(notification){
      drawNotification();
    }

    show();
  }else{
    drawDead();
    show();

    if(digitalRead(button1Pin)){
      asm volatile ("  jmp 0");
    }
  }
}

int countPoops(){
  int poopsCnt = 0;
  for(int i=0; i<3; i++){
    if(poops[i]>0){
      ++poopsCnt;
    }
  }
  return poopsCnt;
}

void resetPoops(){
  for(int i=0; i<3; i++){
    poops[i]=0;
  }
}
