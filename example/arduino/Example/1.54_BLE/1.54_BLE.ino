#include "EPD_GUI.h"
#include "BLEDevice.h"              // BLE driver library
#include "BLEServer.h"              // BLE Bluetooth server library
#include "BLEUtils.h"               // BLE utility library
#include "BLE2902.h"                // Characteristic addition descriptor library

BLECharacteristic *pCharacteristic;
BLEServer *pServer;
BLEService *pService;
bool deviceConnected = false;
char BLEbuf[32] = {0};
uint32_t cnt = 0;

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

// Class to handle callbacks for the BLE server
class MyServerCallbacks : public BLEServerCallbacks {
    // Callback function when a device connects
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
        Serial.println("------> BLE connect.");
    }

    // Callback function when a device disconnects
    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        Serial.println("------> BLE disconnect.");
        pServer->startAdvertising(); // Restart advertising
        Serial.println("start advertising");
    }
};

// Class to handle callbacks for the BLE characteristic
class MyCallbacks : public BLECharacteristicCallbacks {
    // Callback function when data is written to the characteristic
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string rxValue = pCharacteristic->getValue();

        if (rxValue.length() > 0) {
            Serial.print("------>Received Value: ");

            for (int i = 0; i < rxValue.length(); i++) {
                Serial.print(rxValue[i]);
            }
            Serial.println();

            if (rxValue.find("A")!= -1) {
                Serial.print("Rx A!");
            }
            else if (rxValue.find("B")!= -1) {
                Serial.print("Rx B!");
            }
            Serial.println();
        }
    }
};

uint8_t ImageBW[2888];

// Setup function, called once at the start
void setup() {
    Serial.begin(115200);

    // Initialize settings here
    pinMode(7, OUTPUT); // Set GPIO pin 7 as output mode, used to control the screen power
    digitalWrite(7, HIGH); // Set GPIO pin 7 to high level, turn on the screen power

    // Create the BLE Device
    BLEDevice::init("CrowPanel1-54");

    // Create the Bluetooth server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Create the UUID for the advertising service
    pService = pServer->createService(SERVICE_UUID);

    // Create a characteristic for transmitting data with notify property
    pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);
    pCharacteristic->addDescriptor(new BLE2902());

    // Create a characteristic for receiving data with write property
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
    pCharacteristic->setCallbacks(new MyCallbacks());

    // Start the Bluetooth service
    pService->start();

    // Start advertising
    pServer->getAdvertising()->start();
}

int flag = 0;  // Define and initialize a flag variable to track device connection status and display content

// Main loop, code inside will be executed repeatedly
void loop() {
    // Check if the device is connected
    if (deviceConnected) {
        memset(BLEbuf, 0, 32);  // Clear the first 32 bytes of the BLEbuf buffer

        memcpy(BLEbuf, (char*)"Hello BLE APP!", 32);  // Copy the string "Hello BLE APP!" to BLEbuf

        pCharacteristic->setValue(BLEbuf);  // Set the characteristic value to the content of BLEbuf

        pCharacteristic->notify(); // Send a notification to the application, transmitting the value of BLEbuf
        Serial.print("*** Sent Value: ");  // Print the sent value to the serial monitor
        Serial.print(BLEbuf);
        Serial.println(" ***");

        if (flag!= 2)  // If the flag is not 2 (i.e., the connection status has not been processed before), set the flag to 1
            flag = 1;
    } else {
        if (flag!= 4)  // If the device is not connected and the flag is not 4 (i.e., the disconnection status has not been processed before), set the flag to 3
            flag = 3;
    }

    if (flag == 1) {  // Device is connected and the flag is 1
        char buffer[30];  // Buffer for displaying
        char buffer1[30];  // Buffer for displaying
        char buffer2[30];  // Buffer for displaying

        EPD_GPIOInit();
        Paint_NewImage(ImageBW, EPD_W, EPD_H, 270, WHITE);
        Paint_Clear(WHITE);
        EPD_FastMode1Init();
        EPD_Display_Clear();
        EPD_FastUpdate();
        EPD_Clear_R26H();

        EPD_FastMode1Init();
        EPD_Display_Clear();
        EPD_FastUpdate();
        EPD_Clear_R26H();

        strcpy(buffer, "Bluetooth connected");  // Set the string to be displayed
        strcpy(buffer1, "Sent Value:");  // Set the prefix for the value to be displayed
        strcpy(buffer2, "Hello BLE APP!");  // Append the message content to the value prefix

        EPD_ShowString(0, 0 + 0 * 20, buffer, 16, BLACK);  // Display "Bluetooth connected" on the EPD
        EPD_ShowString(0, 0 + 1 * 20, buffer1, 16, BLACK);  // Display "Sent Value: Hello BLE APP!" on the EPD
        EPD_ShowString(0, 0 + 2 * 20, buffer2, 16, BLACK);  // Display "Sent Value: Hello BLE APP!" on the EPD

        EPD_Display(ImageBW);  // Display the image on the EPD
        EPD_FastUpdate();
        EPD_DeepSleep();

        flag = 2;  // Update the flag to 2, indicating that the device connection display has been processed
    } else if (flag == 3) {  // Device is not connected and the flag is 3
        char buffer[30];  // Buffer for displaying
        char buffer1[30];  // Buffer for displaying

        EPD_GPIOInit();
        Paint_NewImage(ImageBW, EPD_W, EPD_H, 270, WHITE);
        Paint_Clear(WHITE);
        EPD_FastMode1Init();
        EPD_Display_Clear();
        EPD_FastUpdate();
        EPD_Clear_R26H();

        EPD_FastMode1Init();
        EPD_Display_Clear();
        EPD_FastUpdate();
        EPD_Clear_R26H();

        strcpy(buffer, "Bluetooth not ");  // Set the string to be displayed
        strcpy(buffer1, "connected!");  // Set the string to be displayed

        EPD_ShowString(0, 0 + 0 * 20, buffer, 16, BLACK);  // Display "Bluetooth not connected!" on the EPD
        EPD_ShowString(0, 0 + 1 * 20, buffer1, 16, BLACK);  // Display "Bluetooth not connected!" on the EPD

        EPD_Display(ImageBW);  // Display the image on the EPD
        EPD_FastUpdate();
        EPD_DeepSleep();

        flag = 4;  // Update the flag to 4, indicating that the device disconnection display has been processed
    }

    delay(1000);  // Delay for 1000 milliseconds (1 second), then re-enter the loop
}
