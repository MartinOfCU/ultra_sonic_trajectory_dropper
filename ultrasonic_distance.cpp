#include <Servo.h> 

// ACTIVITY TO GO HERE
float rail_over_target_intercept = 1; // 1 meter where the rail is above the target
float gravity = 9.81;

// pins
int trig = 4;
int echo = 5;
int servo_pin = 6;

// active changing variables
long lecture_echo;
long cm;

// servo settings
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
    cm = (lecture_echo) / 58;

    float velocity = 0.6; // meters per second
    float acceleration; // used to determine current velocity
    float angle; // concurrent angle for quad copter and its direction

    float height = cm / 10; // get instentaneous value for height in meters

    // // CODE FOR LEGIBILITY
    // float velocity_y = cos(angle)*velocity;
    // float velocity_x = sin(angle)*velocity;

    // // float time = -velocity_y + sqrt(velocity_y*velocity_y + 2*gravity*height);
    // float x_travel = velocity_x * time; // distance object will travel
    // float x_target = tan(angle) * (height - rail_over_target_intercept); // distance to the target, remove 

    // if (x_target <= x_travel) {
    //     // release the mechanism
    // }

    // CODE FOR QUICKER RUNNING TIMES, compressed math
    float gy = gravity * height;
    float compare = (tan(angle)*tan(angle) * 2 * gy) + (velocity/gy);

    if (compare <= 1) {
        // release the mechanism
    }

}