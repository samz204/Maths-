#include <Wire.h>

//#include <ESP32Encoder.h>
#define I2C_SLAVE_ADDR 0x04  // 4 in hexadecimal

// L298 motor driver pin definitions
#define enA 33  // enableA command line
#define enB 25  // enableB command line
#define INa 26  // channel A direction
#define INb 27  // channel A direction
#define INc 14  // channel B direction
#define INd 12  // channel B direction

// setting up the PWM properties used for the motors
const int freq = 2000;
const int ledChannela = 0;  // assumed as the channel used for the left motor
const int ledChannelb = 1;  // assumed as the channel used for the righteft motor
const int resolution = 8;   // 8-bit PWM signal

int servoPin = 13;  //the servo is attached to IO_13 on the ESP32

// setting up the PWM properties of the servo
// as an aside, the control signal of the SG90 servo is a PWM signal with a period of 20ms (50Hz) and the pulse duration has to be between 1ms to 2ms i.e. 5% duty cycle for the minimum angle of 0, and 10% duty cycle for the maximum angle of 180
// it is not recommended that you change any of the four values below
int dutyCycle = 5;
const int servoFrequency = 50;   // 50Hz signal
const int servoChannel = 2;      // channels 0 and 1 are used for the two motors on your EEEBot
const int servoResolution = 12;  // 12-bit PWM signal


void setup() {
  Serial.begin(115200);          // open the serial port at 9600 bps:
  Wire.begin(I2C_SLAVE_ADDR);    //Set Arduino up as an I2C slave at address 0x07
  Wire.onReceive(receiveEvent);  //Prepare to recieve data

  pinMode(INa, OUTPUT);
  pinMode(INb, OUTPUT);
  pinMode(INc, OUTPUT);
  pinMode(INd, OUTPUT);

  // configure LED PWM functionalities
  ledcSetup(ledChannela, freq, resolution);
  ledcSetup(ledChannelb, freq, resolution);
  ledcSetup(servoChannel, servoFrequency, servoResolution);  //servo setup on PWM channel 2, 50Hz, 12-bit (0-4095)

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(enA, ledChannela);
  ledcAttachPin(enB, ledChannelb);
  ledcAttachPin(servoPin, servoChannel);
}

void loop() {
}
void receiveEvent(int numBytes) {
  //Set Up Vars
  int previousError = 0;
  int receive_int = 0;
  int count = 0;
  int u = 0;

  Serial.println(count);
  //We'll recieve one byte at a time. Stop when none left
  while (Wire.available()) {
    char c = Wire.read();  // receive a byte as character
    //Create Int from the Byte Array
    receive_int = c << (8 * count) | receive_int;
    count++;
  }
  //Print the Int out.
  u = (receive_int - 160) * (-1);

  map(u, -160, 160, -50, 50);



  Serial.print("Received Number: ");
  Serial.println(u);
  if (u < 160 && u > -160) {
    int base_speed_left = 110;
    int base_speed_right = 110;
    float k = 0.6;
    int centre_angle = 95;
    int x = base_speed_left;
    int y = base_speed_right;

    int z = centre_angle + (u);


    //Serial.print("x: ");
    //Serial.println(x);
    // Serial.print("y: ");
    //Serial.println(y);
    Serial.print("z: ");
    Serial.println(z);
    Motor_Setup(x, y);
    Servo_Setup(z);
  }



  delay(300);
}

void Servo_Setup(int z) {
  dutyCycle = map((constrain(z, 45, 135)), 0, 180, 205, 410);  // contrain() limits the minimum and maximum values to 0 and 180 respectively, map() proportionally scales values between 0 and 180 to values between 205 (5% duty cycle) and 410 (10% duty cycle)
  ledcWrite(servoChannel, dutyCycle);                          // write the control signal to the PWM
}

void Motor_Setup(int x, int y) {

  x = constrain(x, -255, 255);
  y = constrain(y, -255, 255);

  ledcWrite(ledChannela, abs(x));
  ledcWrite(ledChannelb, abs(y));

  // if the speed value is negative, run the motor backwards
  if (x < 0) {
    digitalWrite(INa, LOW);
    digitalWrite(INb, HIGH);
  }
  // else, run the motor forwards
  else {
    digitalWrite(INa, HIGH);
    digitalWrite(INb, LOW);
  }

  // if the speed value is negative, run the motor backwards
  if (y < 0) {
    digitalWrite(INc, LOW);
    digitalWrite(INd, HIGH);
  }
  // else run the motor forwards
  else {
    digitalWrite(INc, HIGH);
    digitalWrite(INd, LOW);
  }
}
