#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include "EPD_GUI.h"
#include "Pic.h"

// Define an array for black and white image data, which serves as the buffer for the e-paper display.
// The size of the buffer is determined according to the resolution of the e-paper display.
uint8_t ImageBW[2888];

// WiFi network name.
const char* ssid = "yanfa_software";
// WiFi password.
const char* password = "yanfa-123456";

// OpenWeatherMap API key. This key is used to access weather data from the OpenWeatherMap API.
String openWeatherMapApiKey = "3c03d1f4e2dd3e14474a9a3a2f2299ff";

// City and country code for which weather data will be fetched.
String city = "London";
String countryCode = "2643743";

// Variables related to timing and API calls.
// lastTime keeps track of the last time the API was called.
unsigned long lastTime = 0;
// timerDelay determines the time interval between API calls. For testing, it's set to 10 seconds (10000 milliseconds).
// In a final application, this should be set according to the API call limits per hour/minute.
unsigned long timerDelay = 10000;

// Variables related to JSON data.
// jsonBuffer stores the raw JSON data received from the API.
String jsonBuffer;
// httpResponseCode holds the response code from the HTTP request.
int httpResponseCode;
// myObject is a JSON variable that will hold the parsed JSON data.
JSONVar myObject;

// Variables related to weather information.
// weather stores the main weather description.
String weather;
// temperature stores the temperature value.
String temperature;
// humidity stores the humidity value.
String humidity;
// sea_level stores the sea level value.
String sea_level;
// wind_speed stores the wind speed value.
String wind_speed;
// city_js stores the name of the city.
String city_js;
// weather_flag is a flag that determines which weather icon to display based on the weather description.
int weather_flag = 0;

// Function to display weather forecast on the e-paper display.
void UI_weather_forecast()
{
  char buffer[40];

  EPD_HW_RESET();

  // Display a background picture on the e-paper.
  EPD_ShowPicture(0, 0, 152, 152, pic, BLACK);


  //If you need to replace the display options,
  //open the following options: temp, wind, visit, hum
  //The following uses the hum option as an example

  //Visit
  //EPD_ShowPicture(76, 92, 56, 72, gImage_visit, BLACK);
  //Wind
  //EPD_ShowPicture(76, 92, 56, 72, gImage_wind, BLACK);
  //Temp
  //  EPD_ShowPicture(92, 0, 56, 72, gImage_temp, BLACK);
  //Hum

  EPD_Display(ImageBW);
  EPD_FastUpdate();
  EPD_DeepSleep();

  EPD_GPIOInit();
  Paint_NewImage(ImageBW, EPD_W, EPD_H, 270, WHITE);
  // Paint_Clear(WHITE);
  EPD_FastMode1Init();
  // EPD_Display_Clear();
  EPD_FastUpdate();
  EPD_Clear_R26H();

  // Display the update time (it seems to be mislabeled as city_js here, might need to be corrected depending on the actual intention).
  memset(buffer, 0, sizeof(buffer));
  snprintf(buffer, sizeof(buffer), "%s ", city_js);
  EPD_ShowString(20, 140, buffer, 12, BLACK);

  //If you need to replace the display options,
  //open the following options: temp, wind, visit, hum
  //The following uses the hum option as an example

  //Hum
  memset(buffer, 0, sizeof(buffer));
  snprintf(buffer, sizeof(buffer), "%s ", humidity);
  EPD_ShowString(105, 140, buffer,  12, BLACK);

  //Temp
  //       memset(buffer, 0, sizeof(buffer));
  //       snprintf(buffer, sizeof(buffer), "%s C", temperature);
  //       EPD_ShowString(105, 140, buffer, 12, BLACK);

  //Wind
  // memset(buffer, 0, sizeof(buffer));
  // snprintf(buffer, sizeof(buffer), "%s m/s", wind_speed);
  // EPD_ShowString(105, 140, buffer, 12, BLACK);

  //visit
  //  memset(buffer, 0, sizeof(buffer));
  //  snprintf(buffer, sizeof(buffer), "%s ", sea_level);
  //  EPD_ShowString(105, 140, buffer, 12, BLACK);

  // Update the e-paper display with the new content.
  EPD_Display(ImageBW);
  EPD_FastUpdate();
  EPD_DeepSleep();
}

void setup() {
  Serial.begin(115200); // Initialize the serial communication.

  // Connect to the WiFi network.
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP()); // Print the IP address of the device after successful connection.

  Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");

  // Set pin 7 as an output pin to control the power of the e-paper display.
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);

  EPD_GPIOInit();
  Paint_NewImage(ImageBW, EPD_W, EPD_H, 0, WHITE);
  Paint_Clear(WHITE);
  EPD_FastMode1Init();
  EPD_Display_Clear();
  EPD_FastUpdate();
  EPD_Clear_R26H();
}

void loop() {
  js_analysis();
  UI_weather_forecast();
  delay(1000 * 60 * 60);
}

void js_analysis()
{
  // Check if the device is connected to the WiFi network.
  if (WiFi.status() == WL_CONNECTED) {
    // Construct the URL for the OpenWeatherMap API request.
    String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey + "&units=metric";

    // Keep trying until a successful response (status code 200) is received.
    while (httpResponseCode != 200) {
      // Make an HTTP GET request to the API and store the response in jsonBuffer.
      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      myObject = JSON.parse(jsonBuffer);

      // Check if the JSON parsing was successful.
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
      delay(2000);
    }

    // Extract weather information from the parsed JSON data.
    weather = JSON.stringify(myObject["weather"][0]["main"]);
    temperature = JSON.stringify(myObject["main"]["temp"]);
    humidity = JSON.stringify(myObject["main"]["humidity"]);
    sea_level = JSON.stringify(myObject["main"]["sea_level"]);
    wind_speed = JSON.stringify(myObject["wind"]["speed"]);
    city_js = JSON.stringify(myObject["name"]);

    // Print the extracted weather information for debugging purposes.
    Serial.print("String weather: ");
    Serial.println(weather);
    Serial.print("String Temperature: ");
    Serial.println(temperature);
    Serial.print("String humidity: ");
    Serial.println(humidity);
    Serial.print("String sea_level: ");
    Serial.println(sea_level);
    Serial.print("String wind_speed: ");
    Serial.println(wind_speed);
    Serial.print("String city_js: ");
    Serial.println(city_js);

    // Determine the weather icon based on the weather description.
    if (weather.indexOf("clouds") != -1 || weather.indexOf("Clouds") != -1 ) {
      weather_flag = 1;
    } else if (weather.indexOf("clear sky") != -1 || weather.indexOf("Clear sky") != -1) {
      weather_flag = 3;
    } else if (weather.indexOf("rain") != -1 || weather.indexOf("Rain") != -1) {
      weather_flag = 5;
    } else if (weather.indexOf("thunderstorm") != -1 || weather.indexOf("Thunderstorm") != -1) {
      weather_flag = 2;
    } else if (weather.indexOf("snow") != -1 || weather.indexOf("Snow") != -1) {
      weather_flag = 4;
    } else if (weather.indexOf("mist") != -1 || weather.indexOf("Mist") != -1) {
      weather_flag = 0;
    }
  }
  else {
    // Print a message if the device is not connected to the WiFi network.
    Serial.println("WiFi Disconnected");
  }
}

// Function to make an HTTP GET request and return the response.
String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;

  // Initialize the HTTP client with the server name.
  http.begin(client, serverName);

  // Send an HTTP GET request.
  httpResponseCode = http.GET();

  // Initialize the response payload as an empty JSON object.
  String payload = "{}";

  // Check if the response code is positive (indicating a successful request).
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  // Close the HTTP connection.
  http.end();

  return payload;
}
