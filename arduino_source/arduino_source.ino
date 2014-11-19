

//Arduino code for controlling actuator and stepper motor used in syringe pump
//USED:
//Arduino Mega
//TFT Touchscreen
//Relay Board Shield with x4 Relays
//Stepper Motor
//Motor Shield
//12V Linear Actuator

//Current stop and end is based on timming, will use
//position sensors in future version

//See atached diagram
#include <stdint.h>
#include <TouchScreen.h> 
#include <TFT.h>
#include <Servo.h>

#ifdef SEEEDUINO
#define YP A2   // must be an analog pin, use "An" notation!
#define XM A1   // must be an analog pin, use "An" notation!
#define YM 14   // can be a digital pin, this is A0
#define XP 17   // can be a digital pin, this is A3 
#endif

#ifdef MEGA
#define YP A2   // must be an analog pin, use "An" notation!
#define XM A1   // must be an analog pin, use "An" notation!
#define YM 54   // can be a digital pin, this is A0
#define XP 57   // can be a digital pin, this is A3 
#endif 

#define TS_MINX 140
#define TS_MAXX 900

#define TS_MINY 120
#define TS_MAXY 940


// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// The 2.8" TFT Touch shield has 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);


//servo defenitions and relays
Servo myservo;
int pos = 0;

const int RELAY_1_A = 48;
const int RELAY_1_B = 45;
//
void setup(void)
{
  //  Serial.begin(9600);
  Tft.init();  //init TFT library

  /*  Demo of 
   drawRectangle(unsigned int poX, unsigned int poY, unsigned int length,unsigned int width,unsigned int color);
   fillRectangle(unsigned int poX, unsigned int poY, unsigned int length, unsigned int width, unsigned int color);
   */
  showMenu();

  //servo and relay setup
  myservo.attach(10);

  digitalWrite(RELAY_1_A, HIGH);
  digitalWrite(RELAY_1_B, HIGH);

  pinMode(RELAY_1_A, OUTPUT);
  pinMode(RELAY_1_B, OUTPUT);
  //end servo and relay setup
}

void loop(void) {
  // a point object holds x y and z coordinates
  Point p = ts.getPoint();


  p.x = map(p.x, TS_MINX, TS_MAXX, 240, 0);
  p.y = map(p.y, TS_MINY, TS_MAXY, 320, 0);

  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!

  if (p.z > ts.pressureThreshhold) {
    // Serial.print("\tX = "); Serial.print(p.x);
    //  Serial.print("\tY = "); Serial.println(p.y);

    //Green button start
    if(p.x>=145&&p.x<=225&&p.y>=10&&p.y<=90){
      //Serial.print("HIT GREEN = "); Serial.println(p.z);
      startFullCycle();
    }

    //yellow button reset
    else if(p.x>=145&&p.x<=225&&p.y>=110&&p.y<=190){
      //  Serial.print("HIT YELLOW = "); Serial.println(p.z);

    }

    //blue up button
    else if(p.x>=75&&p.x<=135&&p.y>=10&&p.y<=100){
      //Serial.print("hit up = "); Serial.println(p.z);
      retractActuator();
    }

    //white down button
    else if(p.x>=10&&p.x<=70&&p.y>=10&&p.y<=90){
      // Serial.print("Hit down = "); Serial.println(p.z);
      extendActuator();
    }

    //red stop button
    else if(p.x>=35&&p.x<=95&&p.y>=100&&p.y<=180){
      // Serial.print("hit stop = "); Serial.println(p.z);
      stopActuator();
    }

    //green nozzle button
    else if(p.x>=65&&p.x<=115&&p.y>=210&&p.y<=280){
      // Serial.print("Hit green nozle = "); Serial.println(p.z);
      closeNozles();
    }

    //red nozzle button
    else  if(p.x>=10&&p.x<=60&&p.y>=210&&p.y<=280){
      //  Serial.print("hit red nozzle"); Serial.println(p.z);
      openNozles();
    }

  }

  delay(100);
}

//actuator methods
void extendActuator() {
  //Set one relay one and the other off
  //this will move extend the actuator
  digitalWrite(RELAY_1_A, HIGH);
  digitalWrite(RELAY_1_B, LOW);
}

void retractActuator() {
  //Set one relay off and the other on
  //this will move retract the actuator
  digitalWrite(RELAY_1_A, LOW);
  digitalWrite(RELAY_1_B, HIGH);
}

void stopActuator() {
  //Set both relays off
  //this will stop the actuator in a braking
  digitalWrite(RELAY_1_A, HIGH);
  digitalWrite(RELAY_1_B, HIGH);
}

void startFullCycle(){

  //prepare nozles, point them down
  closeNozles();
  delay(1000);


  //Start pushing down for 8 seconds        
  extendActuator();
  delay(5000);

  //stop actuator for 1 second and tilt nozles
  stopActuator();
  delay(1000);

  //start pulling up for for 1 second

  retractActuator();
  delay(1000);

  //return nozzles to normal position and continue pulling for
  //7 seconds
  openNozles();
  delay(4000);

  //STOP pulling, end of cycle
  stopActuator();

}

void openNozles(){
  myservo.write(110);
}
void closeNozles(){
  myservo.write(180);
}

void resetSystem(){

}

void showMenu(){
  Tft.setDisplayDirect(LEFT2RIGHT);
  Tft.fillRectangle(145,10, 80,80,GREEN);
  Tft.fillRectangle(145,110, 80,80,YELLOW);
  Tft.drawRectangle(145,210, 80,80,WHITE);

  Tft.fillRectangle(75,10, 60,80,BLUE);
  Tft.fillRectangle(10,10, 60,80,WHITE);
  Tft.fillRectangle(35,100, 60,80,RED);

  Tft.fillRectangle(65,210, 50,70,GREEN);
  Tft.fillRectangle(10,210, 50,70,RED);
}

