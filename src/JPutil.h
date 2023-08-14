#ifndef JPUTIL_H_INCLUDE
#define JPUTIL_H_INCLUDE

#include <Arduino.h>
#include <M5Stack.h>
#include <vector>
#include <map>

// Load dicitinoary file
boolean loadKanaDicFile(String fileName);
boolean loadKanjiDicFile(String fileName);

// Conversion functions
String kanaFromRoman(String roman);
std::vector<String> kanjiVectorFromKana(String kana, String lastRoman, String lastKana);
std::vector<String> kanjiVectorFromKey(String key);

// Utility functions
std::vector<String> multiByteCharVectorFromString(String multiByteString);
String removeLastMultiByteChar(String multiByteString);
uint16_t firstKanaUint16FromString(String multiByteString);

String katakanaFromHiragana(String kana);

#endif