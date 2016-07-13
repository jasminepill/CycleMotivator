#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include <SFEMP3Shield.h>

SdFat sd;
SFEMP3Shield MP3player;
int8_t current_track = 0;

const int magSensor = 8;
const int light = 13;

void setup() {
  Serial.begin(115200);

  if(!sd.begin(9, SPI_HALF_SPEED)) sd.initErrorHalt();
  if (!sd.chdir("/")) sd.errorHalt("sd.chdir");

  MP3player.begin();
  MP3player.setVolume(10,10);
  
  pinMode(magSensor, INPUT);
  digitalWrite(magSensor, HIGH);
  pinMode(light, OUTPUT);
}

void loop() {
  if (digitalRead(magSensor) == LOW) {
    digitalWrite(light, LOW);
    MP3player.stopTrack();
  } else {
    digitalWrite(light, HIGH);
    MP3player.playTrack(current_track);
  }
}


