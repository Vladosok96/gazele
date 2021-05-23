#include <mcp_can.h>
#include <SPI.h>

const int SPI_CS_PIN = 53;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

void setup() {
    Serial.begin(115200);

    while (CAN_OK != CAN.begin(CAN_500KBPS)) {            // init can bus : baudrate = 500k
        Serial.println("CAN BUS Shield init fail");
        Serial.println(" Init CAN BUS Shield again");
        delay(200);
    }
    Serial.println("CAN BUS Shield init ok!");
}

unsigned char buf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char len = 0;

void loop() {
  if (digitalRead(7)){
    CAN.sendMsgBuf(1, 0, 0, buf); // Запрос на получение угла
    Serial.println("request angle");
    delay(200);
    
  }
  
  if (CAN_MSGAVAIL == CAN.checkReceive()) {                // check if get data

    CAN.readMsgBuf(&len, buf);    // read data
    
    if (CAN.getCanId() == 0x02) { // Получение угла
      Serial.print("Get Data From id: ");
      Serial.println(CAN.getCanId());
      Serial.print("data: ");
      Serial.println(buf[0], DEC);
    }
    
    /*if (CAN.getCanId() == 0x01) {
      buf[0] = getSteeringAngle();
      Serial.print(buf[0]);
      CAN.sendMsgBuf(2, 0, 1, buf);
      
      Serial.println("\r\n------------------------------------------------------------------");
      Serial.print("Get Data From id: ");
      Serial.println(CAN.getCanId());
      for (int i = 0; i < len; i++) { // print the data
        Serial.print("0x");
        Serial.print(buf[i], HEX);
        Serial.print("\t");
      }
      Serial.println();
    }*/
  }
    /*for (int i = 0; i < 10; i++) {
        memset(stmp, i, sizeof(stmp));                 // set id to send data buff
        CAN.sendMsgBuf(i, 0, sizeof(stmp), stmp);
        Serial.print(i);
        Serial.print(" ");
        delay(200);
    }
    Serial.println("\n-------------------");*/
}
