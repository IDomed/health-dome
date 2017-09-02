/* 
 * health dome
 * by fb joao
 * 
 * updated august 29th, 2017
 */

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);

// set constant variables for pins used by the sensor, buzzer, and LEDs:
const int trigPin = 6; // note: set these numbers to what ever pin you attached them to
const int echoPin = 7;
const int ledPin = 5;
const int actLedPin = 2;
const int buzzPin = 4;
boolean alarmActive = false;
const String targetUID = "90 7E DF 87";

/*/ !!!!!!!
  String content1= "";
  String content2= "";


    MFRC522::MIFARE_Key key;

  //some variables we need
  byte block;
  byte len;
  MFRC522::StatusCode status;
*/

void setup() {
  // Initialize serial communication:
  Serial.begin(9600);

  // Initialize RFID Reader:
  SPI.begin();
  mfrc522.PCD_Init();

  // Initialize LEDs:
  pinMode(ledPin, OUTPUT);
  pinMode(actLedPin, OUTPUT);
  pinMode(buzzPin, OUTPUT);

  // Initialize ultrasonic-sensor:
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Ensure alarm is set to off to prevent premature activation:
  alarmActive = false;
  digitalWrite(actLedPin, LOW);
}

void loop()
{
  readForCard();
  runSensor();
}

/* !!!!!!!
void checkCardData(){
  
  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card
  
  Serial.print(F("Name: "));
  byte buffer1[18];
  block = 4;
  len = 18;

  //------------------------------------------- GET FIRST NAME
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
  for (uint8_t i = 0; i < 16; i++)
  {
    if (buffer1[i] != 32)
    {
      content1.concat(buffer1[i]);
      Serial.write(buffer1[i]);
    }
  }
  Serial.print("-");

  Serial.print(content1);

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
  for (uint8_t i = 0; i < 16; i++) {
    content2.concat(String(buffer2[i], HEX));
    Serial.write(buffer2[i] );
  }

  Serial.println(" ");
  //----------------------------------------

}

*/

void readForCard()
{
//!!!!  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  // Look for new cards:
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards:
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  
  /*checkCardData();
    
  // Get UID from card/tag:
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }



  // Check UID for match and set alarm:
  content.toUpperCase();
  // Change the UID below to the tag/card that you want to use for access
  // Use DumpInfo (File > Examples > MFRC522 > DumpInfo) to get this infomation
/*  Serial.print(content1);
  Serial.print("=");
  Serial.println(targetUID);  
  if (content1 == targetUID) */

  if (content.substring(1) == targetUID) 
  {
    if(alarmActive == true)
    {
      alarmActive = false;
      digitalWrite(actLedPin, LOW);
    }
    else
    {
      alarmActive = true;
      digitalWrite(actLedPin, HIGH);
    }
    // Set off a short tone and an LED to signal a sucessful activation/deactivation:
    tone(buzzPin, 500);
    delay(50);
    noTone(buzzPin);
    delay(50);
    tone(buzzPin, 500);
    delay(50);
    noTone(buzzPin);
  }
  else // Make a short tone to indicate that the alarm has failed to activate:
  {
    Serial.println("ACCESS DENIED - ERROR");
    tone(buzzPin, 300);
    delay(200);
    tone(buzzPin, 100);
    delay(200);
    noTone(buzzPin);
  }
/*  content1 = "";
  content2 = "";
 
  alarmActive = true;
   */
}

void runSensor()
{
  //alarmActive = true;
  
  // Reset alarm at initialization to avoid premature activation:
  noTone(buzzPin);
  digitalWrite(ledPin, LOW);
  
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

  // Print off distance data recorded by the sensor:
  printSensor(inches, cm);

  // Check if alarm has been triggered:
  checkAlarm(inches);

  // Delay for 50 milliseconds before looping over again
  // This ensures that the sensor is pulsing fast enough to detect fast-moving objects
  // but still slow enough for the RFID Reader to check if a tag/card has been presented:
  delay(50);
}


void printSensor(long inches, long cm)
{
  Serial.print(inches);
  Serial.print("in / ");
  Serial.print(cm);
  Serial.print("cm");
}



// Checks if the sensor has detected an object and triggers the alarm


void checkAlarm(long inches)
{
  if(inches < 36 &&  alarmActive == true) //!!!!
  {
    Serial.println(" TRIGGERED");
    alarm(600);
  }
  else if(alarmActive == false)
  {
    Serial.println(" DISARMED");
    alarm(600);
  }
  else
  {
    Serial.println(" ARMED");
  }
}


// Triggers the buzzer/alarm for a set amount of time


void alarm(int seconds)
{
  // Convert the pre-set amount of time into a repeated series of alarm beeps:
  int duration = seconds * 4;
  
  for(int i=0; i <= duration; i++)
  {
    // Check if a tag/card has been waived to deactivate the alarm:
    readForCard();
    
    if(alarmActive == true)
    {
      digitalWrite(ledPin, HIGH);
      tone(buzzPin, 1000);
      delay(150);
      digitalWrite(ledPin, LOW);
      noTone(buzzPin);
      delay(150);
    }
    else
    {
      i=0;
      break;
    }
  }
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