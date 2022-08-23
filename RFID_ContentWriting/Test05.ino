#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN    5  // ESP32 pin GIOP5 
#define RST_PIN   27 // ESP32 pin GIOP27 
#define RELAY_PIN 22 // ESP32 pin GIOP22 connects to relay

MFRC522 rfid(SS_PIN, RST_PIN);

MFRC522::MIFARE_Key key;

/* Set the block to which we want to write data */
/* Be aware of Sector Trailer Blocks */
// int blockNum = 2;
/* Create an array of 16 Bytes and fill it with data */
/* This is the actual data which is going to be written into the card */
byte blockData[][2][16] = {
  {"Water", "1000"},
  {"Gin", "250"},
  {"Sherry", "300"},
  {"Rome", "250"},
  {"Vodka", "150"}
};
byte count = 0 ;
/* Create another array to read data from Block */
/* Legthn of buffer should be 2 Bytes more than the size of Block (16 Bytes) */
byte bufferLen = 18;
byte readBlockData[18];

MFRC522::StatusCode status;

void setup()
{
  /* Initialize serial communications with the PC */
  Serial.begin(9600);
  /* Initialize SPI bus */
  SPI.begin();
  /* Initialize MFRC522 Module */
  rfid.PCD_Init();
  Serial.println("Scan a MIFARE 1K Tag to write data...");
  Serial.print("Tag ");
  Serial.print(count+1);
  Serial.print(" ready to write...");
}

void loop()
{
  /* Prepare the ksy for authentication */
  /* All keys are set to FFFFFFFFFFFFh at chip delivery from the factory */
  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = 0xFF;
  }


  /* Look for new cards */
  /* Reset the loop if no new card is present on RC522 Reader */
  if ( ! rfid.PICC_IsNewCardPresent())
  {
    return;
  }

  /* Select one of the cards */
  if ( ! rfid.PICC_ReadCardSerial())
  {
    return;
  }
  Serial.print("\n");
  Serial.println("**Card Detected**");
  /* Print UID of the Card */
  Serial.print(F("Card UID:"));
  for (byte i = 0; i < rfid.uid.size; i++)
  {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.print("\n");
  /* Print type of card (for example, MIFARE 1K) */
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  /* Call 'WriteDataToBlock' function, which will write data to the block */
  Serial.print("\n");
  Serial.println("Writing to Data Block...");
  for (byte i = 1; i < 3; i++ )
    WriteDataToBlock(i, blockData[count][i - 1]);

  /* Read data from the same block */
  Serial.print("\n");
  Serial.println("Reading from Data Block...");
  for (byte i = 1; i < 3; i++ ) {
    ReadDataFromBlock(i, readBlockData);
    /* If you want to print the full memory dump, uncomment the next line */
    //rfid.PICC_DumpToSerial(&(rfid.uid));

    /* Print the data read from block */
    Serial.print("\n");
    Serial.print("Data in Block:");
    Serial.print(i);
    Serial.print(" --> ");
    for (int j = 0 ; j < 16 ; j++)
    {
      Serial.write(readBlockData[j]);
    }
    Serial.print("\n");
  }
  rfid.PICC_HaltA(); // halt PICC
  rfid.PCD_StopCrypto1(); // stop encryption on PCD
  delay(2000);
  count++;
  Serial.print("Tag ");
  Serial.print(count+1);
  Serial.print(" ready to write...");
}



void WriteDataToBlock(int blockNum, byte blockData[])
{
  /* Authenticating the desired data block for write access using Key A */
  status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(rfid.uid));
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Authentication failed for Write: ");
    return;
  }
  else
  {
    Serial.println("Authentication success");
  }


  /* Write data to the block */
  status = rfid.MIFARE_Write(blockNum, blockData, 16);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Writing to Block failed: ");
    return;
  }
  else
  {
    Serial.println("Data was written into Block successfully");
  }

}

void ReadDataFromBlock(int blockNum, byte readBlockData[])
{
  /* Authenticating the desired data block for Read access using Key A */
  byte status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(rfid.uid));

  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Authentication failed for Read: ");
    return;
  }
  else
  {
    Serial.println("Authentication success");
  }

  /* Reading data from the Block */
  status = rfid.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Reading failed: ");
    return;
  }
  else
  {
    Serial.println("Block was read successfully");
  }

}
