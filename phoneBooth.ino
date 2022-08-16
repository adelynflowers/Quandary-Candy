#include <SPI.h>
#include <WiFiNINA.h>

#define RED_LED 7
#define GREEN_LED 8
#define RED_BTN_PIN 9
#define GREEN_BTN_PIN 10

char ssid[] = "QuandaryEscapeRooms";        // your network SSID (name)
char pass[] = "9163374600";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;
WiFiServer server(80);

int greenCount = 0;
int redCount = 0;
unsigned long greenHoldStart = 0;
unsigned long redHoldStart = 0;
unsigned long nextCycleStart = 0;
bool greenHeld = false;
bool redHeld = false;
bool waitForReset = false;
bool buttonsAvailable = true;
#define maxTime 10000
#define minTime 5000
#define holdTime 3000

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
  // put your setup code here, to run once:
  pinMode(RED_BTN_PIN, INPUT);
  pinMode(GREEN_BTN_PIN, INPUT);
  pinMode(RED_LED, INPUT);
  pinMode(GREEN_LED, INPUT);
  Serial.begin(9600);

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

void displayScores() {
  int animationPin;
  if (redCount >= greenCount) {
    animationPin = RED_LED;
  }
  else {
    animationPin = GREEN_LED;
  }

  //Prepare by turning LEDS off
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  //Double Blinks x5
  for (int i = 0; i < 5; i++) {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, HIGH);
    delay(500);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, LOW);
    delay(500);
  }

  //Accelerating single blink
  for (int i = 10; i > 0; i--) {
    digitalWrite(animationPin, HIGH);
    delay(20 * i);
    digitalWrite(animationPin, LOW);
    delay(20 * i);
  }

  //Display Winner
  digitalWrite(animationPin, HIGH);  
}

void timeOut() {
  buttonsAvailable = false;
  nextCycleStart = millis() + random(minTime,maxTime);
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
}

void readButton(int buttonPin, unsigned long& holdStart, bool& buttonHeld, int& colorCount) {
  unsigned long currentTime = millis();
  if (digitalRead(buttonPin)) {
    if (holdStart == 0) {
      holdStart = millis();
    }
    else if ((currentTime - holdStart) >= holdTime) {
      buttonHeld = true;
    }
  }
  else if (!digitalRead(buttonPin)) {
    Serial.print("buttonHeld: ");
    Serial.println(buttonHeld);
    Serial.print("holdStart: ");
    Serial.println(holdStart);
    if (!buttonHeld && (holdStart != 0)) {
      colorCount++;
      timeOut();
    }
    buttonHeld = false;
    holdStart = 0;
  }
}

void resetGame() {
  Serial.println("In reset game");
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  while (redHeld && greenHeld) {
    Serial.println("in while loop");
    readButton(RED_BTN_PIN, redHoldStart, redHeld, redCount);
    readButton(GREEN_BTN_PIN, greenHoldStart, greenHeld, greenCount);
  }
  redCount = greenCount = 0;
  waitForReset = false;
  nextCycleStart = 0;
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
            client.print("{\"redCount\":");  
            client.print(redCount);
            client.print(",\"greenCount\":"); 
            client.print(greenCount); 
            client.print(",\"waitForReset\":"); 
            client.print(waitForReset);
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
        if (currentLine.endsWith("GET /reset")) {
          resetGame();
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
}

void loop() {
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
  
    //Update buttons, if available
    if (millis() >= nextCycleStart && !waitForReset) {
      buttonsAvailable = true;
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, HIGH);
      nextCycleStart = 0;
    }
    if (buttonsAvailable) {
      readButton(RED_BTN_PIN, redHoldStart, redHeld, redCount);
      readButton(GREEN_BTN_PIN, greenHoldStart, greenHeld, greenCount);
    }
    
    //Reset if both held
    if (greenHeld && redHeld) {
      Serial.print("Checking wait for reset ");
      Serial.println(waitForReset);
      if (!waitForReset) {
        waitForReset = true;  
        displayScores();
      }    
      else if (waitForReset) {
        resetGame();
      }
    }
    

}
