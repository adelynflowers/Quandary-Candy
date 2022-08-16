#include <Wire.h>                // NEEDED FOR  I2C
#include <LiquidCrystal_I2C.h>   // NEEDED FOR  I2C
#include <SPI.h>
#include <WiFiNINA.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal_I2C lcd(0x27,20,4); // set the LCD address to 0x27 for a 20 chars and 4 line display   // SCL to PIN A5, SDA to PIN A4  // NEEDED FOR  I2C

char ssid[] = "QuandaryEscapeRooms";        // your network SSID (name)
char pass[] = "9163374600";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                // your network key Index number (needed only for WEP)

int led =  LED_BUILTIN;
int status = WL_IDLE_STATUS;
WiFiServer server(80);

const int WarningLED = 10;
const int SoundDelay = 200;
const int ScreenResetDelay = 3000;

const int FinalDoor = 16;
int Step = 1;

const int  GoodGuyButton = 8;
int GoodGuyState = 0;
int lastGoodGuyState = 0;
int GoodGuyCounter = 0;
int GoodGuyWinCounter = 0;

bool clientReset = false;
const int  BadGuyButton = 9;
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
const int PuzzleSolved = 14;
int PuzzleState = 0;
int TripAlarm = 0;
int lastPuzzleState = 0;
int PuzzleCounter = 0;
int TotalPuzzlesSolved = 0;
int ButtonPushsPerRound = 0;

int goodButtonPush_old = 0;
int badButtonPush_old = 0;

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

void printStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);

}

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
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();
  // you're connected now, so print out the status:
  printStatus();


  
}

void loop() {
  lcd.clear();
  clientReset = false;
  FourPlayers = true;
  digitalWrite(WarningLED, HIGH);
  MainGame = false;
  // compare the previous status to the current status
  if (status != WiFi.status()) {
    // it has changed update the variable
    status = WiFi.status();

    if (status == WL_AP_CONNECTED) {
      // a device has connected to the AP
      Serial.println("Device connected to AP");
    } else {
      // a device has disconnected from the AP, and we are back in listening mode
      Serial.println("Device disconnected from AP");
    }
  }
 
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {
    processClient(client);
  }
  while(true) {
    while(FourPlayers){
        WiFiClient client = server.available();   // listen for incoming clients
        if (client) {
          processClient(client);
        }
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
        WiFiClient client = server.available();   // listen for incoming clients
        if (client) {
          processClient(client);
        }
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
        WiFiClient client = server.available();   // listen for incoming clients
        if (client) {
          processClient(client);
        }
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
    
    WiFiClient client = server.available();   // listen for incoming clients
    if (client) {
       Serial.println("Client found");
       processClient(client);
    }
    // Print Screen Message
    //Serial.println("Main game loop");
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
   // Serial.print("Checking pin ");
    //Serial.print(PuzzleSolved);
    //Serial.print(": ");
    //Serial.println(PuzzleState);

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

    //Serial.print("Puzzle state is ");
    //Serial.println(PuzzleState);
    // If a puzzle has been solved, 
    Serial.print("PS: ");
    Serial.println(analogRead(A0));
    //Serial.print("TA: ");
    //Serial.println(TripAlarm);
    PuzzleState = LOW;
    if ((PuzzleState == HIGH) || (TripAlarm == HIGH)) {
        PuzzleState = LOW;
        TripAlarm = LOW;
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
      //Serial.println("TRIPPED_ALARM LOW");
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
          goodButtonPush_old = GoodGuyCounter;
          badButtonPush_old = BadGuyCounter;
          
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
            WiFiClient client = server.available();   // listen for incoming clients
            if (client) {
              processClient(client);
            }
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
    while(true){
        WiFiClient client = server.available();   // listen for incoming clients
        if (client) {
          processClient(client);
        }
        if (clientReset) {
          break;
        }
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
            lcd.print("SYSTEM TIED UP");
            lcd.setCursor(0, 1);
            lcd.print("      0        ");
            lcd.setCursor(0, 2);
            lcd.print(" Points Awarded");
            lcd.setCursor(0,3);
            lcd.print("     S.A.S.S.I.  ");
            ScoreCounter();
            delay(2000);
            lcd.clear();
            FinalScore();
            delay(2000);
            lcd.clear();
        }
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

        if (badGuyHeld && goodGuyHeld) { // *********************change to HIGH
            resetGame();
            break;
        }
    }
}

void resetGame() { 
   Serial.println("Reset Game");
            Serial.println("--------------");
            GoodGuyCounter = 0;
            GoodGuyWinCounter = 0;
            BadGuyCounter = 0;
            BadGuyWinCounter = 0;
            buttonPushCounter = 0;
            PuzzleCounter = 0;
            TotalPuzzlesSolved = 0;
            CheckForBadGuys = false;
            GetVotes = false;
            MainGame = false;
            FourPlayers = true;
            EndGame = false;
            WaitForReset = false;
            goodButtonPush_old = 0;
            badButtonPush_old = 0;
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

void processClient(WiFiClient & client) {
  Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:application/json");
            client.println();

           // the content of the HTTP response follows the header:
            client.print("{\"puzzles\":");  
            client.print(TotalPuzzlesSolved);
            client.print(",\"badGuy\":"); 
            client.print(badButtonPush_old); 
            client.print(",\"goodGuy\":"); 
            client.print(goodButtonPush_old);
            client.print("}"); 
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          TripAlarm = HIGH;               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /4")) {
          FourPlayers = false;
          FiveOrSixPlayers = false;
          SevenOrEightPlayers = false;
          MainGame = true;
          ButtonPushsPerRound = 2;
          lcd.clear();
          delay(300);
        }
        if (currentLine.endsWith("GET /6")) {
          FourPlayers = false;
          FiveOrSixPlayers = false;
          SevenOrEightPlayers = false;   
          MainGame = true;    
          ButtonPushsPerRound = 3;
          lcd.clear();
          delay(300);     
        }
        if (currentLine.endsWith("GET /8")) {
          FourPlayers = false;
          FiveOrSixPlayers = false;
          SevenOrEightPlayers = false;
          MainGame = true;
          ButtonPushsPerRound = 4;
          lcd.clear();
          delay(300);
        }
        if (currentLine.endsWith("GET /reset")) {
          resetGame();
          clientReset = true;        
        }
        if (currentLine.endsWith("GET /status")) {
               
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
}
