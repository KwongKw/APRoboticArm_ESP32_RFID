/*
   This ESP32 code is created by esp32io.com

   This ESP32 code is released in the public domain

   For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-rfid-nfc-relay
*/

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN    5  // ESP32 pin GIOP5 
#define RST_PIN   27 // ESP32 pin GIOP27 
#define RELAY_PIN 22 // ESP32 pin GIOP22 connects to relay

#define CM_PIN 22 // ESP32 pin GIOP26 connects to Dobot Arm

MFRC522 rfid(SS_PIN, RST_PIN);

MFRC522::MIFARE_Key key;

// Read from excel or something else //TAG_A: 4E 8A D6 C5 | TAG_B: AE 8A D6 C5 | TAG_C: 4E 0A D6 C5 | TAG_D: CE 1A D6 C5 | TAG_E: 6E 1A D6 C5
byte authorizedUIDs[][4] = {
  {0x4E, 0x8A, 0xD6, 0xC5},
  {0xAE, 0x8A, 0xD6, 0xC5},
  {0x4E, 0x0A, 0xD6, 0xC5},
  {0xCE, 0x1A, 0xD6, 0xC5},
  {0x6E, 0x1A, 0xD6, 0xC5}  
};

byte bufferLen = 18;
byte readBlockData[18];
enum readerState {Idle, True, False};

readerState GetReaderState();

void setup() {
  Serial.begin(9600);
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522
  pinMode(RELAY_PIN, OUTPUT);   // initialize pin as an output.
  digitalWrite(RELAY_PIN, LOW); // deactivate the relay
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF; //Key

  pinMode(CM_PIN, OUTPUT);
  digitalWrite(CM_PIN, LOW);

  Serial.println("Tap an RFID/NFC tag on the RFID-RC522 reader...");
}

void loop() {
  switch (GetReaderState()) {
    
    case Idle:
      break;
    
    case True:
      /* Print the data read from block */
      digitalWrite(CM_PIN, HIGH);
      ReadDataFromBlock(1, readBlockData);
      Serial.print("\nType      : ");
      for (int j = 0 ; j < 16 ; j++) Serial.write(readBlockData[j]);
      ReadDataFromBlock(2, readBlockData);
      Serial.print("\nRemaining : ");
      for (int j = 0 ; j < 16 ; j++) Serial.write(readBlockData[j]);
      Serial.print(" ml\n");
      delay(2000);
      digitalWrite(CM_PIN, LOW);
      break;
    
    case False:
      Serial.print("\nTag not recognised\n");
      delay(2000);
      break;
  }
  
  rfid.PICC_HaltA(); // halt PICC
  rfid.PCD_StopCrypto1(); // stop encryption on PCD
}


readerState GetReaderState() {
  if (!rfid.PICC_IsNewCardPresent()) return Idle; // new tag is available
  if (!rfid.PICC_ReadCardSerial()) return Idle; // NUID has been readed

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

  for (int i = 0; i < sizeof(authorizedUIDs) / sizeof(authorizedUIDs[0]); i++)
    if (rfid.uid.uidByte[0] == authorizedUIDs[i][0] &&
        rfid.uid.uidByte[1] == authorizedUIDs[i][1] &&
        rfid.uid.uidByte[2] == authorizedUIDs[i][2] &&
        rfid.uid.uidByte[3] == authorizedUIDs[i][3] ) return True;
  return False;
}

void ReadDataFromBlock(int blockNum, byte readBlockData[])
{
  /* Authenticating the desired data block for Read access using Key A */
  byte status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(rfid.uid));
  if (status != MFRC522::STATUS_OK) return;

  /* Reading data from the Block */
  status = rfid.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK) return;
}
