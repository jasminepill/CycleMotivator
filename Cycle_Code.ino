#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include <SFEMP3Shield.h>

//#include <APA102.h>

SdFat sd;
SFEMP3Shield MP3player;
int8_t last_song = 0;

//const uint8_t dataPin = A3;
//const uint8_t clockPin = A4;

const int magSensor = 5;
//const int toyPin = 10;

//const int dwellPotPin = A0; //analog pin 4 used for analog input
const int audioEnable = A1; //analog pin 5 used for digital input
//const int lightEnable = A2; //analog pin 6 used for digital input
//const int toyEnable = A5; 

//APA102<dataPin, clockPin> ledStrip;

//const uint16_t ledCount = 29;
//
//rgb_color colors[ledCount];
//
//const uint8_t brightness = 1;

void setup() {
  Serial.begin(115200);

  if(!sd.begin(9, SPI_HALF_SPEED)) sd.initErrorHalt();
  if (!sd.chdir("/")) sd.errorHalt("sd.chdir");

  MP3player.begin();
  MP3player.setVolume(10,10);
  
  pinMode(magSensor, INPUT);
  digitalWrite(magSensor, LOW);

  pinMode(audioEnable, INPUT);
//  pinMode(lightEnable, INPUT);

  digitalWrite(audioEnable, HIGH);
//  digitalWrite(lightEnable, HIGH);
//  digitalWrite(toyEnable, HIGH);

}

/* Converts a color from HSV to RGB.
 * h is hue, as a number between 0 and 360.
 * s is the saturation, as a number between 0 and 255.
 * v is the value, as a number between 0 and 255. */
//rgb_color hsvToRgb(uint16_t h, uint8_t s, uint8_t v)
//{
//    uint8_t f = (h % 60) * 255 / 60;
//    uint8_t p = (255 - s) * (uint16_t)v / 255;
//    uint8_t q = (255 - f * (uint16_t)s / 255) * (uint16_t)v / 255;
//    uint8_t t = (255 - (255 - f) * (uint16_t)s / 255) * (uint16_t)v / 255;
//    uint8_t r = 0, g = 0, b = 0;
//    switch((h / 60) % 6){
//        case 0: r = v; g = t; b = p; break;
//        case 1: r = q; g = v; b = p; break;
//        case 2: r = p; g = v; b = t; break;
//        case 3: r = p; g = q; b = v; break;
//        case 4: r = t; g = p; b = v; break;
//        case 5: r = v; g = p; b = q; break;
//    }
//    return (rgb_color){r, g, b};
//}

int StartTime = 0;

void loop() {
  //NOT PEDALING
  if (digitalRead(magSensor) == LOW) {
    unsigned long CurrentTime = millis();
    if (StartTime > 0) { 
      unsigned long ElapsedTime = CurrentTime - StartTime;
      if (ElapsedTime > 5000) {
        //light
//        uint8_t time = millis() >> 4;
//        for(uint16_t i = 0; i < ledCount; i++)
//        {
//          uint8_t p = time - i * 8;
//          colors[i] = hsvToRgb((uint32_t)p * 359 / 256, 255, 255);
//        }
//        ledStrip.write(colors, ledCount, 0);
//        delay(10);
        //music
        MP3player.pauseMusic();
        //toy
//      digitalWrite(toyPin, LOW);
      }
    }
  //PEDALING
  } else {
    //light
//    if (digitalRead(lightEnable) == HIGH) {
//      uint8_t time = millis() >> 4;
//      for(uint16_t i = 0; i < ledCount; i++)
//      {
//        uint8_t p = time - i * 8;
//        colors[i] = hsvToRgb((uint32_t)p * 359 / 256, 255, 255);
//      }
//      ledStrip.write(colors, ledCount, brightness);
//      delay(10);
//    }
    //music
    if (digitalRead(audioEnable) == HIGH) {
      if (MP3player.getState() == paused_playback) {
        MP3player.resumeMusic();
      } else if (!MP3player.isPlaying()) {
        last_song++;
        MP3player.playTrack(last_song);
      } else if (!MP3player.isPlaying() && last_song == 1) {
        MP3player.playTrack(last_song);
      }
    }
    //toy
//    digitalWrite(toyPin, HIGH);
    unsigned long StartTime = millis();
//    int dwell = analogRead(dwellPotPin);
//    dwell = map(dwell, 0, 1023, 0, 15000); //scale to value between 0 and 15 seconds
    delay(1000); //dwell time
  }
}

