# Arduino 8x8 LED Matrix Tetris Game

![🎮](https://em-content.zobj.net/thumbs/240/twitter/322/video-game_1f3ae.png) ![💡](https://em-content.zobj.net/thumbs/240/twitter/322/light-bulb_1f4a1.png)

**TETRIS on an LED Matrix? Yes, it's not just possible, it's awesome.** This is a complete, nerd-approved Tetris clone running on an 8x8 LED matrix driven by the MAX7219 chip and an Arduino UNO.

> "You haven't truly geeked out until you've written a game from 1984 on a display smaller than your smartwatch!"

---

## 🔧 What You Need (Hardware)

| Component          | Spec/Model              | Quantity  | Description                    |
| ------------------ | ----------------------- | --------- | ------------------------------ |
| Arduino            | UNO / Nano / Compatible | 1         | Your main controller brain 🧠  |
| LED Matrix         | MAX7219 8x8 Module      | 1         | Retro LED grid display 💡      |
| Push Buttons       | Any (momentary)         | 4         | For user control 🎮            |
| Resistors          | 10kΩ (Pull-up)          | 4         | To stabilize button inputs 🛡️ |
| Breadboard & Wires | -                       | As needed | For prototyping 🔌             |
| Power Supply       | USB/5V                  | 1         | Arduino power ⚡                |

---

## 🔌 Wiring Diagram

| Module Signal | Arduino Pin | Description           |
| ------------- | ----------- | --------------------- |
| DIN           | D11         | Data In to Matrix     |
| CLK           | D13         | Clock to Matrix       |
| CS            | D10         | Chip Select to Matrix |
| LEFT Button   | D2          | Move piece left       |
| RIGHT Button  | D3          | Move piece right      |
| ROTATE Button | D4          | Rotate the piece      |
| DROP Button   | D5          | Fast drop the piece   |

All buttons are configured with `INPUT_PULLUP`. Connect them between each pin and GND.

---

## 💻 Software Setup

### 1. Install Library

Install `LedControl` from Arduino Library Manager:

```
Sketch > Include Library > Manage Libraries > Search: LedControl
```

### 2. Upload Code

* Open Arduino IDE
* Paste the provided `.ino` code
* Connect board and choose correct COM port
* Upload and PLAY 🔁

---

## 🕹️ Controls

| Button      | Function                 |
| ----------- | ------------------------ |
| Left (D2)   | Move piece to the left   |
| Right (D3)  | Move piece to the right  |
| Rotate (D4) | Rotate piece (90 deg)    |
| Drop (D5)   | Instantly drop to bottom |

---

## 🎯 Game Mechanics

* Tetriminos fall every \~700ms (configurable)
* When a row is full, it gets cleared with visual effect
* Score increases based on lines cleared
* Game over occurs when new piece can't spawn

---

## 🌈 Features

* LED animations when clearing lines
* Smooth piece movement
* Debounced button presses
* Rotation handling near walls
* Optional difficulty ramping

---

## 🧠 Geek Tour of the Code

### 💡 Matrix Representation

Each 8x8 row is stored as an `int` with bits indicating LED state:

```
B10000001 = [X . . . . . . X]
```

You manipulate these using bitwise ops to draw tetriminos efficiently.

### 🧩 Pieces

Tetriminos are defined as 4x4 binary masks. Rotation is pre-baked into arrays, so no real-time matrix rotation needed.

### 📐 Collision Detection

Every move checks if the new piece overlaps with fixed blocks. Simple and blazing fast using AND masks.

### 🎮 Game Loop

```cpp
loop() {
  handleInput();
  if (timeToFall()) {
    movePieceDownOrFix();
  }
  drawBoard();
}
```

Modular, elegant, responsive.

### 🔥 Optimization Notes

* Debounced buttons
* Frame delay control
* LED brightness tweakable via `lc.setIntensity()`

---

## 🧪 Future Ideas

* ✅ High score EEPROM save
* ✅ Level system (faster drops)
* ✅ OLED screen for UI
* ✅ Sound effects via buzzer 🎵
* ✅ Multiplayer over serial! 🤯

---

## 👨‍💻 Made By

**Amir Mahdi Barati**
GitHub: [@Amir-Mahdi-Barati](https://github.com/Amir-Mahdi-Barati)
"Because blinking lights are better with purpose."

---

## 📜 License

This project is released under the [MIT License](https://opensource.org/licenses/MIT). Use it, fork it, improve it, make it blink harder!

---

> 🧠 *"Tetris was invented in 1984 on a Soviet mainframe. Now you’re running it on an 8x8 grid with 512 pixels total. That’s not just retro – that’s hacker art."*

---

Enjoy & have fun playing Tetris like a real microcontroller wizard 👾
