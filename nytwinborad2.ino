#include <Servo.h>
#include <Wire.h>
#include <WebSockets2_Generic.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>


//Output values
const byte SPEED_R = 5; //, frem
const byte SPEED_F = 0; //, bak?
const byte SERVO_OUT = 15; //D4, Rorudlæget til styremaskinen, flyttet fra 
const int RORBEGRAENSNING = 30;
const unsigned long TIMEOUT = 20000; //20,0ms

//Input values
const byte ROR_AUTO = 4; //D2  
const byte ROR_MAN = 14; //D5 
const byte FART = 12; //D6
const byte MODE = 13; //D7
// bool fjernbetjeningFlag = false;
// int rorUdlaeg = 90; //default val
bool autoMode = false; //default val
uint32_t t1;
int udl=90;
unsigned long timeout=0;
Servo servo1;  

/* initialiserer moduler */
using namespace websockets2_generic;
WebsocketsClient client;

void setup(){  
    Serial.begin(115200);
    while (!Serial) { ;}
    delay(2000);// venter 2 sek 
    Serial.println("Starter");
    bool conn = initConnectToWifi();
    client.onMessage(onMessageCallback);
	client.onEvent(onEventsCallback);
    if(conn){
        client.connect("192.168.137.1", 8000, "/ws3");
        client.ping("ping fra motor");
        // client.send("STOR SUCCES");
    }
    
    // delay(25000);// venter 25 sek ter til vi har kontakt til serielporten
    
    pinMode(ROR_AUTO, INPUT);
    pinMode(ROR_MAN, INPUT);
    pinMode(FART, INPUT);
    pinMode(MODE, INPUT);

    servo1.attach(SERVO_OUT);
    analogWrite(SPEED_F,100);
    analogWrite(SPEED_R,100);
    servo1.write(175);
    delay(1000);
    analogWrite(SPEED_F,0);
    analogWrite(SPEED_R,0);
    servo1.write(45);
    delay(1000);
     analogWrite(SPEED_F,0);
    analogWrite(SPEED_R,100);   
    servo1.write(90);
    delay(1000);
    analogWrite(SPEED_F,0);
    analogWrite(SPEED_R,0);
}

void loop(){
client.poll();
	if(!sleep(50)){//opdaterer hvert 50 ms - kører hvert loop
    timeout = pulseIn(MODE,HIGH,TIMEOUT);//venter TIMEOUT = 20000 microseconds =20 ms to wait for the pulse to be completed: the function returns 0 if no complete pulse was received within the timeout
    // Serial.print("------------------------------------------------------------------------>");  
    //     Serial.println(timeout);
    //     Serial.print(udl);
    //     Serial.print(" , ");
    //     Serial.print(map(pulseIn(ROR_MAN,HIGH),955,2040,45,135));
    //     Serial.print(" , ");
    //     Serial.print(map(pulseIn(FART,HIGH),1002,2004,-200,200));
    //     Serial.print(" , ");
    //     Serial.println(pulseIn(MODE,HIGH)>1500);
        
        if(timeout < 10 ){// ikke tændt for fjernbetjening, da der ikke er registreret en pulseIn fra fjernbetjeningen
            udl = pulseIn(ROR_AUTO,HIGH)/10 + 45;
                Serial.print(" ............................., ");
                Serial.println(udl);
                client.send(String(udl));
                servo1.write(udl);
                analogWrite(SPEED_F,0);//marsh hast
                analogWrite(SPEED_R,0);//marsh hast
        }else if(timeout > 1500){// auto mode - da pulseIn(MODE HIGH)= ca. 2000 micro sek = ca. 2 ms (the length of the pulse (in microseconds) or 0 if no pulse is completed before the timeout)
                udl = pulseIn(ROR_AUTO,HIGH)/10 + 45;//input fra ROR_AUTO pin
                Serial.print(" ##############################, ");
                Serial.println(udl);
                servo1.write(udl);
                analogWrite(SPEED_F,210);//marsh hast
                analogWrite(SPEED_R,0);//marsh hast
        }else{ //manuel

            float k = map(pulseIn(ROR_MAN,HIGH),1000,2000,45,135);
            float s = map(pulseIn(FART,HIGH),1000,2000,-250,250);
            Serial.print(" ****************************, ");
            Serial.println(k);
            servo1.write(k);
            if (s<40){
                analogWrite(SPEED_F,-s);
                analogWrite(SPEED_R,0);
            }else if(s>40){
                analogWrite(SPEED_R,s);
                analogWrite(SPEED_F,0);
            }else{
                analogWrite(SPEED_R,0);
                analogWrite(SPEED_F,0); 
            }
        }
    }
}

bool sleep(int t){

    uint32_t t0 = millis(); //opdaterer timer ved nyt tik
    if (t0 - t1 <t) {
        return true;
    }
    t1 = t0;//opdaterer closure 
    return false;
}

////////////////////// WiFi + websoket2 //////////////////////////
bool initConnectToWifi(){
	// ESP8266 Connect to wifi
	for (byte j = 0; j < 5; j++) {
	delay(500);
		
		WiFi.begin("SKIB","marnavfablab");
        Serial.println("Prøver at forbinde til LAPTOP-RJIJOOL9 4301");


		for (int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) 
		{
        Serial.print(".");
			delay(1000);
		}
		if (WiFi.status() == WL_CONNECTED) {
            // WiFi.setAutoReconnect(false);
	        // WiFi.persistent(false);
            Serial.println("Succesfuld forbindelse til WiFi");
			return true;
			}
	}
	// Check if connected to wifi
	if (WiFi.status() != WL_CONNECTED) 
	{
        Serial.println("No Wifi!");
		return false;
	}
}

void onMessageCallback(WebsocketsMessage message) 
{
	// String besked = message.data();
	// char nr = besked.charAt(0);
    Serial.print("Got Message: ");
    Serial.println(message.data());
    // Serial.print(" ");
    // Serial.println(besked);
}
void onEventsCallback(WebsocketsEvent event, String data) 
{
	if (event == WebsocketsEvent::ConnectionOpened) 
	{
Serial.println("Connnection Opened");
Serial.println(data);
		
	} 
	else if (event == WebsocketsEvent::ConnectionClosed) 
	{
Serial.println("Connnection Closed");
	} 
	else if (event == WebsocketsEvent::GotPing) 
	{
Serial.println("Got a Ping!");
	} 
	else if (event == WebsocketsEvent::GotPong) 
	{
Serial.println("Got a Pong!");
	}
}