#include <Arduino.h>
#include "EPD_GUI.h"

// Pre-allocated black and white image array
uint8_t ImageBW[2888];

// Button definitions
#define HOME_KEY 2      // The home button is connected to digital pin 2
#define EXIT_KEY 1      // The exit button is connected to digital pin 1
#define PRV_KEY 6       // The previous page button is connected to digital pin 6
#define NEXT_KEY 4      // The next page button is connected to digital pin 4
#define OK_KEY 5        // The OK button is connected to digital pin 5

// Initialize counters for each button
int HOME_NUM = 0;
int EXIT_NUM = 0;
int PRV_NUM = 0;
int NEXT_NUM = 0;
int OK_NUM = 0;

// Array to store button counts
int NUM_btn[5] = {0};

// Function to count button presses and display the counts
void count_btn(int NUM[5])
{
    char buffer[30];  // Buffer to store strings

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

    // Display the count of each button
    int length = sprintf(buffer, "HOME_KEY_NUM:%d", NUM[0]);
    buffer[length] = '\0';
    EPD_ShowString(0, 0 + 0 * 20, buffer, 16, BLACK);

    length = sprintf(buffer, "EXIT_KEY_NUM:%d", NUM[1]);
    buffer[length] = '\0';
    EPD_ShowString(0, 0 + 1 * 20, buffer, 16, BLACK);

    length = sprintf(buffer, "PRV_KEY_NUM:%d", NUM[2]);
    buffer[length] = '\0';
    EPD_ShowString(0, 0 + 2 * 20, buffer, 16, BLACK);

    length = sprintf(buffer, "NEXT__NUM:%d", NUM[3]);
    buffer[length] = '\0';
    EPD_ShowString(0, 0 + 3 * 20, buffer, 16, BLACK);

    length = sprintf(buffer, "OK_NUM:%d", NUM[4]);
    buffer[length] = '\0';
    EPD_ShowString(0, 0 + 4 * 20, buffer, 16, BLACK);

    // Update the display content and enter deep sleep mode
    EPD_Display(ImageBW);  // Display the image on the EPD
    EPD_FastUpdate();
    EPD_DeepSleep();
}

// Setup function
void setup() {
    Serial.begin(115200);  // Initialize serial communication with a baud rate of 115200

    // Set up button pins
    pinMode(7, OUTPUT);    // Set pin 7 as output, used for power control
    digitalWrite(7, HIGH); // Set the power to high level to turn on the screen power

    pinMode(HOME_KEY, INPUT);  // Set the home button pin as input
    pinMode(EXIT_KEY, INPUT);  // Set the exit button pin as input
    pinMode(PRV_KEY, INPUT);   // Set the previous page button pin as input
    pinMode(NEXT_KEY, INPUT);  // Set the next page button pin as input
    pinMode(OK_KEY, INPUT);    // Set the OK button pin as input
}

// Main loop function
void loop() {
    int flag = 0; // Flag to indicate if any button has been pressed

    // Check if the home button has been pressed
    if (digitalRead(HOME_KEY) == 0)
    {
        delay(100); // Debounce delay
        if (digitalRead(HOME_KEY) == 1)
        {
            Serial.println("HOME_KEY"); // Print the information that the home button has been pressed
            HOME_NUM++; // Increment the home button count
            flag = 1; // Set the flag
        }
    }
    // Check if the exit button has been pressed
    else if (digitalRead(EXIT_KEY) == 0)
    {
        delay(100); // Debounce delay
        if (digitalRead(EXIT_KEY) == 1)
        {
            Serial.println("EXIT_KEY"); // Print the information that the exit button has been pressed
            EXIT_NUM++; // Increment the exit button count
            flag = 1; // Set the flag
        }
    }
    // Check if the previous page button has been pressed
    else if (digitalRead(PRV_KEY) == 0)
    {
        delay(100); // Debounce delay
        if (digitalRead(PRV_KEY) == 1)
        {
            Serial.println("PRV_KEY"); // Print the information that the previous page button has been pressed
            PRV_NUM++; // Increment the previous page button count
            flag = 1; // Set the flag
        }
    }
    // Check if the next page button has been pressed
    else if (digitalRead(NEXT_KEY) == 0)
    {
        delay(100); // Debounce delay
        if (digitalRead(NEXT_KEY) == 1)
        {
            Serial.println("NEXT_KEY"); // Print the information that the next page button has been pressed
            NEXT_NUM++; // Increment the next page button count
            flag = 1; // Set the flag
        }
    }
    // Check if the OK button has been pressed
    else if (digitalRead(OK_KEY) == 0)
    {
        delay(100); // Debounce delay
        if (digitalRead(OK_KEY) == 1)
        {
            Serial.println("OK_KEY"); // Print the information that the OK button has been pressed
            OK_NUM++; // Increment the OK button count
            flag = 1; // Set the flag
        }
    }

    // If any button has been pressed, update the display content
    if (flag == 1)
    {
        NUM_btn[0] = HOME_NUM;
        NUM_btn[1] = EXIT_NUM;
        NUM_btn[2] = PRV_NUM;
        NUM_btn[3] = NEXT_NUM;
        NUM_btn[4] = OK_NUM;

        count_btn(NUM_btn); // Call the function to update the display
        flag = 0; // Reset the flag
    }
}