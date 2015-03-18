/*
 * This file runs the main Pain Box Logic and control routines.
 *  Portions of this file are taken from the WAVEHC example that is intended 
 *  for use with the Wave Shield.  Other portions May be subject to different 
 *  license terms than the original file.  Please see each portions license 
 *  agreement for more information.
 *  
 *  Otherwise:
 *  This code is free software: you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This Code is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this Code.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 *  IMPORTANT NOTES!!!!!!!
 *  You must have the Wave Shield libraries installed, and the Wave Shield wired
 *  differently than the instructions for the example; 
 *  - Pin2->Pin6 
 *  - Pin3->Pin2.  
 *  Changes must be made in the WavePinDefs.h file to reflect this wiring change:
 *  DAC Chip Select must be set to Arduino Pin6
 *  DAC Serial Clock must be set to Arduino Pin2
 *  This frees up Arduino Pin3 for high speed hardware PWM for the HV PSU on TIMER2
 */
 
#include <WaveHC.h>
#include <WaveUtil.h>
#include <avr/pgmspace.h>
#include <NewPing.h>

/*
 * Define the IO pin numbers here
 */

//PSU Control
#define psuPWMPin          3     // Power Suppy PWM output - cant be changed!
#define psuPWMGate         A0    // Power Supply PWM Gate output
#define psuConnect         A1    // Connect Relay output
#define v12Enable          A2    // 12V PSU enable (AMP and HV PSU)


//Touch/Person Sensor control
#define tchHandOn          9     // Hand Sensor Input
#define tchPersonEcho      8     // Person Sensor Input
#define tchPersonTrig      7     // Person Sensor Trigger 
#define tchHandIllum       A3    // Hand Sensor Illumination Control

#define ON                 1
#define OFF                0

#define HandCheckCount     10
#define TauntDistCM        40

#define ItchDuration       70
#define BurnDuration       150
#define BurnBurnDuration   300

#define ItchPSUSetting     10
#define BurnPSUSetting     40
#define PainPSUSetting     80

static WaveHC     wave;

/*
 *  SD Card and FAT related variables 
 */
SdReader   card;        // This object holds the information for the card
FatVolume  vol;         // This holds the information for the partition on the card
FatReader  root;        // This holds the information for the volumes root directory
uint8_t    dirLevel;    // indent level for file/dir names    (for prettyprinting)
dir_t      dirBuf;      // buffer for directory reads

/*
 *  Pain Box Control and Logic Variables
 */

uint8_t    psuDuty;          // Duty Cycle variable for HV PSU
bool       handOn;           // Bool to keep track of Hand State
bool       Person;           // Bool to keep track of Person Presence 
bool       psuActive;        // Bool to keep track of PSU State
bool       VEnabled;         // Bool to keep track of 12V state
bool       SoundEnabled;     // Bool to keep track of sound enabled state 

/*
 *  Ping sensor support variables
 */
NewPing PeopleSensor(tchPersonTrig, tchPersonEcho, 200);

/* 
 *  This enum is used to keep track of the Box's State.
 */
enum PainBoxModeEnum_t {
  pbModeAttract = 0,        // The Box is in attract Mode "Come Here..."
  pbModeTaunt,              // The Box is in taunt Mode "Put your hand in box..."
  pbModeStart,              // The Box begins a pain sequence
  pbModeRun,                // A Pain sequence is active
  pbModeFail,               // Test Subject failed (Removed hand)
  pbModeWin                 // Test Subject passed Test "And how can this be...?"
} PainBoxMode, OldpbMode;

/*
 * Define macros to put error messages in flash memory
 */
#define error(msg)         error_P(PSTR(msg))
#define error_fatal(msg)   error_fatal_P(PSTR(msg))
/*
 * Define Macro to put informational messages in flash memory
 */
#define info(msg)          info_P(PSTR(msg))


/*
 *  Wave File name List Array - by index
 *    Use this section to customize the Wav file 
 *    Sounds played for different events/States
 *
 *    Please Note that all files should be in the root
 *    Directory, or they will not be found!
 */
prog_char  sndAttract1[] PROGMEM = "ComeHere";         //
prog_char  sndAttract2[] PROGMEM = "YouCome";          //
prog_char  sndTaunt   [] PROGMEM = "PutHand";          //
prog_char  sndStart1  [] PROGMEM = "GomJabar";         // 
prog_char  sndStart2  [] PROGMEM = "YoullDie";         // 
prog_char  sndPain1   [] PROGMEM = "Itching";          //
prog_char  sndPain2   [] PROGMEM = "ItchBurn";         //
prog_char  sndPain3   [] PROGMEM = "HeatHeat";         //
prog_char  sndDuring1 [] PROGMEM = "TheOne";           //
prog_char  sndDuring2 [] PROGMEM = "FMantra1";         //
prog_char  sndDuring3 [] PROGMEM = "FMantra2";         //
prog_char  sndFail1   [] PROGMEM = "KillYou";          //
prog_char  sndFail2   [] PROGMEM = "Animals";          //
prog_char  sndEnd1    [] PROGMEM = "Enough";           //
prog_char  sndEnd2    [] PROGMEM = "ThatMuch";         //   
prog_char  sndEnd3    [] PROGMEM = "HandOut";          //
prog_char  sndVictory [] PROGMEM = "Victory";          //

//Create pointer array for file names
PROGMEM const char* FileName[] = {
  sndAttract1,    //0  - Played randomly in attract mode
  sndAttract2,    //1  - Played randomly in attract mode
  sndTaunt,       //2  - Played when a person is detected in front of box
  sndStart1,      //3  - Played as soon as user put hand in and touches electrodes
  sndStart2,      //4  - Played just before sequence begins
  sndPain1,       //5  - Played just as sequence starts 
  sndPain2,       //6  - Played when sequence begins to ramp up 
  sndPain3,       //7  - Played as sequence reaches maximum
  sndDuring1,     //8  - Played during the sequence randomly
  sndDuring2,     //9  - Played during the sequence randomly
  sndDuring3,     //10 - Played during the sequence randomly
  sndFail1,       //11 - Played randomly if they give up
  sndFail2,       //12 - Played randomly if they give up
  sndEnd1,        //13 - Played if they make it to end 
  sndEnd2,        //14 - Played after previous - end sequence
  sndEnd3,        //15 - Played after previous - end sequence
  sndVictory      //16 - Played after winner removes hand
};

//////////////////////////////////// SETUP
void setup() {
  Serial.begin(9600);               // set up Serial library at 9600 bps for debugging
  info("Pain Box v1.0");  // say we woke up!
  Serial.println();
  //Assume Sound will be enabled
  SoundEnabled = true;
  //Set up state variables 
  handOn = false;
  psuActive = false;
  //Set initial Mode to attract
  PainBoxMode = pbModeAttract;
    
  if (!card.init()) {                   // play with 8 MHz spi (default faster!)  
    error("Could Not init SD Card!");   // Something went wrong, lets print out why
    SoundEnabled = false;               // Disable Sound - need SD card for samples
  }
  
  // enable optimize read - some cards may timeout. Disable if you're having problems
  card.partialBlockRead(true);
  
  // Now we will look for a FAT partition!
  uint8_t part;
  for (part = 0; part < 5; part++) {   // we have up to 5 slots to look in
    if (vol.init(card, part)) 
      break;                           // we found one, lets bail
  }
  if (part == 5) {                           // if we ended up not finding one  :(
    error("No valid FAT partition found!");  // Something went wrong, lets print out why
    SoundEnabled = false;                    // Disable Sound 
  }
  
  // Lets tell the user about what we found
  info("Using partition ");
  Serial.print(part, DEC);
  putstring(", type is FAT");
  Serial.println(vol.fatType(), DEC);     // FAT16 or FAT32?
  
  // Try to open the root directory
  if (!root.openRoot(vol)) {
    error("Can't open root dir!");      // Something went wrong,
    SoundEnabled = false;                    //Disable Sound 
  }
  
  // Whew! We got past the tough parts.
  info("Wave Files found...\r\n");

  // Print out all of the files in all the directories.
  // root.ls(LS_R | LS_FLAG_FRAGMENTED);
  
  //Set up I/O pins
  //Outputs
  pinMode(psuPWMPin, OUTPUT);      // HV supply PWM Output
  pinMode(psuPWMGate, OUTPUT);     // HV supply PWM gate Output
  pinMode(psuConnect, OUTPUT);     // HV supply Connect Relay Output
  pinMode(tchPersonTrig, OUTPUT);  // Person Sensor test Trigger
  pinMode(v12Enable, OUTPUT);      // 12V PSU enable (AMP and HV PSU ON)
  pinMode(tchHandIllum, OUTPUT);   // Hand Sensor Illumination Control
    
  //Inputs
  pinMode(tchHandOn, INPUT);           // Hand On Sensor Input H=ON, L=OFF
  pinMode(tchPersonEcho, INPUT);       // Person  Sensor Input H=Person, L=None
  
  //Set up Timer 2 for Fast PWM
  TCCR2A = _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS21);   //Use CS20 for faster PWM
  OCR2B = 0;            //Set Zero Duty Cycle
  
  //Initialize random generator
  randomSeed(analogRead(A5));
  
  //We are ready to start Main
  info("Setup Complete...\n");  
}

/*
 pbModeAttract = 0,        // The Box is in attract Mode "Come Here..."
  pbModeTaunt,              // The Box is in taunt Mode "Put your hand in box..."
  pbModeStart,              // The Box begins a pain sequence
  pbModeRun,                // A Pain sequence is active
  pbModeFail,               // Test Subject failed (Removed hand)
  pbModeWin                 // Test Subject passed Test "And how can this be...?"

*/

//////////////////////////////////// LOOP
//This loop runs the Pain Box Logic and control

void loop() {

  uint32_t waitValue, CurValue, MaxDelay, MinDelay;
  uint8_t TestStage, TestStageAnnounce, PSUIncrement;
  uint32_t TestDuration;
  bool Timeout = true;
  uint8_t var, var1, var2;
  
  //Turn on 12v To amplifier and HV PSU
  digitalWrite(v12Enable, HIGH);
  
  delay(5000);
    
  while (1){
   
    switch(PainBoxMode){
      
      //Attempt to attract people
      case (pbModeAttract):
        MaxDelay = 5000;
        MinDelay = 6000;
        //Play one of the two Attract sound files
        if( (Timeout) && (!wave.isplaying) ){
          var = random(10);
          if (var >  5) PlaySound(0);
          if (var <= 5) PlaySound(1);
          Timeout = false;
        }
        
        if( Person ){
          PainBoxMode = pbModeTaunt;
          Timeout = true;
        }
      break;
      
      //Taunt the detected person
      case (pbModeTaunt):
       //Make sure there is a person, and that we can play the sound
       if( Timeout && Person ){
          if( !wave.isplaying ){
             PlaySound(2);
             Timeout = false;
          }
        }
        //Check for hand
        if( handOn  ) {
          PainBoxMode = pbModeStart;
          Timeout = true;
          var = 3;
        }
        //Check for person
        if( !Person ) {
          PainBoxMode = pbModeAttract;
          Timeout = true;
        }
      break;
      
      //Start the 'Test'
      case (pbModeStart):
        //Verify that a person is present, a hand is in contact
        //   with the electrodes
        if( Person && handOn ){
          if(Timeout && (!wave.isplaying) ){
            if (var == 3) PlaySound(var);
            if (var == 4) PlaySound(var);
              var++;
            if (var == 5) Timeout = false;
          }
        }
        
        //Did person keep hand inside?
        if(var == 5){
          //wait for sound to stop
          while(wave.isplaying);
          //Next Mode
          PainBoxMode = pbModeRun;
          TestStage = TestDuration = 0;
          Timeout = false;
        }
      
        if(!Person || ! handOn) {
          PainBoxMode = pbModeTaunt;    
          Timeout = false;
        }
      break;
      //Run the test sequence
      case (pbModeRun):
        MaxDelay = 1000;
        MinDelay = 2000;
        //Did Person move or was hand removed?  If So FAIL!
        if(!handOn || !Person) {
          PainBoxMode = pbModeFail;
          Timeout = true;
          info("Test failed!\r\n");
          hvStop();
        }
        //Play background sounds
        if(Timeout && (!wave.isplaying) ){
          var = random(10 * TestStage);
          if( (var > 5) && (var < 10) )   PlaySound(8);
          if( (var >= 20) && (var < 30) ) PlaySound(9);
          if( (var >= 50) && (var < 55) ) PlaySound(10);
          if( (var >= 55) && (var < 60) ) PlaySound(7);
          Timeout = false;
        }
        //announce teste stages (itching, burning, more burning)
        if(TestStageAnnounce && (!wave.isplaying) ){
          if(TestStageAnnounce == 1) PlaySound(5);
          if(TestStageAnnounce == 2) PlaySound(6);
          if(TestStageAnnounce == 3) PlaySound(7);
          //Test stage has been announced
          TestStageAnnounce = 0;
        }
        //What part of test is running...?
        switch (TestStage){
          //Start PSU and connect
          case 0:
            //announce 'itching'
            TestStageAnnounce = 1;
            //start the HV PSU
            hvStart();
            setPSU(ItchPSUSetting);
            psuDuty = ItchPSUSetting; 
            //Go to next stage
            TestStage++;
            TestDuration = 0;
            info("Starting Test...\r\n");
          break;
          //Test endurance at low level signal
          case 1:
            if(TestDuration > ItchDuration) TestStage++;
          break;
          //Adjust PSU to next level
          case 2:
            //announce 'Itching to Burning'
            TestStageAnnounce = 2;
            setPSU(BurnPSUSetting);
            psuDuty = BurnPSUSetting;
            //Go to next stage
            TestStage++;
            TestDuration = 0;
            info("Itching to Burning...\r\n");
          break;
          //Test duration at this level
          case 3:
            if(TestDuration > BurnDuration) TestStage++;
          break;
          //Adjust PSU to next Level
          case 4:
            //Announce Burn on Burn
            TestStageAnnounce = 3;
            setPSU(PainPSUSetting);
            psuDuty = PainPSUSetting;
            //Go to next stage
            TestStage ++;
            TestDuration = 0;
            info("Look out...!\r\n");
          break;
          //Test endurance at this higher level
          case 5:
            if(TestDuration > BurnBurnDuration) TestStage++;
            //SLowly Ramp PSU up even more as time goes on
            if( PSUIncrement++ > 10) {
              if(psuDuty < 200) psuDuty = psuDuty + 5;
              setPSU(psuDuty);
              PSUIncrement = 0;
              info("MORE...!\r\n");
            }
          break;
          //Set Victory state
          case 6:
            //Wait for wave to stop
            while(wave.isplaying);
            //Subject made it through - set victory
            PainBoxMode = pbModeWin;
            Timeout = true;
            var = 1;
            //Shut off PSU
            hvStop();
            info("Test Complete!\r\n");
            TestStage ++;
          break;
        }
        
        //Increment Duration Counter
        TestDuration++;
      break;
      //Subject failed - let them know they suck
      case (pbModeFail):
        //The test subject FAILED!
        //If a sound was playing, wait for it to stop
        while (wave.isplaying);       
        //Play one of the fail sounds
        if( Timeout && (!wave.isplaying) ){
          var = random(10);
          if(var  > 5) PlaySound(11);
          if(var <= 5) PlaySound(12);
          Timeout = false;
        }
        //Go back to taunt mode (person is likely standing in front of box still)
        PainBoxMode = pbModeTaunt;
      break;
      //Subject passed the test
      case (pbModeWin):
        if( Timeout && (!wave.isplaying) ){
           if( var == 1) PlaySound(13);   //enough
           if( var == 2) PlaySound(14);   //No child withstood
           if( var == 3) PlaySound(15);   //Hand Out
           if( var == 4) Timeout = false;
           var++;
        }
        
        if(!handOn){
          if(!wave.isplaying) PlaySound(16); //How can this be
          //Wait for it!
          while(wave.isplaying);
          PainBoxMode = pbModeAttract;
        }
      
      break;
    }
    
    checkHand();
    checkPerson();
      
    //Generate a new random timeout value
    CurValue = CurValue + 105;
    if( (!Timeout) && (CurValue > waitValue) ){
      waitValue = random(MaxDelay) + MinDelay;
      if(PainBoxMode != pbModeWin) Timeout = true;
      CurValue = 0;
    }   
    
    delay(1);
  }
}
/////////////////////////////////// HELPERS
/*
 * print error message and halt if SD I/O error, great for debugging!
 */
void sdErrorCheck(void) {
  if (!card.errorCode()) return;
  PgmPrint("\r\nSD I/O error: ");
  Serial.print(card.errorCode(), HEX);
  PgmPrint(", ");
  Serial.println(card.errorData(), HEX);
  while(1);
}
/*
 * print error message and continue
 */
void error_P(const char *str) {
  PgmPrint("Error: ");
  SerialPrint_P(str);
  sdErrorCheck();
  Serial.println();
  //We will keep executing if no SD error
}
/*
 * print error message and halt (fatal error)
 */
void error_Fatal_P(const char *str) {
  PgmPrint("Fatal Error: ");
  SerialPrint_P(str);
  sdErrorCheck();
  while(1);
} 
/*
 * Display an informational message 
 */
void info_P(const char *str){
  SerialPrint_P(str);
}

/*
 * Check the hand sensor
 */
bool checkHand(void){
   uint8_t cycleCount = 0;
   uint8_t testValue = 0;
   
   //First, turn on the hand sensor illumination
   digitalWrite(tchHandIllum, HIGH);
   //Let LEDS come on...  
   delay(5);
   //Check the sensor input for a few cycles 
   //  to see what is going on
   for(cycleCount=0; cycleCount < 20; cycleCount ++)
   {
     //If sensor input is high, hand is there
     if (digitalRead(tchHandOn) == HIGH) 
       testValue++;
     else
       if(testValue) testValue--;
     delay(5);    
   }
   //disable the illumination
   digitalWrite(tchHandIllum, LOW);

   //return hand sense state
   if (testValue > HandCheckCount){
     handOn = true;
     return true;
   }
   else{
     handOn = false;
     return false;
   }
}

/* 
 * Set the PSU Level
 */
 void setPSU(uint8_t Level){
    
   if(Level > 200) Level = 200;
   if(Level < 10) Level = 10;
   OCR2B = Level;
 }
 
/*
 *  Control the 12V power
 */
 void set12V(uint8_t state){
   if(state)
   {
     digitalWrite(v12Enable, HIGH);
     VEnabled = true;
   }
   else
   {
     digitalWrite(v12Enable, LOW);
     VEnabled = false;
   }
 }
 
/*
 *  Start the HV and connect it to the electrodes
 */
 void hvStart(void){
   //Start the 12 Power (if not on already)
   set12V(ON);
   delay(5);
   //Set the min PWM
   setPSU(10);
   //Gate the PWM
   digitalWrite(psuPWMGate, HIGH);
   delay(5);
   //Connect the PSU output to the Electrodes
   digitalWrite(psuConnect, HIGH);
   //set Variable to indicate state
   psuActive = true;
 }
/*
 * Stop the HV
 */
void hvStop(void){
   //disconnect the PSU from the electrodes
   digitalWrite(psuConnect, LOW);
   //Stop the PWM
   digitalWrite(psuPWMGate, LOW);
   delay(5);
   //Set MIN PWM
   setPSU(10);
   //We dont turn off 12v because something else may be using it...
   //CLear State of PSU
   psuActive = false;
}
/*
  *  ChekcPerson checks for a person in front of the box
  *  using the ping or HC-SR04 sensor
  */
bool checkPerson(void){
  
  uint8_t cycleCount;
  unsigned int SensorTimeUS = 0;   //Holder for people sensor(ping) measurement
  unsigned int PersonDist = 0;
  
  SensorTimeUS += PeopleSensor.ping();
  
  PersonDist = (SensorTimeUS / US_ROUNDTRIP_CM );
  
  if((PersonDist > 0) && (PersonDist < TauntDistCM)){
    
    //Serial.println("Person Detected... ");
    Person = true; 
    return true;
  }
  else{
    //Serial.println("Person not Detected... ");
    Person = false;
    return false;
  }
}
 
/*
 * Get a wave file name from Program memory
 *  Wave file name array to buffer 
 */
uint8_t getSndName( uint8_t index, char bufName[] ){
  uint8_t len;
  //copy name from program memory
  strcpy_P(bufName, (char*)pgm_read_word(&(FileName[index])));
  //How long is file name?
  len = strlen(bufName);
  //Append the ".wav" to the name
  strcpy_P(&bufName[len], PSTR(".wav"));
  //Return the length of the file name
  return (len+4);
}

/*
 * Open and Play a wave file by name
 */
int OpenSound( char name[]){
  static FatReader  file;
  
  //Check if sound is enabled before even trying 
  if(SoundEnabled) {
    //Wait for any currently playing wave to stop
    if(wave.isplaying) {
      info("A Sound is still playing...");
      Serial.println();
      return (-1);
    }
    else{
      //Open the requested file
      //Serial.print("Opening ");
      //Serial.println(name);
     
      if(!file.open(root, name) ){
        error("Could Not Open file...");
      }
      else{
        //Opened file, Try to play sound
        if (!wave.create(file)) {            // Figure out, is it a WAV proper?
          error("Invalid WAV file!");
        } 
        else{
          //Start playing file
          wave.play();                       // make some noise!
          info("Playing Sound...");
          Serial.println(name);
        }
      }
      //Check if SD card is OK before exiting
      sdErrorCheck();
      return (0);      
    }
  }
  else{
     error("Sound is disabled!"); 
     return (-2);
  }
}

/* 
 * Play a sound by index
 */
 
int PlaySound( uint8_t index){
   char NameBuf[16];          //Wave File Name Buffer

   if( getSndName( index, NameBuf) > 0 )
   {  
     return OpenSound(NameBuf);
   }
   else
   {
     info("Failed to Load Sound Name...");
     Serial.println();
     return (-1);
   }
 }
