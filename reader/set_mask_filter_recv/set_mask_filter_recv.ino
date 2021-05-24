#include <SPI.h>
#include "mcp_can.h"
#include <avr/eeprom.h>

const int SPI_CS_PIN = 5;

MCP_CAN CAN(SPI_CS_PIN);                    // Set CS pin

unsigned char len = 0;
unsigned char buf[8];
int currentAngle = 360 * 2.5;
int wheelOffset;
bool autoMode = false;

int getSteeringAngle() {                   // Получение угла поворота руля
  int resPWM = (analogRead(A0) - wheelOffset) * 2.3;
  return resPWM;
}

void setup() {

  TCCR1A = 0b00000001;
  TCCR1B = 0b00000010;

  tone(3, 25);
  //TCCR2B = 0b00000111;
  //TCCR2A = 0b00000001;                      // Эмулятор тахометра 30гц
  //analogWrite(3, 157);
  analogWrite(9, 127 + 20);
  analogWrite(10, 127 + 20);
  analogWrite(6, 127);
  delay(300);
  digitalWrite(6, 0);

  wheelOffset = eeprom_read_word(0);

  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  
  Serial.begin(115200);
  
  while (CAN_OK != CAN.begin(CAN_500KBPS)) {            // init can bus : baudrate = 500k
    Serial.println("CAN BUS Shield init fail");
    Serial.println(" Init CAN BUS Shield again");
    delay(200);
  }
  Serial.println("CAN BUS Shield init ok!");
  
  // Фильтрция сообщений
  CAN.init_Mask(0, 0, 0x3ff);
  CAN.init_Mask(1, 0, 0x3ff);
  
  CAN.init_Filt(0, 0, 0x01); // запрос на получение угла поворота
  CAN.init_Filt(1, 0, 0x03); // запрос управление ЭУРом
  CAN.init_Filt(2, 0, 0x04); // запрос на обновление крайнего положения руля
  CAN.init_Filt(3, 0, 0x05); // принудительная установка напряжения ЭУРа
}

void loop() {
  if (CAN_MSGAVAIL == CAN.checkReceive()) {     // Прорверка получения данных
    
    CAN.readMsgBuf(&len, buf);    // read data
    
    if (CAN.getCanId() == 0x01) {               // Зпарос на отправку текущего положения руля
      int angle = getSteeringAngle();
      byte *tempBuf = (byte*)&angle;
      CAN.sendMsgBuf(2, 0, 2, tempBuf);
    }
    
    if (CAN.getCanId() == 0x03) {               // Запрос на установку положения руля
      autoMode = true;
      currentAngle = (buf[1] << 8) + buf[0];
    }

    if (CAN.getCanId() == 0x04) {               // Запрос на обновление крайнего положения руля
      analogWrite(9, 127);
      analogWrite(10, 127);

      analogWrite(6, 127);
      delay(500);
      digitalWrite(6, 0);
      delay(29500);
      int leftBoundary = analogRead(A0);
      analogWrite(6, 127);
      delay(500);
      digitalWrite(6, 0);
      delay(500);
      analogWrite(6, 127);
      delay(500);
      digitalWrite(6, 0);
      delay(28500);
      int rightBoundary = analogRead(A0);
      analogWrite(6, 127);
      delay(500);
      digitalWrite(6, 0);
      delay(500);
      analogWrite(6, 127);
      delay(500);
      digitalWrite(6, 0);
      delay(500);
      analogWrite(6, 127);
      delay(500);
      digitalWrite(6, 0);

      int result = (leftBoundary + rightBoundary) / 2; // Простой и надежный алгоритм калибровки и определения нуля по двум крайним точкам
      
      eeprom_update_word(0, result);
      wheelOffset = eeprom_read_word(0);
    }

    if (CAN.getCanId() == 0x05) {               // Ставим напряжения
      autoMode = false;
      analogWrite(9, buf[0] + 20);
      analogWrite(10, 255 - buf[0] + 20);
    }
  }

  if (autoMode) {                               // Поддержание угла поворота руля
    int difference = 20 * sqrt(abs(currentAngle - getSteeringAngle()));
    if (difference > 235)
      difference = 235;
    if (currentAngle > getSteeringAngle()) {
      analogWrite(9, 255 - difference + 20);
      analogWrite(10, difference + 20);
    }
    else {
      analogWrite(9, difference + 20);
      analogWrite(10, 255 - difference + 20);
    }
  }
}
