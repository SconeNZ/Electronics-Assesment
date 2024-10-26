#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT11.h>

// Initialize the LCD display with I2C address 0x27 and size 20x4
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Initialize the DHT11 sensor on digital pin 2
DHT11 dht11(2);

// Define the pin for the relay
const int relayPin = 8;  // Pin D8 for relay

// Define a temperature threshold for relay. To change to humidity - const int humidityThreshold = 70
const int tempThreshold = 20;

// Define the pin for the button
const int buttonPin = 3;  // Pin D3 for button (for interrupt)

// Volatile variable to track the last time the button was pressed
volatile unsigned long lastPressed = 0;
volatile bool buttonPressed = false;  // Track if the button was pressed

// Variable to track the backlight state
bool backlightOn = false;

void setup() {
  // Initialize the LCD
  lcd.init();
  lcd.backlight();  // Backlight starts on initially

  // Set up serial communication 
  Serial.begin(9600);
  
  // Set the relay pin as an output
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);  // Make sure the relay is off initially
  
  // Set the button pin as an input with a pull-up resistor
  pinMode(buttonPin, INPUT_PULLUP);

  // Attach interrupt to the button pin, triggering on a button press
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonPressed, FALLING);

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

  // Check if button was pressed
  if (buttonPressed) {
    lastPressed = millis();      // Record the last time the button was pressed
    lcd.backlight();             // Turn on the backlight
    backlightOn = true;          // Set backlight state to on
    buttonPressed = false;   // Reset the button pressed flag
  }

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

  // Check if 5 seconds have passed since the button was pressed
  if (backlightOn && millis() > lastPressed + 5000) {
    lcd.noBacklight();  // Turn off the backlight
    backlightOn = false;
  }

  // Wait 2 seconds before the next reading
  delay(2000);
}

// ISR function to handle button press
void buttonPressed() {
  unsigned long currentMillis = millis();
  
  // Simple debounce logic: only act on the button press if 200ms have passed
  if (currentMillis - lastPressed > 200) {
    buttonPressed = true;  // Set the flag to handle the button press in the loop
    lastPressed = currentMillis;  // Update the lastPressed time
  }
}
