#include<Servo.h>

const int filterBufferSize = 11;  // Odd sized buffer for easier median searches
const int ultraPin = A2;
const int servoPin = 9;

class ultraSonicModule{

/*
UltraSonic module: Range Finder - LV-MaxSonar-EZ1 interface
Maintainer: SubramanianKrish - Last Updated: 2/4/2020

Ref: https://arduino.stackexchange.com/questions/38177/how-to-sort-elements-of-array-in-arduino-code for using quicksort in arduino

Calibration Test Data: (From Experimentation)

  -------------------------
  |Distance (in)|   ADC   |
  -------------------------
  |       6     |   11    |
  |      10     |   15    |
  |      14     |   23    |
  |      18     |   32    |
  |      22     |   39    |
  |      26     |   47    |
  |      30     |   55    |
  |      34     |   63    |
  |      38     |   71    |
  |      42     |   79    |
  -------------------------

Approximation of a linear transfer function (from calib data):
  measurement(in) = (0.514)*ADC + 1.632
*/
  public:

  //Constructor initialization
  ultraSonicModule(int pinNumber){
    sensorPin = pinNumber;
    pinMode(sensorPin, INPUT);
    for(int i=0; i<filterBufferSize; ++i)
      {
        bufferArray[i] = 0;
        sortedArray[i] = 0;  
      }
  }

  float measureDistance(){
    for(int i=filterBufferSize-1; i>= 0; --i){
      if (i==0) bufferArray[i] = analogRead(sensorPin);
      else
      {
      bufferArray[i] = bufferArray[i-1];
      }
      sortedArray[i] = bufferArray[i];
    }
    
    qsort(sortedArray, filterBufferSize, sizeof(sortedArray[0]), comparator);
    return 0.514*sortedArray[int((filterBufferSize-1)/2)] + 1.632;
  }
  
  private:
  int sensorPin;
  double bufferArray[filterBufferSize];
  double sortedArray[filterBufferSize];
  
  static int comparator(int *cmp1, int *cmp2)
  {
    // Need to cast the void * to int *
    int a = *(cmp1);
    int b = *(cmp2);
    // The comparison
    return a < b ? -1 : (a < b ? 1 : 0);
  }
  
};

ultraSonicModule myUltra(ultraPin);
Servo myServo;

void setup() {
  // power servo
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);
  
  Serial.begin(9600);
  myServo.attach(servoPin);
}

void loop() {
  float reading = myUltra.measureDistance();
  Serial.println(reading);
  
  // Map reading to servo input
  int servo_input = map(int(reading), 10, 40, 10, 170);
  
  // Rotate Servo
  myServo.write(servo_input);
  
  delay(15);
}
