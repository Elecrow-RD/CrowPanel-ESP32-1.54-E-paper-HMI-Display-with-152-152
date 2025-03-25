#include <Arduino.h>
#include "EPD_GUI.h"

// Define a black and white image array as the buffer for e-paper display
uint8_t ImageBW[2888];

// Pin definition for the home button
#define HOME_KEY 2

// Counter variable for the home button
int HOME_NUM = 0;

// Setup function, called once at the start of the program
void setup() {
    // Initialize serial communication with a baud rate of 115200
    Serial.begin(115200);

    // Set the screen power pin to output mode and set it to high level to turn on the power
    pinMode(7, OUTPUT);
    digitalWrite(7, HIGH);

    // Set the POWER LED pin to output mode
    pinMode(19, OUTPUT);

    // Set the home button pin to input mode
    pinMode(HOME_KEY, INPUT);
}

// Main loop function, which is executed repeatedly
void loop() {
    // Flag variable to indicate if any button has been pressed
    int flag = 0;

    // Check if the home button has been pressed
    if (digitalRead(HOME_KEY) == 0) {
        delay(100); // Debounce delay to ensure the button press is stable

        // Check the home button status again to prevent false triggering due to jitter
        if (digitalRead(HOME_KEY) == 1) {
            Serial.println("HOME_KEY"); // Print the information that the home button has been pressed to the serial monitor
            HOME_NUM =!HOME_NUM; // Toggle the status of the home button (on/off)

            flag = 1; // Set the flag to indicate that the display content needs to be updated
        }
    }

    // If any button has been pressed, update the display content
    if (flag == 1) {
        char buffer[30]; // Buffer used to store the display content

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

        // Set the status of the POWER LED and the display message according to the status of the home button
        if (HOME_NUM == 1) {
            digitalWrite(19, HIGH); // Turn on the POWER LED
            strcpy(buffer, "PWR:on"); // Set the display content to "PWR:on"
        } else {
            digitalWrite(19, LOW); // Turn off the POWER LED
            strcpy(buffer, "PWR:off"); // Set the display content to "PWR:off"
        }

        // Display the content on the e-paper
        EPD_ShowString(0, 0 + 0 * 20, buffer, 16, BLACK);

        EPD_Display(ImageBW);  // Display the image on the EPD
        EPD_FastUpdate();
        EPD_DeepSleep();
    }
}
