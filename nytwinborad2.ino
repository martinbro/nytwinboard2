#include <Servo.h>
#include <Wire.h>


//Output values
const byte SPEED_F = 0; //, frem
const byte SPEED_R = 15; //, bak?
const byte SERVO_OUT = 5; //D1, Rorudlæget til styremaskinen
const int RORBEGRAENSNING = 45;
const int TIMEOUT = 20000; //20,0ms

//Input values

const byte ROR_AUTO = 4; //D2  
const byte ROR_MAN = 14; //D5 
const byte FART = 12; //D6
const byte MODE = 13; //D7

int rorUdlaeg = 90; //default val
bool autoMode = false; //default val
uint32_t t1;
int udl=90;
int32_t timeout=0;
Servo servo1;  
// Servo servo2;  
// Servo servo3;  
//bool fjernbetj = false;

void setup(){
    delay(25000);
Serial.begin(115200);
    while (!Serial) { ;}//venter til vi har kontakt til serielporten
    pinMode(ROR_AUTO, INPUT);
    pinMode(ROR_MAN, INPUT);
    pinMode(FART, INPUT);
    pinMode(MODE, INPUT);

    // servo3.attach(SPEED_F);
    // servo2.attach(SPEED_R);
    servo1.attach(SERVO_OUT);
    delay(500);
    servo1.write(0);
    delay(1000);
    servo1.write(180);
    delay(1000);
    servo1.write(0);
    delay(1000);
    servo1.write(90);
    delay(1000);
     
}

void loop(){

	if(!sleep(50)){
    timeout = pulseIn(MODE,HIGH,TIMEOUT);
    Serial.print("------------------------------------------------------------------------>");  
    Serial.println(timeout);
        // Serial.print(udl);
        // Serial.print(" , ");
        // Serial.print(map(pulseIn(ROR_MAN,HIGH),955,2040,45,135));
        // Serial.print(" , ");
        // Serial.print(map(pulseIn(FART,HIGH),1002,2004,-100,100));
        // Serial.print(" , ");
        // Serial.println(pulseIn(MODE,HIGH)>1500);
        // rorUdlaeg = 90;
        if(timeout >10 && timeout < 1500){//fx. 850
            float f = map(pulseIn(ROR_MAN,HIGH),955,2040,45,135);
            Serial.print(" ****************************, ");
            Serial.println(f);
            servo1.write(f);
        }else{ //timeout overskredet
           udl = pulseIn(ROR_AUTO,HIGH)/10+3+45;
            Serial.print(" ............................., ");
            Serial.println(udl);
            servo1.write(udl);

        }
        // servo2.write(pulseIn(ROR_AUTO,HIGH));
        // servo3.write(pulseIn(ROR_AUTO,HIGH));

    }
    // else{
    //     udl = pulseIn(ROR_AUTO,HIGH)/10+3+45;
    //     servo1.write(udl);
    // }
}

bool sleep(int t){

    uint32_t t0 = millis(); //opdaterer timer ved nyt tik
    if (t0 - t1 <t) {
        return true;
    }
    t1 = t0;//opdaterer closure 
    return false;
}