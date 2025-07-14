// Advanced Calculator with LCD display and keypad input
// Supports arithmetic, trigonometric, exponential, and logarithmic operations
// Uses 4x4 keypad for input and 16x2 LCD for output
// User controls: # = confirm entry, C = clear, D = backspace


#include <math.h> // Include math library 
#include <LiquidCrystal_I2C.h> // Import LCD library 
#include <Keypad.h> // Import keypad library

// LCD connected via I2C at address 0x27, 16 columns x 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2);
String input = "";

// 4x4 keypad configuration
const byte ROWS = 4;
const byte COLS = 4;

// Keypad layout mapping
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Pin connections for keypad rows and columns
byte rowPins[ROWS] = {9, 8, 7, 6};  // Connect to digital pins 9, 8, 7, 6
byte colPins[COLS] = {5, 4, 3, 2};  // Connect to digital pins 5, 4, 3, 2


Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Display main menu and reset calculator to initial state
void resetCalculator();

// Get numeric input from keypad with validation
// Returns: float value entered by user (# confirms input)
float getNumberFromKeypad(const char* prompt);

// Handle basic arithmetic operations (+, -, *, /)
void arithmetic();

// Handle trigonometric functions (sin, cos)
// Note: Input angles must be in radians
void trigonometric();

// Calculate base raised to power using pow() function
void exponential();


// Calculate natural logarithm (ln) of input value
// Note: Returns error for values <= 0 (undefined for ln)
void logarithmic();

void setup() {
  lcd.init();        // Initialize LCD display
  lcd.backlight();   // Turn on LCD backlight
  resetCalculator(); // Show initial menu
}

void loop() {
  char key = keypad.getKey(); 

  if (key) {
    // Menu selection: 1=arithmetic, 2=trig, 3=exponential, 4=logarithmic
    if (key == '1') arithmetic();
    else if (key == '2') trigonometric();
    else if (key == '3') exponential();
    else if (key == '4') logarithmic();
    else {
        lcd.setCursor(3, 0);
        lcd.print("Error!"); // Invalid menu selection
    }
  }
}

void resetCalculator() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("ADVANCED");
  lcd.setCursor(2, 1);
  lcd.print("CALCULATOR");
  delay(2000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1.Arith  2.Trig ");
  lcd.setCursor(0, 1);
  lcd.print("3.Expo  4.Log ");
}

float getNumberFromKeypad(const char* prompt) {
  String numInput = "";
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(prompt);
  
  
  while (true) {
    char key = keypad.getKey();
    if (key) {
      if (key >= '0' && key <= '9') {
        numInput += key;
        lcd.setCursor(0, 1);
        lcd.print(numInput);
      } else if (key == '#') {
        return numInput.toFloat();   // # key confirms number entry
      } else if (key == 'C') {
        numInput = "";               // C key clears current input
        lcd.setCursor(0, 1);
        lcd.print("       ");        // Clear display line
      } else if (key == 'D' && numInput.length() > 0) {
        numInput.remove(numInput.length() - 1);   // D key acts as backspace

        lcd.setCursor(0, 1);
        lcd.print("       ");
        lcd.setCursor(0, 1);
        lcd.print(numInput);
      }
    }
  }
}

void arithmetic() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1:+ 2:- 3:* 4:/");
  
  char op;
  // Wait for valid operation selection (1-4)
  while (true) {
    op = keypad.getKey();
    if (op >= '1' && op <= '4') break;
  }

  float A = getNumberFromKeypad("Enter first num:");
  float B = getNumberFromKeypad("Enter second num:");
  float result = 0;

  switch (op) {
    case '1': result = A + B; break;
    case '2': result = A - B; break;
    case '3': result = A * B; break;
    // Check for division by zero to prevent undefined result
    case '4': result = (B != 0) ? A / B : NAN; break;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Result:");
  lcd.setCursor(0, 1);
  if (op == '4' && B == 0) lcd.print("Error: Div by 0");
  else lcd.print(result);
  
  delay(3000);
  resetCalculator();
}

void trigonometric() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1.Sin 2.Cos");
  
  char op;
  // Wait for valid operation selection (1 or 2)
  while (true) {
    op = keypad.getKey();
    if (op == '1' || op == '2') break;
  }

  float val = getNumberFromKeypad("Enter angle (rad):");
  float result = (op == '1') ? sin(val) : cos(val);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Result:");
  lcd.setCursor(0, 1);
  lcd.print(result);

  delay(3000);
  resetCalculator();
}

void exponential() {
  float base = getNumberFromKeypad("Enter base:");
  float exponent = getNumberFromKeypad("Enter exponent:");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Result:");
  lcd.setCursor(0, 1);
  lcd.print(pow(base, exponent));

  delay(3000);
  resetCalculator();
}

void logarithmic() {
  float value = getNumberFromKeypad("Enter value:");

  lcd.clear();
  lcd.setCursor(0, 0);
  if (value > 0) {
    lcd.print("Result:");
    lcd.setCursor(0, 1);
    lcd.print(log(value));
  } else {
    lcd.print("Error: Invalid");
  }

  delay(3500); // Display result for 3.5 seconds before returning to menu
  resetCalculator();
}
