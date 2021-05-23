#include <SPI.h>
#include "mcp_can.h"
#include <avr/eeprom.h>


int getSteeringAngle() {                   // Получение угла поворота руля
  int resPWM = analogRead(A0);
  return resPWM;
}

const int SPI_CS_PIN = 5;

MCP_CAN CAN(SPI_CS_PIN);                    // Set CS pin

unsigned char len = 0;
unsigned char buf[8];
int currentAngle = 360 * 2.5;
int whellKf = eeprom_read_word(0);
bool autoMode = true;

void setup() {

  TCCR1A = TCCR1A & 0xe0 | 2;
  TCCR1B = TCCR1B & 0xe0 | 0x0a;            // Изменение частоты на 9 и 10 пинах до 3900гц для ЭУРа
  
  TCCR2B = 0b00000111;
  TCCR2A = 0b00000001;                      // Эмулятор тахометра 30гц
  analogWrite(3, 157);
  
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
}

void loop() {
  if (CAN_MSGAVAIL == CAN.checkReceive()) {     // Прорверка получения данных
    
    CAN.readMsgBuf(&len, buf);    // read data
    
    if (CAN.getCanId() == 0x01) {               // Зпарос на отправку текущего положения руля
      buf[0] = getSteeringAngle();
      CAN.sendMsgBuf(2, 0, 1, buf);
    }
    
    if (CAN.getCanId() == 0x03) {               // Запрос на установку положения руля
      currentAngle = buf[0];
    }

    if (CAN.getCanId() == 0x04) {               // Запрос на обновление крайнего положения руля
      analogWrite(9, 127);
      analogWrite(10, 127);
      
      delay(30000);
      int leftBoundary = getSteeringAngle();
      delay(30000);
      int rightBoundary = getSteeringAngle();

      int result = 0; // Простой и надежный алгоритм калибровки и определения нуля по двум крайним точкам
      
      eeprom_update_word(0, result);
    }
  }

  if (autoMode) {                               // Поддержание угла поворота руля
    if (currentAngle > getSteeringAngle()) {
      analogWrite(9, 50);
      analogWrite(10, 200);
    }
    else if (currentAngle < getSteeringAngle()) {
      analogWrite(9, 200);
      analogWrite(10, 50);
    }
  }
}
