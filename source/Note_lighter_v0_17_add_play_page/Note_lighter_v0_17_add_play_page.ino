// Note Lighter
// by Hippolyte Mounier
// 2024-02-25
//

#define APP_VERSION "v0.16"
#include "LEDKeyboard.h"
LEDKeyboard myLeds;

#include "Pots.h"
Potentiometers myPots;
#define ID_POT_KEYBOARD_SIZE 0
#define ID_POT_TEMPO_CONTROL 1

#include "MidiPlayer.h"
MidiPlayer myPlayer;

#include "FileManager.h"
FileManager myFileList;

#include "GUI.h"
GUI myGUI;

void setup() {
  Serial.begin(38400);
  Serial.println(F("Setup started"));
  Serial.println(APP_VERSION);

  Serial.println(F("initializing pots"));

  // hardware
  myPots.definePot(ID_POT_KEYBOARD_SIZE, 14, 0, 459, 1023, 18, 23.8, 32);
  myPots.definePot(ID_POT_TEMPO_CONTROL, 15, 0, 503, 1023, -100, 0, 100);

  Serial.println(F("initializing leds"));
  myLeds.init();

  Serial.println(F("initializing midi file player"));
  myPlayer.init();

  Serial.println(F("loading files"));
  myFileList.loadFiles("/");

  Serial.println(F("initializing menu"));
  myGUI.init();

  Serial.println(F("Setup ended"));
}

int lastVal = 0;

void callbackNoteDisplay(int note) {
  myLeds.activateNote(note, true);
}
void callbackNoteHide(int note) {
  myLeds.activateNote(note, false);
}

void userPotentiometerControl() {
  myPots.readPotentiometers();

  float octaveWidth = myPots.getPotValueMaped(ID_POT_KEYBOARD_SIZE);
  if (myPots.hasChanged(ID_POT_KEYBOARD_SIZE))
    DEBUGVAL2("octaveWidth", octaveWidth, "raw", myPots.getRawPotValue(ID_POT_KEYBOARD_SIZE))

  myLeds.setOctaveWidth(octaveWidth);

  int tempoAdjust = myPots.getPotValueMaped(ID_POT_TEMPO_CONTROL);
  if (myPots.hasChanged(ID_POT_TEMPO_CONTROL))
    DEBUGVAL2("tempoAdjust", tempoAdjust, "raw", myPots.getRawPotValue(ID_POT_TEMPO_CONTROL))

  myPlayer.adjustTempo(tempoAdjust);
}

void loop() {
  myPlayer.loop();
  myLeds.computeBufferedNote();
  myLeds.displayKeyboard();
  myLeds.show();
  userPotentiometerControl();

  myGUI.computeUserInteration();
  int page = myGUI.getCurrentPageNr();
  int songNr = myGUI.getCurrentSongNr();

  if (songNr >= 1 && page == PAGE_MENU ) {
    DEBUGVAL("songNr", songNr)
    myLeds.clearNotes();
    char songName[50] = "";
    strcpy(songName, myFileList.getFileName(songNr - 1));
    myPlayer.loadFile(songName);

    myGUI.displaySongPlayingPage(songName);
  }
  
  if (songNr < 1 && page == PAGE_PLAYING)
  {
    myPlayer.loadFile("");
    myLeds.clearNotes();
    myGUI.displayMenuPage();
  }

}
