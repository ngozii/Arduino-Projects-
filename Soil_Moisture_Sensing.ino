
// Define the pins
int sensorPin = A0;
int ledPin = 7;

// Variables to store sensor value
int sensorValue = 0;
// Note: Calibrate valAir and valWater values for your specific sensor
const int valAir = 1023; // Sensor value when completely dry (in air)
const int valWater = 0; // Sensor value when fully submerged in water

// Converts raw sensor value to moisture percentage (0-100%)
int toPercent(int sensorValue) {
  int percent = map(sensorValue, valAir, valWater, 0, 100); 
  return constrain(percent, 0, 100); 
}
void setup() {
  // Initialize serial communication at 9600 baud rate
  Serial.begin(9600);
  
  // Initialize the LED pin as an output
  pinMode(ledPin, OUTPUT);
  
}

void loop() {
  // Read the analog value from the sensor
  sensorValue = analogRead(sensorPin);
  // Converts raw sensor value to moisture percentage (0-100%)
   int percent = toPercent( sensorValue);
  
  Serial.print("Soil Moisture Value: ");
  Serial.println(sensorValue);
  Serial.print(percent);
  Serial.println("%");
  
  // Check if the soil needs watering (Higher values signify drier soil)
  if (sensorValue > 500) {  // Threshold for "dry" soil (you can tune based on your sensor/soil type)
    digitalWrite(ledPin, HIGH);  // LED on = needs watering
} else {
    digitalWrite(ledPin, LOW);   // LED off = soil is moist
}
  
  // Wait for a second before taking another reading
  delay(1000);
}
