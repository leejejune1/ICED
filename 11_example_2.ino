#include <Servo.h>

// Arduino pin assignment
#define PIN_LED 9
#define PIN_SERVO 10
#define PIN_TRIG 12
#define PIN_ECHO 13

// configurable parameters
#define SND_VEL 346.0 // sound velocity at 24 celsius degree (unit: m/s)
#define INTERVAL 25 // sampling interval (unit: ms)
#define _DIST_MIN 100 // minimum distance to be measured (unit: mm)
#define _DIST_MAX 400 // maximum distance to be measured (unit: mm)

#define _DUTY_MIN 550 // servo full clockwise position (0 degree)
#define _DUTY_NEU 1475 // servo neutral position (90 degree)
#define _DUTY_MAX 2400 // servo full counterclockwise position (180 degree)


// global variables
float timeout; // unit: us
float dist_min, dist_max, dist_raw, dist_prev, dist_ema; // unit: mm
float ema_element[10];
unsigned long last_sampling_time; // unit: ms
float scale; // used for pulse duration to distance conversion
Servo myservo;

void setup() {
  
// initialize GPIO pins
  pinMode(PIN_LED,OUTPUT);
  pinMode(PIN_TRIG,OUTPUT);
  digitalWrite(PIN_TRIG, LOW); 
  pinMode(PIN_ECHO,INPUT);

  myservo.attach(PIN_SERVO); 
  myservo.writeMicroseconds(_DUTY_NEU);

// initialize USS related variables
  dist_min = _DIST_MIN; 
  dist_max = _DIST_MAX;
  timeout = (INTERVAL / 2) * 1000.0; // precalculate pulseIn() timeout value. (unit: us)
  dist_raw = dist_prev = dist_ema = 0.0; // raw distance output from USS (unit: mm)
  scale = 0.001 * 0.5 * SND_VEL;
  
  for(int i=0; i<10; i++)
  {
    ema_element[i] = 0;
  }

// initialize serial port
  Serial.begin(57600);

// initialize last sampling time
  last_sampling_time = 0;
}

void loop() {
// wait until next sampling time. 
// millis() returns the number of milliseconds since the program started. Will overflow after 50 days.

 // 측정 주기
  if(millis() < last_sampling_time + INTERVAL) return;

//측정(범위기반 필터링도 같이 들어 있음)
  dist_raw = USS_measure(PIN_TRIG,PIN_ECHO);

// ema 계산

//dist_ema = ema_calc(dist_raw);

// 출력

  Serial.print("Min:100,raw:");
  Serial.print(dist_raw);
  //Serial.print(",ema:");
  //Serial.print(dist_ema);
  Serial.print(",servo:");
  Serial.print(myservo.read());  
  Serial.println(",Max:400");

  // 서보 모터 조작

if(dist_raw <= 180.0) {
     digitalWrite(PIN_LED, HIGH);
     myservo.writeMicroseconds(_DUTY_MIN);
  }
  else if(dist_raw > 180.0 &&dist_raw < 360.0){
    digitalWrite(PIN_LED, LOW);
    int duty = (int)dist_raw * 10 -_DUTY_MIN - 700;
    myservo.writeMicroseconds(duty);
  }
  else {
    digitalWrite(PIN_LED, HIGH);
    myservo.writeMicroseconds(_DUTY_MAX);
  }
   
// update last sampling time
  last_sampling_time += INTERVAL;
}

//float ema_calc(float dist_raw)
//{
//  for(int i=9; i>1; i--)
//  {
//    ema_element[i-1] = ema_element[i] 
//  }
//  
//  ema_element[9] = dist_raw;
//
// //ema1 0.1 * ema_element[9]
// //ema2 0.1 * ema_element[9] + 0.9 * ema_element[8]
//  return 0.1 * ema_element[9] + (0.9 * 0.9) * ema_element[8] + (0.9 * 0.9 * 0.9) * ema_element[7] + 
//}

// get a distance reading from USS. return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  float reading;
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  reading = pulseIn(ECHO, HIGH, timeout) * scale; // unit: mm
  if(reading <= 180.0 || reading >= 360.0) reading = 0.0; // return 0 when out of range.

  if(reading == 0.0) reading = dist_prev;
  else dist_prev = reading;
  
  return reading;
}
