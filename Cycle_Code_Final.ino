//LIBRARIES
#include <CountUpDownTimer.h>
#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include <SFEMP3Shield.h>
#include <APA102.h>

//ARDUINO PINS
const int magSensor = 5;
const int toyPin = 10;
const int dwellPotPin = A0; 
const int audioEnable = A1; 
const int lightEnable = A2; 
const uint8_t dataPin = A3;
const uint8_t clockPin = A4;

//VARIABLE INITIALIZATION
CountUpDownTimer conductorTimer(DOWN); //Timer instance
SdFat sd; //SD Card
SFEMP3Shield MP3player; //MP3 shield
int8_t current_song = 1; //current track playing from SD card
boolean began = false; //Boolean stating whether the first song has begun
int updateCount = 0; //Number of times conductor's timer has been reset
APA102<dataPin, clockPin> ledStrip; //LED strip
const uint16_t ledCount = 28; //number of LEDs on strip
rgb_color colors[ledCount]; //determines color of each LED needed to make a rainbow
const uint8_t brightness = 1; //LED brightness

//SETUP
void setup() {
  Serial.begin(115200); //Serial monitor

  //SD card initializations
  if(!sd.begin(9, SPI_HALF_SPEED)) sd.initErrorHalt(); 
  if (!sd.chdir("/")) sd.errorHalt("sd.chdir");

  MP3player.begin();
  MP3player.setVolume(20,20);

  //Pin modes: INPUT or OUTPUT (LED and MP3 already taken care of)
  pinMode(toyPin, OUTPUT);
  digitalWrite(toyPin, LOW);
  pinMode(audioEnable, INPUT);
  pinMode(lightEnable, INPUT);
  pinMode(magSensor, INPUT);
  digitalWrite(magSensor, LOW); //Set reed switch to be initially low
  
  digitalWrite(lightEnable, HIGH);
  digitalWrite(audioEnable, HIGH);

  //Determine chosen dwell time
//  int dwell = analogRead(dwellPotPin); //Read value from potentiometer
//  dwell = map(dwell, 0, 1023, 10, 30); //scale to value between 10 and 30 seconds

  int dwell = 5;
  
  //Set and initialize the conductor's timer
  conductorTimer.SetTimer(0, 0, dwell); //Countdown timer from dwell time to 0 sec
  conductorTimer.StartTimer();
}

//MAIN LOOP
void loop() {
  
  //Run the conductor's timer
  conductorTimer.Timer();

  
  //Check for magnetic sensor HIGH
  if (digitalRead(magSensor) == HIGH) { 
    //Every time you get a HIGH, "recharge" the conductor.
    conductorUpdate();
  }
  
  
  //Check if symphony should be on or off
  symphonyCheck();

  
}

//HELPER FUNCTIONS

void symphonyCheck() {
  /*If the conductor's timer has expired (reached 0 sec), then symphony 
      should stop playing.*/
  if (conductorTimer.ShowSeconds() == 0) {
    symphonyOff();
  /*
    If the conductor's timer has not yet expired (it's above 0 sec), 
  then symphony should continue playing.
    "updateCount" is used to know whether or not the first HIGH has occurred.
  If first HIGH hasn't occurred yet, don't play anything.
  */
  } else if (conductorTimer.ShowSeconds() > 0 && updateCount > 0) {
    symphonyOn();
  }
}

void symphonyOn() {
  
  //MUSIC
  if (digitalRead(audioEnable) == LOW) {
    if (MP3player.getState() == paused_playback) { //In the middle of song: resume track
      MP3player.resumeMusic();
    } else if (!MP3player.isPlaying() && !began) { //Before the first song: play track
      MP3player.playTrack(current_song);
      began = true;
    } else if (!MP3player.isPlaying() && began) { //Between songs: move to next track and play
      current_song++;
      MP3player.playTrack(current_song);
    }
  }
  
  //LIGHTS
  if (digitalRead(lightEnable) == LOW) {
    uint8_t time = millis() >> 4;
    for(uint16_t i = 0; i < ledCount; i++)
    {
      uint8_t p = time - i * 8;
      colors[i] = hsvToRgb((uint32_t)p * 359 / 256, 255, 255);
    }
    ledStrip.write(colors, ledCount, brightness);
    delay(10);
  }
  
    //TOY
    digitalWrite(toyPin, HIGH);

  
}

void symphonyOff() {
  
  //MUSIC
  MP3player.pauseMusic();

  //LIGHTS
  uint8_t time = millis() >> 4;
  for(uint16_t i = 0; i < ledCount; i++)
  {
    uint8_t p = time - i * 8;
    colors[i] = hsvToRgb((uint32_t)p * 359 / 256, 255, 255);
  }
  ledStrip.write(colors, ledCount, 0);
  delay(10);

  //TOY
  digitalWrite(toyPin, LOW);

  
}

/*Reset conductor's timer to 10 sec every time pedal is cranked 1 revolution*/
void conductorUpdate() {
  updateCount++; 
  conductorTimer.ResetTimer();
}





/* Converts a color from HSV to RGB.
 * h is hue, as a number between 0 and 360.
 * s is the saturation, as a number between 0 and 255.
 * v is the value, as a number between 0 and 255. */
rgb_color hsvToRgb(uint16_t h, uint8_t s, uint8_t v)
{
    uint8_t f = (h % 60) * 255 / 60;
    uint8_t p = (255 - s) * (uint16_t)v / 255;
    uint8_t q = (255 - f * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t t = (255 - (255 - f) * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t r = 0, g = 0, b = 0;
    switch((h / 60) % 6){
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
    return (rgb_color){r, g, b};
}
