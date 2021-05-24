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
    void calibrate() {                                   // Запуск калибровки
      CAN.sendMsgBuf(4, 0, 0, buf);
      Serial.println("Calibrate");
    }

    void setPWM(int val) {                                   // Запуск калибровки
      buf[0] = val;
      CAN.sendMsgBuf(5, 0, 1, buf);
      Serial.println("Succes set pwm");
    }
   
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
  Serial.setTimeout(100);
  Serial.println("Start");
  wheel.init();
  Serial.println("Type action id");
}

void loop() {
  int action = 0;
  if (Serial.available() > 0) {
      action = Serial.parseInt();

      if (action == 1) {
        wheel.getAngleNow();
      }
      else if (action == 3) {
        int angle = 9999;
        Serial.println("Type target angle");
        while(angle == 9999){
          if (Serial.available() > 0) {
            angle = Serial.parseInt();
          }
        }
        wheel.setAngleNow(angle);
      }
      else if (action == 4) {
        wheel.calibrate();
      }
      else if (action == 5) {
        int val = 9999;
        Serial.println("Type pwm value");
        while(val == 9999){
          if (Serial.available() > 0) {
            val = Serial.parseInt();
          }
        }
        Serial.println(val);
        wheel.setPWM(val);
      }
    }

  delay(50);
}
