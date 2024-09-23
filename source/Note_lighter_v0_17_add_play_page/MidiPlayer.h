#include "pins_arduino.h"
/* Open Sound Control for Audio Library for Teensy 3.x, 4.x
 * Copyright (c) 2021, Jonathan Oakley, teensy-osc@0akley.co.uk
 *
 * Development of this library was enabled by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards, implementing libraries, and maintaining
 * the forum at https://forum.pjrc.com/ 
 *
 * Please support PJRC's efforts to develop open source software by 
 * purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "MD_MIDIFile.h"
#include "iferror.h"
#include <SD.h>

#define FILENAME_MAX_LENGTH 50
#define WAIT_DELAY 2000  // ms

void callbackNoteDisplay(int);
void callbackNoteHide(int);

void midiCallback(midi_event *pev) {
  if (pev->data[0] == 0x90)  // note on
  {
    int velocity = pev->data[2];
    int note = pev->data[1] - 48;
    if (velocity > 0)
      callbackNoteDisplay(note);
    else
      callbackNoteHide(note);
  }
  if (pev->data[0] == 0x80)  // note on
  {
    int velocity = pev->data[2];
    int note = 0;
    if (velocity > 0) {
      note = pev->data[1] - 48;
      callbackNoteHide(note);
    }
  }
}

class MidiPlayer
{
private:
  MD_MIDIFile file_;
  enum { S_IDLE,
        S_PLAYING,
        S_END,
        S_WAIT_BETWEEN } state_ = S_IDLE;

  char currentTuneFileName_[FILENAME_MAX_LENGTH] = "";
  uint32_t timeStart_;

public:
  void init() {
    file_.begin(&(SdFat &)SD);

    while (!(SD.begin(BUILTIN_SDCARD))) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
    File root = SD.open("/");

    file_.setMidiHandler(midiCallback);
  }

  void loadFile(char *fileName) {
    IFERROR(strlen(fileName) >=50 , "MidiReload error - string too big")
    
    strcpy(currentTuneFileName_, fileName);

    DEBUGVAL("loadFile", currentTuneFileName_)
    state_ = S_END;
  }

  void adjustTempo(int bpmAdjust) {
    file_.setTempoAdjust(bpmAdjust);
  }

  void loop() {
    switch (state_) {
      case S_IDLE:  // now idle, set up the next tune
        {
          if (strlen(currentTuneFileName_) > 0) {
            DEBUGTXT("S_IDLE")

            // use the next file name and play it
            DEBUGVAL("file_.load ", currentTuneFileName_)

            int err = file_.load(currentTuneFileName_);
            if (err != MD_MIDIFile::E_OK) {
              DEBUGVAL(" - file_ load Error ", err)

              timeStart_ = millis();
              state_ = S_WAIT_BETWEEN;
              DEBUGTXT("WAIT_BETWEEN")
            } else {
              state_ = S_PLAYING;
            }
          }
        }
        break;

      case S_PLAYING:  // play the file

        if (!file_.isEOF()) 
          file_.getNextEvent();
        else
          state_ = S_END;
        break;

      case S_END:  // done with this one
        DEBUGTXT("S_END")
        file_.close();

        timeStart_ = millis();
        state_ = S_WAIT_BETWEEN;
        DEBUGTXT("WAIT_BETWEEN")
        break;

      case S_WAIT_BETWEEN:  // signal finished with a dignified pause

        if (millis() - timeStart_ >= WAIT_DELAY)
          state_ = S_IDLE;
        break;

      default:
        state_ = S_IDLE;
        break;
    }
  }

};