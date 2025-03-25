#include <Arduino.h>  // Include the Arduino library which provides basic functionality
#include "EPD_GUI.h"      // Include the e-paper display library used to control the e-paper display screen
#include <WiFi.h>     // Include the WiFi library used for WiFi connection

// Define the SSID and password of the WiFi network
String ssid = "yanfa_software";
String password = "yanfa-123456";

// Define a black and white image array as the buffer for e-paper display
uint8_t ImageBW[2888];

// Setup function, called once at the start of the program
void setup() {
    // Initialize serial communication with a baud rate of 115200
    Serial.begin(115200);

    // Set the screen power pin to output mode and set it to high level to turn on the power
    pinMode(7, OUTPUT);
    digitalWrite(7, HIGH);

    // Start connecting to WiFi
    WiFi.begin(ssid, password);

    // Wait for the WiFi connection to be successful
    while (WiFi.status()!= WL_CONNECTED) {
        delay(500); // Check the connection status every 500 milliseconds
        Serial.print("."); // Output a dot on the serial monitor to show the connection progress
    }

    // Output information after the WiFi connection is successful
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP()); // Output the IP address of the device

    // Create character arrays for displaying information
    char buffer[40];
    char buffer1[40];
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

    // Set the text content to be displayed
    strcpy(buffer, "WiFi connected"); // Copy "WiFi connected" to buffer
    EPD_ShowString(0, 0 + 0 * 20, buffer, 16, BLACK); // Display the text on the e-paper at position (0, 0) with a font size of 16

    // Set and display the IP address
    strcpy(buffer, "IP address: "); // Copy "IP address: " to buffer
    strcpy(buffer1, WiFi.localIP().toString().c_str()); // Append the IP address to buffer
    EPD_ShowString(0, 0 + 1 * 20, buffer, 16, BLACK); // Display the IP address on the e-paper at position (0, 20) with a font size of 16
    EPD_ShowString(0, 0 + 2 * 20, buffer1, 16, BLACK); // Display the IP address on the e-paper at position (0, 20) with a font size of 16

    EPD_Display(ImageBW);  // Display the image on the EPD
    EPD_FastUpdate();
    EPD_DeepSleep();
}

// Main loop function, currently no functionality is implemented in this loop
void loop() {
    delay(10); // Wait for 10 milliseconds
}
