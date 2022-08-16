
#include <Wire.h>                // NEEDED FOR  I2C
#include <LiquidCrystal_I2C.h>   // NEEDED FOR  I2C

// initialize the library with the numbers of the interface pins
LiquidCrystal_I2C lcd(0x27,20,4); // set the LCD address to 0x27 for a 20 chars and 4 line display   // SCL to PIN A5, SDA to PIN A4  // NEEDED FOR  I2C


#define WarningLED 10
#define SoundDelay 200
#define ScreenResetDelay 3000

#define FinalDoor 16
int Step = 1;

#define GoodGuyButton 8
int GoodGuyState = 0;
int lastGoodGuyState = 0;
int GoodGuyCounter = 0;
int GoodGuyWinCounter = 0;

#define BadGuyButton 9
int BadGuyState = 0;
int lastBadGuyState = 0;
int BadGuyCounter = 0;
int BadGuyWinCounter = 0;

int buttonPushCounter = 0;

//4: Color Catch
//5: Break in box
#define TRIPPED_ALARM 6
#define GOOD_GUY_ALARM 5
#define BAD_GUY_ALARM 4
#define PuzzleSolved 14
int PuzzleState = 0;
int lastPuzzleState = 0;
int PuzzleCounter = 0;
int TotalPuzzlesSolved = 0;
int ButtonPushsPerRound = 0;

unsigned long goodGuyButtonStartTime = 0;
unsigned long badGuyButtonStartTime = 0;

bool ResecureSystem;
bool GetVotes;
bool CheckForBadGuys;
bool MainGame;
bool WaitForReset;
bool EndGame;

bool FourPlayers;
bool FiveOrSixPlayers;
bool SevenOrEightPlayers;

void setup() {
  lcd.init(); //initialize the lcd         // NEEDED FOR  I2C
  lcd.backlight(); //open the backlight    // NEEDED FOR  I2C
  lcd.begin(20, 4);                        // NEEDED FOR  I2C
  Serial.begin(9600);

  pinMode(GoodGuyButton, INPUT);
  pinMode(BadGuyButton, INPUT);
  pinMode(14,INPUT);
  pinMode(5,OUTPUT);
  digitalWrite(5, HIGH);
  pinMode(WarningLED, OUTPUT);
  Serial.println("End setup");
  pinMode(TRIPPED_ALARM,OUTPUT);
  pinMode(GOOD_GUY_ALARM,OUTPUT);
  pinMode(BAD_GUY_ALARM,OUTPUT);
  digitalWrite(TRIPPED_ALARM,HIGH);
  digitalWrite(BAD_GUY_ALARM, HIGH);
  digitalWrite(GOOD_GUY_ALARM, HIGH);
  pinMode(FinalDoor, INPUT);
}

void loop() {
  lcd.clear();
  FourPlayers = true;
  digitalWrite(WarningLED, HIGH);
  MainGame = false;
  while(true) {
    while(FourPlayers){
        lcd.setCursor(0, 0);
        lcd.print("Number of Players:");
        lcd.setCursor(0, 1);
        lcd.print("        4-5  ");
        lcd.setCursor(7,1);
        lcd.setCursor(0,3);
        lcd.print("     S.A.S.S.I.  ");
        if (digitalRead(BadGuyButton) == HIGH){
            delay(100);
            lcd.clear();
            FourPlayers = false;
            FiveOrSixPlayers = true;
        }
        if (digitalRead(GoodGuyButton) == HIGH){
            delay(300);
            lcd.clear();
            ButtonPushsPerRound = 2;
            FourPlayers = false;
            MainGame = true;
        }
    }
    while(FiveOrSixPlayers){
        lcd.setCursor(0, 0);
        lcd.print("Number of Players:");
        lcd.setCursor(0, 1);
        lcd.print("        6-7");
        lcd.setCursor(7,1);
        lcd.setCursor(0,3);
        lcd.print("     S.A.S.S.I.  ");
        if (digitalRead(BadGuyButton) == HIGH){
            delay(300);
            lcd.clear();
            FiveOrSixPlayers = false;
            SevenOrEightPlayers = true;
        }
        if (digitalRead(GoodGuyButton) == HIGH){
            delay(300);
            lcd.clear();
            ButtonPushsPerRound = 3;
            FiveOrSixPlayers = false;
            MainGame = true;
        }
    }
    while(SevenOrEightPlayers){
        lcd.setCursor(0, 0);
        lcd.print("Number of Players:");
        lcd.setCursor(0, 1);
        lcd.print("         8");
        lcd.setCursor(7,1);
        lcd.setCursor(0,3);
        lcd.print("     S.A.S.S.I.  ");
        if (digitalRead(BadGuyButton) == HIGH){
        delay(100);
        lcd.clear();
        SevenOrEightPlayers = false;
        FourPlayers = true;
        }
        if (digitalRead(GoodGuyButton) == HIGH){
        delay(100);
        lcd.clear();
        ButtonPushsPerRound = 4;
        SevenOrEightPlayers = false;
        MainGame = true;
        }
    }
    if (MainGame) break;
    }
  while (MainGame){

    // Print Screen Message
    Serial.println("Main game loop");
    CheckForBadGuys = false;
    digitalWrite(WarningLED, LOW);
    lcd.setCursor(0, 0);
    lcd.print("  Security System ");
    lcd.setCursor(0, 1);
    lcd.print("  ----DISABLED----");
    lcd.setCursor(0,3);
    lcd.print("     S.A.S.S.I.  ");
    
    // Check if a puzzle has been solved
    PuzzleState = digitalRead(PuzzleSolved);
    Serial.print("Checking pin ");
    Serial.print(PuzzleSolved);
    Serial.print(": ");
    Serial.println(PuzzleState);

     bool badGuyHeld = false;
    bool goodGuyHeld = false;
    // Begin Good guy hold
    if (digitalRead(GoodGuyButton) == HIGH) {
        if (goodGuyButtonStartTime == 0) {
            goodGuyButtonStartTime = millis();
        } else if ((millis() - goodGuyButtonStartTime) >= 2000) {
          goodGuyHeld = true;
        }
        
    } else if (digitalRead(GoodGuyButton) == LOW) {
        goodGuyButtonStartTime = 0;    
    }
    //Begin Bad guy hold
    if (digitalRead(BadGuyButton) == HIGH) {
        if (badGuyButtonStartTime == 0) {
            badGuyButtonStartTime = millis();
        } else if ((millis() - badGuyButtonStartTime) >= 2000) {
          badGuyHeld = true;
        }
    } else if (digitalRead(BadGuyButton) == LOW) {
        badGuyButtonStartTime = 0;
    }

    if (badGuyHeld && goodGuyHeld) {
        displayScores();
    }

    // TODO REMOVE
   /* if (digitalRead(FinalDoor) == LOW){  //****************************************************change to LOW
      
      EndGame = true;

    }*/

    // If a puzzle has been solved, 
    if (PuzzleState == HIGH) {
        PuzzleState = LOW;
        PuzzleCounter++;
        TotalPuzzlesSolved++;
        lcd.setCursor(0,0);
        Serial.print("Puzzle Count:");
        Serial.println(PuzzleCounter);
        Serial.print("Total Puzzles Solved:");
        Serial.println(TotalPuzzlesSolved);
        Serial.println("--------------");
    }
    delay(50);

    if (PuzzleCounter >= 1){
      lcd.clear();
      GetVotes = true;
    }
    while (GetVotes) {
      digitalWrite(TRIPPED_ALARM,LOW);
      Serial.println("TRIPPED_ALARM LOW");
      lcd.setCursor(0, 0);
      lcd.print("ResetSystemNow!");
      lcd.setCursor(0, 1);
      lcd.print("Pushes Left:");
      lcd.setCursor(0, 3);
      lcd.print("     S.A.S.S.I.");
      lcd.setCursor(12, 1);
      lcd.print((ButtonPushsPerRound)-(buttonPushCounter));
      lcd.setCursor(13, 1);
      lcd.print("   ");
      digitalWrite(WarningLED, HIGH);
      // Good Guy Button Stuff----------------------------------------
      GoodGuyState = digitalRead(GoodGuyButton);
      if (GoodGuyState != lastGoodGuyState) {
        if (GoodGuyState == HIGH) {
          GoodGuyCounter++;
          buttonPushCounter++;
          SerialPrint();
        }
        digitalWrite(WarningLED, LOW);
        delay(500);
      }
      lastGoodGuyState = GoodGuyState;
      // Bad Guy Button Stuff----------------------------------------
      BadGuyState = digitalRead(BadGuyButton);
      if (BadGuyState != lastBadGuyState) {
        if (BadGuyState == HIGH) {
          BadGuyCounter++;
          buttonPushCounter++;
          SerialPrint();
        }
        digitalWrite(WarningLED, LOW);
        delay(500);
      }
      lastBadGuyState = BadGuyState;
      // Check to See Who Won the Round-----------------------------
      if (buttonPushCounter == ButtonPushsPerRound) {
        CheckForBadGuys = true;
        lcd.clear();

        while (CheckForBadGuys){
          digitalWrite(WarningLED, LOW);
          digitalWrite(TRIPPED_ALARM, HIGH);
          delay(200);

          if (BadGuyCounter >= 1){
            BadGuyWinCounter++;
            ScoreCounter();
            lcd.setCursor(0, 0);
            lcd.print(" !!System Alerted!!");
            lcd.setCursor(0, 1);
            lcd.print("RED PUSHED:");
            lcd.setCursor(15, 1);
            lcd.print(BadGuyCounter);
            lcd.setCursor(0, 2);
            lcd.print("GREEN PUSHED:");
            lcd.setCursor(15, 2);
            lcd.print(GoodGuyCounter);
            lcd.setCursor(16, 3);
            
    
            lcd.setCursor(0,3);
            lcd.print("     S.A.S.S.I.  ");
            digitalWrite(BAD_GUY_ALARM, LOW);
            delay(SoundDelay);
            digitalWrite(BAD_GUY_ALARM, HIGH);
            delay(2000);
            delay(ScreenResetDelay);
          }
          if (BadGuyCounter < 1){
            GoodGuyWinCounter++;
            ScoreCounter();
            lcd.setCursor(0, 0);
            lcd.print("  System Resetting ");
            lcd.setCursor(0, 1);
            lcd.print("  ----------------");
            lcd.setCursor(0, 2);
            lcd.print("  ALL GREEN PUSHED");
            lcd.setCursor(0,3);
            lcd.print("     S.A.S.S.I.  ");
            digitalWrite(GOOD_GUY_ALARM, LOW);
            delay(SoundDelay);
            digitalWrite(GOOD_GUY_ALARM, HIGH);
            delay(ScreenResetDelay);
          }

          //Reset to another Round after the reveal--------------------
          buttonPushCounter = 0;
          BadGuyCounter = 0;
          GoodGuyCounter = 0;
          PuzzleCounter = 0;
          lcd.clear();
          CheckForBadGuys = false;
          GetVotes = false;
          Serial.println ("Reset Counters");
          Serial.println("--------------");

          //Spy Portion of the Game Ends after 5 rounds----------------
          
          if (TotalPuzzlesSolved == 5){
            EndGame = true;
            lcd.clear();
          }
          while (EndGame){
            Serial.println("End Game loop");
            lcd.setCursor(0,0);
            lcd.setCursor(0, 0);
            lcd.print("Security System ");
            lcd.setCursor(0, 1);
            lcd.print("----DISABLED----");
            digitalWrite(WarningLED, LOW);

                    // Begin Good guy hold
            if (digitalRead(GoodGuyButton) == HIGH) {
                if (goodGuyButtonStartTime == 0) {
                    goodGuyButtonStartTime = millis();
                }
            } else if (digitalRead(GoodGuyButton) == LOW) {
                goodGuyButtonStartTime = 0;    
            }
            //Begin Bad guy hold
            if (digitalRead(BadGuyButton) == HIGH) {
                if (badGuyButtonStartTime == 0) {
                    badGuyButtonStartTime = millis();
                }
            } else if (digitalRead(BadGuyButton) == LOW) {
                badGuyButtonStartTime = 0;
            }

            bool badGuyHeld = false;
            bool goodGuyHeld = false;
            if (goodGuyButtonStartTime > 0) {
                if ((millis() - goodGuyButtonStartTime) >= 2000) {
                    goodGuyHeld = true;
                }
            }
            if (badGuyButtonStartTime > 0) {
                if ((millis() - badGuyButtonStartTime) >= 2000) {
                    badGuyHeld = true;
                }
            }
            if (badGuyHeld && goodGuyHeld) {
                displayScores();
            }
          }
        }
      }
    }
  }
}

void displayScores() {
    lcd.clear();
    delay(2000);
    goodGuyButtonStartTime = 0;
    badGuyButtonStartTime = 0;
    while(true){
        Serial.println("Display scores loop");
        if (BadGuyWinCounter > GoodGuyWinCounter){
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("SYSTEM ALERTED");
            lcd.setCursor(0, 1);
            lcd.print("   AGENTS FAILED");
            lcd.setCursor(0, 2);
            lcd.print("DoubleAgents 1 Point");
            lcd.setCursor(0,3);
            lcd.print("     S.A.S.S.I.  ");

            delay(2000);
            lcd.clear();
            FinalScore();
            delay(2000);
            lcd.clear();
        }
        // System tied up \n 0 points awarded
        //any point hold down both buttons and goes to end game, then delay 2 secs to avoid accicdental reset
        if (BadGuyWinCounter < GoodGuyWinCounter){
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("SECURITY EVADED");
            lcd.setCursor(0, 1);
            lcd.print("    GOT AWAY!");
            lcd.setCursor(0, 2);
            lcd.print("  Agents 1 Point");
            lcd.setCursor(0,3);
            lcd.print("     S.A.S.S.I.  ");
            ScoreCounter();
            delay(2000);
            lcd.clear();
            FinalScore();
            delay(2000);
            lcd.clear();
        }
        if (BadGuyWinCounter == GoodGuyWinCounter){
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("  SYSTEM TIED UP");
            lcd.setCursor(0, 1);
            lcd.print("         0        ");
            lcd.setCursor(0, 2);
            lcd.print("   Points Awarded");
            lcd.setCursor(0,3);
            lcd.print("     S.A.S.S.I.  ");
            ScoreCounter();
            delay(2000);
            lcd.clear();
            FinalScore();
            delay(2000);
            lcd.clear();
        }
        bool badGuyHeld = false;
        bool goodGuyHeld = false;
        // Begin Good guy hold
        if (digitalRead(GoodGuyButton) == HIGH) {
            if (goodGuyButtonStartTime == 0) {
                goodGuyButtonStartTime = millis();
            } else if ((millis() - goodGuyButtonStartTime) >= 2000) {
              goodGuyHeld = true;
            }
            
        } else if (digitalRead(GoodGuyButton) == LOW) {
            goodGuyButtonStartTime = 0;    
        }
        //Begin Bad guy hold
        if (digitalRead(BadGuyButton) == HIGH) {
            if (badGuyButtonStartTime == 0) {
                badGuyButtonStartTime = millis();
            } else if ((millis() - badGuyButtonStartTime) >= 2000) {
              badGuyHeld = true;
            }
        } else if (digitalRead(BadGuyButton) == LOW) {
            badGuyButtonStartTime = 0;
        }

        if (badGuyHeld && goodGuyHeld) { // *********************change to HIGH
            Serial.println("Reset Game");
            Serial.println("--------------");
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("  RESETTING");
            GoodGuyCounter = 0;
            GoodGuyWinCounter = 0;
            BadGuyCounter = 0;
            BadGuyWinCounter = 0;
            buttonPushCounter = 0;
            PuzzleCounter = 0;
            TotalPuzzlesSolved = 0;
            CheckForBadGuys = false;
            goodGuyHeld = false;
            badGuyHeld = false;
            goodGuyButtonStartTime = 0;
            badGuyButtonStartTime = 0;
            GetVotes = false;
            MainGame = false;
            FourPlayers = true;
            EndGame = false;
            WaitForReset = false;
            delay(2000);
            break;
        }
    }
}

void SerialPrint(){
  Serial.print("BGCount:");
  Serial.println(BadGuyCounter);
  Serial.print("GGCount:");
  Serial.println(GoodGuyCounter);

  lcd.setCursor(0, 1);
  Serial.print("PushCount:");
  Serial.println(buttonPushCounter);
  Serial.println("--------------");
}

void ScoreCounter(){
  lcd.setCursor(0, 0);
  Serial.print("BadGuyWinCount:");
  Serial.println(BadGuyWinCounter);
  Serial.print("GoodGuyCount:");
  Serial.println(GoodGuyWinCounter);
  Serial.println("--------------");
}

void FinalScore(){
  lcd.print("FINAL SCORE:");
  lcd.setCursor(0, 1);
  lcd.print("Agents:");
  lcd.setCursor(10,1);
  lcd.print(GoodGuyWinCounter);
  lcd.setCursor(0, 2);
  lcd.print("DoubleAgents:");
  lcd.setCursor(14,2);
  lcd.print(BadGuyWinCounter);
}

void ChoosePlayers(){

  FourPlayers = true;
  while(FourPlayers){
    lcd.print("No. of Players:");
    lcd.setCursor(0, 1);
    lcd.print("4");
    lcd.setCursor(7,1);
    if (digitalRead(BadGuyButton) == HIGH){
      delay(500);
      FourPlayers = false;
      FiveOrSixPlayers = true;
    }
  }

  while(FiveOrSixPlayers){
    lcd.print("No. of Players:");
    lcd.setCursor(0, 1);
    lcd.print("5-6");
    lcd.setCursor(7,1);
    if (digitalRead(BadGuyButton) == HIGH){
      delay(500);
      FiveOrSixPlayers = false;
      SevenOrEightPlayers = true;
    }
  }

  while(SevenOrEightPlayers){
    lcd.print("No. of Players:");
    lcd.setCursor(0, 1);
    lcd.print("7-7");
    lcd.setCursor(7,1);
    if (digitalRead(BadGuyButton) == HIGH){
      delay(500);
      SevenOrEightPlayers = false;
      FourPlayers = true;
    }
  }
}
