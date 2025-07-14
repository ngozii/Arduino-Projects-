int ledPin = 3;            // LED connected to pin 3
int buttonPin1 = 2;        // Button connected to pin 2
int buttonPin2 = 4;        // Button connected to pin 4
bool ledState = false;     // Current state of the LED
bool lastBState1 = false;  // Last state of the button1
bool lastBState2 = false;  // Last state of the button2

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin1, INPUT);  // Button pin1 set as input
  pinMode(buttonPin2, INPUT);  // Button pin2 set as input
  pinMode(ledPin, OUTPUT);     // LED pin set as output
  digitalWrite(ledPin, LOW);   // Ensure LED starts off
}

void loop() {
  bool onButton = digitalRead(buttonPin1);   // Read button1 state
  bool offButton = digitalRead(buttonPin2);  // Read button2 state

  if (onButton == HIGH && lastBState1 == LOW) {  // Detect button press
    ledState = true;
    digitalWrite(ledPin, HIGH);  // Turn LED on
    delay(50);
  }
  lastBState1 = onButton;  // Update the last button state

  if (offButton == HIGH && lastBState2 == LOW) {  // Detect button press
    ledState = false;
    digitalWrite(ledPin, LOW);  // Turn LED off
    delay(50);
  }
  lastBState2 = offButton;  // Update the last button state
}
