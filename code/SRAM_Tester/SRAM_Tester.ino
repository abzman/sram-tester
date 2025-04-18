// To write to 2102 SRAM, !CE=LOW, RW=LOW, Data sent to DI pin.
// To read from 2102 SRAM, !CE=LOW, RW=HIGH,  Read DO pin
#define LEDPin 13

#define RAM2102 1

#define RAMTest 1

char RAMType[] = "2102";

#define VCC_pin 11
#define CE_pin A2
#define RW_pin 5
#define GND_pin 12

#define addr_A00_pin 10
#define addr_A01_pin 6
#define addr_A02_pin 7
#define addr_A03_pin 8
#define addr_A04_pin 9
#define addr_A05_pin 4
#define addr_A06_pin 3
#define addr_A07_pin A5
#define addr_A08_pin A4
#define addr_A09_pin A3
#define data_DO_pin A1
#define data_DI_pin A0

#define max_addr 0xff


//If the test will FAIL some pin could be shorted to GND or to VCC
//so all pins will be set as input to prevent damage to the Arduino
//A small resistor (150 Ohm) between each Arduino GPIO and memory pin
//should prevent any damage to the Arduino.




// Verb = 0 the tester will print only the test results
// Verb = 1 the tester will print on the serial port all the memory content
int Verb = 1;

uint8_t ctrl_pins[] = { CE_pin, RW_pin };
uint8_t addr_pins[] = { addr_A00_pin, addr_A01_pin, addr_A02_pin, addr_A03_pin, addr_A04_pin, addr_A05_pin, addr_A06_pin, addr_A07_pin, addr_A08_pin, addr_A09_pin };
uint8_t inout_pins[] = { data_DO_pin, data_DI_pin };

int failed = 0;
int PassFail[] = { 0, 0, 0, 0 };

void SetPinModes() {
  pinMode(addr_A00_pin, OUTPUT);
  pinMode(addr_A01_pin, OUTPUT);
  pinMode(addr_A02_pin, OUTPUT);
  pinMode(addr_A03_pin, OUTPUT);
  pinMode(addr_A04_pin, OUTPUT);
  pinMode(addr_A05_pin, OUTPUT);
  pinMode(addr_A06_pin, OUTPUT);
  pinMode(addr_A07_pin, OUTPUT);
  pinMode(addr_A08_pin, OUTPUT);
  pinMode(addr_A09_pin, OUTPUT);

  pinMode(LEDPin, OUTPUT);

  pinMode(CE_pin, OUTPUT);
  pinMode(VCC_pin, OUTPUT);
  pinMode(GND_pin, OUTPUT);
  pinMode(data_DO_pin, INPUT);
  pinMode(data_DI_pin, OUTPUT);
  pinMode(RW_pin, OUTPUT);
}

void powerChip() {
  digitalWrite(VCC_pin, HIGH);
  digitalWrite(GND_pin, LOW);
}

void unpowerChip() {
  digitalWrite(VCC_pin, LOW);
  digitalWrite(GND_pin, LOW);
}


void setup() {
  Serial.begin(2000000);

  Serial.println(F("\n*****************************************"));
  Serial.println(F("ArduinoMega SRAM Test for"));
  Serial.println(F("SRAM Type: "));
  Serial.write(RAMType);
  Serial.println(F("*****************************************\n"));
  Serial.println();
  SetPinModes();  //  Setup pinmodes depending on chip
  digitalWrite(LEDPin, HIGH);
  powerChip();
}


void checkPins() {
  int readback, index, i;
  String text;

  for (index = 0; index <= 1; index++) {
    if (index == 0) text = "HIGH ";
    else text = "LOW  ";

    for (i = 0; i < sizeof(ctrl_pins); i++) {
      if (index == 0) pinMode(ctrl_pins[i], INPUT_PULLUP);
      else {
        pinMode(ctrl_pins[i], OUTPUT);
        digitalWrite(ctrl_pins[i], LOW);
      }
      readback = digitalRead(ctrl_pins[i]);
      if (readback != index) {
        if (Verb == 1) Serial.println("Check: Ctrl pin  " + String(i, DEC) + " " + text + "PASS");
        pinMode(ctrl_pins[i], OUTPUT);
        digitalWrite(ctrl_pins[i], HIGH);
      } else {
        if (Verb == 1) Serial.println("Check: Ctrl pin  " + String(i, DEC) + " " + text + "FAIL");
        pinMode(ctrl_pins[i], INPUT);
        failed++;
      }
    }
    if (Verb == 1) Serial.println();

    //set address pins as output
    for (i = 0; i < sizeof(addr_pins); i++) {
      if (index == 0) pinMode(addr_pins[i], INPUT_PULLUP);
      else {
        pinMode(addr_pins[i], OUTPUT);
        digitalWrite(addr_pins[i], LOW);
      }
      if (readback != index) {
        if (Verb == 1) {
          if (i < 10) Serial.println("Check: Addr pin  " + String(i, DEC) + " " + text + "PASS");
          else Serial.println("Check: Addr pin " + String(i, DEC) + " " + text + "PASS");
        }
      } else {
        if (Verb == 1) {
          if (i < 10) Serial.println("Check: Addr pin  " + String(i, DEC) + " " + text + "FAIL");
          else Serial.println("Check: Addr pin " + String(i, DEC) + " " + text + "FAIL");
        }
        pinMode(addr_pins[i], INPUT);
        failed++;
      }
    }
    if (Verb == 1) Serial.println();

    //set I/O pins as input
    for (i = 0; i < sizeof(inout_pins); i++) {
      if (index == 0) pinMode(inout_pins[i], INPUT_PULLUP);
      else {
        pinMode(inout_pins[i], OUTPUT);
        digitalWrite(inout_pins[i], LOW);
      }
      if (readback != index) {
        if (Verb == 1) Serial.println("Check: I/O  pin  " + String(i, DEC) + " " + text + "PASS");
        pinMode(inout_pins[i], INPUT_PULLUP);
      } else {
        if (Verb == 1) Serial.println("Check: I/O  pin  " + String(i, DEC) + " " + text + "FAIL");
        pinMode(inout_pins[i], INPUT);
        failed++;
      }
    }
    if (Verb == 1) Serial.println();
  }
  if (Verb == 0) {
    if (failed == 0) Serial.println(F("Check pins PASS\n"));
    else Serial.println(F("Check pins FAIL\n"));
  }
}

void setAddress(int address) {
  for (int i = 0; i < sizeof(addr_pins); i++) {
    digitalWrite(addr_pins[i], ((address >> i) & 1));
    /*
    Serial.println();
    Serial.print("address  ");
    Serial.print(address);
    Serial.print(" address pin ");
    Serial.print(i);
    Serial.print(" arduino pin ");
    Serial.print(addr_pins[i]);
    Serial.print(" data ");
    Serial.println(((address >> i) & 1));
*/
  }
}

void writeData(uint8_t data) {

  digitalWrite(RW_pin, LOW);
  digitalWrite(CE_pin, LOW);

  digitalWrite(data_DI_pin, data);

  digitalWrite(RW_pin, HIGH);
  digitalWrite(CE_pin, HIGH);
}

int readData() {

  digitalWrite(RW_pin, HIGH);
  digitalWrite(CE_pin, LOW);
  delayMicroseconds(1);
  uint8_t readback = 0;

  readback = (digitalRead(data_DO_pin));

  digitalWrite(CE_pin, HIGH);
  return readback;
}

void test( int Verb) {
   Serial.println(F("Test Entire Memory"));

  int Addr, index;
  uint8_t Data[] = { 0x00, 0x01 };
  uint8_t readback;
  int tab = 0;
  int IC;

  PassFail[0] = 1;
  PassFail[1] = 1;
  PassFail[2] = 1;
  PassFail[3] = 1;

  for (IC = 1; IC <= 4; IC++) {
    for (index = 0; index <= 1; index++) {
      Serial.print("IC" + String(IC) + " Content");
      Serial.println(" after filling it with 0x" + String(Data[index], HEX));
      for (Addr = 0; Addr <= max_addr; Addr++) {
        if ((tab == 0) && (Verb == 1)) {
          if (Addr <= 0xF) Serial.print("00");
          else if (Addr <= 0xFF) Serial.print("0");
          Serial.print(Addr, HEX);
          Serial.print(" ");
        }
        setAddress(Addr);
        writeData(Data[index]);
        readback = readData();

        if (Verb == 1) {
          if (readback == Data[index]) Serial.print(" ");
          else {
            Serial.print("x");
            failed++;
            PassFail[IC - 1] = 0;
          }
        } else if (Verb == 0) {
          if (readback != Data[index]) {
            failed++;
            PassFail[IC - 1] = 0;
          }
        }
        if (Verb == 1) {
          if (readback <= 0xF) Serial.print("0");
          Serial.print(readback, HEX);
          tab++;
          if (tab == 32) {
            tab = 0;
            Serial.println();
          }
        }
      }
      if (Verb == 1) Serial.println();
    }
  }
  if (Verb == 0) Serial.println();
}

void loop() {
  checkPins();

  if (failed == 0) {
    digitalWrite(LEDPin, HIGH);
    unsigned long duration = millis();
    test(Verb);
    duration = millis() - duration;
    Serial.println("Duration: " + String(duration, DEC) + "mS");
  }

  for (int IC = 1; IC <= 4; IC++) {
    if (PassFail[IC - 1]) Serial.println("IC" + String(IC) + " PASS");
    else Serial.println("IC" + String(IC) + " FAIL");
  }

  if (failed > 0) {
    Serial.println("Result: FAIL (" + String(failed, DEC) + ")");
    digitalWrite(LEDPin, LOW);
  } else {
    Serial.println("Result: PASS");
  }
  unpowerChip();
  while (1)
    ;
}
