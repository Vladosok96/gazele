// demo: CAN-BUS Shield, receive data with interrupt mode, and set mask and filter
//
// when in interrupt mode, the data coming can't be too fast, must >20ms, or else you can use check mode
// loovee, 2014-7-8

#include <SPI.h>
#include "mcp_can.h"


char getSteeringAngle() {
  int resPWM = analogRead(A0);
  return resPWM;
}

const int SPI_CS_PIN = 5;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

const byte interruptPin = 0;
unsigned char flagRecv = 0;
unsigned char len = 0;
unsigned char buf[8];
char str[20];

void setup() {

  //TCCR1A = TCCR1A & 0xe0 | 2;
  //TCCR1B = TCCR1B & 0xe0 | 0x0a;  // Изменение частоты для ЭУРа
  //analogWrite(9, 157);
  //analogWrite(10, 157);
  
  //TCCR2B = 0b00000111;
  //TCCR2A = 0b00000001;  // Эмулятор тахометра
  //analogWrite(3, 157);
  
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
}

void loop() {
  if (CAN_MSGAVAIL == CAN.checkReceive()) {      // check if get data
    
    CAN.readMsgBuf(&len, buf);    // read data
    
    if (CAN.getCanId() == 0x01) {
      Serial.print("Get Data From id: ");
      Serial.println(CAN.getCanId());

      buf[0] = getSteeringAngle();
      Serial.print("data: ");
      Serial.println(buf[0]);
      CAN.sendMsgBuf(2, 0, 1, buf);
      
      /*Serial.println("\r\n------------------------------------------------------------------");
      Serial.print("Get Data From id: ");
      Serial.println(CAN.getCanId());
      for (int i = 0; i < len; i++) { // print the data
        Serial.print("0x");
        Serial.print(buf[i], HEX);
        Serial.print("\t");
      }
      Serial.println();*/
    }
    if (CAN.getCanId() == 0x03) {
      Serial.print("Get Data From id: ");
      Serial.println(CAN.getCanId());

      
    }
  }
}

/*********************************************************************************************************
    END FILE
*********************************************************************************************************/
