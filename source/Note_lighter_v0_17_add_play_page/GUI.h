// NoteLighterMenu.h
// by Hippolyte Mounier
// 2024-06-19
//
// Goal : Display a nice menu over the rectangular touchscreen

#include "ILI9341_t3_Menu.h"  // custom utilities definition
#include "ILI9341_t3.h"       // high speed display that ships with Teensy
#include "font_Arial.h"       // custom fonts that ships with ILI9341_t3.h
#include <font_ArialBold.h>
#include <XPT2046_Touchscreen.h>  // touch driver for a TFT display
#include <ILI9341_t3_Controls.h>
#include "importedGraphics.h"

// hardware pins for XPT2046 Touchscreen
#define CS_PIN 10
#define DC_PIN 9
#define T_CS 8
#define T_IRQ 2

#define FONT_TITLE Arial_16
#define ITEM_ROW_HEIGHT 40
#define ITEM_MAX_ROWS_PER_SCREEN 5

// set default colors
#define COLOR_MENU_TEXT 0X0000
#define COLOR_MENU_BACKGROUND 0XFFFF
#define COLOR_MENU_HIGHLIGHTTEXT 0X0000
#define COLOR_MENU_HIGHLIGHT 0XFDEE
#define COLOR_MENU_SELECTTEXT 0XFFFF
#define COLOR_MENU_SELECT 0XF800
#define COLOR_MENU_SELECTBORDER 0X9000
#define COLOR_MENU_DISABLE 0XC618
#define COLOR_TITLE_TEXT 0XE71C
#define COLOR_TITLE_BACK 0X0812
#define COLOR_ICONS 0X0000

enum GUI_Pages { PAGE_MENU,
                 PAGE_PLAYING };

class GUI {
private:

  ILI9341_t3 display_;
  ItemMenu menu_;
  XPT2046_Touchscreen touchpad_;
  int BtnX, BtnY;
  bool previouslyPressed_ = false;
  bool isPressed_ = false;
  int currentSong_ = -1;
  Button stopButton_;
  BarChartH progress_;

  enum GUI_Pages displayPage_ = PAGE_MENU;

  void processTouch_() {
    //DEBUGTXT("processTouch_")

    previouslyPressed_ = isPressed_;
    isPressed_ = touchpad_.touched();
    if (isPressed_) {
      TS_Point TouchPoint;
      TouchPoint = touchpad_.getPoint();
      BtnX = map(TouchPoint.x, 3700, 300, 319, 0);
      BtnY = map(TouchPoint.y, 280, 3800, 239, 0);
    }
  }

  bool isNewTouch_() {
    return isPressed_ && !previouslyPressed_;
  }

public:
  GUI()
    : touchpad_(T_CS, T_IRQ), display_(CS_PIN, DC_PIN), menu_(&display_), stopButton_(&display_), progress_(&display_) {}

  void init() {

    display_.begin();
    display_.setRotation(1);
    touchpad_.begin();
    touchpad_.setRotation(3);

    display_.fillScreen(COLOR_MENU_BACKGROUND);

    menu_.init(COLOR_MENU_TEXT, COLOR_MENU_BACKGROUND, COLOR_MENU_HIGHLIGHTTEXT, COLOR_MENU_HIGHLIGHT,
               ITEM_ROW_HEIGHT, ITEM_MAX_ROWS_PER_SCREEN,
               "Select song",
               FONT_TITLE, FONT_TITLE);

    //menu_.setTitleColors(COLOR_TITLE_TEXT, COLOR_TITLE_BACK);
    menu_.setTitleBarSize(0, 0, 320, 40);
    menu_.setTitleTextMargins(60, 7);
    menu_.setMenuBarMargins(0, 310, 10, 4);
    menu_.setItemColors(COLOR_MENU_DISABLE, COLOR_MENU_SELECTBORDER);
    menu_.setItemTextMargins(10, 15, 5);
    menu_.setTitleText("Select song", "Select song");

    DEBUGTXT("Populating menu")
    for (int i = 0; i < myFileList.getNbFiles(); i++) {
      char txt[50];
      sprintf(txt, "%d       ", i + 1);
      strcpy(txt + 3, myFileList.getFileName(i));
      txt[29] = '\0';    // cuts string at 29th character
      menu_.addNI(txt);  // NI stands for No Icon
    }


    //init(int16_t ButtonX, int16_t ButtonY, uint8_t ButtonWidth, uint8_t ButtonHeight,
    //		uint16_t OutlineColor, uint16_t ButtonColor, uint16_t BackgroundColor,
    //		const uint16_t  *UpIcon, const uint16_t *DnIcon, int16_t IconWidth, int16_t IconHeight, int OffsetLeft, int OffsetTop ) {

    stopButton_.init(60, 200, 60, 60, COLOR_ICONS, ILI9341_RED, COLOR_MENU_BACKGROUND, COLOR_ICONS, "Stop", -25, -10, FONT_TITLE);
    stopButton_.setCornerRadius(10);

    progress_.init(20, 140, 280, 20, 0, 1.0, 0.1, "",
                   COLOR_ICONS, ILI9341_RED, COLOR_ICONS, COLOR_MENU_BACKGROUND, COLOR_MENU_BACKGROUND,
                   FONT_TITLE, FONT_TITLE);
    progress_.showScale(false);
    progress_.showTitle(false);

    menu_.draw();
  }

  void computeUserInteration() {
    //DEBUGTXT("computeUserInteration")
    //DEBUGVAL2("displayPage_", displayPage_, "currentSong_", currentSong_)

    processTouch_();
    //DEBUGVAL2("displayPage_", displayPage_, "currentSong_", currentSong_)

    switch (displayPage_) {
      case PAGE_MENU:
        if (isNewTouch_()) {
          DEBUGTXT("PAGE_MENU")
          int newItem = menu_.press(BtnX, BtnY);
          DEBUGVAL("newItem", newItem)

          if (newItem >= 1) {
            if (currentSong_ >= 1)
              menu_.drawRow(currentSong_, BUTTON_NOTPRESSED);

            currentSong_ = newItem;
            menu_.drawRow(currentSong_, BUTTON_PRESSED);

            DEBUGVAL("currentSong_", currentSong_)
          }
        }
        break;

      case PAGE_PLAYING:
        if (isNewTouch_()) {
          DEBUGTXT("PAGE_PLAYING")

          bool isStopped = stopButton_.press(BtnX, BtnY);

          if (isStopped)
            currentSong_ = -1;
        }

        break;
    };
  }

  int getCurrentSongNr() {
    return currentSong_;
  }

  int getCurrentPageNr() {
    return displayPage_;
  }

  void displayMenuPage() {
    displayPage_ = PAGE_MENU;
    DEBUGTXT("displayMenuPage")
    DEBUGVAL2("displayPage_", displayPage_, "currentSong_", currentSong_)

    display_.fillScreen(COLOR_MENU_BACKGROUND);
    menu_.draw();
  }

  void displaySongPlayingPage(char* songName) {
    displayPage_ = PAGE_PLAYING;
    DEBUGTXT("displaySongPlayingPage")
    DEBUGVAL2("displayPage_", displayPage_, "currentSong_", currentSong_)

    display_.fillScreen(COLOR_MENU_BACKGROUND);
    display_.setCursor(100, 50);
    display_.println("Playing...");
    display_.setTextColor(ILI9341_BLACK);
    display_.setTextSize(2);
    display_.setCursor(10, 100);
    display_.println(songName);
    stopButton_.draw();
    progress_.draw(0.01);

  }
};