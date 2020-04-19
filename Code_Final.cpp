#include<FEHUtility.h>
#include<FEHServo.h>
#include <FEHLCD.h>
#include <FEHIO.h>
#include<FEHMotor.h>
#include <FEHRPS.h>
#include <FEHBattery.h>
#include <FEHSD.h>
#include<math.h>

#define DIAMETER 2.65
#define TICKSPERROTATION 32
#define RADIUSBACK 4
#define RADIUSMID 2.9
#define PI 3.14159

FEHMotor BLMotor(FEHMotor::Motor0,7.2);
FEHMotor BRMotor(FEHMotor::Motor1,7.2);
FEHMotor MMotor(FEHMotor::Motor3,7.2);

FEHServo TrayTicket(FEHServo::Servo0);
FEHServo BurgerFlip(FEHServo::Servo7);

AnalogInputPin cdsCell(FEHIO::P0_0);

DigitalEncoder midEncoder(FEHIO::P1_0);
DigitalEncoder backEncoder(FEHIO::P1_3);

bool wait;
int timer;

void start();
void stop();

void moveFB(float Lpower, float Rpower, float distance);
void moveFBTime(float Lpower, float Rpower, int time);
void moveLR(float power, float distance);
void moveLRTimeS(float power, int time);
void moveLRTimeM(float power, int time);
void goAngle(double power, double angle, double dist);
void goAngleSmall(double power, double angle, double dist);
void turnright(float power, int angle);
void turnleft(float power, int angle);
void turn(float power, int angle);
void align(float power, float dist);

void jukebox();
void rampTest1();
void rampTest2();
void Tray();
void Ticket();
void Burger();
void IceCreamDown();
void IceCreamUp();

void checkX(float x_coordinate);
void checkY(float x_coordinate);
void check_heading(float heading);
void DataLog();

int main(void) {
   float x,y;
   wait = true;
   //Initialize RPS
   RPS.InitializeTouchMenu();
   //DataLog();
   //intialize servo min and max/starting degree positions
   TrayTicket.SetMax(122);
   TrayTicket.SetMin(42);

   BurgerFlip.SetMax(147);
   BurgerFlip.SetMin(60);

   BurgerFlip.SetDegree(60);
   TrayTicket.SetDegree(117);

   //String right = "right", left = "left", forward = "forward", backward = "backward";
   LCD.WriteLine(Battery.Voltage());
   LCD.WriteLine("Waiting for Initialization Touch.");
   while(wait){
       while(!LCD.Touch(&x, &y)){
           LCD.Touch(&x, &y);
       }
       wait = false;
   }

   LCD.Clear();

   //Full Course Code
   start();

   //Moves to Bottom of Ramp
   goAngle(30.0, 0.0, 17.0);
   turn(25.0, 40);
   check_heading(90.00);

   //Goes up Ramp and Aligns to Ice Cream Levers
   goAngle(70.0, 0.0 , 30.0);
   checkX(18.0);
   checkY(51.0);
   turn(25.0, -37);
   check_heading(135.00);
   //Moves to Correct Ice Cream and Flips it and Moves to Sink
   IceCreamDown();
   //Drops tray
   goAngle(35.0, -125.0, 21.0);
   Tray();

   //Moves to Ticket and Moves Ticket
   goAngleSmall(45.0, 90.0, 27.0);
   Ticket();

   //Moves to Burger and Flips it
   //goAngleSmall(40.0, 24.0, 18.0);
   align(40.0, 18.0);
   moveFBTime(-15.0,15.0, 4);
   Burger();
   TrayTicket.SetDegree(117);

   //Moves to Ice Cream and Flips it Up and Move to Ramp
   //moveFB(15.0,-15.0, 4);
   IceCreamUp();   
   turn(25.0, 37);
   check_heading(90.00);
   checkX(18.0);
   checkY(51.0);

   //Move to Jukebox Light
   goAngle(50.0, 180.0, 30.0);
   goAngle(40.0, -125.0, 12.0);
   checkX(10.5);
   checkY(14.7);
   check_heading(90);
   //Presses Jukebox Light and Hits Stop Button
   jukebox();


/*
LCD.WriteLine("180");
check_heading(180.00);
LCD.WriteLine("270");
check_heading(270.00);
LCD.WriteLine("90");
check_heading(90.00);
*/
//moveFB(-20.0, 20.0, 6);


/* Performance Test 3
start();
moveFB(-20.0,20.0,12);
turnright(-25.0, 37);
moveLR(-45.0, 22);
rampTest2();
moveLR(45.0, 23);
moveFB(-20.0,20.0, 14.5);
turnright(25.0, 5);
moveLRTimeS(20.0, 4);
//this affects how much we come off the wall
//comment out encoder and try time
//moveLR(-20.0, .50);
//here is time line
//moveLRTimeM(-20.0, 175);
moveFBTime(-15.0,15.0, 3);
Burger();
moveFB(15.0,-15.0, 4);
moveLR(-20.0, 18);
turnright(25.0, 45);
moveLR(-20.0, 2.25);
moveFB(-15.0,15.0, 4.875);
IceCream();
*/

//moveLR(50,9);
//moveFB(-75,75, 16);
//turnright(-25.0,90);
//goAngle(10.0, 30.0, 30.0);
//moveLR(-30, 6.0);
//moveFB(30, -30, 8.0);

/*Performance test 1//////////////////////////////////////////////////////////////////////////////////////////

start();
moveFB(-20.0,20.0,9);
turnright(-25.0,65);
moveLR(-25.0, 12.5);
jukebox();
rampTest1();
///////////////////////////////////////////////////////////////////////////////////////////////////////////*/


/*Performance Test 2!////////////////////////////////////////////////////////////////////////////////////////

start();
moveFB(-20.0,20.0,16.0);
turnright(-25.0,45);
//moveLR(-45.0, 20);
//ramptest2 finishes with robot ready to drop tray
rampTest2();
//call tray to drop it
Tray();
//call move LR to get in position for ticket
moveLR(45.0, 25.5);
Ticket();
moveFB(-15.0, 15.0, 4.5);
stop();
TrayTicket.SetDegree(117);
moveLR(35, 4.75);
stop();
moveLR(-25, 13.5);
stop();
//takes shifted position to reach the burger flip and should end run
//moveFB(-20.0,20.0,30);
return 0;

/////////////////////////////////////////////////////////////////////////////////////////////////////*/

}


/*
* After initialization, ready to begin run./////////////////////////////
*/
void start() {
   LCD.WriteLine("Ready to begin Run.");
   bool lightIsOn = false;

   while(!lightIsOn)
   {
       if(cdsCell.Value() < 0.400) {
           lightIsOn = true;
       }
   }
}


/*
* RUDIMENTARY FUNCTIONS/////////////////////////////////
*/
void stop() {
   BLMotor.Stop();
   BRMotor.Stop();
   MMotor.Stop();
}

/*
* Functions for all types of moving//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

void moveFB(float Lpower,float Rpower, float distance) {      //LEFT POWER <0 AND RIGHT POWER >0 FOR FORWARD
   stop();
   int CountNeeded;
   CountNeeded = (distance / (DIAMETER * 3.1415)) * 32;

   backEncoder.ResetCounts();

   while(backEncoder.Counts() < CountNeeded) {
       BLMotor.SetPercent(Lpower);
       BRMotor.SetPercent(Rpower);
   }

   LCD.WriteLine("Back Counts");
   LCD.WriteLine(backEncoder.Counts());
   stop();
}

//Moves robot front and back for a given amount of time in seconds at a power
void moveFBTime(float Lpower, float Rpower, int time)
{
   stop();

   backEncoder.ResetCounts();
   int tmr;
   tmr = TimeNowSec();

   while (TimeNowSec() < (tmr + time)){
       BLMotor.SetPercent(Lpower);
       BRMotor.SetPercent(Rpower);
   }

   //LCD.WriteLine("Back Counts");
   //LCD.WriteLine(backEncoder.Counts());
   stop();
}

//Turns robot right given power and angle
void turnright(float power, int angle) {                      //Angle in degrees input, power must be negative

   stop();
   float arclength;
   arclength = (angle * RADIUSBACK * 2 * 3.1415) / 360;
   int CountArc = (arclength / (DIAMETER * 3.1415)) * TICKSPERROTATION;


   //Calculates power for middle wheel to rotate properly using angular velocity
   float midPower = power * RADIUSMID/ RADIUSBACK;
   BLMotor.SetPercent(power);
   BRMotor.SetPercent(power);
   MMotor.SetPercent(-midPower);

   backEncoder.ResetCounts();

   while(backEncoder.Counts() < CountArc) {                      //SUBJECT TO CHANGE

   }

   LCD.Clear();
  // LCD.WriteLine("Mid Counts");
  // LCD.WriteLine(midEncoder.Counts());
 //  LCD.WriteLine("Back Counts");
  // LCD.WriteLine(backEncoder.Counts());
   stop();

}

//Moves the robot left and right given power and distance
void moveLR(float power, float distance) {                    //POWER <0 FOR LEFT

   stop();

   int CountNeeded;
   CountNeeded = (distance / (DIAMETER * 3.1415)) * 32;

   midEncoder.ResetCounts();

   while(midEncoder.Counts() < CountNeeded) {
       MMotor.SetPercent(power);
   }

   LCD.WriteLine("Mid Counts");
   LCD.WriteLine(midEncoder.Counts());
   stop();

}

//Moves the robot left and right for a given time in seconds at a power
void moveLRTimeS(float power, int time) {    //SECONDS

   stop();

   int tmr;
   tmr = TimeNowSec();

   while (TimeNowSec() < (tmr + time)){
       MMotor.SetPercent(power);
   }

   LCD.WriteLine("Mid Counts");
   LCD.WriteLine(midEncoder.Counts());
   stop();

}

//Moves the robot left and right for a given time in milliseconds at a power
void moveLRTimeM(float power, int time) {    //MILLISECONDS

   stop();

   int tmr;
   tmr = TimeNowMSec();

   while (TimeNowMSec() < (tmr + time)){
       MMotor.SetPercent(power);
   }

   LCD.WriteLine("Mid Counts");
   LCD.WriteLine(midEncoder.Counts());
   stop();

}

//Turns robot given angle and power
//To turn robot left, input angle from 0 to -180; to turn right, input angle from 0 to 180
void turn(float power, int angle) {                     

   stop();
   float arclength;
   int angleCoef = angle/abs(angle);
   arclength = (abs(angle) * RADIUSBACK * 2 * 3.1415) / 360;
   int CountArc = (arclength / (DIAMETER * 3.1415)) * TICKSPERROTATION;


   //Calculates power for middle wheel to rotate properly using angular velocity
   float midPower = power * RADIUSMID/ RADIUSBACK;
   BLMotor.SetPercent(-angleCoef*power);
   BRMotor.SetPercent(-angleCoef*power);
   MMotor.SetPercent(angleCoef*(midPower));

   backEncoder.ResetCounts();

   while(backEncoder.Counts() < CountArc) {                      //SUBJECT TO CHANGE
   }

   LCD.Clear();
  // LCD.WriteLine("Mid Counts");
  // LCD.WriteLine(midEncoder.Counts());
 //  LCD.WriteLine("Back Counts");
  // LCD.WriteLine(backEncoder.Counts());
   stop();

}

//Moves robot a distance at an angle at a power
void goAngle(double power, double angle, double dist) {

   angle = angle*PI/180;
   double x = cos(angle);
   double y = sin(angle);
   double distRatio = dist/(DIAMETER * PI);
   double ticks = TICKSPERROTATION*distRatio;
   int xticks = abs(x*ticks);
   int yticks = abs(y*ticks);

   // Strafing
   bool backOn = true;
   bool midOn = true;
   int midCoef = 1;
   int backCoef = 1;

   if(angle < 0) {
       midCoef = -1;
   }
   if((angle > PI/2) || (angle < -PI/2)) {
       backCoef = -1;
   }

   MMotor.SetPercent(midCoef*power*abs(tan(angle)));
   BRMotor.SetPercent(backCoef*power);
   BLMotor.SetPercent(backCoef*-power);
   backEncoder.ResetCounts();
   midEncoder.ResetCounts();

   while(backOn || midOn) {

       if(backEncoder.Counts() >= xticks) {

           backOn = false;
           BRMotor.Stop();
           BLMotor.Stop();
           LCD.WriteLine("Back Counts");
           LCD.WriteLine(backEncoder.Counts());

        }
        if(midEncoder.Counts() >= yticks) {

           midOn = false;
           MMotor.Stop();
           LCD.WriteLine("Mid Counts");
           LCD.WriteLine(midEncoder.Counts());
        }
   }
}

//Moves robot a distance at an angle at a power in a triangle shape
//Fixes problem where middle motor power is too low
void goAngleSmall(double power, double angle, double dist) {

   angle = angle*PI/180;
   double x = cos(angle);
   double y = sin(angle);
   double distRatio = dist/(DIAMETER * PI);
   double ticks = TICKSPERROTATION*distRatio;
   int xticks = abs(x*ticks);
   int yticks = abs(y*ticks);

   // Strafing
   bool backOn = true;
   bool midOn = true;
   int midCoef = 1;
   int backCoef = 1;

   if(angle < 0) {
       midCoef = -1;
   }
   if((angle > PI/2) || (angle < -PI/2)) {
       backCoef = -1;
   }

   MMotor.SetPercent(midCoef*power);
   BRMotor.SetPercent(backCoef*power);
   BLMotor.SetPercent(backCoef*-power);
   backEncoder.ResetCounts();
   midEncoder.ResetCounts();

   while(backOn || midOn) {

       if(backEncoder.Counts() >= xticks) {

           backOn = false;
           BRMotor.Stop();
           BLMotor.Stop();
           LCD.WriteLine("Back Counts");
           LCD.WriteLine(backEncoder.Counts());

        }
        if(midEncoder.Counts() >= yticks) {

           midOn = false;
           MMotor.Stop();
           LCD.WriteLine("Mid Counts");
           LCD.WriteLine(midEncoder.Counts());
        }
   }
}

//Aligns robot with the right sidewall while moving forward for a given distance at a power
void align(float power, float dist) {

   double distRatio = dist/(DIAMETER * PI);
   double ticks = TICKSPERROTATION*distRatio;

   bool backOn = true;
   MMotor.SetPercent(abs(power));
   BRMotor.SetPercent(power);
   BLMotor.SetPercent(-power);
   backEncoder.ResetCounts();

   while(backOn) {
       if(backEncoder.Counts() >= ticks) {
           backOn = false;
           stop();
           LCD.WriteLine("Back Counts");
           LCD.WriteLine(backEncoder.Counts());
        }
   }
}
/*
* TASK FUNCTIONS/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

   //JUKEBOX FUNCTION
void jukebox() {

  stop();

  bool red = false;
  stop();
  int CountNeeded;
  CountNeeded = (3.25 / (DIAMETER * PI)) * 32;

  backEncoder.ResetCounts();
  float LowRead = 4;

  while(backEncoder.Counts() < CountNeeded) {
      BLMotor.SetPercent(20);
      BRMotor.SetPercent(-20);

      if (cdsCell.Value() < LowRead) {
          LowRead = cdsCell.Value();
      }
   }

  stop();

  //Moves to the red button
  if(LowRead < 0.500) {
      LCD.WriteLine(LowRead);
      //moveLR(-20.0, 6);                           //MOVING LEFT, SUBJECT TO CHANGE
      goAngle(25.0, -160.0, 5.0);
      LCD.WriteLine("Light is Red");
      red = true;
      //moveFB(45.0, -56.0, 3.5);
   }

   //Moves to the blue button
   else
   {
       LCD.WriteLine(LowRead);
       //moveLR(20.0, 2.3);                            //MOVING RIGHT, SUBJECT TO CHANGE
       goAngle(25.0, 160.0, 5.0);
       LCD.WriteLine("Light is Blue");
       //moveFB(47.0, -40.0, 3);
   }

   //Pushes button                          //MOVING backward, SUBJECT TO CHANGE
   stop();
   //moveFB(-50.0, 50.0, 1);
   if(red){
       Sleep(1000);
       //moveLR(-40.0, 7);                           //MOVING forward, SUBJECT TO CHANGE
       moveLR(60, 18);
       moveFB(-50.0, 50.0, 2);
       stop();
   }
   else {
       Sleep(1000);
       //moveLR(-40.0, 10);                           //MOVING forward, SUBJECT TO CHANGE
       moveLR(60, 20);
       moveFB(-50.0, 50.0, 2);
       stop();
   }
}

//Ramp bonus for PT1
void rampTest1() {
   //Aligns with ramp
   moveLR(50, 13.50);
   stop();
   turnright(-25.0, 5);
   stop();
   //Goes up ramp
   moveFB(-80.0, 80.0, 35);
   stop();
   //Aligns with sidewall and back to ramp
   moveLR(55, 15.75);
   stop();
   moveLR(-30, 12.75);
   stop();
   turnright(-25.0, 7);
   //Goes down ramp
   moveFB(60.0, -60.0, 25);
   stop();
}

//Aligns with ramp for PT3
void rampTest2() {
   //Moves off side wall to ramp
   moveLR(40, 15);
   stop();
   //Moves up ramp
   moveFB(-70.0, 70.0, 33);
   stop();
   //Aligns with tray corner
   moveLR(-40, 14);
   stop();
   moveFB(30.0, -30.0, 5);
   stop();
}

//Drops tray
void Tray() {
   TrayTicket.SetDegree(42);

   int tmr;
   tmr = TimeNowSec();
   while (TimeNowSec() < (tmr + 1.00))
   {
       //wait for tray to drop
   }
}

//assumes we come in contact with wall right before it
//Moves ticket
void Ticket()
{
   //moveLR(-35, 0.25);
   //stop();
   //moveFB(15.0, -15.0, 4.5);
   goAngleSmall(25.0, 165, 4.5);
   stop();
   moveLR(-50, 6.0);
   stop();
   moveLR(50, 2);
   stop();
}

//Flips burger
void Burger()
{
   //flips burger
   BurgerFlip.SetDegree(147);
   int tmr;
   tmr = TimeNowSec();
   while (TimeNowSec() < (tmr + 2))
   {
       //wait for burger flip to complete
   }
   //returns grill to original position
    BurgerFlip.SetDegree(60);
}

//Moves icecream lever down
void IceCreamDown()
{
   int position = RPS.GetIceCream();
   LCD.WriteLine(position);
   //Moves to correct Icecream Lever
   if(position == 0){
       goAngleSmall(30, -20.0, 11.75);
   }
   if(position == 1){
       goAngleSmall(30, 0.0, 11.5);
   }
   if(position == 2){
       goAngleSmall(30, 20.0, 12.0);
   }
   //Flips lever
   BurgerFlip.SetDegree(105);
   int tmr;
   tmr = TimeNowSec();
   while (TimeNowSec() < (tmr + 2))
   {
       //wait for Ice Cream flip to complete
   }
   //Moves out to make sure lever is flipped
   moveFB(25.0, -25.0, 1.0);
   BurgerFlip.SetDegree(60);
   //Moves to tray corner
   turn(25.0, 37);
   if(position == 0){
       goAngle(30, 170, 16);
   }
   if(position == 1){
       goAngle(30, 165, 18);
   }
   if(position == 2){
       goAngle(30, 160, 22.0);
   }
}

//Moves Icecream lever up
void IceCreamUp()
{
   //Move Functions
   int position = RPS.GetIceCream();
   //goAngleSmall(15.0, 165.0, 2.5);
   //Moves out from burger
   align(-15.0, 2.5);
   BurgerFlip.SetDegree(147);
   LCD.WriteLine(position);
   //Moves to correct lever
   if(position == 0){
       moveLR(-40.0, 18);
       turnright(25.0, 45);
       moveLR(-30.0, 2.1);
       moveFB(-20.0,20.0, 4.75);
   }
   if(position == 1){
       moveLR(-40.0, 18);
       turnright(25.0, 45);
       moveLR(-30.0, 1.7);
       moveFB(-20.0,20.0, 4.75);
   }
   if(position == 2){
       moveLR(-40.0, 16);
       turnright(25.0, 45);
       moveLR(-30.0, .75);
       moveFB(-20.0,20.0, 4.75);
   }
   //Flips the lever up
   BurgerFlip.SetDegree(105);
   int tmr;
   tmr = TimeNowSec();
   while (TimeNowSec() < (tmr + 2))
   {
       //wait for Ice Cream flip to complete
   }
   BurgerFlip.SetDegree(147);
   //Moves to center of top region
   if(position == 0){
       goAngleSmall(30, 160.0, 11.75);
   }
   if(position == 1){
       goAngleSmall(30, 0.0, 11.5);
   }
   if(position == 2){
       goAngleSmall(30, -160.0, 12.0);
   }
}


/*
* RPS FUNCTIONS//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

//Corrects X coordinate of robot
void checkX(float x_coordinate) //using RPS while robot is in the -x direction
{
   //check if receiving proper RPS coordinates and whether the robot is within an acceptable range

   LCD.Write("Adjusting X Val: ");
   LCD.WriteLine(RPS.X());
   while(RPS.X() >= 0 && (RPS.X() < x_coordinate - 1 || RPS.X() > x_coordinate + 1))
   {
       if(RPS.X() > x_coordinate)
       {
           //pulse the motors for a short duration in the correct direction

           MMotor.SetPercent(-20);
       }
       else if(RPS.X() < x_coordinate)
       {
           //pulse the motors for a short duration in the correct direction

           MMotor.SetPercent(20);
       }
   }

   stop();
   Sleep(100);
}

//Corrects Y coordinate of robot
void checkY(float y_coordinate) //using RPS while robot is in the y direction
{
   LCD.WriteLine("CHECKING Y MINUS");
   //check if receiving proper RPS coordinates and whether the robot is within an acceptable range
   while(RPS.Y() >= 0 && (RPS.Y() < y_coordinate - 1 || RPS.Y() > y_coordinate + 1))
   {
       if(RPS.Y() > y_coordinate)
       {
           //pulse the motors for a short duration in the correct direction

           BLMotor.SetPercent(20);
           BRMotor.SetPercent(-20);
           Sleep(250);
       }
       else if(RPS.Y() < y_coordinate)
       {
           //pulse the motors for a short duration in the correct direction

           BLMotor.SetPercent(-20);
           BRMotor.SetPercent(20);
           Sleep(250);
       }
   }
   stop();
   Sleep(100);
}

//Corrects heading of robot
void check_heading(float heading) //using RPS
{
   //LCD.WriteLine("CHECKING HEADING");
   while(RPS.Heading() >= 0 && (RPS.Heading() < heading - 4 || RPS.Heading() > heading + 4))
   {
       LCD.WriteLine(RPS.Heading());
       if(RPS.Heading() < heading) {
           turnright(20, 2);
           Sleep(150);
       }
       else if(RPS.Heading() > heading){
           turnright(-20, 2);
           Sleep(150);
       }
   }
   BLMotor.Stop();
   BRMotor.Stop();
   Sleep(100);
}

void DataLog(){
   //Declare variables
     float touch_x, touch_y;
     int n;
     char points[2] = {'A','B'};

     //Open SD file for writing
     FEHFile *fptr = SD.FOpen("RPS_Test.txt","w");

     //Wait for touchscreen to be pressed and released
     LCD.WriteLine("Press Screen to Start");
     while(!LCD.Touch(&touch_x, &touch_y));
     while(LCD.Touch(&touch_x, &touch_y));

     //Clear screen
     LCD.Clear();

     //Write initial screen info
     LCD.WriteRC("X Position:",2,0);
     LCD.WriteRC("Y Position:",3,0);
     LCD.WriteRC("   Heading:",4,0);

     //Step through each path point to record position and heading
     for (n=0; n<=1; n++)
     {
         //Write point letter
         LCD.WriteRC("Touch to set point ",0,0);
         LCD.WriteRC(points[n],0,20);

         //Wait for touchscreen to be pressed and display RPS data
         while(!LCD.Touch(&touch_x, &touch_y))
         {
             LCD.WriteRC(RPS.X(),2,12); //update the x coordinate
             LCD.WriteRC(RPS.Y(),3,12); //update the y coordinate
             LCD.WriteRC(RPS.Heading(),4,12); //update the heading

             Sleep(10); //wait for a 10ms to avoid updating the screen too quickly
         }
         while(LCD.Touch(&touch_x, &touch_y));

         //Print RPS data for this path point to file
         SD.FPrintf(fptr, "%f %f\n", RPS.X(), RPS.Y());
     }

     //Close SD file
     SD.FClose(fptr);

}



