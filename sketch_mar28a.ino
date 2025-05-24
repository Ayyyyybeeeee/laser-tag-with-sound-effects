/*
  ************************************************************************************
  * MIT License
  *
  * Copyright (c) 2025 Crunchlabs LLC (Laser Tag Code)

  * Permission is hereby granted, free of charge, to any person obtaining a copy
  * of this software and associated documentation files (the "Software"), to deal
  * in the Software without restriction, including without limitation the rights
  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  * copies of the Software, and to permit persons to whom the Software is furnished
  * to do so, subject to the following conditions:
  *
  * The above copyright notice and this permission notice shall be included in all
  * copies or substantial portions of the Software.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
  * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
  * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
  * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  *
  ************************************************************************************
*/
// >>>>>>>>>>>>>>>>>>>>>>>>>>>> PIN DEFINITIONS <<<<<<<<<<<<<<<<<<<<<<<<<<<<
#define IR_SEND_PIN         3
#define IR_RECEIVE_PIN      5 
#define _IR_TIMING_TEST_PIN 7
// note: the receive pin OF DFR1173 is not connected / not used
#define SOUND_SEND_PIN         2
#define SOUND_RECEIVE_PIN      4 
#define AUDIO_READ_DELAY    25


//#define LED_PIN     6
//#define LED_COUNT   6

//#define RELOAD_PIN      8
#define SERVO_PIN       9
//#define BUZZER_PIN      11 
#define TRIGGER_PIN     12

#define TEAM1_PIN       15      // A1 pin 
#define TEAM2_PIN       16      // A2 pin
#define TEAM3_PIN       17      // A3 pin

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> LIBRARIES <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#define DECODE_NEC          // defines RIR Protocol (Apple and Onkyo)

#include <IRremote.hpp>   
//#include <Adafruit_NeoPixel.h>  
#include <Arduino.h>
#include <Servo.h>
// USE SOFTWARE SERIAL FOR DFR1173 SO USB NOT AFFECTED
#include <SoftwareSerial.h>

//Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
Servo myservo;
SoftwareSerial *audioSerial;


// >>>>>>>>>>>>>>>>>>>>>>>>>>> GAME PARAMETERS <<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#define DEBOUNCE_DELAY 20

#define SERVO_INITIAL_POS 150     // how agressively to undarken goggles 
#define SERVO_READY_POS 120       // reduce aggresiveness near end of action
#define SERVO_HIT_POS 50

#define TRIGGER_COOLDOWN 500      // milliseconds  
#define HIT_TIMEOUT 10000         // milliseconds
//#define RELOAD_TIME_EACH 1000     // milliseconds

/*const bool infiniteAmmo = true;
const int maxAmmo = LED_COUNT;        
const bool deathTakesAmmo = true;*/          

int team = 1;     // default 

// >>>>>>>>>>>>>>>>>>>>>>>>>>> GAME VARIABLES <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

int lastTriggerVal = 1;                     // trigger debounce variable
unsigned long lastTDebounceTime = 0;        // trigger button debounce time
int triggerState;                           // trigger debounce result
bool buttonWasReleased = true;              // release check, no "full auto"
unsigned long previousTriggerMillis = 0;    // cooldown timestamp between shots

/*int lastReloadVal = 1;                      // reload button, debounce 
unsigned long lastRDebounceTime = 0;        // reload button debounce time 
int reloadState;                            // reload button debounce result

bool isReloading = false;                   // allows reloading sequence 
unsigned long reloadTimer;                  // time to add shots to ammo bar
int ammo = maxAmmo;                         // current ammo bootup at max
*/

// Initialize game timeout variable
unsigned long timeoutStartTime = - HIT_TIMEOUT - 1000;

// IR pulse, tracks team distinction
uint8_t sCommand;                            // IR command being sent
uint8_t rcvCommand1;                         // IR command being recieved
uint8_t rcvCommand2;                         // IR command being recieved
uint8_t soundCommand;                        // Audio file number for shoot
uint8_t dieCommand;                          // Audio file number for die


// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SETUP <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
void setup() {
  // Move Goggles to start config
  myservo.attach(SERVO_PIN);
  myservo.write(SERVO_INITIAL_POS);
  delay(500);
  myservo.write(SERVO_READY_POS);
  delay(500);
  myservo.detach();

  pinMode(TRIGGER_PIN, INPUT_PULLUP);
//  pinMode(RELOAD_PIN, INPUT_PULLUP);
//  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(TEAM1_PIN, INPUT_PULLUP);
  pinMode(TEAM2_PIN, INPUT_PULLUP);
  pinMode(TEAM3_PIN, INPUT_PULLUP);

  if (digitalRead(TEAM1_PIN) == LOW) {
    team = 1;
  } else if (digitalRead(TEAM2_PIN) == LOW) {
    team = 2;
  } else if (digitalRead(TEAM3_PIN) == LOW) {
    team = 3;
  }

  if (team == 1) {
    sCommand = 0x34;
    rcvCommand1 = 0x35;
    rcvCommand2 = 0x36;
    soundCommand =2;
    dieCommand =6;
  } else if (team == 2) {
    sCommand = 0x35;
    rcvCommand1 = 0x34;
    rcvCommand2 = 0x36;
    soundCommand =3;
    dieCommand =7;
  } else {
    sCommand = 0x36;
    rcvCommand1 = 0x34;
    rcvCommand2 = 0x35;
    soundCommand =5;
    dieCommand =8;
  }

  Serial.begin(115200);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  audioSerial = &SoftwareSerial(SOUND_RECEIVE_PIN,SOUND_SEND_PIN);
  audioSerial->begin(9600);
  //delay(800); // Give the module some time to initialize
  set_volume(20);
  delay(50);
  play_track(1); //  protongun_powerup

  /*
  strip.begin();
  strip.show();
  strip.setBrightness(50);
  */
}


// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> LOOP <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
void loop() {
  unsigned long currentMillis = millis();

  handleTrigger(currentMillis, false);
  handleIRReception();
  //UpdateLights(currentMillis);
  /*ReadReloadButton(currentMillis);

  if (isReloading && ammo < maxAmmo) {
    UpdateAmmo(true, currentMillis);
  }*/
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>> AUDIO SOUND EFFECT FUNCTIONS FOR DFR1173 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<

// Helper function to send a command to the MP3 module
void send_audio_command(uint8_t command[], size_t length) {
  for (size_t i = 0; i < length; i++) {
    audioSerial->write(command[i]);
  }
}

void get_audio_command(uint8_t command[], size_t length, uint8_t *result) {
  short count=0;
  for (size_t i = 0; i < length; i++) {
    audioSerial->write(command[i]);
  }
  while (audioSerial->available() == 0)
  {
    delay(AUDIO_READ_DELAY);
    count++;
    if (count>30)
    {
      *result=0;
      return;
    }
  }
  *result = audioSerial->read();
}

// Volume Control Functions
void set_volume(uint8_t level) {
  if (level > 30) {
    //Serial.println("Illegal volume %d, Volume level must be between 0 and 30.",(int)level);
    return;
  }
  uint8_t command[] = {0x7E, 0x06, 0x00, 0x02, 0x01, level, 0xEF};
  send_audio_command(command, sizeof(command));
  return;
}

// Playback Control Functions
void play_track_folder(uint8_t track_number, uint8_t folder_number) {
  uint8_t command[] = {0x7E, 0x0F, 0x00, 0x02, folder_number, track_number, 0xEF};
  send_audio_command(command, sizeof(command));
}

// Playback Control Functions
void play_track(uint8_t track_number) {
  play_track_folder(track_number, 2);
}



// >>>>>>>>>>>>>>>>>>>>>>>>>>>> GAMEPLAY FUNCTIONS <<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// Read Trigger ----------
void handleTrigger(unsigned long currentMillis, bool istimeout) {
  //if (ReadTriggerButton() && buttonWasReleased && ammo > 0 && currentMillis - previousTriggerMillis >= TRIGGER_COOLDOWN) {
  if (ReadTriggerButton() && buttonWasReleased && currentMillis - previousTriggerMillis >= TRIGGER_COOLDOWN) {
    previousTriggerMillis = currentMillis;
    buttonWasReleased = false;
    //isReloading = false;

    //digitalWrite(BUZZER_PIN, HIGH);
    set_volume(30);
    if (istimeout)
    {
      delay(10);
      play_track(4); //  alarm
    }
    else
    {
      sendIR_Pulse(false,false);
      play_track(soundCommand); // blaster
      //UpdateAmmo(false, currentMillis);
    }
    delay(10);
  } else if (!ReadTriggerButton()) {
    buttonWasReleased = true;
    //digitalWrite(BUZZER_PIN, LOW);
  } else {
    //digitalWrite(BUZZER_PIN, LOW);
  }
}

// Fire "Shot" ----------
void sendIR_Pulse(bool doFlush, bool doDelay) {
  if (doFlush)
    Serial.flush();
  IrSender.sendNEC(0x00, sCommand, 1);
  if (doDelay)
    delay(10);
}

// Read incoming message ----------
void handleIRReception() {
  if (IrReceiver.decode()) {
    checkPlayerHit();
    IrReceiver.resume(); // Ensure IR receiver is reset
  }
}

// Check if message is a "shot" from an enemy team ----------
void checkPlayerHit() {
  if (IrReceiver.decodedIRData.command == rcvCommand1 || IrReceiver.decodedIRData.command == rcvCommand2) {
    if (millis() - timeoutStartTime > HIT_TIMEOUT + 1000) {
      markHit();
    }
  }
}

// Move goggles if hit ----------
void markHit() {
  // get current time
  timeoutStartTime = millis();
  unsigned long currentMillis = millis();

  set_volume(30);
  delay(10);
  play_track(dieCommand); // r2d2 hit by jawas
  // move goggles to darken
  myservo.attach(SERVO_PIN);
  myservo.write(SERVO_HIT_POS);
  //digitalWrite(BUZZER_PIN, HIGH);
  //delay(10);
  //delay(500);


  // flash LEDs and vibrate buzzer during timeout. !!! section is blocking !!!
  while (millis() - timeoutStartTime < HIT_TIMEOUT) {
    // Flash LEDs RED
    /*
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(127.5 * (1.0 + sin(millis() / 100.0)), 0, 0));
    }
    strip.show();
    */

    currentMillis = millis();
    handleTrigger(currentMillis, true);


    // In last 20% of timeout, begin to move servo towards starting position
    // 20% is too soon, the glasses become clear, but the trigger won't soon
    // which is really obvious with the buzzer 'no shoot' sound.  
    // even 5% is too mucn, try making it 2%
    //int timeVal = (millis() - timeoutStartTime) / 100;
    if (millis() - timeoutStartTime >= (HIT_TIMEOUT * (0.98))) {
      myservo.write(SERVO_INITIAL_POS);
    }

    // Pulse the buzzer (uses moduluo millis)
    /*if (timeVal % 10 < 1) {
      digitalWrite(BUZZER_PIN, LOW);
    } else if (millis() < timeoutStartTime + min(HIT_TIMEOUT * (4.0 / 5.0), 3000)) {
      digitalWrite(BUZZER_PIN, HIGH);
    } else {
      digitalWrite(BUZZER_PIN, LOW);
    }*/
  }

  // after time out reset buzzer and lights 
  /*digitalWrite(BUZZER_PIN, LOW);
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();
  isReloading = false;
  if (deathTakesAmmo && !infiniteAmmo) {
    ammo = 0;
  }
  */
  myservo.detach();
}

// >>>>>>>>>>>>>>>>>>>>> RUN UI COMPONENTS <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
/*void UpdateLights(unsigned long currentMillis) {
  for (int i = 0; i < strip.numPixels(); i++) {
    if (i < ammo) {
      int green = max(20, 255 * (0.5 * (1.0 + sin(currentMillis / 1000.0 * PI + i / 6.0 * PI))));
      strip.setPixelColor(i, strip.Color(0, green, 0));
    } else {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
  }
  strip.show();
}*/

/*void UpdateAmmo(bool dir, unsigned long currentMillis) {
  if (ammo > maxAmmo) {
    ammo = maxAmmo;
    isReloading = false;
  }
  if (!infiniteAmmo) {
    if (!dir) {
      ammo -= 1;
    } else {
      int cVal = min(255.0, 255.0 * pow(double(currentMillis - reloadTimer) / double(RELOAD_TIME_EACH), 2.0));
      strip.setPixelColor(ammo - 1, strip.Color(cVal, cVal, cVal));
      strip.show();
      if (currentMillis - reloadTimer > RELOAD_TIME_EACH) {
        ammo += 1;
        reloadTimer = currentMillis;
      }
    }
  }
}*/

// >>>>>>>>>>>>>>>>>>>>>>> BUTTON DEBOUNCE FILTERS <<<<<<<<<<<<<<<<<<<<<<<<<
bool ReadTriggerButton() {
  int triggerVal = digitalRead(TRIGGER_PIN);
  if (triggerVal != lastTriggerVal) {
    lastTDebounceTime = millis();
  }
  if ((millis() - lastTDebounceTime) > DEBOUNCE_DELAY) {
    if (triggerVal != triggerState) {
      triggerState = triggerVal;
    }
  }
  lastTriggerVal = triggerVal;
  return triggerState == LOW;
}

/*void ReadReloadButton(unsigned long currentMillis) {
  int reloadVal = digitalRead(RELOAD_PIN);
  if (reloadVal != lastReloadVal) {
    lastRDebounceTime = currentMillis;
  }
  if ((currentMillis - lastRDebounceTime) > DEBOUNCE_DELAY) {
    if (reloadVal != reloadState) {
      reloadState = reloadVal;
    }
  }
  lastReloadVal = reloadVal;
  if (reloadState == LOW) {
    isReloading = true;
    reloadTimer = currentMillis;
  }
}*/
