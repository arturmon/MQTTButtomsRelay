/**
 * 
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * DESCRIPTION
 *
 * 
 * Используется библиотека MySensors 1.5
 * 
 * 
 */


// Включить отладку
#define MY_DEBUG 

// Выбор трансорта NRF24 поумолчанию
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69

#define MY_NODE_ID AUTO

#define MY_REPEATER_FEATURE

#include <SPI.h>
#include <MySensor.h>
#include <Bounce2.h>

// Пины подключения Реле
#define RELAY_PIN_1  6  
#define RELAY_PIN_2  7  
// Пины подключения Выключателей
#define BUTTON_PIN_1  4  
#define BUTTON_PIN_2  5  
// ID Сенсоров 
#define CHILD_ID_1 1   
#define CHILD_ID_2 2   
// Какое Состояние включает и выключает реле 0 или 1
#define RELAY_ON 0
#define RELAY_OFF 1

Bounce debouncer_1 = Bounce(); 
Bounce debouncer_2 = Bounce();
int oldValue_1=0;
int oldValue_2=0;
bool state_1;
bool state_2;

MySensor gw;
MyMessage msg_1(CHILD_ID_1,V_LIGHT);
MyMessage msg_2(CHILD_ID_2,V_LIGHT);

void setup()  
{  
  gw.begin(incomingMessage, AUTO, true);

  gw.sendSketchInfo("Buttoms_Relay_v3", "1.6 29.10.2015");   
  // Настройки Кнопки
  pinMode(BUTTON_PIN_1,INPUT);
  // Включение подтягивающего резистора
  digitalWrite(BUTTON_PIN_1,HIGH);
  // Настройки Кнопки
  pinMode(BUTTON_PIN_2,INPUT);
  // Включение подтягивающего резистора
  digitalWrite(BUTTON_PIN_2,HIGH);

 
  // Натсройка антидребезга
  debouncer_1.attach(BUTTON_PIN_1);
  debouncer_1.interval(5);
  // Натсройка антидребезга
  debouncer_2.attach(BUTTON_PIN_2);
  debouncer_2.interval(5);


  // Регистрация Датчиков
  gw.present(CHILD_ID_1, S_LIGHT);
  // Регистрация Датчиков
  gw.present(CHILD_ID_2, S_LIGHT);  



  // Проверяем что при включении реле выключенны
  digitalWrite(RELAY_PIN_1, RELAY_OFF);
  // Включаем Пин на вывод
  pinMode(RELAY_PIN_1, OUTPUT);   

      
  // Устанавливаем последнее состояние Реле
  state_1 = gw.loadState(CHILD_ID_1);
  digitalWrite(RELAY_PIN_1, state_1?RELAY_ON:RELAY_OFF);  

  // Проверяем что при включении реле выключенны
  digitalWrite(RELAY_PIN_2, RELAY_OFF);
  // Включаем Пин на вывод
  pinMode(RELAY_PIN_2, OUTPUT);   
      
  // Устанавливаем последнее состояние Реле
  state_2 = gw.loadState(CHILD_ID_2);
  digitalWrite(RELAY_PIN_2, state_2?RELAY_ON:RELAY_OFF);    
}


void loop() 
{
  gw.process();
  debouncer_1.update();
  debouncer_2.update();
  // Обновляем значения
  int value_1 = debouncer_1.read();
  if (value_1 != oldValue_1 && value_1==0) {
      gw.send(msg_1.set(state_1?false:true), true); // Отправляем новое значение с ACK подтверждением
  }
  oldValue_1 = value_1;
   
  // Обновляем значения
  int value_2 = debouncer_2.read();
  if (value_2 != oldValue_2 && value_2==0) {
      gw.send(msg_2.set(state_2?false:true), true); // Отправляем новое значение с ACK подтверждением
  }
  oldValue_2 = value_2;  
} 

void incomingMessage(const MyMessage &message) {
  // Ожидаем только один тип сообщения. Но лучше проверим.
  if (message.isAck()) {
     Serial.println("This is an ack from gateway");
  }

  if (message.type == V_LIGHT && message.sensor == CHILD_ID_1) {
     // Изменяем значение реле
     state_1 = message.getBool();
     digitalWrite(RELAY_PIN_1, state_1?RELAY_ON:RELAY_OFF);
     // Сохраняем значение в  eeprom
     gw.saveState(CHILD_ID_1, state_1);
    
     Serial.print("Incoming change for sensor:");
     Serial.print(message.sensor);
     Serial.print(", New status: ");
     Serial.println(message.getBool());
   } 
  if (message.type == V_LIGHT && message.sensor == CHILD_ID_2) {
     // Изменяем значение реле
     state_2 = message.getBool();
     digitalWrite(RELAY_PIN_2, state_2?RELAY_ON:RELAY_OFF);
     // Сохраняем значение в  eeprom
     gw.saveState(CHILD_ID_2, state_2);
    
     Serial.print("Incoming change for sensor:");
     Serial.print(message.sensor);
     Serial.print(", New status: ");
     Serial.println(message.getBool());
   }    
}

