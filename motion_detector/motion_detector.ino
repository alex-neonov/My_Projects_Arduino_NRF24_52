// Enable debug prints
//#define MY_DEBUG
// Passive mode requires static node ID
#define MY_NODE_ID 1
// Enable and select radio type attached
#define MY_RADIO_RF24
#include <MySensors.h>

#define MOTION1_SENS_ID 1
#define MOTION2_SENS_ID 2

MyMessage msg_motion1(MOTION1_SENS_ID, V_TRIPPED);
MyMessage msg_motion2(MOTION2_SENS_ID, V_TRIPPED);

int const Light = A1;
int const Relay = 3;
int const Move1 = 4;
int const Move2 = 5;
uint32_t paused = 180000; // 3 минуты
int motion1 = 0;
int motion2 = 0;


void setup() {
  pinMode(Relay, OUTPUT);
  pinMode(Move1, INPUT);
  pinMode(Move2, INPUT);
  digitalWrite(Move1, LOW);
  digitalWrite(Move2, LOW);
  digitalWrite(Relay, HIGH);
}

void presentation()
{
  sendSketchInfo("Motion Sensor", "1.2");
  wait(100);
  present(MOTION1_SENS_ID, S_MOTION, "MOTION STATUS1");
  wait(100);
  present(MOTION2_SENS_ID, S_MOTION, "MOTION STATUS2");
  
}

void loop() {
int svet = analogRead(Light);
send_motions();
if (svet>=1000){
if (digitalRead(Move1)==1){digitalWrite(Relay, LOW);send(msg_motion1.set(1));
delay(paused);
digitalWrite(Relay, HIGH);
send(msg_motion1.set(0));
}
if (digitalRead(Move2)==1){digitalWrite(Relay, LOW);send(msg_motion2.set(1));
delay(paused);
digitalWrite(Relay, HIGH);
send(msg_motion2.set(0));
}
delay(500);
}
delay(5000);
}
void send_motions() {
if (digitalRead(Move1)==1){send(msg_motion1.set(1));}
if (digitalRead(Move2)==1){send(msg_motion2.set(1));}
}
