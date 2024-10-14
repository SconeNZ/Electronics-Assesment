#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT11.h>

// Initialize the LCD display with I2C address 0x27 and size 20x4
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Initialize the DHT11 sensor on digital pin 2
DHT11 dht11(2);

// Define the pin for the relay
const int relayPin = 8;  // Pin D8 for relay

// Define a temperature threshold for relay activation
const int tempThreshold = 20;

void setup() {
  // Initialize the LCD
  lcd.init();
  lcd.backlight();
  
  // Set up serial communication for debugging (optional)
  Serial.begin(9600);
  
  // Set the relay pin as an output
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);  // Make sure the relay is off initially
  
  // Print a welcome message to the LCD
  lcd.setCursor(3, 0);
  lcd.print("DHT11 Sensor");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  
  // Wait 2 seconds before starting
  delay(2000);
  
  // Clear the initialization message
  lcd.clear();
}

void loop() {
  int temperature = 0;
  int humidity = 0;

  // Read temperature and humidity from the DHT11 sensor
  int result = dht11.readTemperatureHumidity(temperature, humidity);

  // If the reading is successful, display values on the LCD
  if (result == 0) {
    // Update temperature and humidity on the LCD without clearing the screen
    lcd.setCursor(0, 0); // Set cursor to the first line
    lcd.print("Temp: ");
    lcd.print(temperature);
    lcd.print(" C   ");  // Extra spaces to overwrite previous characters

    lcd.setCursor(0, 1); // Set cursor to the second line
    lcd.print("Humidity: ");
    lcd.print(humidity);
    lcd.print(" %   ");  // Extra spaces to overwrite previous characters

    // Optionally, print to Serial for debugging
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C\tHumidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    // Control the relay based on the temperature
    if (temperature >= tempThreshold) {
      digitalWrite(relayPin, HIGH);  // Turn the relay on
      lcd.setCursor(0, 2);  // Set cursor to the third line
      lcd.print("Relay ON       ");  // Display Relay status
    } else {
      digitalWrite(relayPin, LOW);  // Turn the relay off
      lcd.setCursor(0, 2);  // Set cursor to the third line
      lcd.print("Relay OFF      ");  // Display Relay status
    }
    
  } else {
    // Handle any error and print the error message to the LCD
    lcd.setCursor(0, 0);
    lcd.print("Error: ");
    lcd.print(DHT11::getErrorString(result));

    // Optionally, print error to Serial for debugging
    Serial.println(DHT11::getErrorString(result));
  }

  // Wait 2 seconds before the next reading
  delay(2000);
}
