#include <Servo.h>

/*----------Initializing the Servo library, Servo Objects and Servo Pins----------*/
const unsigned int totalServos = 6;
const unsigned int totalButtons = 4;

Servo servoList[totalServos];
int servoPins[totalServos] = {6, 7, 8, 9, 10, 11};

// Initializing Joystick X and Y Pins
int joyPins[totalServos] = {A0, A1, A2, A3, A4, A5};

// Joystick Switches
int joySwitches[totalButtons] = {5, 2, 3, 4};

// Declaration of Variables
int pos;
int posCons[totalServos] = {500, 0, 0, 0, 0, 0};
int posFinal[totalServos] = {500, 0, 0, 0, 0, 0};

// Switch Pins STATES
int oldJoyState[totalButtons]; // pB, 1, 2, 3
int newJoyState[totalButtons];

// millis Delay Variables
unsigned long
    currTime,                // current time
    moveTime,                // time when servo should move
    speedofservo;            // future code for varying speed
unsigned long timedelay = 5; // let time delay equal 5 milliseconds for now
unsigned long readtime = 5;

// Servo Range defined by min, and MAX, also initial position
int minRange[totalServos] = {500, 1150, 500, 500, 800, 950};
int iniRange[totalServos] = {1800, 1300, 1400, 1500, 1700, 1500};
int maxRange[totalServos] = {2300, 2200, 2300, 2500, 2300, 1800};
int servoPos[totalServos] = {902, 147, 881, 512, 614, 663};

int v1;
int gripperspeed = 3; // changes the speed of gripper open and close

void setup()
{
    // attach the servos
    for (int i = 0; i < totalServos; i++)
        servoList[i].attach(servoPins[i], minRange[i], maxRange[i]);

    // obtain millis time
    moveTime = millis();

    // initialize switch pins
    for (int i = 0; i < totalButtons; i++)
        pinMode(joySwitches[i], INPUT_PULLUP);

    // read switch pins
    for (int i = 0; i < totalButtons; i++) 
        newJoyState[i] = digitalRead(joySwitches[i]);

    // home the servo to initial position
    for (int i = 0; i < totalServos; i++)
        servoList[i].writeMicroseconds(iniRange[i]);
}

void (*resetFunc)(void) = 0;

void loop()
{
    for (int i = 0; i < totalButtons; i++) {
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
    else {
        readJoysticks();
        moveServos();
    }
}

void readJoysticks()
{
    currTime = millis(); // get the current millisecond time
    if (currTime - moveTime >= readtime)
    {                        // if the difference between move time and current time is the delay period, then
        moveTime = currTime; // update move time

        for (int i = 0; i < totalServos; i++) {
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
    for (int i = 0; i < totalServos; i++)
        servoList[i].writeMicroseconds(posFinal[i]);
}

/* LEGACY FUNCTION */
void spin()
{
    delay(500);
    movePreset(5, 1495, 1000);
    movePreset(0, 2087, 500);
    for (pos = 0; pos <= 180; pos += 1)
    { // goes from 0 degrees to 180 degrees
        // in steps of 1 degree
        servoList[0].write(pos); // tell servo to go to position in variable 'pos'
        delay(50);         // waits 15ms for the servo to reach the position
    }
    for (pos = 180; pos >= 0; pos -= 1)
    {                      // goes from 180 degrees to 0 degrees
        servoList[0].write(pos); // tell servo to go to position in variable 'pos'
        delay(50);         // waits 15ms for the servo to reach the position
    }
    movePreset(0, 500, 2087);
    movePreset(5, 1000, 1495);
    delay(500);
    resetFunc();
}

/* LEGACY FUNCTION */
void wave()
{
    movePreset(1, 1300, 1150);
    movePreset(2, 2050, 1800);
    movePreset(4, 1701, 1418);
    delay(500);
    movePreset(5, 1495, 1000);
    delay(1000);
    for (int i = 0; i < 10; i++)
    {
        for (pos = 0; pos <= 180; pos += 2)
        { // goes from 0 degrees to 180 degrees
            // in steps of 1 degree
            servoList[4].write(pos); // tell servo to go to position in variable 'pos'
            delay(50);         // waits 15ms for the servo to reach the position
        }
        for (pos = 180; pos >= 0; pos -= 2)
        {                      // goes from 180 degrees to 0 degrees
            servoList[4].write(pos); // tell servo to go to position in variable 'pos'
            delay(50);         // waits 15ms for the servo to reach the position
        }
        delay(10);
    }
    delay(1000);
    movePreset(5, 1000, 1495);
    movePreset(1, 1150, 1300);
    movePreset(2, 1800, 2050);
    resetFunc();
}

/* LEGACY FUNCTION */
void grabCan()
{
    movePreset(4, 1700, 1200);
    movePreset(1, 1300, 1500);
    movePreset(2, 2050, 2500);
    movePreset(5, 1500, 1000);
    movePreset(1, 1515, 1300);
    movePreset(2, 2500, 1700);
    movePreset(4, 1200, 1685);
    delay(1);
    movePreset(0, 2088, 1450);
    delay(1);
    movePreset(1, 1300, 1515);
    movePreset(3, 1500, 2318);
    delay(2);
    movePreset(3, 2318, 1500);
    movePreset(4, 1685, 1481);
    movePreset(1, 1515, 1300);
    delay(1);
    movePreset(0, 1450, 2088);
    delay(1);
    movePreset(2, 1700, 2150);
    movePreset(1, 1300, 1450);
    movePreset(5, 1100, 1500);
    movePreset(2, 2150, 2050);
    movePreset(1, 1450, 1300);
    movePreset(4, 1481, 1700);
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