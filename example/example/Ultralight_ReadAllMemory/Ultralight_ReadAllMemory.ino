/*!
    @flie Ultralight_ReadAllMemory.ino
    @Copyright   [DFRobot](http://www.dfrobot.com), 2016
    @Copyright   GNU Lesser General Public License
    @version  V1.0
    @date  07/03/2019

    @brief his demo runs on the arduino platform
           Download this demo to View everything in Ultralight’s memory

    This demo and related libraries are for DFRobot Gravity: I2C NFC Module
    Product(CH): http://www.dfrobot.com.cn/goods-762.html
    Product(EN): https://www.dfrobot.com/product-892.html
*/

#include <DFRobot_PN532.h>
//#define S50LEN    (64)
//#define S70LEN    (256)

// the IRQ and reset lines.  Use the value for the shield!
#define PN532_IRQ      (4)
#define  INTERRUPT      (1)
#define  POLLING        (0)
//use this line for a breakout or shield with an I2C connection
//check the card by polling
DFRobot_PN532_IIC  nfc(PN532_IRQ, POLLING);  //Use abbreviations instead of full names

uint8_t dataRead[4] = {0};
struct card NFCcard ;
//This procedure is to print the corresponding module of I.
void printi(uint8_t block) {
  //uint8_t data[4]={0};
  Serial.print(":");
  if (nfc.readUltralight(dataRead, block) == 1) {               /*Read 16 data in module 1 of IC card*/
    //Serial.println("Data read(HEX):");
    for (int i = 0; i < 4; i++) {
      Serial.print(dataRead[i], HEX);
      Serial.print(" ");
	  //dataRead[i] = 0;
    }
    Serial.println("");
  }
}

void setup() {
  Serial.begin(115200);
  while (!nfc.begin()) {
    //Wait for NFC module activation
    Serial.println("initial failure");
    delay (1000);
  }
  Serial.println("Waiting for a card......");
}

void loop() {
  /*Check if there is an IC card*/
  //
  //Serial.println("Waiting for a card......");
  delay(2000);
  if (nfc.scan()) {
     /*!
     The Ultralight'S EEPROM memory is organized in pages with 4 bytes per page. The Ultralight
     has 14d pages in total. The memory organization can be seen  in the following table . 
     
              page Adr              BYTE number within a page
                           0           |    1    |   2       |    3      |    
               0                       serial  number
               1                       serial  number
               2          serial number|internal |       lock bytes      |
               3           OTP         | OTP     |  OTP      |   OTP     |
               -----------------------------------------------------------
               4
               ...                     user memory
               15                  
               -----------------------------------------------------------
               16
               17                      Configuration pages   
               18
               19                      Counter pages
     
     
*/ 
    NFCcard = nfc.getInformation();
    if (NFCcard.AQTA[1] == 0x44 && memcmp(NFCcard.cardType, "Ultralight", 10) == 0) {
      Serial.println("----------------Here is the card information to read-------------------");
      for (int i = 0; i < NFCcard.blockNumber; i++) {
        Serial.print("block"); Serial.print(i);
        if (i == 0 || i == 1 || i == 2) {
          Serial.print("(Manufacturer data and lock bytes)------>");
          printi(i);
        }
        else if (i == 3) {
          Serial.print("(One Time Programmable)----------------->");
          printi(i);
        }
        else if (i >= NFCcard.blockNumber - 4 && i < NFCcard.blockNumber) {
          Serial.print("(Configuration pages)------------------->");
          printi(i);
        }
        else {
          Serial.print("---------------------------------------->");
          printi(i);
        }
      }
    }
    else {
      Serial.println("The card type is not Ultralight...");
    }
  }
