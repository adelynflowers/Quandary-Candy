

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal_I2C lcd(0x27,20,4); // set the LCD address to 0x27 for a 20 chars and 4 line display

int Wire1 = 0; 
int Wire2 = 0;
int Wire3 = 0;
int Wire4 = 0;
int Button = 0;

int ConnectLoop = 4;

const int SuccessSound = 16;
const int FailureSound = 15;
const int ModemSound= 14;
const int GameRunningLED = 13;
const int LightsRelay = 2;
bool EndGame;

void setup() {


  pinMode(8, INPUT);
  pinMode(9, INPUT);
  pinMode(10, INPUT);
  pinMode(11, INPUT);
  pinMode(12, INPUT);
  pinMode(SuccessSound, OUTPUT);
  pinMode(FailureSound, OUTPUT);
  pinMode(ModemSound, OUTPUT);
  pinMode(GameRunningLED, OUTPUT);
  pinMode(LightsRelay, OUTPUT);

  
  digitalWrite(SuccessSound, LOW);
  digitalWrite(FailureSound, LOW);
  digitalWrite(ModemSound, LOW);
  digitalWrite(GameRunningLED, HIGH);
  digitalWrite(LightsRelay, LOW);
  
  lcd.init(); //initialize the lcd
  lcd.backlight(); //open the backlight 
  lcd.begin(20, 4);
  lcd.print("  Security  System");
  lcd.setCursor(0, 1);
  lcd.print("       ACTIVE    ");
  lcd.setCursor(0,3);
  lcd.print("     S.A.S.S.I.  ");
}

void loop() {
  
  EndGame = false;
  ReadWires();
  
  lcd.setCursor(0, 1);
  lcd.print("    -  ACTIVE  -  ");
  delay (100);
  lcd.setCursor(0, 1);
  lcd.print("       ACTIVE    ");
  delay (100);

  if(Wire1 == HIGH && Wire2 == HIGH && Wire3 == HIGH && Wire4 == HIGH && Button == HIGH){
    
    ConnectingScreenAnimation();    
    EndGame = true;
  }
  
  else if ( Button == HIGH){
    
    ConnectingScreenAnimation(); 
    
    digitalWrite(FailureSound, HIGH);
    
    lcd.begin(16, 2);
    lcd.print("  Attempt Failed ");
    lcd.setCursor(0, 1);
    lcd.print("  Check Connection");
    delay (2000);
    
    digitalWrite(FailureSound, LOW);
       
    lcd.begin(16, 2);
    lcd.print("  Security  System");
    lcd.setCursor(0, 1);
    lcd.print("       ACTIVE    ");  
  }
   
  while(EndGame){

    ReadWires();
    
    digitalWrite(SuccessSound, HIGH);
    digitalWrite(GameRunningLED, LOW);
    digitalWrite(LightsRelay, HIGH);
    
    lcd.setCursor(0, 0);
    lcd.print("  Security  System");
    lcd.setCursor(0, 1);
    lcd.print("   Access Granted");
    
    if(Wire1 == LOW && Wire2 == LOW && Wire3 == LOW && Wire4 == LOW && Button == HIGH){
      
      digitalWrite(GameRunningLED, HIGH);
      digitalWrite(SuccessSound, LOW);
      digitalWrite(LightsRelay, LOW);
      
      lcd.begin(16, 2);
      lcd.print("  Security  System");
      lcd.setCursor(0, 1);
      lcd.print("       ACTIVE    ");
      
      EndGame = false;
    }
  }
      
}

void ConnectingScreenAnimation(){
  
  digitalWrite(ModemSound, HIGH);
  
  for(int i=0; i<ConnectLoop; i++){   
      
    lcd.begin(16, 2);
      lcd.print("Connecting[>   ] ");
      lcd.setCursor(0, 1);
      lcd.print("Please Wait...");
      delay (300);
    
      lcd.setCursor(0, 0);
      lcd.print("Connecting[->  ] ");
      delay (300);
    
      lcd.setCursor(0, 0);
      lcd.print("Connecting[--> ] ");
      delay (300);
    
      lcd.setCursor(0, 0);
      lcd.print("Connecting[--->] ");
      delay (300);
    }
  
  digitalWrite(ModemSound, LOW);
}

void ReadWires(){
     
    Wire1 = digitalRead(8);
    Wire2 = digitalRead(9);
    Wire3 = digitalRead(10);
    Wire4 = digitalRead(11);
    Button = digitalRead(12);
}
