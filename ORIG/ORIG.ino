/* LEGACY CODE BEGINS HERE */
/*----------------------Initializing the Servo library, Servo Objects and Servo Pins------------------------------*/
#include <Servo.h>
Servo servo1; // base
Servo servo2; // link1
Servo servo3; // link1
Servo servo4; // link2
Servo servo5; // toolplate
Servo servo6; // gripper

#define s1pin 6  // Servo 1 to PWM enabled pin 6
#define s2pin 7  // Servo 2 to PWM enabled pin 7
#define s3pin 8  // Servo 3 to PWM enabled pin 8
#define s4pin 9  // Servo 4 to PWM enabled pin 9
#define s5pin 10 // Servo 5 to PWM enabled pin 10
#define s6pin 11 // Servo 6 to PWM enabled pin 11

//-------------------------Initializing Joystick X and Y Pins----------------------------------
// Joystick Pins333
#define J1x A0 // Controls Servo1
#define J1y A1 // Controls Servo2
#define J2x A2 // Controls Servo3
#define J2y A3 // Controls Servo4
#define J3x A4 // Controls Servo5
#define J3y A5 // Controls Servo6
//-----------------------Joystick Switches-------------------------
#define sJ1 2 // switch in J1 attached to interrupt pin 0
#define sJ2 3 // switch in J2 attached to interrupt pin 1
#define sJ3 4 // switch in J3 attached to interrupt pin 4
#define pB 5  // pushbutton
//--------------Declaration of Variables--------------------
// int zeroval = 1500; //1500 us --- neutral position
// int Jzeroval = 512; //home pos of joystick--deadband
int pos;
int pos1cons = 500;
int pos1final = 500;
int pos2cons = 0;
int pos2final = 0;
int pos3cons = 0;
int pos3final = 0;
int pos4cons = 0;
int pos4final = 0;
int pos5cons = 0;
int pos5final = 0;
int pos6cons = 0;
int pos6final = 0;
//------------------------- Switch Pins STATES------------------------------
int sJ1old;
int sJ2old;
int sJ3old;
int pBold;
int sJ1new;
int sJ2new;
int sJ3new;
int pBnew;
//---------------- millis Delay Variables------------------------------------
unsigned long
    currTime,                // current time
    moveTime,                // time when servo should move
    speedofservo;            // future code for varying speed
unsigned long timedelay = 5; // let time delay equal 15 milliseconds for now
unsigned long readtime = 5;
// Servo Range defined by min, and MAX, also initial position
int s1min = 500;
int s1ini = 1800;
int s1MAX = 2300;
int s2min = 1150;
int s2ini = 1300;
int s2MAX = 2200;
int s3min = 500;
int s3ini = 1400;
int s3MAX = 2300;
int s4min = 500;
int s4ini = 1500;
int s4MAX = 2500;
int s5min = 800;
int s5ini = 1700;
int s5MAX = 2300;
int s6min = 950;
int s6ini = 1500;
int s6MAX = 1800;
int pos1 = 902; // needs to be mapped to 1800
int pos2 = 147; // needs to be mapped to 1300
int pos3 = 881; // needs to be mapped to 1400
int pos4 = 512; // needs to be mapped to 1500
int pos5 = 614; // needs to be mapped to 1700
int pos6 = 663; // needs to be mapped to 1500
int v1;
int gripperspeed = 3; // changes the speed of gripper open and close
//************************************** SET UP******************************************************
void setup()
{
    // attach the servos
    servo1.attach(s1pin, s1min, s1MAX);
    servo2.attach(s2pin, s2min, s2MAX);
    servo3.attach(s3pin, s3min, s3MAX);
    servo4.attach(s4pin, s4min, s4MAX);
    servo5.attach(s5pin, s5min, s5MAX);
    servo6.attach(s6pin, s6min, s6MAX);
    // obtain millis time
    moveTime = millis(); // get the millisecond time
    // initialize switch pins
    pinMode(sJ1, INPUT_PULLUP);
    pinMode(sJ2, INPUT_PULLUP);
    pinMode(sJ3, INPUT_PULLUP);
    pinMode(pB, INPUT_PULLUP);
    // read switch pins
    sJ1new = digitalRead(sJ1);
    sJ2new = digitalRead(sJ2);
    sJ3new = digitalRead(sJ3);
    pBnew = digitalRead(pB);
    // home the servo to initial position
    //  homePosition();
    servo1.writeMicroseconds(s1ini); // move servo 1 in the base
    servo2.writeMicroseconds(s2ini); // move servo 2 in link 1
    servo3.writeMicroseconds(s3ini); // move servo 3 in link 1
    servo4.writeMicroseconds(s4ini); // move servo 4 in link 2
    servo5.writeMicroseconds(s5ini); // move servo 5 before tool plate
    servo6.writeMicroseconds(s6ini); // open and close end effector
    // debug
    //  Serial.begin(9600);
}
//----RESET----
void (*resetFunc)(void) = 0;

//*************************************** LOOP************************************************
void loop()
{
    // read joystick button 1
    sJ1old = sJ1new;
    sJ1new = digitalRead(sJ1);
    if (sJ1old == 1 && sJ1new == 0)
    {
        spin();
    }
    // read joystick button 2
    sJ2old = sJ2new;
    sJ2new = digitalRead(sJ2);
    if (sJ2old == 1 && sJ2new == 0)
    {
        grabCan();
    }
    // read joystick button 3
    sJ3old = sJ3new;
    sJ3new = digitalRead(sJ3);
    if (sJ3old == 1 && sJ3new == 0)
    {
        wave();
    }
    // read pushbutton
    pBold = pBnew;
    pBnew = digitalRead(pB);
    if (pBold == 1 && pBnew == 0)
    {
        resetFunc();
    }
    // else move the servos normally
    else
    {
        readJoysticks();
        moveServos();
    }
}
//**********************************************************************************************************
//-------------------------------------FUNCTIONS----------------------------------------------
//----------------------------------------------------reading JOYSTICKS-----------------------------------------------
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void readJoysticks()
{
    //---------------------------finding the position of Servo1 motion---------------
    currTime = millis(); // get the current millisecond time
    if (currTime - moveTime >= readtime)
    {                        // if the difference between move time and current time is the delay period, then
        moveTime = currTime; // update move time
        readJoy1();
        readJoy2();
        readJoy3();
        readJoy4();
        readJoy5();
        readJoy6();
    }
}
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void readJoy1()
{
    int v1 = analogRead(J1x); // reads the jostick X-value
    // jostick vals go from 0-to-1023 where 500-550 is a deadband to prevent servo motion when it returns back home
    if (v1 > 800)
    {           // above the positive deadband
        pos1++; // increment the position of servo
    }
    else if (v1 < 200)
    {           // below the negative deadband
        pos1--; // decrement the position of servo
    }
    pos1cons = constrain(pos1, 0, 1023);              // constrained position of joystick x-val
    pos1final = map(pos1cons, 0, 1023, s1min, s1MAX); // map joy-val to servo-microsecond value
    pos1 = pos1cons;                                  // equal pos1 to pos1constrained to prevent going over the constrained range
}
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void readJoy2()
{
    int v2 = analogRead(J1y);
    if (v2 > 800)
    { // above the positive deadband
        pos2++;
    }
    else if (v2 < 200)
    { // below the negative deadband
        pos2--;
    }
    pos2cons = constrain(pos2, 0, 1023);
    pos2final = map(pos2, 0, 1023, s2min, s2MAX);
    pos2 = pos2cons;
}
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void readJoy3()
{
    int v3 = analogRead(J3y);
    if (v3 > 800)
    { // above the positive deadband
        pos3++;
    }
    else if (v3 < 200)
    { // below the negative deadband
        pos3--;
    }
    pos3cons = constrain(pos3, 0, 1023);
    pos3final = map(pos3, 0, 1023, s3min, s3MAX);
    pos3 = pos3cons;
    // Serial.print("3: ");
    // Serial.println(pos3final);
}
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void readJoy4()
{
    int v4 = analogRead(J3x);
    if (v4 > 800)
    { // above the positive deadband
        pos4++;
    }
    else if (v4 < 200)
    { // below the negative deadband
        pos4--;
    }
    pos4cons = constrain(pos4, 0, 1023);
    pos4final = map(pos4, 0, 1023, s4min, s4MAX);
    pos4 = pos4cons;
}
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void readJoy5()
{
    int v5 = analogRead(J2x);
    if (v5 > 800)
    { // above the positive deadband
        pos5++;
    }
    else if (v5 < 200)
    { // below the negative deadband
        pos5--;
    }
    pos5cons = constrain(pos5, 0, 1023);
    pos5final = map(pos5, 0, 1023, s5min, s5MAX);
    pos5 = pos5cons;
}
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void readJoy6()
{
    int v6 = analogRead(J2y);
    if (v6 > 800)
    { // above the positive deadband
        pos6 = pos6 + gripperspeed;
    }
    else if (v6 < 200)
    { // below the negative deadband
        pos6 = pos6 - gripperspeed;
    }
    pos6cons = constrain(pos6, 0, 1023);
    pos6final = map(pos6, 0, 1023, s6min, s6MAX);
    pos6 = pos6cons;
    // Serial.print("6: ");
    // Serial.println(pos6final);
}
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void moveServos()
{
    servo1.writeMicroseconds(pos1final); // move servo 1 in the base
    servo2.writeMicroseconds(pos2final); // move servo 2 in link 1
    servo3.writeMicroseconds(pos3final); // move servo 3 in link 1
    servo4.writeMicroseconds(pos4final); // move servo 4 in link 2
    servo5.writeMicroseconds(pos5final); // move servo 5 before tool plate
    servo6.writeMicroseconds(pos6final); // open and close end effector
}
void spin()
{
    delay(500);
    movePreset(6, 1495, 1000);
    movePreset(1, 2087, 500);
    for (pos = 0; pos <= 180; pos += 1)
    { // goes from 0 degrees to 180 degrees
        // in steps of 1 degree
        servo1.write(pos); // tell servo to go to position in variable 'pos'
        delay(50);         // waits 15ms for the servo to reach the position
    }
    for (pos = 180; pos >= 0; pos -= 1)
    {                      // goes from 180 degrees to 0 degrees
        servo1.write(pos); // tell servo to go to position in variable 'pos'
        delay(50);         // waits 15ms for the servo to reach the position
    }
    movePreset(1, 500, 2087);
    movePreset(6, 1000, 1495);
    delay(500);
    resetFunc();
}
void wave()
{
    movePreset(2, 1300, 1150);
    movePreset(3, 2050, 1800);
    movePreset(5, 1701, 1418);
    delay(500);
    movePreset(6, 1495, 1000);
    delay(1000);
    int count = 0;
    while (count <= 10)
    {
        for (pos = 0; pos <= 180; pos += 2)
        { // goes from 0 degrees to 180 degrees
            // in steps of 1 degree
            servo5.write(pos); // tell servo to go to position in variable 'pos'
            delay(50);         // waits 15ms for the servo to reach the position
        }
        for (pos = 180; pos >= 0; pos -= 2)
        {                      // goes from 180 degrees to 0 degrees
            servo5.write(pos); // tell servo to go to position in variable 'pos'
            delay(50);         // waits 15ms for the servo to reach the position
        }
        delay(10);
        count = count + 1;
    }
    delay(1000);
    movePreset(6, 1000, 1495);
    movePreset(2, 1150, 1300);
    movePreset(3, 1800, 2050);
    resetFunc();
}
void grabCan()
{
    movePreset(5, 1700, 1200);
    movePreset(2, 1300, 1500); // 2
    movePreset(3, 2050, 2500); // 3
    // movePreset(5, 1200, 1450); //5
    movePreset(6, 1500, 1000); // 6
    movePreset(2, 1515, 1300);
    movePreset(3, 2500, 1700); // 2
    movePreset(5, 1200, 1685); // 1
    delay(1);
    movePreset(1, 2088, 1450); // 3
    delay(1);
    movePreset(2, 1300, 1515);
    movePreset(4, 1500, 2318); // 5
    delay(2);
    // reverse
    movePreset(4, 2318, 1500);
    movePreset(5, 1685, 1481);
    movePreset(2, 1515, 1300);
    delay(1);
    movePreset(1, 1450, 2088);
    delay(1);
    movePreset(3, 1700, 2150);
    movePreset(2, 1300, 1450);
    movePreset(6, 1100, 1500);
    movePreset(3, 2150, 2050);
    movePreset(2, 1450, 1300);
    movePreset(5, 1481, 1700);
    delay(2);
    resetFunc();
}
void movePreset(int servoNum, int startPos, int endPos)
{
    int dtime = 5;
    if (endPos > startPos)
    {
        if (servoNum == 1)
        {
            for (pos1 = startPos; pos1 <= endPos; pos1++)
            {
                servo1.writeMicroseconds(pos1);
                delay(dtime);
            }
        }
        if (servoNum == 2)
        {
            for (pos2 = startPos; pos2 <= endPos; pos2++)
            {
                servo2.writeMicroseconds(pos2);
                delay(dtime);
            }
        }
        if (servoNum == 3)
        {
            for (pos3 = startPos; pos3 <= endPos; pos3++)
            {
                servo3.writeMicroseconds(pos3);
                delay(dtime);
            }
        }
        if (servoNum == 4)
        {
            for (pos4 = startPos; pos4 <= endPos; pos4++)
            {
                servo4.writeMicroseconds(pos4);
                delay(dtime);
            }
        }
        if (servoNum == 5)
        {
            for (pos5 = startPos; pos5 <= endPos; pos5++)
            {
                servo5.writeMicroseconds(pos5);
                delay(dtime);
            }
        }
        if (servoNum == 6)
        {
            for (pos6 = startPos; pos6 <= endPos; pos6++)
            {
                servo6.writeMicroseconds(pos6);
                delay(dtime);
            }
        }
    }
    else if (endPos < startPos)
    {
        if (servoNum == 1)
        {
            for (pos1 = startPos; pos1 >= endPos; pos1--)
            {
                servo1.writeMicroseconds(pos1);
                delay(dtime);
            }
        }
        if (servoNum == 2)
        {
            for (pos2 = startPos; pos2 >= endPos; pos2--)
            {
                servo2.writeMicroseconds(pos2);
                delay(dtime);
            }
        }
        if (servoNum == 3)
        {
            for (pos3 = startPos; pos3 >= endPos; pos3--)
            {
                servo3.writeMicroseconds(pos3);
                delay(dtime);
            }
        }
        if (servoNum == 4)
        {
            for (pos4 = startPos; pos4 >= endPos; pos4--)
            {
                servo4.writeMicroseconds(pos4);
                delay(dtime);
            }
        }
        if (servoNum == 5)
        {
            for (pos5 = startPos; pos5 >= endPos; pos5--)
            {
                servo5.writeMicroseconds(pos5);
                delay(dtime);
            }
        }
        if (servoNum == 6)
        {
            for (pos6 = startPos; pos6 >= endPos; pos6--)
            {
                servo6.writeMicroseconds(pos6);
                delay(dtime);
            }
        }
    }
}
