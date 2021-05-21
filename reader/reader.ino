#include <SPI.h>
#include "mcp_can.h"

const int spiCSPin = 10;
boolean ledON = false;

MCP_CAN CAN(spiCSPin);

void setup()
{
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);

    while (CAN_OK != CAN.begin(CAN_500KBPS))
    {
        Serial.println("CAN BUS Init Failed");
        delay(100);
    }
    Serial.println("CAN BUS  Init OK!");


    CAN.setConfigMode();
    CAN.setFilterMask(MCP2515::MASK0, false, 0x7FF);
    CAN.setFilter(MCP2515::RXF0, false, 0x613);
    CAN.setFilter(MCP2515::RXF1, false, 0x613);
    CAN.setFilterMask(MCP2515::MASK1, false, 0x7FF);
    CAN.setFilter(MCP2515::RXF2, false, 0x613);
    CAN.setFilter(MCP2515::RXF3, false, 0x613);
    CAN.setFilter(MCP2515::RXF4, false, 0x613);
    CAN.setFilter(MCP2515::RXF5, false, 0x613);
    CAN.setNormalMode();
    //CAN.init_Mask(0,0,0x7E8);
    //CAN.init_Filt(0,0,0x7E8);
}

unsigned char len = 0;
unsigned char buf[8];

void loop()
{
    if(CAN_MSGAVAIL == CAN.checkReceive())
    {
        /*if(ledON) {
          digitalWrite(LED_BUILTIN, LOW);
          ledON = false;
          Serial.println("false");
        }
        else {
          digitalWrite(LED_BUILTIN, HIGH);
          ledON = true;
          Serial.println("true");
        }*/
            
        CAN.readMsgBuf(&len, buf);
        unsigned long canId = CAN.getCanId();

        Serial.println("-----------------------------");
        Serial.print("Data from ID: 0x");
        Serial.println(canId, HEX);

        for (int i = 0; i < len; i++) {
            Serial.print(buf[i]);
            Serial.print("\t");
        }
        Serial.println();
    }
}
