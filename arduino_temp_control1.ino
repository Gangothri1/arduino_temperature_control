// Constants for pins
const int LM35_PIN = A0; // LM35 temperature sensor pin
const int LED_PIN = 13;    // Onboard LED pin

// Constants for temperature thresholds and blink intervals
const int TEMP_THRESHOLD = 30;         // Temperature threshold in degrees Celsius
const unsigned long BLINK_INTERVAL_1 = 250; // Blink interval when below threshold (milliseconds)
const unsigned long BLINK_INTERVAL_2 = 500; // Blink interval when above threshold (milliseconds)

// Variables to store temperature and LED state
int temperature;
bool ledState = LOW;

// Variables to track timer interrupts
volatile bool interruptFlag = false;
volatile unsigned long timerCounter = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);  // Set LED pin as output

  // Configure Timer1 for interrupt every 1 millisecond
  cli(); // Disable interrupts
  TCCR1A = 0; // Set Timer1 control register A to 0
  TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10); // Set Timer1 control register B for CTC mode and prescaler of 64
  OCR1A = 249; // Set Timer1 output compare register for 1 millisecond
  TIMSK1 = (1 << OCIE1A); // Enable Timer1 output compare A interrupt
  sei(); // Enable interrupts
}

void loop() {
  if (interruptFlag) {
    interruptFlag = false; // Reset interrupt flag
    temperature = readTemperature(TMP36_PIN); // Read temperature from LM35 sensor

    if (temperature < TEMP_THRESHOLD) {
      updateLED(BLINK_INTERVAL_1); // Update LED state with interval 250 ms if temperature is below threshold
    } else {
      updateLED(BLINK_INTERVAL_2); // Update LED state with interval 500 ms if temperature is above or equal to threshold
    }
  }
}

// Function to read temperature from LM35sensor
int readTemperature(int pin) {
  int rawValue = analogRead(pin); // Read raw ADC value from LM35 sensor
  float temperatureC = (rawValue * 5.0 / 1023) * 100; // Convert raw ADC value to temperature in Celsius
  return (int)temperatureC; // Return temperature
}

// Function to control LED blinking
void updateLED(unsigned long interval) {
  static unsigned long previousToggleTime = 0; // Static variable to store previous toggle time

  if (timerCounter - previousToggleTime >= interval) {
    previousToggleTime = timerCounter; // Save the last time LED was toggled

    // Toggle the LED state
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState); // Update LED state
  }
}

// Timer1 output compare A interrupt service routine
ISR(TIMER1_COMPA_vect) {
  interruptFlag = true; // Set interrupt flag
  timerCounter++; // Increment timer counter
}

