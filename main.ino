// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "TMPLRzpJtSIp"
#define BLYNK_DEVICE_NAME "Stepper"
#define BLYNK_AUTH_TOKEN "TsAjMQ-WN7zfNY0LxyNdo7gSFNAbVoyS"


// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

// define pins for stepper motor
// right steppermotor
#define firstStepperPin1 11
#define firstStepperPin2 10
#define firstStepperPin3 9
#define firstStepperPin4 8
// left steppermotor
#define secondStepperPin1 7
#define secondStepperPin2 6
#define secondStepperPin3 5
#define secondStepperPin4 4

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Asus";
char pass[] = "karzan11";

// or Software Serial on Uno, Nano...
#include <SoftwareSerial.h>
SoftwareSerial EspSerial(2, 3); // RX, TX

// Your ESP8266 baud rate:
#define ESP8266_BAUD 9600

ESP8266 wifi(&EspSerial);

// fullstep sequence for stepper motor
const byte seqArr[4][4] = {
  {1, 0, 0, 0},
  {0, 1, 0, 0},
  {0, 0, 1, 0},
  {0, 0, 0, 1}
};

// array with pins used for stepper for easy iteration
const byte pins[2][4] = {
  {secondStepperPin1, secondStepperPin2, secondStepperPin3, secondStepperPin4},          // Left stepper
  {firstStepperPin1, firstStepperPin2, firstStepperPin3, firstStepperPin4}               // Right stepper
};

// button status variables
bool forward_status = false;
bool backward_status = false;
bool right_status = false;
bool left_status = false;

// funciton for moving, direction parameter for forward respective backward
void travel(String dir) {
  if (dir.equals("f")) {
    // Moving forward
    for (int stepper = 0; stepper < 2; stepper++) {                                      // Index of the stepper used
      // Repeat this process for each stepper for a whole step
      for (int arrNum = 0; arrNum < 4; arrNum++) {                                       // Index of what sequence array is used in the 2d arrary seqArr
        delay(4);
        for (int pinStatus = 0; pinStatus < 4; pinStatus++) {                            // Index of what the status of the pin in this position will be
          digitalWrite(pins[stepper][pinStatus], seqArr[arrNum][pinStatus]);             // either 1 or 0
        }
      }
    }
  } else if (dir.equals("b")) {
    // Moving backward (loop the sequence array from reverse)
    for (int stepper = 0; stepper < 2; stepper++) {                                      // Index of the stepper used
      // Repeat this process for each stepper for a whole step
      for (int arrNum = 3; arrNum > -1; arrNum--) {                                      // Index of what sequence array is used in the 2d arrary seqArr
        delay(4);
        for (int pinStatus = 0; pinStatus < 4; pinStatus++) {                            // Index of what the status of the pin in this position will be
          digitalWrite(pins[stepper][pinStatus], seqArr[arrNum][pinStatus]);             // write the correct status for each of the pins in each stepper (1 or 0)
        }
      }
    }
  } else if (dir.equals("L")) {
    // turning left
    // left stepper back and right stepper forward
    // left stepper
    for (int arrNum = 3; arrNum > -1; arrNum--) {                                      // Index of what sequence array is used in the 2d arrary seqArr
      delay(4);
      for (int pinStatus = 0; pinStatus < 4; pinStatus++) {                            // Index of what the status of the pin in this position will be
        digitalWrite(pins[1][pinStatus], seqArr[arrNum][pinStatus]);                   // write the correct status for each of the pins in right stepper
      }
    }
    // right stepper
    for (int arrNum = 0; arrNum < 4; arrNum++) {                                       // Index of what sequence array is used in the 2d arrary seqArr
      delay(4);
      for (int pinStatus = 0; pinStatus < 4; pinStatus++) {                            // Index of what the status of the pin in this position will be
        digitalWrite(pins[0][pinStatus], seqArr[arrNum][pinStatus]);                   // write the correct status for each of the pins in left stepper
      }
    }
  } else if (dir.equals("R")) {
    // turning right
    // move right stepper back and left forward
    // right stepper
    for (int arrNum = 3; arrNum > -1; arrNum--) {                                      // Index of what sequence array is used in the 2d arrary seqArr
      delay(4);
      for (int pinStatus = 0; pinStatus < 4; pinStatus++) {                            // Index of what the status of the pin in this position will be
        digitalWrite(pins[0][pinStatus], seqArr[arrNum][pinStatus]);                   // write the correct status for each of the pins in right stepper
      }
    }
    // left stepper
    for (int arrNum = 0; arrNum < 4; arrNum++) {                                       // Index of what sequence array is used in the 2d arrary seqArr
      delay(4);
      for (int pinStatus = 0; pinStatus < 4; pinStatus++) {                            // Index of what the status of the pin in this position will be
        digitalWrite(pins[1][pinStatus], seqArr[arrNum][pinStatus]);                   // write the correct status for each of the pins in left stepper
      }
    }
  }
}

void setup() {
  // setup stepper pins to output pins
  for (int stepper = 0; stepper < 2; stepper++) {
    for (int stepperPin = 0; stepperPin < 4; stepperPin++) {
      pinMode(pins[stepper][stepperPin], OUTPUT);
    }
  }

  // Debug console
  Serial.begin(9600);

  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  // Blynk.begin(auth, wifi, ssid, pass);
  Blynk.begin(auth, wifi, ssid, pass, "blynk.cloud", 80);
}

BLYNK_WRITE(V0) {                                 // Status for the V0 virtual pin
  forward_status = param.asInt();                 // Sets the status to a variable
}
BLYNK_WRITE(V1) {                                 // Status for the V1 virtual pin
  backward_status = param.asInt();                // Sets the status to a variable
}
BLYNK_WRITE(V2) {                                 // Status for the V2 virtual pin
  right_status = param.asInt();                   // Sets the status to a variable
}
BLYNK_WRITE(V3) {                                 // Status for the V3 virtual pin
  left_status = param.asInt();                    // Sets the status to a variable
}

void loop() {
  Blynk.run();
  if (backward_status) {
    // backward button is pressed, move backward
    travel("b");
  } else if (forward_status) {
    // forward button is pressed, move forward
    travel("f");
  } else if (right_status) {
    // right button pressed, move right
    travel("R");
  } else if (left_status) {
    // left button pressed, move left
    travel("L");
  } else {
    // stop motor by turning off all pins
    for (byte pin : pins) {
      digitalWrite(pin, LOW);
    }
  }
}
