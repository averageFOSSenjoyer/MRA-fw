/*----------------------Initializing the Servo library, Servo Objects and Servo Pins------------------------------*/
#include <Servo.h>
Servo servo0; // placeholder for 1-indexed
Servo servo1; // base
Servo servo2; // link1
Servo servo3; // link1
Servo servo4; // link2
Servo servo5; // toolplate
Servo servo6; // gripper

// array for simplicity
Servo servoList[7] = {servo0, servo1, servo2, servo3, servo4, servo5, servo6};

int servoPins[7] = {0, 6, 7, 8, 9, 10, 11};

//-------------------------Initializing Joystick X and Y Pins----------------------------------
int joyPins[7] = {0, A0, A1, A2, A3, A4, A5};

//-----------------------Joystick Switches-------------------------
int joySwitches[4] = {5, 2, 3, 4};

//--------------Declaration of Variables--------------------
// int zeroval = 1500; //1500 us --- neutral position
// int Jzeroval = 512; //home pos of joystick--deadband
int pos;
int posCons[7] = {0, 500, 0, 0, 0, 0, 0};
int posFinal[7] = {0, 500, 0, 0, 0, 0, 0};
//------------------------- Switch Pins STATES------------------------------
int oldJoyState[4]; // pB, 1, 2, 3
int newJoyState[4];

//---------------- millis Delay Variables------------------------------------
unsigned long
    currTime,                // current time
    moveTime,                // time when servo should move
    speedofservo;            // future code for varying speed
unsigned long timedelay = 5; // let time delay equal 15 milliseconds for now
unsigned long readtime = 5;
// Servo Range defined by min, and MAX, also initial position
int minRange[7] = {0, 500, 1150, 500, 500, 800, 950};
int iniRange[7] = {0, 1800, 1300, 1400, 1500, 1700, 1500};
int maxRange[7] = {0, 2300, 2200, 2300, 2500, 2300, 1800};
int servoPos[7] = {0, 902, 147, 881, 512, 614, 663};    // 1-index for ease of documentation

int v1;
int gripperspeed = 3; // changes the speed of gripper open and close
//************************************** SET UP******************************************************
void setup()
{
    // attach the servos
    for (int i = 1; i <= 6; i++)
        servoList[i].attach(servoPins[i], minRange[i], maxRange[i]);

    // obtain millis time
    moveTime = millis(); // get the millisecond time

    // initialize switch pins
    for (int i = 0; i <= 3; i++)
        pinMode(joySwitches[i], INPUT_PULLUP);

    // read switch pins
    for (int i = 0; i <= 3; i++) 
        newJoyState[i] = digitalRead(joySwitches[i]);

    // home the servo to initial position
    //  homePosition();
    for (int i = 1; i <= 6; i++)
        servoList[i].writeMicroseconds(iniRange[i]);

    // debug
    //  Serial.begin(9600);
}

//----RESET----
void (*resetFunc)(void) = 0;

//*************************************** LOOP************************************************
void loop()
{
    for (int i = 0; i <= 3; i++) {
        oldJoyState[i] = newJoyState[i];
        newJoyState[i] = digitalRead(joySwitches[i]);
    }
    if (oldJoyState[0] == 1 && newJoyState[0] == 0)
        resetFunc();
    else if (oldJoyState[1] == 1 && newJoyState[1] == 0)
        spin();
    else if (oldJoyState[2] == 1 && newJoyState[2] == 0)
        grabCan();
    else if (oldJoyState[3] == 1 && newJoyState[3] == 0)
        wave();
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

        for (int i = 1; i <= 6; i++) {
            if (analogRead(joyPins[i]) > 800) 
                (servoPos[i])++;
            else if (analogRead(joyPins[i] < 200))
                (servoPos[i])--;
            
            posCons[i] = constrain(servoPos[i], 0, 1023);
            posFinal[i] = map(posCons[i], 0, 1023, minRange[i], maxRange[i]);
            servoPos[i] = posCons[i];
        }
    }
}

void moveServos()
{
    for (int i = 1; i <= 6; i++)
        servoList[i].writeMicroseconds(posFinal[i]);
}

/* LEGACY FUNCTION */
void spin()
{
    delay(500);
    movePreset(6, 1495, 1000);
    movePreset(1, 2087, 500);
    for (pos = 0; pos <= 180; pos += 1)
    { // goes from 0 degrees to 180 degrees
        // in steps of 1 degree
        servoList[1].write(pos); // tell servo to go to position in variable 'pos'
        delay(50);         // waits 15ms for the servo to reach the position
    }
    for (pos = 180; pos >= 0; pos -= 1)
    {                      // goes from 180 degrees to 0 degrees
        servoList[1].write(pos); // tell servo to go to position in variable 'pos'
        delay(50);         // waits 15ms for the servo to reach the position
    }
    movePreset(1, 500, 2087);
    movePreset(6, 1000, 1495);
    delay(500);
    resetFunc();
}

/* LEGACY FUNCTION */
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

/* LEGACY FUNCTION */
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

    if (endPos > startPos) {
        for (servoPos[servoNum] = startPos; servoPos[servoNum] != endPos; (servoPos[servoNum])++) {
            (servoList[servoNum]).writeMicroseconds(servoPos[servoNum]);
            delay(dtime);
        }
    } else {
        for (servoPos[servoNum] = startPos; servoPos[servoNum] != endPos; (servoPos[servoNum])--) {
            (servoList[servoNum]).writeMicroseconds(servoPos[servoNum]);
            delay(dtime);
        }
    }

}