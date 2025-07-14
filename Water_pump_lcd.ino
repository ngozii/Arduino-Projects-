#include <Wire.h>
#include <RTClib.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

// Configuration Constants
namespace Config {
    // Time Constraints
    const int MIN_OPERATING_HOUR = 10;
    const int MAX_OPERATING_HOUR = 15;
    
    // Voltage Thresholds
    const float MIN_ACCEPTABLE_VOLTAGE = 180.0;
    const float MAX_ACCEPTABLE_VOLTAGE = 260.0;
    
    // Sensor Pins
    const int PIN_GPS_RX = 8;
    const int PIN_GPS_TX = 9;
    const int PIN_POTENTIOMETER = A0;
    const int PIN_WATER_SENSOR = A1;
    const int PIN_GRID_VOLTAGE = A2;
    const int PIN_INVERTER_VOLTAGE = A3;
    const int PIN_OVERRIDE_BUTTON = 2;
    const int PIN_PUMP_RELAY = 3;
    const int PIN_GRID_RELAY = 4;
    const int PIN_INVERTER_RELAY = 5;

    // Calibration
    const float VOLTAGE_DIVIDER_R1 = 30000.0;
    const float VOLTAGE_DIVIDER_R2 = 10000.0;
    
    // Potentiometer Threshold
    const int POT_OFFSET = 50; // Adjustable range around potentiometer value
}

class WaterPumpSystem {
private:
    // Hardware Interfaces
    SoftwareSerial gpsSerial;
    TinyGPSPlus gps;
    RTC_DS3231 rtc;
    LiquidCrystal_I2C lcd;

    // System State Variables
    bool manualOverride;
    bool pumpRunning;
    bool usingGridPower;
    
    // Sensor Readings
    int waterLevel;
    float gridVoltage;
    float inverterVoltage;
    int thresholdLevel;

    // Debounce Handling
    unsigned long lastDebounceTime;
    const unsigned long DEBOUNCE_DELAY = 1000;

    // Private Methods
    float readVoltage(int pin) {
        int analogValue = analogRead(pin);
        float voltage = (analogValue / 1023.0) * 5.0;
        return voltage * (Config::VOLTAGE_DIVIDER_R1 + Config::VOLTAGE_DIVIDER_R2) / Config::VOLTAGE_DIVIDER_R2;
    }

    void updateGPS() {
        while (gpsSerial.available()) {
            gps.encode(gpsSerial.read());
        }
    }

    int getCurrentHour() {
        if (gps.time.isValid()) {
            return gps.time.hour();
        } else {
            DateTime now = rtc.now();
            return now.hour();
        }
    }

    void displaySystemStatus() {
        lcd.clear();
        
        // First Line: Water Level and Threshold
        lcd.setCursor(0, 0);
        lcd.print("WL:");
        lcd.print(waterLevel);
        lcd.print(" TH:");
        lcd.print(thresholdLevel);

        // Second Line: Pump and Power Source Status
        lcd.setCursor(0, 1);
        if (manualOverride) {
            lcd.print("OVERRIDE ");
        }

        if (pumpRunning) {
            lcd.print("PUMP:ON ");
            lcd.print(usingGridPower ? "GRID" : "INV");
        } else {
            lcd.print("PUMP:OFF");
        }
    }

    void displayError(const String& errorMsg) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ERROR:");
        lcd.setCursor(0, 1);
        lcd.print(errorMsg);
    }

    void selectPowerSource() {
        bool gridOK = (gridVoltage >= Config::MIN_ACCEPTABLE_VOLTAGE && 
                       gridVoltage <= Config::MAX_ACCEPTABLE_VOLTAGE);
        bool inverterOK = (inverterVoltage >= Config::MIN_ACCEPTABLE_VOLTAGE && 
                           inverterVoltage <= Config::MAX_ACCEPTABLE_VOLTAGE);

        if (gridOK) {
            digitalWrite(Config::PIN_GRID_RELAY, HIGH);
            digitalWrite(Config::PIN_INVERTER_RELAY, LOW);
            usingGridPower = true;
        } else if (inverterOK) {
            digitalWrite(Config::PIN_GRID_RELAY, LOW);
            digitalWrite(Config::PIN_INVERTER_RELAY, HIGH);
            usingGridPower = false;
        } else {
            digitalWrite(Config::PIN_GRID_RELAY, LOW);
            digitalWrite(Config::PIN_INVERTER_RELAY, LOW);
            usingGridPower = false;
        }
    }

public:
    WaterPumpSystem() :
        gpsSerial(Config::PIN_GPS_RX, Config::PIN_GPS_TX),
        lcd(0x27, 16, 2),
        manualOverride(false),
        pumpRunning(false),
        usingGridPower(false),
        lastDebounceTime(0)
    {}

    void initialize() {
        // Serial and GPS Setup
        Serial.begin(9600);
        gpsSerial.begin(9600);
        
        // LCD Setup
        Wire.begin();
        lcd.init();
        lcd.backlight();
        
        // Pin Mode Setup
        pinMode(Config::PIN_OVERRIDE_BUTTON, INPUT_PULLUP);
        pinMode(Config::PIN_PUMP_RELAY, OUTPUT);
        pinMode(Config::PIN_GRID_RELAY, OUTPUT);
        pinMode(Config::PIN_INVERTER_RELAY, OUTPUT);
        
        // Initial State
        digitalWrite(Config::PIN_PUMP_RELAY, LOW);
        digitalWrite(Config::PIN_GRID_RELAY, LOW);
        digitalWrite(Config::PIN_INVERTER_RELAY, LOW);
        
        // RTC Check
        if (!rtc.begin()) {
            displayError("RTC Module Failed");
            while(1); // Halt system
        }
        
        lcd.clear();
        lcd.print("System Initialized");
        delay(2000);
    }

    void update() {
        // Read Sensors
        thresholdLevel = analogRead(Config::PIN_POTENTIOMETER);
        waterLevel = analogRead(Config::PIN_WATER_SENSOR);
        gridVoltage = readVoltage(Config::PIN_GRID_VOLTAGE);
        inverterVoltage = readVoltage(Config::PIN_INVERTER_VOLTAGE);

        // Check Voltage Sensor Validity
        if (gridVoltage < 0 || gridVoltage > 300 || 
            inverterVoltage < 0 || inverterVoltage > 300) {
            displayError("Voltage Sensor Error");
            stopPump();
            return;
        }

        // Manual Override Check
        checkManualOverride();

        // Pump Control Logic
        controlPump();

        // Update Display
        displaySystemStatus();

        // Update GPS (if needed)
        updateGPS();

        delay(1000);
    }

    void checkManualOverride() {
        if (digitalRead(Config::PIN_OVERRIDE_BUTTON) == LOW && 
            (millis() - lastDebounceTime > DEBOUNCE_DELAY)) {
            manualOverride = !manualOverride;
            lastDebounceTime = millis();
        }
    }

    void controlPump() {
        int currentHour = getCurrentHour();
        bool timeConstraintMet = (currentHour >= Config::MIN_OPERATING_HOUR && 
                                  currentHour <= Config::MAX_OPERATING_HOUR);

        // Determine if pump should run
        if (manualOverride || 
            (timeConstraintMet && waterLevel <= (thresholdLevel - Config::POT_OFFSET))) {
            
            selectPowerSource();
            
            if (usingGridPower || manualOverride) {
                startPump();
            } else {
                stopPump();
            }
        } else if (waterLevel >= (thresholdLevel + Config::POT_OFFSET)) {
            stopPump();
        }
    }

    void startPump() {
        digitalWrite(Config::PIN_PUMP_RELAY, HIGH);
        pumpRunning = true;
    }

    void stopPump() {
        digitalWrite(Config::PIN_PUMP_RELAY, LOW);
        digitalWrite(Config::PIN_GRID_RELAY, LOW);
        digitalWrite(Config::PIN_INVERTER_RELAY, LOW);
        pumpRunning = false;
        usingGridPower = false;
    }
};

// Global Instance
WaterPumpSystem waterPump;

void setup() {
    waterPump.initialize();
}

void loop() {
    waterPump.update();
}
