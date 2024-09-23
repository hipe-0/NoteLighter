// LEDKeyboard.h
#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
#include "iferror.h"

FASTLED_USING_NAMESPACE

#define DATA_PIN 4
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 110
#define NUM_NOTES 128
#define FADE_AMOUNT 10
#define BRIGHTNESS 20
#define FRAMES_PER_SECOND 60
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
#define MAX_LOAD 40
#define NOTE_DELAY 50
#define LED_OFFSET_NOTE 0.5
#define NOTES_IN_OCTAVE 12
#define BUFFER_SIZE 200



class LEDKeyboard {
private:
  CRGB leds_[NUM_LEDS];
  int loadNoteActive_[NUM_NOTES];

  int bufferTimer_[BUFFER_SIZE];
  int bufferNote_[BUFFER_SIZE];
  bool bufferIsOn_[BUFFER_SIZE];
  int bufferInsertCursor_;
  int bufferReadCursor_;

  float octaveWidth_;
  int hue_;
  int saturation_;
  int brightness_;

  const bool isWhiteKey[12] = { true, false, true, false, true, true, false, true, false, true, false, true };

  bool isLedForNote_(int ledNr, int noteNr) {
    float note = ledNr * float(NOTES_IN_OCTAVE) / octaveWidth_;
    return int(note) == noteNr;
  }

  int computeLedForNote_(int noteNr) {
    int ledNr = int((noteNr + LED_OFFSET_NOTE) / float(NOTES_IN_OCTAVE) * octaveWidth_);
    if (ledNr >= NUM_LEDS)
      ledNr = NUM_LEDS - 1;

    return ledNr;
  }


  CRGB fadeTowardColor_(CRGB& cur, const CRGB& target, uint8_t amount) {
    nblendU8TowardU8_(cur.red, target.red, amount);
    nblendU8TowardU8_(cur.green, target.green, amount);
    nblendU8TowardU8_(cur.blue, target.blue, amount);
    return cur;
  }

  // Helper function that blends one uint8_t toward another by a given amount
  void nblendU8TowardU8_(uint8_t& cur, const uint8_t target, uint8_t amount) {
    if (cur == target) return;

    if (cur < target) {
      uint8_t delta = target - cur;
      delta = scale8_video(delta, amount);
      cur += delta;
    } else {
      uint8_t delta = cur - target;
      delta = scale8_video(delta, amount);
      cur -= delta;
    }
  }

public:
  void init() {
    // tell FastLED about the LED strip configuration
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds_, NUM_LEDS).setCorrection(TypicalLEDStrip);

    // set master brightness control
    FastLED.setBrightness(BRIGHTNESS);
    octaveWidth_ = 24.0;
    hue_ = 0;
    saturation_ = 255;
    brightness_ = 20;

    FastLED.clear(true);  // clear all pixel data

    clearNotes();

    for (int i = 0; i < BUFFER_SIZE; i++) {
      bufferTimer_[i] = 0;
      bufferNote_[i] = 0;
      bufferIsOn_[i] = 0;
    }
  }


  void clearNotes() {
    for (int i = 0; i < NUM_NOTES; i++)
      loadNoteActive_[i] = 0;

    for (int i = 0; i < BUFFER_SIZE; i++) {
      bufferTimer_[i] = 0;
      bufferNote_[i] = 0;
      bufferIsOn_[i] = 0;
    }

    bufferInsertCursor_ = 0;
    bufferReadCursor_ = 0;
  }


  void computeBufferedNote() {

    for (int i = 0; i < BUFFER_SIZE; i++) {
      if (bufferTimer_[i] > 1)
        bufferTimer_[i]--;

      if (bufferTimer_[i] == 1) {
        //DEBUGVAL("played i", i)
        bufferTimer_[i] = 0;

        int note = bufferNote_[bufferReadCursor_];
        bool isOn = bufferIsOn_[bufferReadCursor_];

        if (isOn) {
          loadNoteActive_[note] = 255;
        } else {
          loadNoteActive_[note] = 0;
          int led = computeLedForNote_(note);
          leds_[led] = CHSV(0, 0, 0);
        }

        bufferReadCursor_++;
        if (bufferReadCursor_ >= BUFFER_SIZE)
          bufferReadCursor_ = 0;
        //DEBUGVAL("bufferReadCursor_", bufferReadCursor_)
      }
    }
  }


  void setOctaveWidth(float newOctaveWidth) {
    octaveWidth_ = newOctaveWidth;
  }



  void fade() {
    fadeToBlackBy(leds_, NUM_LEDS, 30);
  }


  void activateNote(int note, bool isOn) {

    // add note to buffer
    bufferTimer_[bufferInsertCursor_] = NOTE_DELAY;
    bufferNote_[bufferInsertCursor_] = note;
    bufferIsOn_[bufferInsertCursor_] = isOn;

    if (loadNoteActive_[note] == 0)
      loadNoteActive_[note] = 1;
    //DEBUGVAL2("note added", note, "bufferInsertCursor_", bufferInsertCursor_)

    bufferInsertCursor_++;
    if (bufferInsertCursor_ >= BUFFER_SIZE)
      bufferInsertCursor_ = 0;
  }


  void displayKeyboard() {

    bool isDisplayed[NUM_LEDS];
    for (int i = 0; i < NUM_LEDS; i++)
      isDisplayed[i] = false;

    for (int i = 0; i < NUM_NOTES; i++) {

      int led = computeLedForNote_(i);

      CRGB keyboardTargetcolor;

      int noteWithinOctave = i % 12;
      if (isWhiteKey[noteWithinOctave])
        keyboardTargetcolor = CRGB::Black;
      else
        keyboardTargetcolor = CRGB(0, 0, 8);


      if (loadNoteActive_[i] > 0 && loadNoteActive_[i] < 255) {
        loadNoteActive_[i]++;
        if (loadNoteActive_[i] >= MAX_LOAD)
          loadNoteActive_[i] = MAX_LOAD;

        //fadeTowardColor_(leds_[led], CHSV(0, 255, loadNoteActive_[i]/2 ), FADE_AMOUNT);
        leds_[led]= CHSV(0, 255, loadNoteActive_[i]/2 );
      }

      if (loadNoteActive_[i] == 255)
        fadeTowardColor_(leds_[led], CRGB::White, FADE_AMOUNT);

      if (loadNoteActive_[i] == 0)
        leds_[led] = keyboardTargetcolor;

      isDisplayed[led] = true;
    
    }

    for (int i = 0; i < NUM_LEDS; i++)
      if (!isDisplayed[i])
        fadeTowardColor_(leds_[i], CRGB::Black, FADE_AMOUNT);
  }

  void show() {
    FastLED.show();
    FastLED.delay(1000 / FRAMES_PER_SECOND);
  }
};