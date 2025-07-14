

#define pwmPin 9 

#define analogPin A0


void setup () {
Serial.begin(9600);
pinMode(pwmPin, OUTPUT);

}

void loop () {
int fanSpeed = analogRead(analogPin);  
 int pwmValue = map(fanSpeed, 0, 1023, 0, 255);
    if (pwmValue < 50) {
        analogWrite(pwmPin, 0);  // Off fan
    } 
    else if (pwmValue < 101) {
        analogWrite(pwmPin, 64);  //speed 1
    } 
    else if (pwmValue < 153) {
        analogWrite(pwmPin, 128);  //speed 2
    } 
    else if (pwmValue < 203) {
        analogWrite(pwmPin, 192);  // speed 3
    } 
    else {
        analogWrite(pwmPin, 255);  // speed 4
    }

    delay(100); 



}
