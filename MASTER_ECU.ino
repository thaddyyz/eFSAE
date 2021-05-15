//Rear ECU
//Monitor rear sensors (Datalogging)
//Monitor 
//Define Variables
//#define pcbC A1//PCB fan current (analog if dirrect connection)
//Current to all PCB under 
// Only need to monitor if there is an error
#define IMD A2//Insulation monitoring (analog)
#define APPS1 A3//Insulation monitoring (analog)
#define APPS2 A4//Insulation monitoring (analog)
#define BPPS1 A5//Insulation monitoring (analog)
#define BPPS2 A6//Insulation monitoring (analog)
#define RFE D1//BAMOCAR (digital)
#define RUN D2//BAMOCAR (digital)
#define TSD D3//Tractive system (digital)
#define RTD D4//Ready to drive (digital)
#define CAN3RX 30//CAN bus Rx
#define CAN3TX 31//CAN bus Tx 
int pcdCval;
void setup() {
  // put your setup code here, to run once:
//Serial.begin(9600);//Add if monitoring
pinMode();
}

void loop() {
  
  while (charging==false){//Operationalloop
    pcdCval=analogRead(pcbC);
    if(pcdCval>565)// monitor current for PCBs FANs
    {
      //Warning message
    }
    if(pcdCval>565)// monitor current for PCBs FANs
    {
      //Warning message
    }
    // assign RFE&RUN(BAMOCAR)
    // resistance IMD (Insulation Monitoring) LOW resistance = short
    // SENSOR CHECK receive values from all sensors incl APPS, BPPS display on driver display
    // interrupt for tractive system
  }
  //torque response curve
  //torque profile saved
  //launch control
  //failure check for APPS (always running)
}
