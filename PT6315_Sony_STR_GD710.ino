/****************************************************/
/* This is only one example of code structure       */
/* OFFCOURSE this code can be optimized, but        */
/* the idea is let it so simple to be easy catch    */
/* where we can do changes and look to the results  */
/****************************************************/
/*
Timer0	8-bit	Millis(), micros(), PWM on pins 5, 6
Timer1	16-bit	Servo library, PWM on pins 9, 10
Timer2	8-bit	Tone() function, PWM on pins 3, 11
*/
/*
Is important don't let active lines of debug, like Serial.println(...); 
because they will cause the stop of the system case output is not available!
*/
#define VFD_dataIn 7// 
#define VFD_clk 8 // 
#define VFD_stb 9 // 
#define alarm_ledGreen 10//
#define alarm_ledRed 12//
#define pinButton 11 // used to control cycle of search segments

#define ledAdc1 4  // This is the pin number 4 on Arduino UNO
#define ledAdc2 5 // This is the pin number 5 on Arduino UNO

#define AdjustPins    PIND // before is C, but I'm use port C to VFC Controle signals

// #define BUTTON_PIN 2 //Att check wich pins accept interrupts... Uno is 2 & 3, This is our trigger button
// volatile byte buttonReleased = false;

// variables will change:
int buttonState = 0;  // variable for reading the pushbutton status

/*Global Variables Declarations*/
bool flagSet = false;
bool flagReached = false;
byte myByte= 0x01;   // this variable is only related with swapLed1.
unsigned int numberOfGrids = 0b00000111;  //Here I define the global value to the number of digits used, is cmd1.
unsigned int numberSegments = 17; // The PT6315 allow 17 segments when we select the 11 grids, see table of data sheet!
//value: 0b00000111 is 11 grids and 17 segments to the VFD in this panel.

unsigned char hours = 0;
unsigned char minutes = 0;
unsigned char seconds = 0;
unsigned char milisec = 0;

unsigned char digit = 0;
unsigned char number = 0;

unsigned char numberA = 0;
unsigned char numberB = 0;
unsigned char numberC = 0;
unsigned char numberD = 0;
unsigned char numberE = 0;
unsigned char numberF = 0;

unsigned char wakeSecondsUnits=0;
unsigned char wakeSecondsDozens=0;
unsigned char wakeMinutesUnits=0;
unsigned char wakeMinutesDozens=0;
unsigned char wakeHoursUnits=0;
unsigned char wakeHoursDozens=0;

unsigned char wakeSeconds=0;
unsigned char wakeMinutes=0;
unsigned char wakeHours=0;
 
unsigned char grid;

byte Aa, Ab, Ac, Ad, Ae, Af, Ag, Ah;
byte blockBit =0x00;

boolean toggle0 = 0;
boolean toggle1 = 0;
boolean toggle2 = 0;
uint16_t valClk0 = 0;  //To delay the timer of 8 bits, implement a loop to increase by each event timer!
uint16_t valClk2 = 0;  //To delay the timer of 8 bits, implement a loop to increase by each event timer!
int val0 = 0;   // Used to read the ADCx port
int val1 = 0;   // Used to read the ADCx port

char strADC0[5]; //initializing a character of size 5 for showing the ADC result
char strADC1[5]; //initializing a character of size 5 for showing the ADC result

unsigned char msgHiFolks[30] ={
  //76543210....FEDCBC98.................
  0B00000000, 0B00000000, 0B00000000, //Empty to grid 0 registers
  0B00000000, 0B00000000, 0B00000000, //Empty to grid 1 registers
  0B11100001, 0B00100001, 0B00000000, //S 
  0B10101000, 0B00010010, 0B00000000, //K
  0B00100000, 0B00100010, 0B00000000, //L
  0B00110001, 0B00100011, 0B00000000, //O
  0B11100001, 0B00000010, 0B00000000, //F
  0b00000000, 0b00000000, 0b00000000, //spc//
  0B00000101, 0B00101000, 0B00000000, //I
  0B11110000, 0B00000011, 0B00000000  //H
  };
unsigned char numbers[30] = {
  //76543210....FEDCBC98.................
  0B00110001, 0B00100011, 0B00000000, //0
  0B00011000, 0B00000001, 0B00000000, //1
  0B01010001, 0B00100100, 0B00000000, //2
  0B11010001, 0B00100001, 0B00000000, //3
  0B11110000, 0B00000001, 0B00000000, //4
  0B10100001, 0B00110000, 0B00000000, //5
  0B11100001, 0B00100011, 0B00000000, //6
  0B00010001, 0B00000001, 0B00000000, //7
  0B11110001, 0B00100011, 0B00000000, //8
  0B11110001, 0B00000001, 0B00000000  //9
  };
unsigned char letters[78] = {
  //76543210....FEDCBC98.................
  0B11110001, 0B00000011, 0B00000000, //A
  0B10010101, 0B00101001, 0B00000000, //B
  0B00100001, 0B00100010, 0B00000000, //C
  0B00010101, 0B00101001, 0B00000000, //D
  0B11100001, 0B00100010, 0B00000000, //E
  0B11100001, 0B00000010, 0B00000000, //F
  0B01100001, 0B00100011, 0B00000000, //G
  0B11110000, 0B00000011, 0B00000000, //H
  0B00000101, 0B00101000, 0B00000000, //I
  0B00010000, 0B00100011, 0B00000000, //J
  0B10101000, 0B00010010, 0B00000000, //K
  0B00100000, 0B00100010, 0B00000000, //L
  0B00111010, 0B00000011, 0B00000000, //M
  0B00110010, 0B00010011, 0B00000000, //N
  0B00110001, 0B00100011, 0B00000000, //O
  0B11110001, 0B00000010, 0B00000000, //P
  0B00110001, 0B00110011, 0B00000000, //Q
  0B11110001, 0B00010000, 0B00000000, //R
  0B11100001, 0B00100001, 0B00000000, //S
  0B00000101, 0B00001000, 0B00000000, //T
  0B00110000, 0B00100011, 0B00000000, //U
  0B00101000, 0B00000110, 0B00000000, //V
  0B00001010, 0B00010100, 0B00000000, //X
  0B00001001, 0B00100100, 0B00000000, //Z
  0B00110000, 0B00001011, 0B00000000, //W
  0B10000000, 0B00000000, 0B00000000  //W
  };
/************************** Initialize of driver pt6315**************************************/
void pt6315_init(void){
  delayMicroseconds(250); //power_up delay
  // Note: Allways the first byte in the input data after the STB go to LOW is interpret as command!!!

  // Configure VFD display (grids)
  cmd_with_stb(numberOfGrids);//  (0b01000000)    cmd1 10 grids 18 segm in 6318 // (0b00001001) ten digits!
  delayMicroseconds(1);
  // turn vfd on, stop key scannig
   cmd_with_stb(0b10000000);//(BIN(01100110)); 
  delayMicroseconds(1);
  // Write to memory display, increment address, normal operation
  cmd_with_stb(0b01000000);//(BIN(01000000));
  delayMicroseconds(1);
  // Address 00H - 15H ( total of 11*2Bytes=176 Bits)
  cmd_with_stb(0b11000000);//(BIN(01100110)); 
  delayMicroseconds(1);
  // set DIMM/PWM to value
  cmd_with_stb((0b10001000) | 7);//0 min - 7 max  )(0b01010000)
  delayMicroseconds(1);
  }
/*********************** Send command without strobe/chip select*****************************/
void cmd_without_stb(unsigned char a){
  // send without stb
  unsigned char transmit = 7; //define our transmit pin
  unsigned char data = 170; //value to transmit, binary 10101010
  unsigned char mask = 1; //our bitmask
  
  data=a;
  //This don't send the strobe signal, to be used in burst data send
   for (mask = 00000001; mask>0; mask <<= 1) { //iterate through bit mask
     digitalWrite(VFD_clk, LOW);
     if (data & mask){ // if bitwise AND resolves to true
        digitalWrite(VFD_dataIn, HIGH);
     }
     else{ //if bitwise and resolves to false
       digitalWrite(VFD_dataIn, LOW);
     }
    delayMicroseconds(5);
    digitalWrite(VFD_clk, HIGH);
    delayMicroseconds(5);
   }
   //digitalWrite(VFD_clk, LOW);
  }
/********************** Send a command with the strobe/chip select **************************/
void cmd_with_stb(unsigned char a){
  // send with stb
  unsigned char data = 0x00; //value to transmit, binary 10101010
  unsigned char mask = 1; //our bitmask
  
  data=a;
  
  //This send the strobe signal
  //Note: The first byte input at in after the STB go LOW is interpreted as a command!!!
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(1);
    for (mask = 0b00000001; mask>0; mask <<= 1) { //iterate through bit mask
      digitalWrite(VFD_clk, LOW);
      delayMicroseconds(1);
        if (data & mask){ // if bitwise AND resolves to true
            digitalWrite(VFD_dataIn, HIGH);
        }
        else{ //if bitwise and resolves to false
          digitalWrite(VFD_dataIn, LOW);
        }
    digitalWrite(VFD_clk, HIGH);
    delayMicroseconds(1);
   }
   digitalWrite(VFD_stb, HIGH);
   delayMicroseconds(1);
  }
/*********************** Run a test to VFD, let it all bright********************************/
void showNumbers(void){
    for(uint8_t i = 0; i < 30; i=i+3){
      cmd_with_stb(0B01000000); // cmd 2
      digitalWrite(VFD_stb, LOW);
      delayMicroseconds(4);
      cmd_with_stb((0b11000000) | 0x03); //cmd 3 Each grid use 3 bytes here define the 1º, 2º, 3º until max address

      cmd_without_stb(numbers[i]); // data
      cmd_without_stb(numbers[i+1]); // data
      cmd_without_stb(numbers[i+2]); // data
            
      digitalWrite(VFD_stb, HIGH);
      delayMicroseconds(4);

      cmd_with_stb(numberOfGrids); // cmd1 Here I define the 8 grids and 20 Segments
      cmd_with_stb((0B10001000) | 7); //cmd 4
      delayMicroseconds(4);
      delay(500);
    }
  }
void test_VFD(void){
  clear_VFD();   
      digitalWrite(VFD_stb, LOW);
      delayMicroseconds(1);
      cmd_with_stb(numberOfGrids); // Set the number of Grids and Segments
      cmd_with_stb(0b01000000); // cmd 2 //Write VFD, Normal operation; Set pulse as 1/16, Auto increment
      digitalWrite(VFD_stb, LOW);
      delayMicroseconds(1);
      cmd_without_stb((0b11000000)); //cmd 3 wich define the start address (00H to 23H)
        // Only here must change the bit to test, first 2 bytes and 1/2 byte of third.
         for (int i = 0; i < 11 ; i++){ // test base to 16 segm and 5 grids
          // Zone of test, if write 1 on any position of 3 bytes below position, will bright segment corresponding it.
         cmd_without_stb(0b11111111); // Data to fill table 5*16 = 80 bits
         cmd_without_stb(0b11111111); // Data to fill table 5*16 = 80 bits
         cmd_without_stb(0b11111111); // Data to fill table 5*16 = 80 bits
         }
       digitalWrite(VFD_stb, HIGH);  
      cmd_with_stb((0b10001000) | 7); //cmd 4  //set on, dimmer to max
      digitalWrite(VFD_stb, LOW);
      delay(1); 
  }
void dimming(unsigned int dimmer){
    digitalWrite(VFD_stb, LOW);
    delayMicroseconds(20);
    cmd_without_stb((0b10001000) | dimmer); // This switch On the VFD and define the Dimming Quantity Settings 
    delayMicroseconds(20);
    digitalWrite(VFD_stb, HIGH);
    delayMicroseconds(20);
  }
/*********************** Clear of VFD display ******************************************/
void clear_VFD(void){
  /*
  Here I clean all registers 
  Could be done only on the number of grid
  to be more fast. The 12 * 3 bytes = 36 registers
  */
      for (int n=0; n < 36; n++){ //  // Note: The char is constituted by 2&1/2 bytes, by this reason count 3 bytes by char!!!
        cmd_with_stb(numberOfGrids); //cmd 1 // Set number of Grids & Segments
        cmd_with_stb(0b01000000); // cmd 2 //Normal operation; Set pulse as 1/16
        digitalWrite(VFD_stb, LOW);
        delayMicroseconds(1);
            cmd_without_stb((0b11000000) | n); // cmd 3 //wich define the start address (00H to 15H)
            cmd_without_stb(0b00000000); // Data to fill table of 5 grids * 16 segm = 80 bits on the table
            cmd_without_stb(0b00000000); // Data to fill table of 5 grids * 16 segm = 80 bits on the table
            cmd_without_stb(0b00000000); // only half byte of third byte.
            //
            digitalWrite(VFD_stb, HIGH);
            cmd_with_stb((0b10001000) | 7); //cmd 4
            digitalWrite(VFD_stb, LOW);
            delayMicroseconds(1);
     }
  }
/*********************** Assigne values to variable of clock********************************/
void send_update_clock(void){
  if (seconds >=60){
    seconds =0;
    minutes++;
  }
  if (minutes >=60){
    minutes =0;
    hours++;
  }
  if (hours >=24){
    hours =0;
  }
    //-----------------------Seconds-------------------------------------
    number = (seconds%10);
    addressRegisters(number);
    numberA=number;
    number = (seconds/10);
    addressRegisters(number);
    numberB=number;
    //-----------------------Minutes-------------------------------------
    number = (minutes%10);
    addressRegisters(number);
    numberC=number;
    number =  (minutes/10);
    addressRegisters(number);
    numberD=number;
    //-----------------------Hours-------------------------------------
    number = (hours%10);
    addressRegisters(number);
    numberE=number;
    number = (hours/10);
    addressRegisters(number);
    numberF=number;
    //-----------------------Show digits-------------------------------
    showTime();
    //-----------------------------------------------------------------
    //The next lines of print is only to be used as debug help, uncomment if necessary!
    // Serial.print("A: "); Serial.println(numberA, HEX);
    // Serial.print("B: "); Serial.println(numberB, HEX);
    // Serial.print("C: "); Serial.println(numberC, HEX);
    // Serial.print("D: "); Serial.println(numberD, HEX);
    // Serial.print("E: "); Serial.println(numberE, HEX);
    // Serial.print("F: "); Serial.println(numberF, HEX);
    //
  }
/********************** Assigne values to Alarm variables***********************************/
void send_update_alarm(void){
  if (wakeSeconds >= 60){
    wakeSeconds =0;
    wakeMinutes++;
  }
  if (wakeMinutes >= 60){
    wakeMinutes =0;
    wakeHours++;
  }
  if (wakeHours >= 24){
    wakeHours =0;
  }
    //------------------------------------------------------------
    number = (wakeSeconds%10);
    addressRegisters(number);
    wakeSecondsUnits=number;
    number = (wakeSeconds/10);
    addressRegisters(number);
    wakeSecondsDozens=number;
    //------------------------------------------------------------
    number = (wakeMinutes%10);
    addressRegisters(number);
    wakeMinutesUnits=number;
    number = (wakeMinutes/10);
    addressRegisters(number);
    wakeMinutesDozens=number;
    //------------------------------------------------------------
    number = (wakeHours%10);
    addressRegisters(number);
    wakeHoursUnits=number;
    number = (wakeHours/10);
    addressRegisters(number);
    wakeHoursDozens=number;
    //------------------------------------------------------------
     showWakeUp();
  }
/************************* Show values presents on variables of Clock **********************/
void showTime(){
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(10);
      cmd_with_stb(numberOfGrids); // cmd 1 // 8 Grids 
      cmd_with_stb(0b01000000); // cmd 2 //Normal operation; Set pulse as 1/16
      
        digitalWrite(VFD_stb, LOW);
        delayMicroseconds(10);
        cmd_without_stb((0b11000000) | 0x00); //cmd 3 wich define the start address (00H to 15H)
          //Grid 0
          cmd_without_stb(0x00);//
          cmd_without_stb(0x00);//
          cmd_without_stb(0x00);// 
          //Grid 1
          cmd_without_stb(0x00);//
          cmd_without_stb(0x00);//
          cmd_without_stb(0x00);// 
          // Grid 2
          cmd_without_stb(numbers[numberA]);// seconds unit //------------0
          cmd_without_stb(numbers[numberA+1]);// seconds unit //------------1 
          cmd_without_stb(0x00);//--------------------------2
          // Grid 3
          cmd_without_stb(numbers[numberB]); //----------------------------3
          cmd_without_stb(numbers[numberB+1]);// seconds dozens  //---------4
          cmd_without_stb(0x00);//--------------------------5
          // Grid 4
          cmd_without_stb(0xC0);//
          cmd_without_stb(0x00);//
          cmd_without_stb(0x00);//
          // Grid 5
          cmd_without_stb(numbers[numberC]); // Minuts dozens //----------6
          cmd_without_stb(numbers[numberC+1]); // Minuts unit //------------7
          cmd_without_stb(0x00);//--------------------------8
          // Grid 6
          cmd_without_stb(numbers[numberD]); // Hours dozens //-----------A
          cmd_without_stb(numbers[numberD+1]); // Hours unit //-------------9
          cmd_without_stb(0x00);//
          // Grid 7
          cmd_without_stb(0xC0);//
          cmd_without_stb(0x00);//
          cmd_without_stb(0x00);// 
          // Grid 8
          cmd_without_stb(numbers[numberE]); // Minuts dozens //----------6
          cmd_without_stb(numbers[numberE+1]); // Minuts unit //------------7
          cmd_without_stb(0x00);//--------------------------8
          // Grid 9
          cmd_without_stb(numbers[numberF]); // Hours dozens //-----------A
          cmd_without_stb(numbers[numberF+1]); // Hours unit //-------------9
          cmd_without_stb(0x00);//
           // Grid 10, only to keep it clean!
          cmd_without_stb(0x00);//
          cmd_without_stb(0x00);//
          cmd_without_stb(0x00);// 
          // Grid number 6 is hwere is the wheel
          //
      digitalWrite(VFD_stb, HIGH);
      delayMicroseconds(10);
      cmd_with_stb((0b10001000) | 7); //cmd 4
      delay(5);   
  }
/*************************** Show values presents on the Wake up Set variables **************/
void showWakeUp(){
  digitalWrite(VFD_stb, LOW);
  delayMicroseconds(10);
      cmd_with_stb(0b00001000); // cmd 1 // 8 Grids 
      cmd_with_stb(0b01000000); // cmd 2 //Normal operation; Set pulse as 1/16
      
        digitalWrite(VFD_stb, LOW);
        delayMicroseconds(10);
        cmd_without_stb((0b11000000) | 0x00); //cmd 3 wich define the start address (00H to 15H)
          //
          cmd_without_stb(0x00);//
          cmd_without_stb(0x00);//
          cmd_without_stb(0x00);// 
          //
          cmd_without_stb(0x00);//
          cmd_without_stb(0x80);//
          cmd_without_stb(0x00);// 
          // Grid 2
          cmd_without_stb(numbers[wakeSecondsUnits]);// seconds unit //--------1
          cmd_without_stb(numbers[wakeSecondsUnits+1]);// seconds unit //--------1
          cmd_without_stb(0x00);//-------------------------------2
          // Grid 3
          cmd_without_stb(numbers[wakeSecondsDozens]);// //---------------------------------3
          cmd_without_stb(numbers[wakeSecondsDozens+1]);// seconds dozens //-----4
          cmd_without_stb(0x00);//-------------------------------5
          // Grid 4
          cmd_without_stb(0xC0);//
          cmd_without_stb(0x00);//
          cmd_without_stb(0x00);//
          // Grid 5
          cmd_without_stb(numbers[wakeMinutesUnits]); // Minuts unit //--------7
          cmd_without_stb(numbers[wakeMinutesUnits+1]); // Minuts unit //--------7
          cmd_without_stb(0x00);//-------------------------------8
          // Grid 6
          cmd_without_stb(numbers[wakeMinutesDozens]); // Minuts dozens //-----6
          cmd_without_stb(numbers[wakeMinutesDozens+1]); // Minuts dozens //-----6
          cmd_without_stb(0x00);//
          // Grid 7
          cmd_without_stb(0xC0);//
          cmd_without_stb(0x00);//
          cmd_without_stb(0x00);//
          // Grid 8
          cmd_without_stb(numbers[wakeHoursUnits]); // Hours unit //-----------9
          cmd_without_stb(numbers[wakeHoursUnits+1]); // Hours unit //-----------9
          cmd_without_stb(0x00);//
          // Grid 9
          cmd_without_stb(numbers[wakeHoursDozens]); // Hours dozens //--------A
          cmd_without_stb(numbers[wakeHoursDozens+1]); // Hours dozens //--------A
          cmd_without_stb(0x00);//
          // Grid 10
          cmd_without_stb(0x00);//
          cmd_without_stb(0x00);//
          cmd_without_stb(0x00);// 
          //
      digitalWrite(VFD_stb, HIGH);
      delayMicroseconds(10);
      cmd_with_stb((0b10001000) | 7); //cmd 4
      delay(5);
       
  }
/********************** To adapt the 7 seg if use VFD with different 7 seg digits ***********/
void addressRegisters( unsigned char digit){
    switch(digit){
      case 0:   number=0x00;     break; 
      case 1:   number=0x03;     break;
      case 2:   number=0x06;     break;
      case 3:   number=0x09;     break;
      case 4:   number=0x0C;     break;
      case 5:   number=0x0F;     break;
      case 6:   number=0x12;     break;
      case 7:   number=0x15;     break;
      case 8:   number=0x18;     break;
      case 9:   number=0x1B;     break;
    }
  } 
/*********************** When use buttons supporte by pins of Arduino uno *****************/ 
void send14segmNum(unsigned int num){
      cmd_with_stb(numberOfGrids); // cmd 1 // 10 Grids & 18 Segments
      cmd_with_stb(0b01000000); // cmd 2 //Normal operation; Set pulse as 1/16
        //
        digitalWrite(VFD_stb, LOW);
        delayMicroseconds(1);
        cmd_without_stb((0b11000000) | (num)); //cmd 3 wich define the start address (00H)
        //
          cmd_without_stb(numbers[num]); // 
          cmd_without_stb(numbers[num+1]); // minuts units
          cmd_without_stb(numbers[num+2]); // minuts dozens
      digitalWrite(VFD_stb, HIGH);
      cmd_with_stb((0b10001000) | 7); //cmd 4
      delay(1);
      delay(100);  
  }
void send14segmAlpha(unsigned int num){
      cmd_with_stb(numberOfGrids); // cmd 1 // 10 Grids & 18 Segments
      cmd_with_stb(0b01000000); // cmd 2 //Normal operation; Set pulse as 1/16
        //
        digitalWrite(VFD_stb, LOW);
        delayMicroseconds(1);
        cmd_without_stb((0b11000000) | (num)); //cmd 3 wich define the start address (00H)
        //
          cmd_without_stb(letters[num]); // 
          cmd_without_stb(letters[num+1]); // minuts units
          cmd_without_stb(letters[num+2]); // minuts dozens
      digitalWrite(VFD_stb, HIGH);
      cmd_with_stb((0b10001000) | 7); //cmd 4
      delay(1);
      delay(100);  
  }
void send14HiFolks(unsigned int num){
      cmd_with_stb(numberOfGrids); // cmd 1 // 10 Grids & 18 Segments
      cmd_with_stb(0b01000000); // cmd 2 //Normal operation; Set pulse as 1/16
        //
        digitalWrite(VFD_stb, LOW);
        delayMicroseconds(1);
        cmd_without_stb((0b11000000) | (num)); //cmd 3 wich define the start address (00H)
        //
          cmd_without_stb(msgHiFolks[num]); // 
          cmd_without_stb(msgHiFolks[num+1]); // minuts units
          cmd_without_stb(msgHiFolks[num+2]); // minuts dozens
      digitalWrite(VFD_stb, HIGH);
      cmd_with_stb((0b10001000) | 7); //cmd 4
      delay(1);
      delay(100);  
  }
/********************* Zone of read BUS keys Port AD0 & AD1 analog ************************/
void actionKeyBus0(void){
    if((val0 > 0) & (val0 < 70)){
           wakeMinutes--;
           }
    if((val0 > 75) & (val0 < 160)){
          wakeMinutes++;
           }
    if((val0 > 165) & (val0 < 220)){  
          wakeHours--;
           }
    if((val0 > 250) & (val0 < 350)){
           wakeHours++;
           }
  }
void actionKeyBus1(void){
      if((val1 > 0) & (val1 < 70)){
           hours=0;
           minutes=0;
           seconds=0;
           }
      if((val1 > 75) & (val1 < 150)){
          flagReached = false; //To active the clock wake-up!
          digitalWrite(alarm_ledGreen, LOW); //This led is port of Arduino
          digitalWrite(alarm_ledRed, HIGH); //This led is port of Arduino
        }
      if((val1 > 180) & (val1 < 250)){  
          digitalWrite(5, !digitalRead(5));
              if(flagSet == false)
                flagSet=true;
                else
                flagSet=false;
              }
      if((val1 > 280) & (val1 < 370)){
          //Put here your action to this key!
           }
      if((val1 > 380) & (val1 < 490)){
          minutes--;
           }
      if((val1 > 500) & (val1 < 645)){
          minutes++;         
          }
      if((val1 > 650) & (val1 < 750)){
          hours--;         
          }
      if((val1 > 770) & (val1 < 890)){
          hours++;         
          }
  }
void testADC0(void){
                String valADC0 = String(analogRead(A0)); //intailizing a string and storing ADC value in it
                valADC0.toCharArray(strADC0, 5); // convert the reading to a char array
                //Serial.println(valADC.toInt());
                val0=(valADC0.toInt());
                actionKeyBus0();
                //Uncomment this line to debug and see value present on the ADCx port!
                //Serial.print("Port A0: ");Serial.println(val0);                           
  }
void testADC1(void){
                String valADC1 = String(analogRead(A1)); //intailizing a string and storing ADC value in it
                valADC1.toCharArray(strADC1, 5); // convert the reading to a char array
                //Serial.println(valADC.toInt());
                val1=(valADC1.toInt());
                actionKeyBus1();
                //Uncomment this line to debug and see value present on the ADCx port! 
                //Serial.print("Port A1: "); Serial.println(val1);                      
  } 
/********************** Compare between WakeUp and Clock ****************************************/
void comparTime(){
     if (( hours == wakeHours) && (minutes == wakeMinutes) && (seconds == wakeSeconds)){
        flagReached = true;
        digitalWrite(alarm_ledGreen, HIGH); //This led is port of Arduino
        digitalWrite(alarm_ledRed, LOW); //This led is port of Arduino
     }
     //Uncomment next lines if is neccessary in debug process!
      // Serial.print(hours, DEC); Serial.print(" "); Serial.println(wakeHours, DEC); 
      // Serial.print(minutes, DEC); Serial.print(" "); Serial.println(wakeMinutes, DEC);
      // Serial.print(seconds, DEC); Serial.print(" "); Serial.println(wakeSeconds, DEC);      
  }
/************************* Swap the led Function ***************************************/
void swapLeds(){
    digitalWrite(VFD_stb, LOW);
    delayMicroseconds(20);
    cmd_without_stb(0b01000001); // This is to define write to LED's
    delayMicroseconds(20);
    myByte ^=(0b00011001);  //Here I do invertion of bit of LED 0,3 & 4, repeat this function if you want use other of 5 remaining leds. //3 led meio
    cmd_without_stb(myByte);
    delayMicroseconds(20);
    digitalWrite(VFD_stb, HIGH);
    delayMicroseconds(20);
  }
void onOff(bool on){
    digitalWrite(VFD_stb, LOW);
    delayMicroseconds(20);
    if (!on){
    cmd_without_stb(0b10000000); // This swithc Off display!!!
    delayMicroseconds(20);
    }else{
      cmd_without_stb(0b10001111); // This swithc On the VFD and define the Dimming Quantity Settings 
    delayMicroseconds(20);
    }
    digitalWrite(VFD_stb, HIGH);
    delayMicroseconds(20);
  }
/************************ Setup Zone & Search Segments ******************************************/
void searchSegments(){
  uint8_t segmentNumber = 0x00;
  long bitArray = 0x00000001; //The long allow 32bits, we will use 24bits
  uint8_t grid = 0x00;
  uint8_t wordA = 0x00;
  uint8_t wordB = 0x00;
  uint8_t wordC = 0x00;
  
  for(uint8_t digit = 0; digit < 36; digit = digit + 3){ //Each digit use 3 positions of memory registers!
    bitArray = 0x00000001;
      for (uint8_t  p = 0; p < numberSegments ; p++){ // Do test to segm and grids
            wordA = (bitArray & 0x0000FF) >> 0;
            wordB = (bitArray & 0x00FF00) >> 8;
            wordC = (bitArray & 0xFF0000) >> 16;
          //
      //Serial.println( p, DEC);
      Serial.print("A: ");
      Serial.println(wordA, BIN);
      Serial.print("B: ");
      Serial.println(wordB, BIN);
      Serial.print("C: ");
      Serial.println(wordC, BIN);
      Serial.print("seg number: ");
      Serial.println(segmentNumber, DEC);
      segmentNumber++;
      //Serial.println(bitArray, BIN);
      bitArray =  (bitArray <<= 1);
      viewSegments(wordA, wordB, wordC, digit);
        while(!digitalRead(pinButton) == LOW){  //This cycle while is to do the debounce of button! Also need add a capacitor of 100nF
          delay(200);
        }
        delay(300);
      }
    } 
  }
void viewSegments(uint8_t wA, uint8_t wB, uint8_t wC, uint8_t grid){
      clear_VFD();
      digitalWrite(VFD_stb, LOW);
      delayMicroseconds(1);
      //cmd_with_stb(0b00000111); // cmd 1 //  Grids & Segments
      cmd_with_stb(0b01000000); // cmd 2 //Write VFD, Normal operation; Set pulse as 1/16, Auto increment
      digitalWrite(VFD_stb, LOW);
      delayMicroseconds(1);
      cmd_without_stb(0b11000000 | grid); //cmd 3 wich define the start address (00H to 23H)
        // Zone of test, if write 1 on any position of 3 bytes below position, will bright segment corresponding it.
          cmd_without_stb(wA); // Data to fill table 
          cmd_without_stb(wB); // Data to fill table 
          cmd_without_stb(wC); // Data to fill table 
      digitalWrite(VFD_stb, HIGH);
      delay(1);
      cmd_with_stb(0b10001000 | 0x07); // cmd 2 //set on, dimmer to max
      digitalWrite(VFD_stb, LOW);
  }
void beginAnimations(){
  bool VFDon = true;
  unsigned int arrayPosition=0;
  //You can comment to avoid the test running.
  //searchSegments();
   clear_VFD();
  
        for(uint8_t x = 0; x < 4; x++){
          test_VFD();
          delay(1000);
          clear_VFD();
          delay(500);
        }
        clear_VFD();
        for (int n=2; n < 10; n++){  // I write only on the digits of 14 segments: 2 to 9
         arrayPosition=n*3;
         send14HiFolks(arrayPosition);
         delay(200);
        }
       for (int n=2; n < 10; n++){  // Write number by forward sequency
          arrayPosition=n*3;
          send14segmNum(arrayPosition);
          delay(100);
          }
         clear_VFD();
        for (int n=9; n >= 2; n--){ // Write the number's by backward sequency
          arrayPosition=n*3;
          send14segmNum(arrayPosition);
          delay(100);
        }
        for (int n=7; n >= 0; n--){  // Adjusting the bright of VFD 
          dimming(n);
          delay(750);
        }
        clear_VFD();
        for (int n=2; n < 10; n++){  // I write only on the digits of 14 segments: 2 to 9
         arrayPosition=n*3;
         send14segmAlpha(arrayPosition);
         delay(200);
        }
        for (int i=0; i<6; i++){    // Switch on/off of VFD
          onOff(VFDon);
          delay(500);
          VFDon=!VFDon;
          delay(500);
        }
  }
void setup() {
  // put your setup code here, to run once:
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(VFD_dataIn, OUTPUT);
  pinMode(VFD_clk, OUTPUT);
  pinMode(VFD_stb, OUTPUT);
  pinMode(pinButton, INPUT_PULLUP);
  pinMode(alarm_ledGreen, OUTPUT);//
  pinMode(alarm_ledRed, OUTPUT);//
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);

  //
  Serial.begin(115200);
  seconds = 0x00;
  minutes =0x00;
  hours = 0x00;
  digitalWrite(alarm_ledGreen, HIGH); // To set the begin status to LED green o alarm!

  pt6315_init();
  test_VFD();
  clear_VFD();
  //This line show initial messages on VFD!
  beginAnimations();
  /*
  THe configurations registers to timer 0, 1 and 2.

  OCR0B Timer/Counter0 Output Compare Register B
  OCR0A Timer/Counter0 Output Compare Register A
  TCNT0  -->  Timer/Counter0 (8-bit)
  TCCR0B   --> FOC0A   FOC0B   –      –    WGM02 CS02  CS01    CS00
  TCCR0A   --> COM0A1 COM0A0 COM0B1 COM0B0   –    –   WGM01   WGM00

  OCR1BH --> Timer/Counter1 - Output Compare Register B High Byte 
  OCR1BL --> Timer/Counter1 - Output Compare Register B Low Byte 
  OCR1AH --> Timer/Counter1 - Output Compare Register A High Byte 
  OCR1AL --> Timer/Counter1 - Output Compare Register A Low Byte 
  ICR1H --> Timer/Counter1 - Input Capture Register High Byte 
  ICR1L --> Timer/Counter1 - Input Capture Register Low Byte 
  TCNT1H --> Timer/Counter1 - Counter Register High Byte 
  TCNT1L  --> Timer/Counter1 - Counter Register Low Byte 
  Reserved -->  –        –   –     –       –     –     –     –
  TCCR1C  --> FOC1A   FOC1B  –     –       –     –     –     – 
  TCCR1B  --> ICNC1   ICES1  –      WGM13  WGM12 CS12  CS11   CS10 
  TCCR1A  --> COM1A1 COM1A0 COM1B1 COM1B0    –    –    WGM11  WGM10

  OCR2B    -->   Timer/Counter2 Output Compare Register B 
  OCR2A    -->   Timer/Counter2 Output Compare Register A 
  TCNT2    -->   Timer/Counter2 (8-bit) 
  TCCR2B   -->   FOC2A   FOC2B   –      –    WGM22   CS22    CS21     CS20 
  TCCR2A   -->   COM2A1 COM2A0 COM2B1 COM2B0   –       –    WGM21     WGM20  
  */

  cli();            // disable all interrupts
  //noInterrupts(); // disable all interrupts other way!
  //
  /******************** Setup timer 0*******************************/
    //initialize timer0. Timer0 is a timer of 8 bits!!!
    TCCR0A = 0;
    TCCR0B = 0;// This initialisations is very important, to have sure the trigger take place!!!
    TCNT0  = 0;//I can apply here one value to start from there!
    // 
    OCR0A = 150;// = (16*10^6) / (8000*8) - 1 (must be <256)
    //OCR0A = 127; //Change to low value to accelarate the clock to debug!  // compare match register 16MHz/256/2Hz
    TCCR0A |= ((1 << WGM01) | (0 << WGM00));  // CTC mode
    TCCR0B |= (0 << WGM02);
    TCCR0B |= ((0 << CS02) | (1 << CS01) | (1 << CS00));    // 64 prescaler !!! Note the prescaler table are different on Timer0 & Timer2!!!
    TIMSK0 |= (1 << OCIE0A);  // enable timer compare interrupt
    /***********************End setup timer 0 *************************/
    /******************************************************************/
    //
   /******************************************************************/
  /******************* Setup to timer 1 ****************************/
      //initialize timer1, Timer1 is a timer of 16 bits!!!
      TCCR1A = 0;
      TCCR1B = 0;// This initialisations is very important, to have sure the trigger take place!!!
      TCNT1  = 0;// I can apply here one value to start from there!
      // Use 62499 to generate a cycle of 1 sex 2 X 0.5 seconds (16MHz / (2*256*(1+62449) = 0.5
      //ATT: Depending of the arduino you use, maybe is necessary redo this calcul. Check your Xtall
      OCR1A = 62499;            // compare match register 16MHz/256/2Hz
      //OCR1A = 1000; //Change to low value to accelarate the clock to debug!  // compare match register 16MHz/256/2Hz
      TCCR1B |= (1 << WGM12);   // CTC mode
      TCCR1B |= ((1 << CS12) | (0 << CS11) | (0 << CS10));    // 256 prescaler 
      TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
      //***************** End setup timer 1 ****************************/
      //
  /******************** Setup timer 2*******************************/
    //initialize timer2. Timer2 is a timer of 8 bits!!!
    TCCR2A = 0;
    TCCR2B = 0;// This initialisations is very important, to have sure the trigger take place!!!
    TCNT2  = 0;
    // 
    OCR2A = 185;// = (16*10^6) / (8000*8) - 1 (must be <256)
    //OCR2A = 127; //Change to low value to accelarate the clock to debug!  // compare match register 16MHz/256/2Hz
    TCCR2A |= ((1 << WGM21) | (0 << WGM20));   // CTC mode
    TCCR2B |= (0 << WGM22);
    TCCR2B |= ((1<< CS22) | (0 << CS21) | (0 << CS20));    // 64 prescaler !!! Note the prescaler table are different on Timer0 & Timer2!!!
    TIMSK2 |= (1 << OCIE2A);  // enable timer compare interrupt
    /***********************End setup timer 2 *************************/
  //
  // Note: this counts is done to a Arduino 1 with Atmega 328... Is possible you need adjust
  // a little the value 62499 upper or lower if the clock have a delay or advnce on hours.
  // CLKPR=(0x80);   //9.12.2 CLKPR – Clock Prescale Register
  
  //only here I active the enable of interrupts to allow run the test of VFD
  //interrupts();             // enable all interrupts
  sei();
  //Repeat the show of initial messages on VFD but now fast!
  beginAnimations();
  }
/********************** Loop Zone *********************************/
void loop() {
        if(!flagSet){
            send_update_clock(); // show the values of clock on VFD
            delay(333);
        }
          else{
              send_update_alarm(); // show the values of clock on VFD
              delay(333);
          }
    comparTime(); //Used to do the verification of Alarme timer!
  }
/******************** Interrupt Zone ******************************/
/******************** ISR of TIMER 1 ******************************/
ISR(TIMER1_COMPA_vect)   {  //This is the interrupt request of Timer1, 16 Bits!
      seconds++;
  }
/******************** ISR of TIMER 0 ******************************/ 
ISR(TIMER0_COMPA_vect)   {  //This is the interrupt request of Timer0, 8 Bits!
  //Important: Note I used a increment of a variable to delay the cycle of read ports ADC
  //I do this because the timer is based on 8 bits, allow count until 255, but this time is 
  //to fast to the remaining cycles! Of course we can change the value present on the register 
  //of Pre-Scaler!
 valClk0++;
  if(valClk0 > 512){
    valClk0 = 0;
      if (toggle0){
        digitalWrite(6, HIGH);
        toggle0 = 0;
      }
      else{
        digitalWrite(6, LOW);
        toggle0 = 1;
        testADC0();
        //testADC1();
      }
  }   
  } 
/******************** ISR of TIMER 2 ******************************/ 
ISR(TIMER2_COMPA_vect)   {  //This is the interrupt request of Timer2, 8 Bits!
  //Important: Note I used a increment of a variable to delay the cycle of read ports ADC
  //I do this because the timer is based on 8 bits, allow count until 255, but this time is 
  //to fast to the remaining cycles! Of course we can change the value present on the register 
  //of Pre-Scaler!
  valClk2++;
  if(valClk2 > 512){
    valClk2 = 0;
      if (toggle2){
        digitalWrite(4,HIGH);
        toggle2 = 0;
      }
      else{
        digitalWrite(4,LOW);
        toggle2 = 1;
        //testADC0();
        testADC1();
      }
  }   
  } 
