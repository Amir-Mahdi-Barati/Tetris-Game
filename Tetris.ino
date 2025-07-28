#include <LedControl.h>

constexpr int DIN_PIN = 11;
constexpr int CLK_PIN = 13;
constexpr int CS_PIN = 10;

constexpr int BTN_LEFT_PIN = 2;
constexpr int BTN_RIGHT_PIN = 3;
constexpr int BTN_ROTATE_PIN = 4;
constexpr int BTN_DROP_PIN = 5;

LedControl lc(DIN_PIN, CLK_PIN, CS_PIN, 1);

constexpr unsigned long FALL_INTERVAL = 700;
unsigned long lastFallTime = 0;

byte board[8] = {0}; // هر بایت نشان‌دهنده یک ردیف 8 خانه (1=پر، 0=خالی)

int currentPiece = 0;
int currentRotation = 0;
int currentX = 0;
int currentY = 0;

bool gameOver = false;

struct Tetromino {
  byte shapes[4][4]; // 4 حالت چرخش، هر حالت 4 ردیف 4 بیت
  int size;          // اندازه مربع قطعه
};

const Tetromino tetrominoes[7] = {
  // I
  { 
    {
      {B0000, B1111, B0000, B0000},  
      {B0010, B0010, B0010, B0010},  
      {B0000, B1111, B0000, B0000},  
      {B0010, B0010, B0010, B0010}   
    }, 4
  },
  // O
  {
    {
      {B0110, B0110, B0000, B0000},
      {B0110, B0110, B0000, B0000},
      {B0110, B0110, B0000, B0000},
      {B0110, B0110, B0000, B0000}
    }, 2
  },
  // T
  {
    {
      {B0100, B1110, B0000, B0000},
      {B0100, B0110, B0100, B0000},
      {B0000, B1110, B0100, B0000},
      {B0100, B1100, B0100, B0000}
    }, 3
  },
  // S
  {
    {
      {B0110, B1100, B0000, B0000},
      {B0100, B0110, B0010, B0000},
      {B0110, B1100, B0000, B0000},
      {B0100, B0110, B0010, B0000}
    }, 3
  },
  // Z
  {
    {
      {B1100, B0110, B0000, B0000},
      {B0010, B0110, B0100, B0000},
      {B1100, B0110, B0000, B0000},
      {B0010, B0110, B0100, B0000}
    }, 3
  },
  // J
  {
    {
      {B1000, B1110, B0000, B0000},
      {B0110, B0100, B0100, B0000},
      {B0000, B1110, B0010, B0000},
      {B0100, B0100, B1100, B0000}
    }, 3
  },
  // L
  {
    {
      {B0010, B1110, B0000, B0000},
      {B0100, B0100, B0110, B0000},
      {B0000, B1110, B1000, B0000},
      {B1100, B0100, B0100, B0000}
    }, 3
  }
};

unsigned long btnLastPressedTime[6] = {0};
constexpr unsigned long DEBOUNCE_DELAY = 150;

// دِباند و خواندن وضعیت دکمه‌ها
bool isButtonPressed(int pin) {
  static unsigned long lastPressTimes[20] = {0}; // اندازه 20 برای امنیت
  unsigned long now = millis();
  if (digitalRead(pin) == LOW) {
    if (now - lastPressTimes[pin] > DEBOUNCE_DELAY) {
      lastPressTimes[pin] = now;
      return true;
    }
  }
  return false;
}

void setup() {
  Serial.begin(9600);
  lc.shutdown(0, false);
  lc.setIntensity(0, 12);
  lc.clearDisplay(0);

  pinMode(BTN_LEFT_PIN, INPUT_PULLUP);
  pinMode(BTN_RIGHT_PIN, INPUT_PULLUP);
  pinMode(BTN_ROTATE_PIN, INPUT_PULLUP);
  pinMode(BTN_DROP_PIN, INPUT_PULLUP);

  randomSeed(analogRead(0)); // مقدار تصادفی بهتر

  resetGame();
}

void loop() {
  if (gameOver) {
    showGameOver();
    delay(2500);
    resetGame();
    return;
  }

  if (isButtonPressed(BTN_LEFT_PIN)) tryMove(currentX - 1, currentY, currentRotation);
  if (isButtonPressed(BTN_RIGHT_PIN)) tryMove(currentX + 1, currentY, currentRotation);
  if (isButtonPressed(BTN_ROTATE_PIN)) tryRotateWithWallKick();
  if (isButtonPressed(BTN_DROP_PIN)) dropPiece();

  if (millis() - lastFallTime > FALL_INTERVAL) {
    lastFallTime = millis();
    if (!tryMove(currentX, currentY + 1, currentRotation)) {
      placePiece();
      clearLines();
      spawnNewPiece();
    }
  }

  draw();
}

void resetGame() {
  memset(board, 0, sizeof(board));
  gameOver = false;
  spawnNewPiece();
}

void showGameOver() {
  for (int i=0; i<4; i++) {
    lc.clearDisplay(0);
    delay(300);
    for (int r=2; r<=5; r++) {
      for (int c=2; c<=5; c++) {
        lc.setLed(0,r,c,true);
      }
    }
    delay(300);
  }
}

void spawnNewPiece() {
  currentPiece = random(0, 7);
  currentRotation = 0;
  currentX = 2;
  currentY = 0;
  if (collision(currentX, currentY, currentRotation)) {
    gameOver = true;
  }
}

void draw() {
  lc.clearDisplay(0);
  for (int r=0; r<8; r++) {
    lc.setRow(0, r, board[r]);
  }
  drawPiece(currentX, currentY, currentRotation);
}

void drawPiece(int x, int y, int rotation) {
  const Tetromino &t = tetrominoes[currentPiece];
  for (int r = 0; r < t.size; r++) {
    byte rowBits = t.shapes[rotation][r];
    for (int c = 0; c < 4; c++) {
      if (bitRead(rowBits, 3 - c)) {
        int px = x + c;
        int py = y + r;
        if (px >= 0 && px < 8 && py >= 0 && py < 8) {
          lc.setLed(0, py, px, true);
        }
      }
    }
  }
}

bool collision(int x, int y, int rotation) {
  const Tetromino &t = tetrominoes[currentPiece];
  for (int r = 0; r < t.size; r++) {
    byte rowBits = t.shapes[rotation][r];
    for (int c = 0; c < 4; c++) {
      if (bitRead(rowBits, 3 - c)) {
        int px = x + c;
        int py = y + r;
        if (px < 0 || px >= 8 || py >= 8) return true;
        if (py >= 0 && (board[py] & (1 << (7 - px)))) return true;
      }
    }
  }
  return false;
}

bool tryMove(int x, int y, int rotation) {
  if (!collision(x, y, rotation)) {
    currentX = x;
    currentY = y;
    currentRotation = rotation;
    return true;
  }
  return false;
}

void tryRotateWithWallKick() {
  int newRotation = (currentRotation + 1) % 4;
  if (!collision(currentX, currentY, newRotation)) {
    currentRotation = newRotation;
    return;
  }
  if (!collision(currentX - 1, currentY, newRotation)) {
    currentX--;
    currentRotation = newRotation;
    return;
  }
  if (!collision(currentX + 1, currentY, newRotation)) {
    currentX++;
    currentRotation = newRotation;
    return;
  }
  // چرخش امکان‌پذیر نبود
}

void dropPiece() {
  while (tryMove(currentX, currentY + 1, currentRotation)) {
    currentY++;
  }
  placePiece();
  clearLines();
  spawnNewPiece();
}

void placePiece() {
  const Tetromino &t = tetrominoes[currentPiece];
  for (int r = 0; r < t.size; r++) {
    byte rowBits = t.shapes[currentRotation][r];
    for (int c = 0; c < 4; c++) {
      if (bitRead(rowBits, 3 - c)) {
        int px = currentX + c;
        int py = currentY + r;
        if (px >= 0 && px < 8 && py >= 0 && py < 8) {
          board[py] |= (1 << (7 - px));
        }
      }
    }
  }
}

void clearLines() {
  for (int r = 0; r < 8; r++) {
    if (board[r] == B11111111) {
      for (int rr = r; rr > 0; rr--) {
        board[rr] = board[rr-1];
      }
      board[0] = 0;
    }
  }
}
