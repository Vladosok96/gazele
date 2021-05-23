#include <mcp_can.h>
#include <SPI.h>


class Wheel { 
  private:
    MCP_CAN CAN = MCP_CAN(53);
    int currentAngle;
    int targetAngle;
    unsigned char len = 0;
    unsigned char buf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    
  public: 
    void setAngleNow(int angle) {                        // установка угла
      targetAngle = angle;
      buf[0] = targetAngle;
      byte *tempBuf = (byte*)&angle;
      CAN.sendMsgBuf(3, 0, 2, tempBuf);
      Serial.print("Set angle: ");
      Serial.println(targetAngle);
    }

    int getAngleNow() {
      CAN.sendMsgBuf(1, 0, 0, buf);
      unsigned long time = millis();
      while (millis() - time < 50) {
        if (CAN_MSGAVAIL == CAN.checkReceive()) {       // Прорверка получения данных
          CAN.readMsgBuf(&len, buf);    // read data
          if (CAN.getCanId() == 0x02) {                 // Получение угла
            currentAngle = (buf[1] << 8) + buf[0];
            Serial.print("Obtained angle: ");
            Serial.println(currentAngle);
            return currentAngle;
          }
        }
      }
      Serial.println("CAN get angle timeout!!!");
    }

    init() {
      while (CAN_OK != CAN.begin(CAN_500KBPS)) {            // init can bus : baudrate = 500k
          Serial.println("CAN BUS Shield init fail");
          Serial.println("Init CAN BUS Shield again");
          delay(200);
      }
      Serial.println("CAN BUS Shield init ok!");
      targetAngle = getAngleNow();
    }

    void setAngle(int angle) {
      targetAngle = angle;
    }

    int getAngle(){
      return currentAngle;
    }

    void update() {
      getAngleNow();
      setAngleNow(targetAngle);
    } 
};


Wheel wheel;

void setup() {
  Serial.begin(115200);
  Serial.println("Start");
  wheel.init();
}

void loop() {
  if (digitalRead(7)) {
    wheel.setAngleNow(25);
    delay(200);
  }

  if (digitalRead(6)) {
    wheel.getAngleNow();
    delay(200);
  }
}
