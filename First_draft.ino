#include <SD.h>
#include <SPI.h>
// #### Good to have a general description of what this code is suppose to do. 
const int chipSelect = BUILTIN_SDCARD; 

int EWP,RAD,ACC.GLV; 
bool record=true;
void setup()
{

  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect.
  }

  Serial.print("Initializing SD card...");
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
}

void loop()
{
  // make a string for assembling the data to log:
  String dataString = "";

        if (record == true) {
        Serial.println("button 7 pressed - data saved!");
        //Add timestamp
      //read the potentiometer's value. then use this value as the new value of dataString
          int EWP = analogRead(A0);
          int RAD = analogRead(A1);
          int ACC = analogRead(A2);
          int GLV = analogRead(A3);
          dataString = "EWP"+String(EWP)+"RAD"+String(RAD)+"ACC"+String(ACC)+"GLV"+String(GLV);


      // open the file named datalog.txt on the sd card
          File dataFile = SD.open("datalog.txt", FILE_WRITE);

          // if the file is available, write the contents of datastring to it
          if (dataFile) {
          dataFile.println(dataString);
          dataFile.close();
          }  
          // if the file isn't open, pop up an error:
          else {
          Serial.println("error opening datalog.txt");
        }   
  }
/*
//Open Stored data
        File dataFile = SD.open("datalog.txt");
        if(dataFile) {
          Serial.println("datalog:");
          while (dataFile.available()) {
              Serial.write(dataFile.read());
           }
      // close the file:
         dataFile.close();


//Delete the file so it can be created again at the begining of the loop
SD.remove("datalog.txt");
    
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening datalog.txt");
  }
  
}*/



  delay(150);
 }
