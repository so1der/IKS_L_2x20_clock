#include <DallasTemperature.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <NTPClient.h>
#include <OneWire.h>
#include <RTClib.h>
#include <SPI.h>
#include <WiFiUdp.h>
#include <pages.h>

#define MAX_EEPROM_STRING_SIZE 255


/////////////////////////////////////////////////////////////////
//
//   Human configurable parameters here:
//
//
// WiFi AP name, password and IP address for Web UI
const char *ap_ssid = "Clock_AP";
const char *ap_password = "vfdclock";
IPAddress apIP(192, 168, 4, 2);
//
// Time to display custom text sent from WebUI, milliseconds
const uint TIME_TO_DISPLAY_CUSTOM_TEXT = 10000;
//
/////////////////////////////////////////////////////////////////


// EEPROM parameters
const byte seconds_offset_address = 0;
int seconds_offset;

const byte temperature_offset_address = 5;
int temperature_offset;

const byte eeprom_empty_address = 10;
bool eeprom_empty;

const byte NTP_URL_address = 16;
String NTP_URL;

const int oneWireBus = 2;
String h_hours, h_minutes, h_seconds;
String h_month_day, h_month;
int year;

// Custom text sent from WebUI
String custom_text = "";

// System counters
byte div_step = 0;
unsigned long last_millis = 0;
unsigned long current_millis;

// Period to refresh temperature
unsigned long temp_millis = 0;

// Period to flip text (up/down strings)
unsigned long text_flip_millis = 0;

// Text flip flag
bool flipDisplayedText = false;

// Custom text diaplay flag
bool displayCustomText = false;


// Classes
DateTime now;
// RTC_DS3231 rtc;
RTC_DS1307 rtc;
WiFiUDP ntpUDP;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
NTPClient timeClient(ntpUDP, "192.168.1.1", seconds_offset, 1000);
AsyncWebServer server(80);

const char date_div[] = "\\|/-";
const char time_div[] = ": : ";

// Structure to define mapping table for CP866 Cyrillic characters
struct CP866Mapping {
    const char* utf8Char; // UTF-8 multi-byte character
    uint8_t cp866Byte;    // Corresponding CP866 byte
};

// CP866 Cyrillic mapping table (UTF-8 to CP866 byte) for diplaying custom text from WebUI
const CP866Mapping cp866Table[] = {
    {"А", 0x80}, {"Б", 0x81}, {"В", 0x82}, {"Г", 0x83}, {"Д", 0x84},
    {"Е", 0x85}, {"Є", 0xF2}, {"Ж", 0x86}, {"З", 0x87}, {"И", 0x88},
    {"Й", 0x89}, {"І", 0x49}, {"Ї", 0x49}, {"К", 0x8A}, {"Л", 0x8B},
    {"М", 0x8C}, {"Н", 0x8D}, {"О", 0x8E}, {"П", 0x8F}, {"Р", 0x90},
    {"С", 0x91}, {"Т", 0x92}, {"У", 0x93}, {"Ф", 0x94}, {"Х", 0x95},
    {"Ц", 0x96}, {"Ч", 0x97}, {"Ш", 0x98}, {"Щ", 0x99}, {"Ъ", 0x9A},
    {"Ы", 0x9B}, {"Ь", 0x9C}, {"Э", 0x9D}, {"Ю", 0x9E}, {"Я", 0x9F},
    {"а", 0xA0}, {"б", 0xA1}, {"в", 0xA2}, {"г", 0xA3}, {"д", 0xA4},
    {"е", 0xA5}, {"є", 0xF3}, {"ж", 0xA6}, {"з", 0xA7}, {"и", 0xA8},
    {"і", 0x69}, {"ї", 0xF5}, {"й", 0xA9}, {"к", 0xAA}, {"л", 0xAB},
    {"м", 0xAC}, {"н", 0xAD}, {"о", 0xAE}, {"п", 0xAF}, {"р", 0xE0},
    {"с", 0xE1}, {"т", 0xE2}, {"у", 0xE3}, {"ф", 0xE4}, {"х", 0xE5},
    {"ц", 0xE6}, {"ч", 0xE7}, {"ш", 0xE8}, {"щ", 0xE9}, {"ъ", 0xEA},
    {"ы", 0xEB}, {"ь", 0xEC}, {"э", 0xED}, {"ю", 0xEE}, {"я", 0xEF}
};

// Days of week cyrillic
byte week_days[][10] = {
    {0x8D, 0xA5, 0xA4, 0x69, 0xAB, 0xEF, 0x20, 0x20, 0x20, 0x20},  // Неділя
    {0x8F, 0xAE, 0xAD, 0xA5, 0xA4, 0x69, 0xAB, 0xAE, 0xAA, 0x20},  // Понеділок
    {0x82, 0x69, 0xA2, 0xE2, 0xAE, 0xE0, 0xAE, 0xAA, 0x20, 0x20},  // Вівторок
    {0x91, 0xA5, 0xE0, 0xA5, 0xA4, 0xA0, 0x20, 0x20, 0x20, 0x20},  // Середа
    {0x97, 0xA5, 0xE2, 0xA2, 0xA5, 0xE0, 0x20, 0x20, 0x20, 0x20},  // Четвер
    {0x8F, 0x27, 0xEF, 0xE2, 0xAD, 0xA8, 0xE6, 0xEF, 0x20, 0x20},  // П'ятниця
    {0x91, 0xE3, 0xA1, 0xAE, 0xE2, 0xA0, 0x20, 0x20, 0x20, 0x20}   // Субота
};

String unitHandler(int unit) {
    int a = unit / 10 % 10;
    int b = unit % 10;
    String complete_string = String(a) + String(b);
    return complete_string;
}

// Function to convert UTF-8 cyrillic character to CP866 byte
uint8_t utf8CharToCp866(const String &utf8Char) {
    for (size_t i = 0; i < sizeof(cp866Table) / sizeof(cp866Table[0]); i++) {
        if (utf8Char == cp866Table[i].utf8Char) {
            return cp866Table[i].cp866Byte;
        }
    }
    // Return 0x3F ('?') if the character is not found
    return 0x3F;
}

// Function to convert UTF-8 string to a CP866 byte array
size_t UTF8String_to_CP866(const String &input, uint8_t *outputArray, size_t maxOutputSize) {
    size_t outputIndex = 0;
    size_t i = 0;

    memset(outputArray, 0, maxOutputSize);

    while (i < input.length() && outputIndex < maxOutputSize) {
        // Extract a single UTF-8 character
        char c = input[i];
        if ((c & 0x80) == 0) {
            // Single-byte character (ASCII)
            outputArray[outputIndex++] = (uint8_t)c;
            i++;
        } else {
            // Multi-byte UTF-8 character
            String utf8Char = String(c);
            i++;
            while (i < input.length() && (input[i] & 0xC0) == 0x80) {
                utf8Char += input[i];
                i++;
            }
            // Convert UTF-8 character to CP866 byte
            outputArray[outputIndex++] = utf8CharToCp866(utf8Char);
        }
    }
    // Return the number of bytes written
    return outputIndex; 
}

// Byte position from 0x31 to 0x58 (1-40)
void setCursorVFD(byte place) {
    //                        EOT   SOH     P          ETB
    byte cursor_position[] = {0x04, 0x01, 0x50, place, 0x17};
    Serial.write(cursor_position, sizeof(cursor_position));
}

void clearVFD() {
    setCursorVFD(0x31);
    Serial.print("                                        ");
    setCursorVFD(0x31);
}

void drawTime() {
    setCursorVFD(0x31);
    Serial.print(h_hours + time_div[div_step] + h_minutes + time_div[div_step] +
                 h_seconds);
    setCursorVFD(0x45);
    Serial.write(week_days[now.dayOfTheWeek()],
                 sizeof(week_days[now.dayOfTheWeek()]));
    setCursorVFD(0x4F);
    Serial.print(h_month_day + date_div[div_step] + h_month +
                 date_div[div_step] + year);
}

void drawTimeFlipped() {
    setCursorVFD(0x31);
    Serial.write(week_days[now.dayOfTheWeek()],
                 sizeof(week_days[now.dayOfTheWeek()]));
    setCursorVFD(0x45);
    Serial.print(h_hours + time_div[div_step] + h_minutes + time_div[div_step] +
                 h_seconds);
    setCursorVFD(0x3B);
    Serial.print(h_month_day + date_div[div_step] + h_month +
                 date_div[div_step] + year);
}

void calculateTime() {
    h_hours = unitHandler(now.hour());
    h_minutes = unitHandler(now.minute());
    h_seconds = unitHandler(now.second());
    h_month_day = unitHandler(now.day());
    h_month = unitHandler(now.month());
    year = now.year();
}

void adjustRTCviaNTP() {
    timeClient.setTimeOffset(seconds_offset);
    timeClient.setPoolServerName(NTP_URL.c_str());
    timeClient.begin();
    timeClient.update();
    time_t epochTime = timeClient.getEpochTime();
    struct tm *ptm = gmtime((time_t *)&epochTime);
    rtc.adjust(DateTime(ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday,
                        timeClient.getHours(), timeClient.getMinutes(),
                        timeClient.getSeconds()));
    timeClient.end();
}

void adjustRTCviaPhone(int hours, int minutes, int seconds, int year, int month,
                       int day) {
    rtc.adjust(DateTime(year, month, day, hours, minutes, seconds));
}

void writeStringToEEPROM(int address_offset, const String &strToWrite) {
    int len = strToWrite.length();
    if (len > MAX_EEPROM_STRING_SIZE) {
        len = MAX_EEPROM_STRING_SIZE;
    }
    EEPROM.write(address_offset, len);
    for (int i = 0; i < len; i++) {
        EEPROM.write(address_offset + 1 + i, strToWrite[i]);
    }
    EEPROM.commit();
}

float getTemp() {
    sensors.requestTemperatures();
    float temperatureC = sensors.getTempCByIndex(0);
    temperatureC = temperatureC + temperature_offset;
    return temperatureC;
}

void drawTemp() {
    setCursorVFD(0x3E);
    Serial.print(getTemp());
    Serial.write(0xF8);
    Serial.print('C');
}

void drawTempFlipped() {
    setCursorVFD(0x52);
    Serial.print(getTemp());
    Serial.write(0xF8);
    Serial.print('C');
}

void drawCustomText(String text) {
    const size_t bufferSize = text.length();
    uint8_t outputArray[bufferSize];
    UTF8String_to_CP866(text, outputArray, bufferSize);
    setCursorVFD(0x31);
    Serial.write(outputArray, sizeof(outputArray));
}

void startWebServer() {
    // Main page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html", main_page);
    });

    // Handlers
    server.on("/set_timezone", HTTP_GET, [](AsyncWebServerRequest *request) {
        seconds_offset = request->getParam("offset")->value().toInt();
        EEPROM.put(seconds_offset_address, seconds_offset);
        EEPROM.commit();
        adjustRTCviaNTP();
        request->send(200, "text/plain", "200");
    });
    server.on("/sync_time", HTTP_GET, [](AsyncWebServerRequest *request) {
        int hours = request->getParam("hours")->value().toInt();
        int minutes = request->getParam("minutes")->value().toInt();
        int seconds = request->getParam("seconds")->value().toInt();
        int year = request->getParam("year")->value().toInt();
        int month = request->getParam("month")->value().toInt();
        int day = request->getParam("day")->value().toInt();
        adjustRTCviaPhone(hours, minutes, seconds, year, month, day);
        request->send(200, "text/plain", "200");
    });
    server.on(
        "/temperature_offset", HTTP_GET, [](AsyncWebServerRequest *request) {
            temperature_offset = request->getParam("offset")->value().toInt();
            EEPROM.put(temperature_offset_address, temperature_offset - 1);
            EEPROM.commit();
            request->send(200, "text/plain", "200");
        });
    server.on("/ntp_server", HTTP_GET, [](AsyncWebServerRequest *request) {
        NTP_URL = request->getParam("ntp_url")->value();
        writeStringToEEPROM(NTP_URL_address, NTP_URL);
        request->send(200, "text/plain", "200");
    });
    server.on("/get_data", HTTP_GET, [](AsyncWebServerRequest *request) {
        char buffer[100];
        sprintf(buffer,
                "{\"NTP\": \"%s\", \"temp_offset\": \"%d\", "
                "\"seconds_offset\": \"%d\", "
                "\"ssid\": \"%s\" }",
                NTP_URL.c_str(), temperature_offset, seconds_offset,
                WiFi.SSID().c_str());
        request->send(200, "text/plain", buffer);
    });
    server.on("/set_wifi", HTTP_GET, [](AsyncWebServerRequest *request) {
        String ssid = request->getParam("ssid")->value();
        String password = request->getParam("password")->value();
        request->send(200, "text/plain", "200");
        delay(20);
        WiFi.softAPdisconnect(true);
        WiFi.mode(WIFI_STA);
        WiFi.persistent(true);
        WiFi.begin(ssid, password);
    });
    server.on("/custom_text", HTTP_GET, [](AsyncWebServerRequest *request) {
        custom_text = request->getParam("text")->value();
        request->send(200, "text/plain", "200");
    });

    server.begin();
}

String readStringFromEEPROM(int address_offset) {
    int len = EEPROM.read(address_offset);
    if (len < 0 || len > MAX_EEPROM_STRING_SIZE) {
        return String();
    }
    char data[len + 1];
    for (int i = 0; i < len; i++) {
        data[i] = EEPROM.read(address_offset + 1 + i);
    }
    data[len] = '\0';
    return String(data);
}

void readSettingsFromEEPROM() {
    EEPROM.get(eeprom_empty_address, eeprom_empty);
    if (eeprom_empty) {
        EEPROM.put(seconds_offset_address, 7200);
        EEPROM.put(temperature_offset_address, 0);
        EEPROM.put(eeprom_empty_address, false);
        EEPROM.commit();
        writeStringToEEPROM(NTP_URL_address, "ntp1.time.in.ua");

    } else {
        EEPROM.get(seconds_offset_address, seconds_offset);
        EEPROM.get(temperature_offset_address, temperature_offset);
        NTP_URL = readStringFromEEPROM(NTP_URL_address);
    }
}

void handleWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin();
    unsigned long startAttemptTime = millis();
    Serial.write("Loading");
    while (WiFi.status() != WL_CONNECTED &&
           millis() - startAttemptTime < 5000) {
        setCursorVFD(0x38);
        if (div_step > 3) {
            div_step = 0;
        }
        Serial.write(date_div[div_step]);
        div_step++;
        delay(50);
    }

    if (WiFi.status() == WL_CONNECTED) {
        adjustRTCviaNTP();
        setCursorVFD(0x45);
        Serial.print(WiFi.localIP());
        delay(2000);
    } else {
        WiFi.mode(WIFI_AP);
        WiFi.softAP(ap_ssid, ap_password);
        WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
        if (!rtc.isrunning()) {
            rtc.adjust(DateTime(2024, 2, 22, 22, 22, 22));
            clearVFD();
            setCursorVFD(0x31);
            Serial.print("Clock_AP vfdclock");
            setCursorVFD(0x45);
            Serial.print("192.168.4.2");
            delay(5000);
        }
    }
    startWebServer();
}

void handleTime() {
    now = rtc.now();
    calculateTime();
    flipDisplayedText == true ? drawTimeFlipped() : drawTime();
    div_step++;
    if (div_step > 3) {
        div_step = 0;
    }
}

void handleTemp() {
    flipDisplayedText == true ? drawTempFlipped() : drawTemp();
}

void handleTextFlip() {
    flipDisplayedText = !flipDisplayedText;

    clearVFD();
    flipDisplayedText == true ? drawTimeFlipped() : drawTime();
    flipDisplayedText == true ? drawTempFlipped() : drawTemp();
}

// Initial setup
void setup() {
    EEPROM.begin(512);
    delay(20);
    readSettingsFromEEPROM();
    Serial.begin(9600);
    sensors.begin();
    sensors.setResolution(11);
    rtc.begin();
    clearVFD();
    handleWiFi();
    clearVFD();
    now = rtc.now();
    calculateTime();
    drawTime();
    drawTemp();

    clearVFD();
}


// Main loop
void loop() {
  
    if (custom_text.equalsIgnoreCase("")) {
        displayCustomText = false;
    } else {
        displayCustomText = true;
    }

    if (displayCustomText) {
        unsigned long start_time = millis();
        clearVFD();

        while (millis() - start_time < TIME_TO_DISPLAY_CUSTOM_TEXT) {
            if (!custom_text.equalsIgnoreCase("")) {
                drawCustomText(custom_text);
            }
        }

        custom_text = "";
        clearVFD();
    } else {

        current_millis = millis();

        if (current_millis - last_millis >= 500) {
            handleTime();
            last_millis = current_millis;
        }
        
        if (current_millis - temp_millis >= 10000) {
            handleTemp();
            temp_millis = current_millis;
        }

        if (current_millis - text_flip_millis >= 3600000) {
            handleTextFlip();
            text_flip_millis = current_millis;
        }
    }

    if (h_hours + h_minutes + h_seconds == "222300" && WiFi.status() == WL_CONNECTED) {
        adjustRTCviaNTP();
    }
}