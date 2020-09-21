#include <OneWire.h>
#include <DallasTemperature.h>
//#define MY_DEBUG
#define MY_NODE_ID 11
#define MY_RADIO_RF24
#include <MySensors.h>
#include <TimeLib.h>

#define TEMP_SENS 1
#define ON_OFF 2
OneWire oneWire(14);  // датчик 18b20 на пине A0 на arduino nano
DallasTemperature ds(&oneWire);    

MyMessage msg_temp(TEMP_SENS, V_TEMP);
MyMessage msg_on_off(ON_OFF, V_VAR);

int const Relay = 6; // реле на пине d5 на arduino nano
//int const but_plus = 4; // кнопка +
//int const but_minus = 3; // кнопка -
int const but_select = 2; // кнопка выбора
uint32_t paused = 60000;
float temp_real;
float temp_min = 2;
float temp_max = 4;
const int timeZone = 3;     // мск
long previousMillis = 0;
boolean temp_flag = true;
//int val=0;

void setup() {
  requestTime();
  pinMode(Relay, OUTPUT);
//  pinMode(but_plus, INPUT);
//  pinMode(but_minus, INPUT);
  pinMode(but_select, INPUT);
  digitalWrite(Relay, HIGH);
  ds.begin();
}
void presentation()
{
  sendSketchInfo("Holodos", "1.0");
  wait(100);
  present(TEMP_SENS, S_TEMP, "Temp_Holod");
  wait(100);
  present(ON_OFF, S_CUSTOM, "On_Off_Holod");
  
}
void loop() {
 if (millis() - previousMillis >= paused){
  ds.requestTemperatures();
  temp_real = (ds.getTempCByIndex(0));
 // temp_real = temp_real-8;
send(msg_temp.set(temp_real,1)); // отправляем температуру на шлюз с одним знаком после запятой
if (temp_real>=temp_max && digitalRead(Relay)==HIGH){digitalWrite(Relay, LOW);send(msg_on_off.set(1));}
if (temp_real<=temp_min && digitalRead(Relay)==LOW){digitalWrite(Relay, HIGH);send(msg_on_off.set(0));}
previousMillis = millis();
send(msg_on_off.set(!digitalRead(Relay)));
}
//if(digitalRead(but_select)==HIGH){press_select();}//если нажали кнопку выбора
}

//void press_select() {

//clean; // очищаем экран и выводим начальную и конечную температуру
//echo temp_min;
//echo temp_max
// если еще раз нажали, меняем флаг выбора температуры
//if(digitalRead(but_select) == HIGH){
//  if(digitalRead(but_plus) == HIGH and temp_max<20 and temp_max>0){temp_max=temp_max+0.1;}
//  if(digitalRead(but_minus) == HIGH and temp_max<20 and temp_max>0){temp_max=temp_max-0.1;} }}

void receiveTime(unsigned long controllerTime) {
  controllerTime = controllerTime + timeZone * SECS_PER_HOUR;
  setTime(controllerTime);
}
