/*
* iDomed LEITO
* (c) João Carlos Pandolfi Santana
* (c) fabiano filho
* last updated: 4th september

*/
#include <SPI.h>
#include <MFRC522.h>

constexpr uint8_t RST_PIN = 9;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = 10;     // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance


//Time
#define lockTimeSeconds 15
#define readTimeMs 500
#define loopTimeMs 50
#define beepTimeMs 200
#define relativeTime (lockTimeSeconds*1000)/(readTimeMs+loopTimeMs+beepTimeMs)


//Pins
const int buzzPin = 4;
const int trigPin = 6; // note: set these numbers to what ever pin you attached them to
const int echoPin = 7;

//Global Variables
float calculatedRelativeTime;
int currentTime;
byte lockTime;
byte lockAlarm;

//*****************************************************************************************//
void setup() {

  // Initialize serial communications with the PC
  Serial.begin(9600); 
  
  //initualize RFID                        
  SPI.begin();  // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card    
  
  //Initialize BUZZER and LEDs
  pinMode(buzzPin, OUTPUT);
  
  //Initialize ultrasonic-sensor:
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  //Initialize Variables
  calculatedRelativeTime = relativeTime;
  currentTime = 0;
  lockTime = 0;
  lockAlarm = 0;

  //Ready
  Serial.println(F("iDomed :D"));
}


void loop() {
    if(checkProximity() || lockAlarm){
        
        lockAlarm = 1;
        Serial.println("ALARME ATIVADO");
        
        if(hasCard()){
            if(cleanCard()){
                 Serial.println(F("\n**CARTAO LIMPO**\n"));
                 lockTime = 1; 
                 currentTime++;
                 lockAlarm = 0;
                  deactivateSound();
            }else{          
                  Serial.println(F("\n**CARTAO SUJO**\n"));
                  wrongCardSong();
            }
        }
        else{
            if(lockTime == 0){
               currentTime = 0;
               soundAlarm();
            }else{
                lockTime = inActivatedTime();
                currentTime++;
            }
          
       }
   
        closeConnection();
        readDelay();    
   }
   loopDelay();
}

void readDelay(){
  delay(readTimeMs);
}

void loopDelay(){
 delay(loopTimeMs);
}

/*
* Check if time is finished
*/
byte inActivatedTime(){
    Serial.print("TEMPO ESPERADO: ");
    Serial.println(calculatedRelativeTime);
    Serial.print("TEMPO ATUAL: ");
    Serial.println(currentTime);
    if(currentTime < calculatedRelativeTime)
        return 1;
    
    return 0;
}


/*
* Check People proximity
*/
int checkProximity(){
  // Establish variables for duration of the ultrasonic ping and the distance:
  long duration, inches, cm;

  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Then send out a short HIGH ultrasonic pulse that will bounce off the object infront of it:
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Receive and read the bounced-back HIGH pulse:
  duration = pulseIn(echoPin, HIGH);

  // Convert the time it took for the pulse to back into a distance:
  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);
 
  Serial.print("DISTANCIA:");
  Serial.println(inches);
 
  if(cm < 30)
     return 1;
    
  return 0;
}


// Converts the time taken for the ultrasonic pulse to bounce back into a distance
// Both Imperial and Metric measurement conversions have been included below:
long microsecondsToInches(long microseconds)
{
  return microseconds / 74 / 2;
}
long microsecondsToCentimeters(long microseconds)
{
  return microseconds / 29 / 2;
}


/*
* Make some noise
*/
void soundAlarm(){
    tone(buzzPin, 150);
    delay(beepTimeMs); 
    noTone(buzzPin);
}

void deactivateSound(){
    tone(buzzPin, 500);
    delay(50);
    noTone(buzzPin);
    delay(50);
    tone(buzzPin, 500);
    delay(50);
    noTone(buzzPin);
}

void wrongCardSong(){
    tone(buzzPin, 700);
    delay(200);
    noTone(buzzPin);
    delay(200);
    tone(buzzPin, 100);
    delay(100);
    noTone(buzzPin);
    tone(buzzPin, 500);
    delay(100);
    noTone(buzzPin);
}

/*
* Close RFID Connection
*/
void closeConnection(){
  mfrc522.PICC_HaltA(); // Halt PICC
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
}


/*
* Check if has card
*/
int hasCard(){
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return 0;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return 0;
  }

    return 1;
}


int cleanCard(){
  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
    MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card

  //-------------------------------------------

  Serial.print(F("Name: "));

  byte buffer1[18];
  byte key1[18];
  byte key2[18];
  byte len;
  byte block;
  byte lenKey1;
  byte lenKey2;  
  byte correct;
  
  //============= Setting Keys ===========
  //Key1
  key1[0] = 'o';
  key1[1] = 'k';
  lenKey1 = 2;
  
  //Key2
  key2[0] = 'c';
  key2[1] = 'l';
  key2[2] = 'e';
  key2[3] = 'a';
  key2[4] = 'n';
  lenKey2 = 5;
  
  //============= Initializing local variables ===========
  block = 4;
  len = 18;
  correct = 0;

  //------------------------------------------- GET FIRST NAME
  MFRC522::StatusCode status;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }


  //PRINT FIRST NAME
  for (uint8_t i = 0; i < lenKey1; i++)
  {
    if (buffer1[i] != 32)
    {
        if(buffer1[i] == key1[i])
            correct+=1;
        
    
      Serial.write(buffer1[i]);
    }
  }
  Serial.print(" ");

  //---------------------------------------- GET LAST NAME

  byte buffer2[18];
  block = 1;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer2, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //PRINT LAST NAME
  for (uint8_t i = 0; i < lenKey2; i++) {
      if(buffer2[i] == key2[i])
          correct+= 1;
    
    Serial.write(buffer2[i] );
  }


  //----------------------------------------

  Serial.println(F("\n**End Reading**\n"));

  //mfrc522.PICC_HaltA();
  //mfrc522.PCD_StopCrypto1();

  if(correct == (lenKey1 + lenKey2)){
      writeCard(key,status);
      return 1;
  }
  else
      return 0;

}

// =============================== ESCREVE NO CARTAO ==============================
void writeCard(MFRC522::MIFARE_Key key,MFRC522::StatusCode status){
   
  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  //MFRC522::MIFARE_Key key;
  //for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  
  Serial.print(F("Card UID:"));    //Dump UID
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.print(F(" PICC type: "));   // Dump PICC type
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  byte buffer[34];
  byte block;
  //MFRC522::StatusCode status;
  byte len;

  // Ask personal data: Family name
  //Serial.println(F("Type Family name, ending with #"));
  //len = Serial.readBytesUntil('#', (char *) buffer, 30) ; // read family name from serial
  
  buffer[0] = 'd';
  buffer[1] = 'u';
  buffer[2] = 'r';
  buffer[3] = 't';
  buffer[4] = 'y';
  
  len = 5;
  //Preenche o resto do buffer com ' '
  for (byte i = len; i < 30; i++) buffer[i] = ' ';     // pad with spaces

  block = 1;
  //Serial.println(F("Authenticating using key A..."));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("PCD_Authenticate() success: "));

  // Write block
  status = mfrc522.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));

  block = 2;
  //Serial.println(F("Authenticating using key A..."));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // Write block
  status = mfrc522.MIFARE_Write(block, &buffer[16], 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));

   //ESCREVE NO BLOCO 2

  // Ask personal data: First name
  //Serial.println(F("Type First name, ending with #"));
  //len = Serial.readBytesUntil('#', (char *) buffer, 20) ; // read first name from serial

  buffer[0] = 'i';
  buffer[1] = 's';
    
  len = 2;

  for (byte i = len; i < 20; i++) buffer[i] = ' ';     // pad with spaces

  block = 4;
  //Serial.println(F("Authenticating using key A..."));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // Write block
  status = mfrc522.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));

  block = 5;
  //Serial.println(F("Authenticating using key A..."));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // Write block
  status = mfrc522.MIFARE_Write(block, &buffer[16], 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));


  Serial.println(" ");

}



