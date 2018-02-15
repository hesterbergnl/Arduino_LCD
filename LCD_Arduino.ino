
int keyData0 = 10;
int keyData1 = 11;
int keyData2 = 12;
int keyData3 = 13;

int LCD_DB4 = 4;
int LCD_DB5 = 5;
int LCD_DB6 = 6;
int LCD_DB7 = 7;

int LCD_RS = 8;
int LCD_RW = 9;
int LCD_E = 1;
const int interruptPin = 2;

//constants to move to specific head of line
byte line1 = 0x80;
byte line2 = 0xC0;
byte line3 = 0x94;
byte line4 = 0xD4;

//byte mask = B10000000;
byte lookup[] = {0x31,0x32,0x33,0x00,0x34,0x35,0x36,0x00,0x37,0x38,0x39,0x00,0x2A,0x30,0x23};
volatile char character;
volatile char characterstorage;

//constant system instructions
byte configure = 0x28;
byte entryMode = 0x06;

//constant cursur control instructions
byte offCur = 0x0C;
byte lineCur = 0x0E;
byte blinkCur = 0x0D;
byte combnCur = 0x0F;
byte homeCur = 0x02;
byte shLfCur = 0x10;
byte shRfCur = 0x14;

//constant Display control instructions
byte clrDsp = 0x01;
byte offDsp = 0x08;
byte onDsp = 0x0C;
byte shLfDsp = 0x18;
byte shRtDsp = 0x1C;

//function declarations
//void reset(void);
//void wrLCD4(void);
//void pulseEwait4(void);
//void LCDoutstr(char stringin[]);
//void setitup(void);
//byte getloc(byte loc);
int flag = 0;
int waste = 0;
char line = 1;
char output[] = {0x4E,0x69,0x6B};

void setup() {
  pinMode(LCD_DB4,OUTPUT);
  pinMode(LCD_DB5,OUTPUT);
  pinMode(LCD_DB6,OUTPUT);
  pinMode(LCD_DB7,OUTPUT);
  pinMode(LCD_RS,OUTPUT);
  pinMode(LCD_RW,OUTPUT);
  pinMode(LCD_E,OUTPUT);
  pinMode(keyData3,INPUT);
  pinMode(keyData2,INPUT);
  pinMode(keyData1,INPUT);
  pinMode(keyData0,INPUT);
  pinMode(interruptPin,INPUT_PULLUP); //define interrupt pin
  attachInterrupt(digitalPinToInterrupt(interruptPin),keypress,FALLING);
  reset();
  LCDoutstr("        Dean");
  changeloc(line2);   //line 2
  LCDoutstr("       is the");
  changeloc(line3);  //line 3
  LCDoutstr("      coolest");
}

void loop() {
  // put your main code here, to run repeatedly:
}

void keypress(){
  //reads in portb (8-13) and isolates the data from the keypad
  characterstorage = lookup[(0x3C & PINB) >> 2];  
  //calls function to output the character
  doit();
}


void doit(void){
  //this function moves the cursur to the appropriate location and outputs character
  char location;

  if(flag == 0){  //on first keypress, do setup to clear screen
    setitup();
    flag = 1;
  }

  location = getloc(); //stores location of cursur
  if(characterstorage == 0x23){
    //if cursur is on line 1 and # is pressed, move to line 2
    if((location <= 0x13 && location >= 0x00) || line == 1){ 
      location = line2;
      line = 2;
      changeloc(location);
    }
    else if((location <= 0x53 && location >= 0x40) || line == 2){
      location = line3;
      line = 3;
      changeloc(location);
    }
    else if((location <= 0x27 && location >= 0x14) || line == 3){
      location = line4;
      line = 4;
      changeloc(location);
    }
    else if((location <= 0x67 && location >= 0x54) || line == 4){
      location = line1;
      line = 1;
      changeloc(location);
    }
  }
  //if cursur is on first character of line and * is pressed
  else if(location == 0x40 && characterstorage == 0x2A ){
    location = 0x93;
    line = 1; 
    changeloc(location);}
  else if(location == 0x14 && characterstorage == 0x2A){
    location = 0xD3;
    line = 2;
    changeloc(location);}
  else if(location == 0x54 && characterstorage == 0x2A){
    location = 0xA7;
    line = 3;
    changeloc(location);}
  else if(location == 0x00 && characterstorage == 0x2A){ 
    location = 0xE7;
    line = 4;
    changeloc(location);}
  else if(characterstorage == 0x2A){
    location = shLfCur; //if backspace, move curser back
    changeloc(location);} //change cursur location

  if (characterstorage != 0x2A && characterstorage != 0x23){
    digitalWrite(LCD_RS,HIGH);
    character = characterstorage;
    wrLCD4();}

  //if last character of line is output, move cursur to the beginning of next line
  if((location + 1) == 0x14){
    location = line2;
    line = 2;      //new line to move to
    changeloc(location);}
  else if((location + 1) == 0x54){
    location = line3;
    line = 3; 
    changeloc(location);}
  else if((location + 1) == 0x28){
    location = line4;
    line = 4; 
    changeloc(location);}
  else if((location + 1) == 0x68){
    location = line1;
    line = 1; 
    changeloc(location);}
}

char getloc(){
  byte locationh,locationl,location;
  
  pinMode(LCD_DB4,INPUT);
  pinMode(LCD_DB5,INPUT);
  pinMode(LCD_DB6,INPUT);
  pinMode(LCD_DB7,INPUT);

  digitalWrite(LCD_RS,LOW);
  digitalWrite(LCD_RW,HIGH);
  digitalWrite(LCD_E,HIGH);

  locationh = (PIND & 0xF0);
  digitalWrite(LCD_E,LOW);
  delayMicroseconds(1);
  digitalWrite(LCD_E,HIGH);
  locationl = (PIND & 0xF0);
  digitalWrite(LCD_E,LOW);
  locationl = locationl >> 4;
  
  location = locationh | locationl;
  
  pinMode(LCD_DB4,OUTPUT);
  pinMode(LCD_DB5,OUTPUT);
  pinMode(LCD_DB6,OUTPUT);
  pinMode(LCD_DB7,OUTPUT);
  return(location);
}

void changeloc(char loc){
  digitalWrite(LCD_RS,LOW);
  character = loc;
  wrLCD4();
}

void setitup(void){
  digitalWrite(LCD_RS,LOW);
  character = blinkCur;
  wrLCD4();
  character = clrDsp;
  wrLCD4();
  character = 0x80;  //moves to home cursur position
  wrLCD4();
}

void reset(){
  delay(100);
  digitalWrite(LCD_E,LOW);
  digitalWrite(LCD_RW,LOW);
  digitalWrite(LCD_RS,LOW);
  
  digitalWrite(LCD_DB7,LOW);
  digitalWrite(LCD_DB6,LOW);
  digitalWrite(LCD_DB5,HIGH);
  digitalWrite(LCD_DB4,HIGH);
  
  digitalWrite(LCD_E,HIGH);
  delayMicroseconds(1);
  digitalWrite(LCD_E,LOW);
  delay(10);

  digitalWrite(LCD_E,HIGH);
  delayMicroseconds(1);
  digitalWrite(LCD_E,LOW);
  delay(10);

  digitalWrite(LCD_E,HIGH);
  delayMicroseconds(1);
  digitalWrite(LCD_E,LOW);
  delay(10);

  digitalWrite(LCD_DB7,LOW);
  digitalWrite(LCD_DB6,LOW);
  digitalWrite(LCD_DB5,HIGH);
  digitalWrite(LCD_DB4,LOW);

  digitalWrite(LCD_E,HIGH);
  delayMicroseconds(1);
  digitalWrite(LCD_E,LOW);
  delay(10);

  digitalWrite(LCD_RS,LOW);
  character = configure;
  wrLCD4();
  digitalWrite(LCD_RS,LOW);
  character = offDsp;
  wrLCD4();
  digitalWrite(LCD_RS,LOW);
  character = clrDsp;
  wrLCD4();
  digitalWrite(LCD_RS,LOW);
  character = entryMode;
  wrLCD4();
  digitalWrite(LCD_RS,LOW);
  character = onDsp;
  wrLCD4();
  digitalWrite(LCD_RS,LOW);
  character = blinkCur;
  wrLCD4();
}

void wrLCD4(){
  int storage = 0;
  
  digitalWrite(LCD_E,LOW);
  digitalWrite(LCD_RW,LOW);
  
  storage = bitRead(character,7);
  digitalWrite(LCD_DB7,storage);
  storage = bitRead(character,6);
  digitalWrite(LCD_DB6,storage);
  storage = bitRead(character,5);
  digitalWrite(LCD_DB5,storage);
  storage = bitRead(character,4);
  digitalWrite(LCD_DB4,storage);

  digitalWrite(LCD_E,HIGH);
  delayMicroseconds(1);
  digitalWrite(LCD_E,LOW);

  storage = bitRead(character,3);
  digitalWrite(LCD_DB7,storage);
  storage = bitRead(character,2);
  digitalWrite(LCD_DB6,storage);
  storage = bitRead(character,1);
  digitalWrite(LCD_DB5,storage);
  storage = bitRead(character,0);
  digitalWrite(LCD_DB4,storage);
  pulseEwait4();
}

void pulseEwait4(){
    int storage = 1;
    
    digitalWrite(LCD_E,HIGH);
    delayMicroseconds(1);
    digitalWrite(LCD_E,LOW);

    digitalWrite(LCD_DB7,HIGH);
    digitalWrite(LCD_RS,LOW);
    digitalWrite(LCD_RW,HIGH);

    pinMode(LCD_DB7,INPUT);
    while(storage == 1){
       digitalWrite(LCD_E,HIGH);
       storage = digitalRead(LCD_DB7);
       digitalWrite(LCD_E,LOW);

       digitalWrite(LCD_E,HIGH);
       delayMicroseconds(1);
       digitalWrite(LCD_E,LOW);
    }
    pinMode(LCD_DB7,OUTPUT);
    
}

void LCDoutstr(char stringin[]){
  
  int i = 0;
  while(1){
    character = stringin[i];
    if(character == NULL) break;
    digitalWrite(LCD_RS,HIGH);
    wrLCD4();
    i++;
  } 
}

