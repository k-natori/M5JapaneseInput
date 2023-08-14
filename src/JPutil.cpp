#include "JPutil.h"

String kanaDicFileName;
String kanjiDicFileName;

std::map<String, String> kanaFromRomanMap;
std::multimap<uint16_t, size_t> dicPositionFromFirstKanaMap;

String kanaFromRoman(String roman)
{
    if (kanaFromRomanMap.count(roman) > 0)
    {
        Serial.println(roman);
        return kanaFromRomanMap.at(roman);
    }
    return "";
}

std::vector<String> kanjiVectorFromKana(String kana, String lastRoman, String lastKana)
{

    if (lastRoman.length() > 0 && lastKana.length() > 0)
    {
        int16_t index = kana.lastIndexOf(lastKana);
        if (index > 0)
        {
            String kana2 = kana.substring(0, index);
            kana2 += lastRoman.substring(0, 1);

            if (dicPositionFromFirstKanaMap.count(firstKanaUint16FromString(kana2)) > 0)
            {

                std::vector<String> kanji2vector = kanjiVectorFromKey(kana2);

                size_t count = kanji2vector.size();
                std::vector<String> newVector(0);
                for (size_t i = 0; i < count; i++)
                {
                    String kanji = kanji2vector[i];
                    kanji += lastKana;
                    newVector.push_back(kanji);
                }

                if (dicPositionFromFirstKanaMap.count(firstKanaUint16FromString(kana)) > 0)
                {
                    std::vector<String> kanjiVector = kanjiVectorFromKey(kana);
                    newVector.insert(newVector.end(), kanjiVector.begin(), kanjiVector.end());
                }
                return newVector;
            }
        }
    }
    if (dicPositionFromFirstKanaMap.count(firstKanaUint16FromString(kana)) > 0)
    {
        return kanjiVectorFromKey(kana);
    }

    std::vector<String> nullVector(0);
    return nullVector;
}
std::vector<String> kanjiVectorFromKey(String key)
{
    File kanjidicFile = SD.open(kanjiDicFileName.c_str());
    if (kanjidicFile)
    {
        std::vector<String> results;

        uint16_t firstKana = firstKanaUint16FromString(key);
        size_t firstKanaCount = dicPositionFromFirstKanaMap.count(firstKana);
        Serial.printf("search: %d, %s, count:%d\n", firstKana, key.c_str(), firstKanaCount);

        if (firstKanaCount > 0)
        {
            auto pair = dicPositionFromFirstKanaMap.equal_range(firstKana);
            for (auto it = pair.first; it != pair.second; ++it)
            {
                size_t linePosition = it->second;
                kanjidicFile.seek(linePosition);
                Serial.printf("linePosition: %d\n", linePosition);

                String line = kanjidicFile.readStringUntil('\n');
                int separatorLocation = 0;
                while (kanjidicFile.available())
                {
                    separatorLocation = line.indexOf(" /");
                    if (separatorLocation > -1)
                    {
                        String firstWord = line.substring(0, separatorLocation);
                        if (firstWord.equals(key))
                        {

                            Serial.printf("line found: %s\n", line.c_str());
                            break;
                        }
                        else
                        {
                            if (firstKanaUint16FromString(firstWord) != firstKana)
                            {
                                separatorLocation = -1;
                                Serial.printf("line over!: %s\n", firstWord.c_str());
                                break;
                            }
                        }
                    }
                    line = kanjidicFile.readStringUntil('\n');
                }

                if (separatorLocation > -1)
                {
                    separatorLocation += 1;
                    do
                    {
                        int startLocation = separatorLocation + 1;
                        separatorLocation = line.indexOf("/", startLocation);

                        if (separatorLocation > -1)
                        {
                            String content = line.substring(startLocation, separatorLocation);
                            results.push_back(content);
                        }
                    } while (separatorLocation > -1);
                }
            }
        }
        kanjidicFile.close();
        return results;
    }
    std::vector<String> nullVector(0);
    return nullVector;
}

boolean loadKanaDicFile(String fileName)
{
    // load kanadic.txt
    // a:あ
    kanaDicFileName = fileName;
    File kanadicFile = SD.open(fileName.c_str());
    if (kanadicFile)
    {
        Serial.println("kanadic.txt found");
        while (kanadicFile.available() > 0)
        {
            String line = kanadicFile.readStringUntil('\n');
            if (line.startsWith("//"))
                continue;
            int separatorLocation = line.indexOf(":");
            if (separatorLocation > -1)
            {
                String key = line.substring(0, separatorLocation);
                String content = line.substring(separatorLocation + 1);
                kanaFromRomanMap.insert(std::make_pair(key, content));
            }
        }
        kanadicFile.close();
        Serial.println("kanadic loaded size:" + String(kanaFromRomanMap.size()));
        return true;
    }
    else
    {

        Serial.println("kanadic.txt not found");
    }
    return false;
}
boolean loadKanjiDicFile(String fileName)
{
    // load SKK kanji dictionary
    // あg /挙/揚/上/
    // あい /愛/哀/相/挨/
    // ;; comments
    kanjiDicFileName = fileName;
    File kanjidicFile = SD.open(fileName.c_str());
    if (kanjidicFile)
    {
        Serial.printf("%s found\n", fileName.c_str());
        // boolean isOkuriStarted = false;
        int lineNumber = 0;
        uint16_t prevKana = 0;
        while (kanjidicFile.available() > 0)
        {
            lineNumber++;
            size_t linePosition = kanjidicFile.position();

            String line = kanjidicFile.readStringUntil('\n');
            if (line.startsWith(";;"))
            {
                continue;
            }
            int separatorLocation = line.indexOf(" /");
            if (separatorLocation > -1)
            {
                String key = line.substring(0, separatorLocation);
                uint16_t firstKana = firstKanaUint16FromString(key);
                if (firstKana != 0 && firstKana != prevKana)
                {
                    dicPositionFromFirstKanaMap.insert(std::make_pair(firstKana, linePosition));
                    prevKana = firstKana;
                }
            }
        }
        Serial.println("kanjidic loaded size:" + String(dicPositionFromFirstKanaMap.size()));
        kanjidicFile.close();
        return true;
    }
    else
    {
        Serial.printf("%s not found\n", fileName.c_str());
    }
    return false;
}

std::vector<String> multiByteCharVectorFromString(String multiByteString)
{
    int length = multiByteString.length();
    const byte *cstr = (const byte *)(multiByteString.c_str());

    std::vector<String> result;
    String currentString = "";
    String oneByteString = " ";

    for (size_t i = 0; i < length; i++)
    {
        byte aByte = cstr[i];
        byte mark = aByte & B11000000;
        String oneByteString = " ";

        if (mark <= B01000000)
        {
            // single byte
            if (currentString.length() > 0)
            {
                result.push_back(currentString);
                currentString = "";
            }
            oneByteString.setCharAt(0, aByte);
            result.push_back(oneByteString);
        }
        else if (mark == B10000000)
        {
            // 2nd or later byte
            oneByteString.setCharAt(0, aByte);
            currentString += oneByteString;
        }
        else
        {
            // 1st byte
            if (currentString.length() > 0)
            {
                result.push_back(currentString);
            }
            oneByteString.setCharAt(0, aByte);
            currentString = oneByteString;
        }
    }
    if (currentString.length() > 0)
    {
        result.push_back(currentString);
    }
    return result;
}
String removeLastMultiByteChar(String multiByteString)
{
    int length = multiByteString.length();
    if (length == 0)
        return multiByteString;
    const unsigned char *cstr = (const unsigned char *)(multiByteString.c_str());
    size_t i;
    for (i = length - 1; i >= 0; i--)
    {
        unsigned char byte = cstr[i];
        unsigned char mark = byte & B11000000;
        if (mark != B10000000)
        {
            break;
        }
    }
    return multiByteString.substring(0, i);
}

uint16_t firstKanaUint16FromString(String multiByteString)
{
    int length = multiByteString.length();
    if (length < 3)
        return 0;

    const byte *bytes = (const byte *)(multiByteString.c_str());


    uint16_t unichar16 = bytes[1] & 0b00111111;
    unichar16 = unichar16 << 6;
    unichar16 += bytes[2] & 0b00111111;

    return unichar16;
}

String katakanaFromHiragana(String kana)
{
    String result = "";
    std::vector<String> unicharVector = multiByteCharVectorFromString(kana);
    size_t count = unicharVector.size();
    for (size_t i = 0; i < count; i++)
    {
        byte resultBytes[4];
        resultBytes[0] = 0xE3;
        resultBytes[3] = 0;

        String unichar = unicharVector.at(i);
        const byte *bytes = (const byte *)(unichar.c_str());
        size_t length = unichar.length();
        if (length == 3)
        {
            if (bytes[0] == 0xE3)
            {
                uint16_t unichar16 = bytes[1] & 0b00111111;
                unichar16 = unichar16 << 6;
                unichar16 += bytes[2] & 0b00111111;
                
                if (unichar16 >= 0b01000001 && unichar16 <= 0b10010110)
                { // Hiragana to Katakana
                    unichar16 += (0b10100001 - 0b01000001);
                    resultBytes[1] = ((unichar16 & 0b1111111111000000) >> 6) | 0b10000000;
                    resultBytes[2] = (unichar16 & 0b0000000000111111) | 0b10000000;

                    result += String((const char *)resultBytes);
                } else {
                    result += unichar;
                    continue;
                }
            } else {
                result += unichar;
                continue;
            }
        } else {
            result += unichar;
            continue;
        }
    }
    return result;
}