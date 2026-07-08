/*
  2x3 macropad matrix for Pro Micro (ATmega32U4)

  Rows: C6 -> pin 5,  D7 -> pin 6
  Cols: D1 -> pin 2,  D0 -> pin 3,  D4 -> pin 4
*/

#include <Keyboard.h>

const uint8_t rowPins[2] = {5, 6};       // rows: C6, D7
const uint8_t colPins[3] = {2, 3, 4};    // cols: D1, D0, D4
const uint8_t ROWS = 2, COLS = 3;       // defining matrix
const unsigned long DEBOUNCE_MS = 20;

bool prevPressed[ROWS][COLS] = {{false,false,false},{false,false,false}};
unsigned long lastChange[ROWS][COLS] = {{0,0,0},{0,0,0}};

// --- prototypes ---
static void pressCmdChar(char c);  //establish press and response key
static void pressCmdShiftChar(char c);
static void sendMacro(uint8_t r, uint8_t c);

void setup() {
  // rows as outputs, idle HIGH
  for (uint8_t r=0; r<ROWS; ++r) {
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], HIGH);
  }
  // columns as inputs with pullups (pressed = LOW)
  for (uint8_t c=0; c<COLS; ++c) {
    pinMode(colPins[c], INPUT_PULLUP);
  }
  Keyboard.begin();
}

void loop() {
  unsigned long now = millis();

  for (uint8_t r=0; r<ROWS; ++r) {
    // select one row LOW, others HIGH
    for (uint8_t rr=0; rr<ROWS; ++rr) {
      digitalWrite(rowPins[rr], (rr==r) ? LOW : HIGH);
    }

    delayMicroseconds(200); // settle

    for (uint8_t c=0; c<COLS; ++c) {
      bool pressed = (digitalRead(colPins[c]) == LOW);

      if (pressed != prevPressed[r][c]) {
        if (now - lastChange[r][c] >= DEBOUNCE_MS) {
          lastChange[r][c] = now;
          prevPressed[r][c] = pressed;
          if (pressed) sendMacro(r,c);   // fire on press edge
        }
      }
    }
  }
}

// -------- function bodies (after loop) --------
static void pressCmdChar(char ch) {
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(ch);
  delay(5);
  Keyboard.releaseAll();
}

static void pressCmdShiftChar(char ch) {
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press(ch);
  delay(5);
  Keyboard.releaseAll();
}

static void sendMacro(uint8_t r, uint8_t c) {
  if (r==0 && c==0) { pressCmdChar('c'); return; }      // ⌘C
  if (r==0 && c==1) { pressCmdChar('v'); return; }      // ⌘V
  if (r==0 && c==2) { pressCmdShiftChar('4'); return; } // ⌘⇧4
  if (r==1 && c==0) { pressCmdChar('t'); return; }      // ⌘T
  if (r==1 && c==1) { pressCmdChar('r'); return; }      // ⌘R
  if (r==1 && c==2) { pressCmdChar('z'); return; } // ⌘Z
}
