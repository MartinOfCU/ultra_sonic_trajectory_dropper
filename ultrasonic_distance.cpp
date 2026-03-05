#include <Servo.h> 

int trig = 4;
int echo = 5;
long lecture_echo;
long cm;


int servo_pin = 6;
Servo door_servo;
int servo_min_angle = 0;
int servo_max_angle = 110;

void setup() 
{
    pinMode(trig, OUTPUT);
    digitalWrite(trig, LOW);
    pinMode(echo, INPUT);

    // set servo class to attatch to pinout
    door_servo.attatch(servo_pin);
    // move servo to zero degrees
    door_servo.write(0);

    Serial.begin(9600);
}

void loop()
{
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
    lecture_echo = pulseIn(echo, HIGH);
    cm = (lecture_echo*10) / 58;

    Serial.print("Distance: ");
    Serial.println(mm);

    // float velocity = 0.6; // meters per second
    // float acceleration
}