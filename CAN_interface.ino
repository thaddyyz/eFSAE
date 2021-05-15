// Uses Library
// https://github.com/tonton81
// C:\Program Files (x86)\Arduino\hardware\teensy\avr\libraries\FlexCAN_T4
//
// include FlexCAN_T4FD if you want CANFD
//

#include <FlexCAN_T4.h>
FlexCAN_T4<CAN1, RX_SIZE_16, TX_SIZE_16> Can1; //orig RX_SIZE_256 TX_SIZE_64

//                               1         2         3         4            5
//                          012345678901234567890123456789012345678901234567 8 9 0
const char CAN_String_Init[64] = "      0.000 1 000 Rx d 8 00 00 00 00 00 00 00 00\r\n\0";
char CAN_String_Temp[64] = "      0.000 1 000 Rx d 8 00 00 00 00 00 00 00 00\r\n\0";

char CAN_String_Len = 50;

unsigned int CAN_Buffer_Cnt = 0; //Current chars in buffer
byte Engine_Run_Active = 0;
byte Last_Engine_Run_Active = 0;
byte Cnt_0xC9 = 0;  //Throttle back the 12ms msgs to 100ms
byte Cnt_0x135 = 0;
byte Cnt_0x137 = 0;
byte Cnt_0x18E = 0;
byte Cnt_0x191 = 0;
byte hrs_valid = 0;
byte min_valid = 0;
byte sec_valid = 0;

union Msg_0x135
{
  unsigned long ul[2];
  unsigned int  ui[4];
  unsigned char ub[8];

} Msg_0x135_Data;

union Msg_0x135_Last
{
  unsigned long ul[2];
  unsigned int  ui[4];
  unsigned char ub[8];

} Msg_0x135_Data_Last;

union Msg_0x137
{
  unsigned long ul[2];
  unsigned int  ui[4];
  unsigned char ub[8];

} Msg_0x137_Data;

union Msg_0x137_Last
{
  unsigned long ul[2];
  unsigned int  ui[4];
  unsigned char ub[8];

} Msg_0x137_Data_Last;

// 
// 
#include "TeensyTimerTool.h"

using namespace TeensyTimerTool;

Timer t1; // generate a timer from the pool (Pool: 2xGPT, 16xTMR(QUAD), 20xTCK)
void T1_Callback()
{
    digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));    
}

unsigned int year = 2020;
byte month = 7;     
byte day = 25;
byte hour = 20;
byte minute = 15;
byte second = 9;

#include <SD.h>
#include <SPI.h>

File myFile;
char fn[12] = "test0.asc";
char File_Opened = 0;
    
void setup(void) {
  Serial.begin(115200); //delay(400);
  pinMode(6, OUTPUT); digitalWrite(6, LOW); /* optional tranceiver enable pin */

  //  Unions need to be initialized inside a routing not where defined!
  Msg_0x135_Data_Last.ul[0] = 0;
  Msg_0x135_Data_Last.ul[1] = 0;

  Msg_0x137_Data_Last.ul[0] = 0;
  Msg_0x137_Data_Last.ul[1] = 0;
  
  Can1.begin();
  Can1.setBaudRate(500000);
  Can1.setMaxMB(16);
  Can1.enableFIFO();
  Can1.enableFIFOInterrupt();
  Can1.onReceive(canSniff);
  Can1.mailboxStatus();
  Can1.enableMBInterrupts();
  pinMode(13, OUTPUT);
  
// Initialize CAN_Buffer with default string until real CAN msg comes in
  for ( uint8_t i = 0; i < 50; i++ ) {
   CAN_String_Temp[i] = CAN_String_Init[i];
  } 

//Setup Periodic Timer 
  pinMode(LED_BUILTIN,OUTPUT);  
  t1.beginPeriodic(T1_Callback, 100'000); // 100ms 

// Set time date stamp.
  SdFile::dateTimeCallback(dateTime);

  Serial.print(F("Initializing SD card..."));

  if (!SD.begin(BUILTIN_SDCARD)) {
    Serial.println(F("initialization failed!"));
    return;
  }
  Serial.println(F("initialization done."));
  
  while ((SD.exists(fn)) && (((fn[4] <= 0x39) || (fn[4] < 0x5A)))) { // 0x39 = "9"
    fn[4]++;
    if (fn[4] == 0x3A){
      fn[4] = 0x41; //"A"
    }
    else if (fn[4] >= 0x5A){
      fn[4] = 0x5A; //"Z"
    }
  }
byte hrs_valid = 0;
byte min_valid = 0;
byte sec_valid = 0;

  // Wait here a few seconds to see if a valid time date stamp exists then move on
  while ( (millis() < 10000) && (hrs_valid == 0 || min_valid == 0 || sec_valid == 0))
  
  // open the file. 
  myFile = SD.open(fn, FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (myFile) {
    File_Opened = 1;
    // Write header but make sure it isn't interrupted
    noInterrupts();
    myFile.println(F("date Thu Jun 25 03:47:03 pm 2020"));
    myFile.println(F("base hex  timestamps absolute"));
    myFile.println(F("internal events logged"));
    myFile.println(F("// version 8.2.0"));
    myFile.println(F("Begin Triggerblock Thu Jun 25 03:47:03 pm 2020"));
    interrupts();
    
    Serial.print(F("Writing to testx.asc..."));
    Serial.println(F("done."));
  } else {
    // if the file didn't open, print an error:
    Serial.println(F("error opening testx.txt"));
  }  
}

/******************************************************************************
Function Name  : dec2hexr
Engineer    : raw 
Date      : 07/25/2008
Parameters    : s[] starts at the end
Returns     :  
Notes     :   convert unsigned int to hex (reverse)
******************************************************************************/

void dec2hexr(unsigned int n, char s[])
{
  do {
     *s = n % 16 + '0';
     if (*s > '9') {
       *s+=7;
     }
     s--;     
  } while ((n /= 16) > 0);
}

/******************************************************************************
Function Name : uitoa
Engineer    : raw 
Date        : 07/25/2008
Parameters  : value to convert, string
Returns     : length of string
Notes       : convert unsigned int to string
******************************************************************************/

unsigned int uitoa(unsigned int n, char s[])
{
  unsigned int i = 0;
  do {
     ++i;
     *s-- = n % 10 + '0';
  } while ((n /= 10) > 0);

  return(i);
}

/******************************************************************************
Function Name : ultoa
Engineer    : raw 
Date        : 07/25/2008
Parameters  : value to convert, string
Returns     : length of string
Notes       : convert unsigned int to string
******************************************************************************/

unsigned int ultoa(unsigned long n, char s[])
{
  unsigned int i = 0; 
  do {
     ++i;
     *s-- = n % 10 + '0';
  } while ((n /= 10) > 0);

  return(i);
}

//Callback routine for the timedate stamp of the file
void dateTime(uint16_t* date, uint16_t* time)
{
  *date = FAT_DATE(year, month, day);
  *time = FAT_TIME(hour, minute, second);
}

//Routine to mark time of the recorded can data
void LoadTime () {

  unsigned long RawTimeVal;
  unsigned long TimeValInt;
  unsigned long TimeValRem;
  
  RawTimeVal = millis();
  TimeValInt = RawTimeVal/1000;
  TimeValRem = RawTimeVal - (TimeValInt*1000);
  
//            1         2         3         4            5
//  012345678901234567890123456789012345678901234567 8 9 0
// "      0.000 1 000 Rx d 8 00 00 00 00 00 00 00 00\r\n\0";
  uitoa(TimeValRem, &CAN_String_Temp[10]);
  //note: TimeValInt is a long but divided by 1000 max length of 7
  ultoa(TimeValInt, &CAN_String_Temp[6]);
 }

boolean RecCANID (const CAN_message_t msg) {

  boolean record = 0; 

  switch (msg.id) {

    case 0xC9:
      // Engine Run Active MSB byte 0 ie. 0x80 12 ms
      if (Cnt_0xC9 == 0) {
        record = 1;
      }

      if (((msg.buf[0] & 0x80) == 0x00) && (Last_Engine_Run_Active == 0x80)) {
        myFile.println(F("End TriggerBlock"));
        File_Opened = 0;
        
        myFile.close();
        Serial.println("done.");
      }
      Last_Engine_Run_Active =  msg.buf[0] & 0x80;

      Cnt_0xC9++;
      Cnt_0xC9 &= 0x07;
              
      break;
    
    case 0x135:
      // Wiper, ddmmyr,secs 100ms
      
      Msg_0x135_Data.ub[0] = msg.buf[0];
      Msg_0x135_Data.ub[1] = msg.buf[1];
      Msg_0x135_Data.ub[2] = msg.buf[2];
      Msg_0x135_Data.ub[3] = msg.buf[3];
      Msg_0x135_Data.ub[4] = msg.buf[4];
      Msg_0x135_Data.ub[5] = msg.buf[5];
      Msg_0x135_Data.ub[6] = msg.buf[6];
      Msg_0x135_Data.ub[7] = msg.buf[7];

      if (Msg_0x135_Data.ul[0] != Msg_0x135_Data_Last.ul[0] || 
          Msg_0x135_Data.ul[1] != Msg_0x135_Data_Last.ul[1] ||
          Cnt_0x135 >= 10) {
            
        record = 1;
        Cnt_0x135 = 0;
        day = Msg_0x135_Data.ub[2] & 0x1F;
        month = Msg_0x135_Data.ub[4] & 0x0F;
        year = Msg_0x135_Data.ub[5]; + 2000;
        if ((Msg_0x135_Data.ub[7] & 0x40) == 0) {
          second = Msg_0x135_Data.ub[7] & 0x3F;
          sec_valid = 1;
        } else {
          sec_valid = 0;
        }
      }
      
      Msg_0x135_Data_Last.ul[0] = Msg_0x135_Data.ul[0];
      Msg_0x135_Data_Last.ul[1] = Msg_0x135_Data.ul[1];
      Cnt_0x135++;      
      break;
      
    case 0x137:
      // Minutes, heading, elev, hours, pdop 100ms
      Msg_0x137_Data.ub[0] = msg.buf[0];
      Msg_0x137_Data.ub[1] = msg.buf[1];
      Msg_0x137_Data.ub[2] = msg.buf[2];
      Msg_0x137_Data.ub[3] = msg.buf[3];
      Msg_0x137_Data.ub[4] = msg.buf[4];
      Msg_0x137_Data.ub[5] = msg.buf[5];
      Msg_0x137_Data.ub[6] = msg.buf[6];
      Msg_0x137_Data.ub[7] = msg.buf[7];

      if (Msg_0x137_Data.ul[0] != Msg_0x137_Data_Last.ul[0] || 
          Msg_0x137_Data.ul[1] != Msg_0x137_Data_Last.ul[1] ||
          Cnt_0x137 >= 10) {
            
        record = 1;
        Cnt_0x137 = 0;

        if ((Msg_0x137_Data.ub[1] & 0x80) == 0) {
          minute = Msg_0x137_Data.ub[0] & 0x3F;
          min_valid = 1;
        } else{
          min_valid = 0;
        }

        if ((Msg_0x137_Data.ub[1] & 0x40) == 0) {
          hrs_valid = 1;
          hour = ((Msg_0x137_Data.ub[6] & 0x7C) >> 2); // -4 = EST
        } else {
          hrs_valid = 0;
        }
      }
      
      Msg_0x137_Data_Last.ul[0] = Msg_0x137_Data.ul[0];
      Msg_0x137_Data_Last.ul[1] = Msg_0x137_Data.ul[1];
      Cnt_0x137++;      
      break;
      
    case 0x17D:
      // Veh accel 100ms
      record = 1;
      break;

   case 0x18E:
      // Engine torque 12.5ms
      if (Cnt_0x18E == 0) {
        record = 1;
      }

      Cnt_0x18E++;
      Cnt_0x18E &= 0x07;
      break;

   case 0x191:
      // Engine torque 12.5ms
      if (Cnt_0x191 == 0) {
        record = 1;
      }

      Cnt_0x191++;
      Cnt_0x191 &= 0x07;
      break;

    case 0x1ED:
      // Fuel flow, A/F 12.5ms on 2006 Impala but not 2016 Equinox
      record = 1;
      break;

   case 0x32A:
      // GPS 100ms
      record = 1;
      break;

   case 0x3D1:
      // Fuel Flow, TPS, DFCO, A/C 100ms
      record = 1;
      break;

   case 0x3E9:
      // Vspd 100ms
      record = 1;
      break;

   case 0x3FB:
      // Fuel Alcohol % 100ms
      record = 1;
      break;
           
    default:
      // nothing matches
      record = 0;
    break;
   }  

   return(record);
}

//CAN interrupt Callback function
void canSniff(const CAN_message_t &msg) { 

//            1         2         3         4           5
//  012345678901234567890123456789012345678901234567 8 90 
// "      0.000 1 000 Rx d 8 00 00 00 00 00 00 00 00\r\n";
  if ( RecCANID (msg)){
      // Initialize next CAN_Buffer with default string then overwrite with real CAN msg 
    for ( uint8_t i = 0; i < 50; i++ ) {
        CAN_String_Temp[i] = CAN_String_Init[i];
    } 
    
    LoadTime ();

    dec2hexr(msg.id, &CAN_String_Temp[16]);
    uitoa(msg.len,   &CAN_String_Temp[23]);
    dec2hexr(msg.buf[0], &CAN_String_Temp[26]);
    dec2hexr(msg.buf[1], &CAN_String_Temp[29]);
    dec2hexr(msg.buf[2], &CAN_String_Temp[32]);
    dec2hexr(msg.buf[3], &CAN_String_Temp[35]);
    dec2hexr(msg.buf[4], &CAN_String_Temp[38]);
    dec2hexr(msg.buf[5], &CAN_String_Temp[41]);
    dec2hexr(msg.buf[6], &CAN_String_Temp[44]);
    dec2hexr(msg.buf[7], &CAN_String_Temp[47]);

    for ( uint8_t i = 0; i < 50; i++ ) {
      Serial.print(CAN_String_Temp[i]);
    }

    if (File_Opened > 0) {
      myFile.print(&CAN_String_Temp[0]);
    }
  }
  
}

void loop() {
}
