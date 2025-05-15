/***********************************************************************
 * Project      :     tenergy32hub_lorajson_template
 * Description  :     Template coding for tenergy32hub on vscode with platformIO
 * Hardware     :     tenergy32hub
 * Author       :     Tenergy Innovation Co., Ltd.
 * Date         :     27/04/2025
 * Revision     :     1.0
 * Rev1.0       :     Original
 * website      :     http://www.tenergyinnovation.co.th
 * Email        :     uten.boonliam@tenergyinnovation.co.th
 * TEL          :     +66 89-140-7205
 ***********************************************************************/
#include <Arduino.h>
#include <tenergy32hub.h>
#include <esp_task_wdt.h>
#include <ArduinoJson.h>

/**************************************/
/*          Firmware Version          */
/**************************************/
String version = "0.1";

/**************************************/
/*          Header project            */
/**************************************/
void header_print(void)
{
    Serial.printf("\r\n***********************************************************************\r\n");
    Serial.printf("* Project      :     tenergy32hub_lorajson_template\r\n");
    Serial.printf("* Description  :     Template coding for tenergy32hub on vscode with platformIO\r\n");
    Serial.printf("* Hardware     :     tenergy32hub\r\n");
    Serial.printf("* Author       :     Tenergy Innovation Co., Ltd.\r\n");
    Serial.printf("* Date         :     04/07/2022\r\n");
    Serial.printf("* Revision     :     %s\r\n", version);
    Serial.printf("* Rev1.0       :     Origital\r\n");
    Serial.printf("* website      :     http://www.tenergyinnovation.co.th\r\n");
    Serial.printf("* Email        :     uten.boonliam@tenergyinnovation.co.th\r\n");
    Serial.printf("* TEL          :     +66 89-140-7205\r\n");
    Serial.printf("***********************************************************************/\r\n");
}

/**************************************/
/*        define object variable      */
/**************************************/
Tenergy32Hub mcu;

/**************************************/
/*            GPIO define             */
/**************************************/

/**************************************/
/*       Constand define value        */
/**************************************/
// 10 seconds WDT
#define WDT_TIMEOUT 10

/**************************************/
/*       eeprom address define        */
/**************************************/

/**************************************/
/*        define global variable      */
/**************************************/

// เพิ่มตัวแปรนับรอบไว้ด้านบน (global)
uint8_t loraCounter = 1;
String unitName = "";
String fwVersion = "0.1";      // Version ของโปรแกรมที่ใช้ในบอร์ดนี้
String topic = "tenergy32hub"; // Topic ที่ใช้ในการส่งข้อมูลไปยัง LoRa Gateway

/**************************************/
/*           define function          */
/**************************************/
String getUnitNameFromMac();

/***********************************************************************
 * FUNCTION:    getUnitNameFromMac
 * DESCRIPTION: สร้างชื่อ unitName จาก MAC Address (6 ตัวหลัง)
 * RETURNED:    String ชื่อบอร์ด tenergy32gateway-xxxxxx
 ***********************************************************************/
String getUnitNameFromMac()
{
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    char macStr[7];
    snprintf(macStr, sizeof(macStr), "%02X%02X%02X", mac[3], mac[4], mac[5]);
    return "tenergy32hub-" + String(macStr);
}

/***********************************************************************
 * FUNCTION:    setup
 * DESCRIPTION: setup process
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
void setup()
{
    // Initialize serial communication and print the header
    Serial.begin(115200);
    header_print();

    // Initialize and enable the watchdog with a 10-second timeout.
    esp_task_wdt_init(WDT_TIMEOUT, true); // true resets the CPU on WDT timeout
    esp_task_wdt_add(NULL);               // Add current task to watchdog monitoring

    mcu.begin();
    mcu.displayOLEDInfo();

    unitName = getUnitNameFromMac();
    Serial.printf("unitName: %s\r\n", unitName.c_str());
    mcu.displayOLED(unitName.c_str());
}

/***********************************************************************
 * FUNCTION:    loop
 * DESCRIPTION: loop process
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
void loop()
{
    // --- สร้าง JSON ข้อมูลที่จะส่ง ---
    StaticJsonDocument<256> doc;
    doc["id"] = unitName;
    doc["fw"] = fwVersion;
    doc["rssi"] = ""; // ถ้ายังไม่มีค่า rssi ให้ใส่ค่าว่างหรือค่าที่ต้องการ
    doc["topic"] = topic;
    doc["counter"] = loraCounter++;
    doc["param_1"] = 1.23;
    doc["param_2"] = 2.34;
    doc["param_3"] = 3.45;
    doc["param_4"] = 4.56;
    doc["param_5"] = 5.67;
    doc["param_6"] = 6.78;
    doc["param_7"] = 7.89;
    doc["param_8"] = 8.90;
    doc["param_9"] = 9.01;
    doc["param_10"] = 10.12;

    char jsonBuffer[256];
    size_t len = serializeJson(doc, jsonBuffer, sizeof(jsonBuffer));

    // --- ส่ง JSON ผ่าน LoRa ---
    bool sent = mcu.sendLoRa((uint8_t *)jsonBuffer, len);
    if (sent)
    {
        Serial.printf("LoRa sent: %s\r\n", jsonBuffer);
        Serial.printf("Data size: %d bytes\n", (int)len); // แสดงขนาดข้อมูลที่ส่ง
        mcu.clearOLED();
        char sizeStr[32];
        snprintf(sizeStr, sizeof(sizeStr), "Size: %d bytes", (int)len);
        char counterStr[16];
        snprintf(counterStr, sizeof(counterStr), "Count: %d", loraCounter - 1); // -1 เพราะเพิ่มไปแล้ว
        mcu.displayOLEDLines("LoRa sent OK", sizeStr, counterStr); // แสดงขนาดข้อมูลที่บรรทัด 2, counter ที่บรรทัด 3
    }
    else
    {
        Serial.println("LoRa send failed");
        mcu.displayOLED("LoRa send failed");
    }

    esp_task_wdt_reset();
    vTaskDelay(1000);
}