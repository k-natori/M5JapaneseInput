#include <Wire.h>
#include <M5Stack.h>

#define LGFX_M5STACK
#include <LovyanGFX.hpp>
#include <LGFX_AUTODETECT.hpp>

#include "JPutil.h"

#define CARDKB_ADDR 0x5F
#define defaultKanaDicFileName "/kanadic.txt"
#define defaultSKKDicFileName "/SKK-JISYO.M"

static LGFX lcd;
static LGFX_Sprite screenSprite(&lcd);
static LGFX_Sprite lineSprite(&lcd);

int screenWidth = 320;
int screenHeight = 240;
int inputLineHeight = 30;

int16_t cursorPosition = 0;
std::vector<String> charStringVector;

String screenBuffer = "";
String inputLineBuffer = "";
String romanBuffer = "";
String kanaBuffer = "";

String lastRoman = "";
String lastKana = "";
std::vector<String> kanjiVector;
std::vector<String> nullVector(0);
int16_t kanjiIndex = -1;

// Function prototypes
void updateScreen(String newString);
void updateInputLine(String newString);
void pushEnter();
void pushSpace();
void pushChar(char c);
void pushBackSpace();
void resetKanjiConversion();

// Setup called when booting
void setup()
{
  M5.begin();
  Wire.begin();
  pinMode(5, INPUT);
  digitalWrite(5, HIGH);

  lcd.init();
  lcd.setRotation(1);
  lcd.setBrightness(200);
  screenWidth = lcd.width();
  screenHeight = lcd.height();

  screenSprite.setColorDepth(8);
  screenSprite.setTextWrap(true);
  screenSprite.setTextSize(1);
  screenSprite.setFont(&fonts::lgfxJapanGothic_24);
  screenSprite.setTextColor(WHITE);
  screenSprite.createSprite(screenWidth, screenHeight - inputLineHeight);
  screenSprite.fillScreen(BLACK);

  lineSprite.setColorDepth(8);
  lineSprite.setTextWrap(false);
  lineSprite.setTextSize(1);
  lineSprite.setFont(&fonts::lgfxJapanGothic_24);
  lineSprite.setTextColor(WHITE);
  lineSprite.createSprite(screenWidth, inputLineHeight);
  lineSprite.drawFastHLine(0, 0, screenWidth);

  screenSprite.setCursor(0, 0);
  lineSprite.setCursor(0, 2);

  if (!loadKanaDicFile(defaultKanaDicFileName))
  {
    screenSprite.printf("%s not found in SD\n", defaultKanaDicFileName);
  }

  if (!loadKanjiDicFile(defaultSKKDicFileName))
  {
    screenSprite.printf("%s not found in SD\n", defaultSKKDicFileName);
  }

  screenSprite.pushSprite(0, 0);
  lineSprite.pushSprite(0, screenHeight - inputLineHeight);

  Serial.printf("esp_get_minimum_free_heap_size(): %6d\n", esp_get_minimum_free_heap_size());
}

// loop called repeatedly while power on
void loop()
{
  // read button
  M5.update();
  if (M5.BtnA.wasReleased())
  {
    delay(500);
    M5.Power.deepSleep();
  }
  else if (M5.BtnB.wasReleased())
  {
    updateScreen("");
    updateInputLine("");
    romanBuffer = "";
    kanaBuffer = "";
    lastRoman = "";
    lastKana = "";
    resetKanjiConversion();
  }

  // read keyboard
  Wire.requestFrom(CARDKB_ADDR, 1);
  while (Wire.available())
  {
    char c = Wire.read(); // receive a byte as characterif

    switch (c)
    {
    case 0x00:
    {
      // NULL
      return;
    }
    case 0x1B:
    {
      // ESC
      delay(500);
      M5.Power.deepSleep();
      break;
    }
    case 0x08:
    {
      // BS
      pushBackSpace();
      break;
    }
    case 0x0D:
    {
      // Enter
      pushEnter();
      break;
    }
    case 0x20:
    {
      // Space
      pushSpace();
      break;
    }
    default:
    {
      Serial.printf("%x\n", c);
      if (c >= 0x21 && c <= 0x7E)
      {
        pushChar(c);
      }
      break;
    }
    }
  }

  delay(10);
}

// update main text screen area
void updateScreen(String newString)
{
  screenSprite.fillSprite(BLACK);
  screenSprite.setCursor(0, 0);
  screenSprite.print(newString);
  screenSprite.pushSprite(0, 0);
  screenBuffer = newString;
}
void addScreenString(String newString)
{
  screenSprite.print(newString);
  screenSprite.pushSprite(0, 0);
  screenBuffer += newString;
}

// update input line screen area
void updateInputLine(String newString)
{
  lineSprite.fillSprite(BLACK);
  lineSprite.drawFastHLine(0, 0, screenWidth, WHITE);
  lineSprite.setCursor(0, 2);
  lineSprite.print(newString);
  lineSprite.pushSprite(0, screenHeight - inputLineHeight);
  inputLineBuffer = newString;
}

// push enter key. change line or transfer input line to screen
void pushEnter()
{
  if (inputLineBuffer.length() == 0)
  {
    inputLineBuffer = "\n";
  }
  String newString = screenBuffer + inputLineBuffer;
  updateScreen(newString);
  updateInputLine("");
  romanBuffer = "";
  kanaBuffer = "";
  lastRoman = "";
  lastKana = "";
  resetKanjiConversion();
}

// push space key. insert space or transform kana to kanji
void pushSpace()
{
  if (kanaBuffer.length() > 0)
  {
    size_t count = kanjiVector.size();
    if (kanjiIndex == -1)
    {
      if (count == 0)
      {
        // Search kanji entries in SKK dictionary
        kanjiVector = kanjiVectorFromKana(kanaBuffer, lastRoman, lastKana);

        // Add katakana entry
        String katakana = katakanaFromHiragana(kanaBuffer);
        kanjiVector.push_back(katakana);
        count = kanjiVector.size();
      }
    }

    kanjiIndex++;
    if (count > 0 && count > kanjiIndex)
    {
      String kanji = kanjiVector[kanjiIndex];
      if (kanji.length() > 0)
      {
        Serial.println(kanaBuffer + "->" + kanji);
        updateInputLine(kanji);
      }
    }
    else
    {
      updateInputLine(kanaBuffer);
      kanjiIndex = -1;
    }
  }
}

// Character key pushed
void pushChar(char c)
{
  if (kanjiIndex > -1)
  {
    pushEnter();
  }

  // Transform roman to kana
  romanBuffer += c;
  String kana = kanaFromRoman(romanBuffer);
  if (kana.length() > 0)
  {
    // Kana found
    Serial.println(romanBuffer + "->" + kana);
    lastRoman = romanBuffer;
    lastKana = kana;

    romanBuffer = "";
    kanaBuffer += kana;
  }

  updateInputLine(kanaBuffer + romanBuffer);
}

// Backspace key pushed
void pushBackSpace()
{
  resetKanjiConversion();
  if (romanBuffer.length() > 0)
  {
    romanBuffer = romanBuffer.substring(0, romanBuffer.length() - 1);
    updateInputLine(kanaBuffer + romanBuffer);
  }
  else if (kanaBuffer.length() > 0)
  {
    kanaBuffer = removeLastMultiByteChar(kanaBuffer);
    updateInputLine(kanaBuffer);
  }
  else if (screenBuffer.length() > 0)
  {
    screenBuffer = removeLastMultiByteChar(screenBuffer);
    updateScreen(screenBuffer);
  }
}

// Reset state of kanji conversion
void resetKanjiConversion()
{
  kanjiVector = nullVector;
  kanjiIndex = -1;
}