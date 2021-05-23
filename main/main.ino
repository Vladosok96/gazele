#include <mcp_can.h>
#include <SPI.h>

class EurContol { 
  public: 
    EurContol (int CSPIN) {
      CAN = MCP_CAN(CSPIN);                                    // Set CS pin
      while (CAN_OK != CAN.begin(CAN_500KBPS)) {            // init can bus : baudrate = 500k
          Serial.println("CAN BUS Shield init fail");
          Serial.println(" Init CAN BUS Shield again");
          delay(200);
      }
      Serial.println("CAN BUS Shield init ok!");
    }
    
    void setSteeringAngle(int angle) {                        // установка угла
      buf[1] = angle;
      CAN.sendMsgBuf(3, 0, 1, buf); 
    }

  private:
    MCP_CAN CAN;
    unsigned char len = 0;
    unsigned char buf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
};


EurContol EUR(53);

void setup() {
    Serial.begin(115200);
}

void loop() {
    Serial.println("Request angle");
  if (digitalRead(7)){
    //CAN.sendMsgBuf(1, 0, 0, buf);                 // Запрос на получение угла
    EUR.setSteeringAngle(25);
    delay(200);
  }
  
  /*if (CAN_MSGAVAIL == CAN.checkReceive()) {       // Прорверка получения данных

    CAN.readMsgBuf(&len, buf);    // read data
    
    if (CAN.getCanId() == 0x02) {                 // Получение угла
      Serial.print("Get Data From id: ");
      Serial.println(CAN.getCanId());
      Serial.print("data: ");
      Serial.println(buf[0], DEC);
    }
    
  }*/
}
