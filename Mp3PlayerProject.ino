/*
Arduino Mp3 Player Project
Date Started: July 8 2026
Last Updated: Sep 5 2026
Notes: mp3 files need to be uploaded into the microSD card ahead of time with the file names
       that begin with 001, 002, 003...
*/

#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

SoftwareSerial mp3Serial(10, 11);  // RX, TX on arduino
DFRobotDFPlayerMini player; // initializes the DFPlayer Mini

const int potPin = A0; // potentiometer pin
int lastVolume = -1; 

unsigned long lastPlayTime = 0;
const unsigned long playInterval = 178000;  // 2min 58seconds (Length of Little by Little by The Marias)

//---------FUNCTIONS---------//

void LittleByLittle(void) {
  Serial.println("Little By Little Playing~");
  player.playMp3Folder(3);  // Plays 3rd file in SD card
}

// Function: Checks "Volume" by taking average of potentiometer voltage readings (needed because the Jumper cables tweak out sometimes)
int readPotAveraged() {
  const int numSamples = 10; // takes 10 readings
  int total = 0;

  for (int i = 0; i < numSamples; i++) {
    total += analogRead(potPin);
    delay(1);  // tiny gap between samples that helps with noisy connections
  }

  return total / numSamples;
}

// Function: Changes the volume based on mapped pot. reading, accomodates for small changes
void updateVolume() {
  int raw = readPotAveraged();
  int vol = map(raw, 0, 1023, 0, 30); // maps arduino reading >> DFPlayer volume goes up to 30

  const int deadband = 1;  // more filtering in case signal is jittery

  if (abs(vol - lastVolume) > deadband) {
    player.volume(vol);
    Serial.print("Volume set to: ");
    Serial.println(vol);
    lastVolume = vol;
  }
}

//---------FUNCTIONS END---------//

void setup() {
  Serial.begin(9600);
  mp3Serial.begin(9600);

  Serial.println("Starting DFPlayer~");

  if (!player.begin(mp3Serial)) {
    Serial.println("DFPlayer not found");
    while (true); //Prevents repeated printing of msg above
  }

  Serial.println("DFPlayer Starting");

  updateVolume();     // set initial volume from pot position
  LittleByLittle();   // start first song
  lastPlayTime = millis();
}

void loop() {
  updateVolume();  // checks volume

  unsigned long now = millis(); // checks when to stop playing the song
  if (now - lastPlayTime >= playInterval) {
    LittleByLittle();
    lastPlayTime = now;
  }
}
