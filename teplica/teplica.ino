// 13:55
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"
#define MY_DEBUG
#define MY_NODE_ID 12
#define MY_RADIO_RF24
#include <MySensors.h>
#include <TimeLib.h>
#define DHTPIN 3 // номер пина DHT11

#define TEMP_SENS_ID 1
#define OKNO_ON_OFF_ID 2
#define HUM_1_ID 3
#define HUM_2_ID 4
#define HUM_3_ID 5
#define TEMP_SENS_ZEM_1_ID 6
#define TEMP_SENS_ZEM_2_ID 7
#define POLIV_ON_OFF_ID 8
#define CUSTOM_MODE_ON_OFF_ID 9
#define DVER_ON_OFF_ID 10
#define TEMP_ON_ID 11
#define TEMP_OFF_ID 12
#define MIN_HUM 565 // определяем минимальное показание датчика (в воздухе)
#define MAX_HUM 295 // определяем максимальное показание датчика (в воде)
uint16_t sensor1; // данные датчика влажности почвы №1
uint16_t sensor2; // данные датчика влажности почвы №2

OneWire oneWire(6);  // датчики 18b20 на пине d6 на arduino nano
DallasTemperature ds(&oneWire);
DHT dht(DHTPIN, DHT11);

MyMessage msg_temp(TEMP_SENS_ID, V_TEMP);
MyMessage msg_temp_zemlya_1(TEMP_SENS_ZEM_1_ID, V_TEMP);
MyMessage msg_temp_zemlya_2(TEMP_SENS_ZEM_2_ID, V_TEMP);
MyMessage msg_okno_on_off(OKNO_ON_OFF_ID, V_VAR);
MyMessage msg_hum_1(HUM_1_ID, V_VAR);
MyMessage msg_hum_2(HUM_2_ID, V_VAR);
MyMessage msg_hum_3(HUM_3_ID, V_HUM);
MyMessage msg_poliv_on_off(POLIV_ON_OFF_ID, V_VAR);
MyMessage msg_custom_mode_on_off(CUSTOM_MODE_ON_OFF_ID, V_VAR);
MyMessage msg_dver_on_off(DVER_ON_OFF_ID, V_VAR);
MyMessage msg_temp_on(TEMP_ON_ID, V_VAR);
MyMessage msg_temp_off(TEMP_OFF_ID, V_VAR);

int const relay1 = 8; // реле1 на пине d8 на arduino nano
int const relay2 = 7; // реле2 на пине d7 на arduino nano
int const mikruha_on = 5; // концевик открытого окна на пине d5 (верхняя микрушка)
int const mikruha_off = 4; // концевик закрытого окна на пине d6 (нижняя микрушка)
int const hum_1 = A0; // емкостный датчик влажности почвы №1 на пине A0
int const hum_2 = A1; // емкостный датчик влажности почвы №2 на пине A1
int const relay_open_dver = A2; // реле открытия двери на пине A2
int const relay_close_dver = A3; // реле закрытия двери на пине A3
int const dver_on = A4; // концевик открытой двери на пине A4
int const dver_off = A5; // концевик закрытой двери на пине A5
int const custom_button = A6; // кнопка ручного открытия двери на пине A6
int const relay_poliv = A7; // реле полива на пине A7
uint32_t paused = 30000;
float temp_real_1;
float temp_real_2;
const int timeZone = 3;     // время по мск
long previousMillis = 0;
int temp_on=25; // температура открытия окна
int temp_off=20; // температура закрытия окна
int temp_dver_on=23; // температура открытия двери
int temp_dver_off=19; // температура закрытия двери
boolean okno_status=false; // флаг состояния окна
boolean dver_status=false; // флаг состояния окна
boolean custom_mode=false; // флаг ручного режима
boolean okno=false;
boolean dver=false;
boolean poliv_on_off=false;

void setup() {
  system("chcp 1251");
//Serial.begin(115200);
  requestTime(); // запрос времени
  pinMode(hum_1, INPUT); // порт датчика влажности почвы №1 ставим на вход
  pinMode(hum_2, INPUT); // порт датчика влажности почвы №2 ставим на вход
  pinMode(relay1, OUTPUT); // порт реле1 ставим на выход
  pinMode(relay2, OUTPUT); // порт реле2 ставим на выход
  pinMode(mikruha_on, INPUT); // порт концевика открытой форточки ставим на вход (подтянут на плате к 1 резистором 10К)
  pinMode(mikruha_off, INPUT); // порт концевика закрытой форточки ставим на вход (подтянут на плате к 1 резистором 10К)
  pinMode(relay_open_dver, OUTPUT); // порт реле открытия двери ставим на выход
  pinMode(relay_close_dver, OUTPUT); // порт реле закрытия двери ставим на выход
  pinMode(dver_on, INPUT); // порт концевика открытой двери ставим на вход
  pinMode(dver_off, INPUT); // порт концевика закрытой двери ставим на вход
  pinMode(custom_button, INPUT); // порт кнопки ручного открытия ставим на вход
  pinMode(relay_poliv, OUTPUT); // порт реле полива ставим на вход
  digitalWrite(relay1, HIGH); // При включении выключаем реле №1 ( HIGH - выключено, LOW - включено )
  digitalWrite(relay2, HIGH); // При включении выключаем реле №2 ( HIGH - выключено, LOW - включено )
  digitalWrite(relay_open_dver, HIGH); // При включении выключаем реле полива ( HIGH - выключено, LOW - включено )
  digitalWrite(relay_close_dver, HIGH); // При включении выключаем реле насоса емкости ( HIGH - выключено, LOW - включено )
  ds.begin(); // инициализация датчика 18b20
  dht.begin(); // инициализация датчика DHT11
//  if (digitalRead(mikruha_on)==LOW && digitalRead(mikruha_off)==HIGH){okno_status=true;} // если форточка открыта, ставим статус в 1
//  if (digitalRead(mikruha_off)==LOW && digitalRead(mikruha_on)==HIGH){okno_status=false;} // // если форточка закрыта, ставим статус в 0
/*  if (digitalRead(mikruha_off)==HIGH && digitalRead(mikruha_on)==HIGH){digitalWrite(relay2, LOW); // если форточка при включении контроллера не закрыта и не открыта, закрывам её
while (digitalRead(mikruha_off)) {}
digitalWrite(relay2, HIGH);
okno_status=false;
send(msg_okno_on_off.set(okno_status));}*/
}

void presentation()
{
  sendSketchInfo("Teplica", "0.1");
    wait(50);
  present(TEMP_SENS_ID, S_TEMP, "Temp_Air");
    wait(50);
  present(TEMP_SENS_ZEM_1_ID, S_TEMP, "Temp_Zem_1");
    wait(50);
  present(TEMP_SENS_ZEM_2_ID, S_TEMP, "Temp_Zem_2");
    wait(50);
  present(OKNO_ON_OFF_ID, S_CUSTOM, "Okno_On_Off");
    wait(50);
  present(HUM_1_ID, S_CUSTOM, "Hum_1");
    wait(50);
  present(HUM_2_ID, S_CUSTOM, "Hum_2");
    wait(50);
  present(HUM_3_ID, S_HUM, "Hum_3");
    wait(50);
  present(POLIV_ON_OFF_ID, S_CUSTOM, "Poliv_On_Off");
    wait(50);
  present(CUSTOM_MODE_ON_OFF_ID, S_CUSTOM, "Custom_Mode_On_Off");
    wait(50);
  present(DVER_ON_OFF_ID, S_CUSTOM, "Dver_On_Off");
    wait(50);
	  present(TEMP_ON_ID, S_CUSTOM, "Temp_On");
    wait(50);
	  present(TEMP_OFF_ID, S_CUSTOM, "Temp_Off");
    wait(50);
}

void loop() {
  if (digitalRead(mikruha_on)==LOW && digitalRead(mikruha_off)==HIGH){okno_status=true;} // если форточка открыта, ставим статус в 1
  if (digitalRead(mikruha_off)==LOW && digitalRead(mikruha_on)==HIGH){okno_status=false;} // // если форточка закрыта, ставим статус в 0
//========================================================================================================================================== цикл в 30 секунд
if (millis() - previousMillis >= paused){
ds.requestTemperatures();
temp_real_1 = (ds.getTempCByIndex(0)); // получаем температуру почвы с датчика №1 18b20
temp_real_2 = (ds.getTempCByIndex(1)); // получаем температуру почвы с датчика №2 18b20
sensor1 = analogRead(hum_1);             // Читаем данные с датчика влажности №1
sensor1 = map(sensor1, MIN_HUM, MAX_HUM, 0, 100);  // адаптируем значения от 0 до 100
sensor2 = analogRead(hum_2);             // Читаем данные с датчика влажности №2
sensor2 = map(sensor2, MIN_HUM, MAX_HUM, 0, 100);  // адаптируем значения от 0 до 100
if (sensor1 > 100){sensor1=100;}
if (sensor2 > 100){sensor2=100;}
float h = dht.readHumidity(); //Измеряем влажность
float t = dht.readTemperature(); //Измеряем температуру
sendHeartbeat(false); // спасибо что живой :) false/true - запрашивать эхо или нет
wait(50);
send(msg_temp.set(t,1)); // отправляем температуру воздуха на шлюз с одним знаком после запятой
wait(50);
send(msg_hum_3.set(h,1)); // отправляем влажность воздуха с датчика DHT11
wait(50);
send(msg_temp_zemlya_1.set(temp_real_1,1)); // отправляем температуру почвы с датчика 18b20 №1 с одним знаком после запятой
wait(50);
send(msg_temp_zemlya_2.set(temp_real_2,1)); // отправляем температуру почвы с датчика 18b20 №2 с одним знаком после запятой
wait(50);
send(msg_okno_on_off.set(okno_status)); // отправляем состояние форточки
wait(50);
send(msg_hum_1.set(sensor1)); // отправляем влажность почвы с датчика влажности №1
wait(50);
send(msg_hum_2.set(sensor2)); // отправляем влажность почвы с датчика влажности №2
wait(50);
send(msg_poliv_on_off.set(poliv_on_off)); // отправляем состояние полива
wait(50);
send(msg_custom_mode_on_off.set(custom_mode)); // отправляем состояние ручного режима
wait(50);
send(msg_dver_on_off.set(dver_status)); // отправляем состояние двери
wait(50);
send(msg_temp_on.set(temp_on)); // отправляем температуру открытия окна
wait(50);
send(msg_temp_on.set(temp_on)); // отправляем температуру закрытия окна

if (round(t) >= temp_on && custom_mode == false && okno_status==false){digitalWrite(relay1, LOW);
while (digitalRead(mikruha_on)) {}
digitalWrite(relay1, HIGH);
okno_status=true;
send(msg_okno_on_off.set(okno_status));
}
if (round(t) >= temp_dver_on && custom_mode == false && dver_status==false){digitalWrite(relay_open_dver, LOW);
while (digitalRead(dver_on)) {}
digitalWrite(relay_open_dver, HIGH);
dver_status=true;
send(msg_dver_on_off.set(dver_status));
}
if (round(t)<=temp_off && custom_mode == false && okno_status==true){digitalWrite(relay2, LOW);
while (digitalRead(mikruha_off)) {}
digitalWrite(relay2, HIGH);
okno_status=false;
send(msg_okno_on_off.set(okno_status));
}
if (round(t) <= temp_dver_off && custom_mode == false && dver_status==true){digitalWrite(relay_close_dver, LOW);
while (digitalRead(dver_off)) {}
digitalWrite(relay_close_dver, HIGH);
dver_status=false;
send(msg_dver_on_off.set(dver_status));
}
previousMillis = millis();
} //=============================================================================================================================== конец цикла 30 секунд
}

void receive(const MyMessage & message) {
	  if (message.sensor == TEMP_ON_ID) {
     temp_on = message.getInt ();
    }
  if (message.sensor == TEMP_ON_ID) {
     temp_off = message.getInt ();
    }
if (message.sensor == POLIV_ON_OFF_ID) {
      if (message.type ==  V_VAR) {
     poliv_on_off = message.getBool ();
      if (poliv_on_off == 1){digitalWrite(relay_poliv, LOW);}
      if (poliv_on_off == 0){digitalWrite(relay_poliv, HIGH);}
    }
    send(msg_poliv_on_off.set(poliv_on_off)); // отправляем состояние полива
    }
  if (message.sensor == CUSTOM_MODE_ON_OFF_ID) {
      if (message.type ==  V_VAR) {
     custom_mode = message.getBool ();
    }
    send(msg_custom_mode_on_off.set(custom_mode)); // отправляем состояние режима
    }
if (message.sensor == OKNO_ON_OFF_ID) {
     if (message.type ==  V_VAR) {
    okno = message.getBool ();
     if (okno == 1 && okno_status == false){digitalWrite(relay1, LOW);okno_status=true;
     while (digitalRead(mikruha_on)){}
     digitalWrite(relay1, HIGH);send(msg_okno_on_off.set(okno_status));}
     if (okno == 0 && okno_status == true){digitalWrite(relay2, LOW);okno_status=false;
     while (digitalRead(mikruha_off)){}
     digitalWrite(relay2, HIGH);send(msg_okno_on_off.set(okno_status));}
    }}
if (message.sensor == DVER_ON_OFF_ID) {
     if (message.type ==  V_VAR) {
    dver = message.getBool ();
     if (dver == 1 && custom_mode == true && dver_status == false){digitalWrite(relay_open_dver, LOW);dver_status=true;
     while (digitalRead(dver_on)){}
     digitalWrite(relay_open_dver, HIGH);send(msg_dver_on_off.set(dver_status));}
     if (dver == 0 && custom_mode == true && dver_status == true){digitalWrite(relay_close_dver, LOW);dver_status=false;
     while (digitalRead(dver_off)){}
     digitalWrite(relay_close_dver, HIGH);send(msg_dver_on_off.set(dver_status));}
    }}
}
void printDigits(int digits) {
  Serial.print(":");
    if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void receiveTime(unsigned long controllerTime) {
 controllerTime = controllerTime + timeZone * SECS_PER_HOUR;
 setTime(controllerTime);
}
