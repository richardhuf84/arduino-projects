#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 128

// Arduino Uno SPI pin definitions
#define SCLK_PIN 13
#define MOSI_PIN 11 // SDA
#define DC_PIN   8
#define CS_PIN   10
#define RST_PIN  7

#define BUTTON_PIN 2

// Initialize Adafruit SSD1351 over hardware SPI
Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

// Game variables
int playerX = 20;
int playerY = 96;
int playerSize = 10;
int scoreModifier = random(1, 1000);
bool isJumping = false;
float jumpVelocity = 0;
float gravity = 1.2;

int obsX = 128;
int obsY = 102;
int obsW = 12;
int obsH = 4;
int obsSpeed = 4;

int score = 0;
bool gameOver = false;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  tft.begin();
  tft.fillScreen(0x0000); // Black background
  drawHUD();
}

void loop() {
  if (gameOver) {
    if (digitalRead(BUTTON_PIN) == LOW) {
      resetGame();
      delay(200);
    }
    return;
  }

  // Handle Button Jump
  if (digitalRead(BUTTON_PIN) == LOW && !isJumping) {
    isJumping = true;
    jumpVelocity = -10.0;
  }

  // Update Player Position
  if (isJumping) {
    // Clear previous player frame
    tft.fillRect(playerX, playerY, playerSize, playerSize, 0x0000);
    playerY += jumpVelocity;
    jumpVelocity += gravity;

    if (playerY >= 96) {
      playerY = 96;
      isJumping = false;
    }
  }

  // Update Obstacle Position
  tft.fillRect(obsX, obsY, obsW, obsH, 0x0000); // Clear old obstacle
  obsX -= obsSpeed;

  if (obsX < -obsW) {
    obsX = 128;
    score++;
    obsSpeed = constrain(4 + (score / 3), 4, 9); // Increase speed over time
    updateScore();
  }

  // Draw Player and Obstacle
  tft.fillRect(playerX, playerY, playerSize, playerSize, 0xFFE0); // yellow player
  tft.fillRect(obsX, obsY, obsW, obsH, 0xF800); // Red obstacle

  // Collision Detection
  if (obsX < playerX + playerSize && obsX + obsW > playerX &&
      obsY < playerY + playerSize && obsY + obsH > playerY) {
    triggerGameOver();
  }

  delay(30);
}

void drawHUD() {
  int displayScore = score * scoreModifier;

  tft.drawFastHLine(0, 107, 128, 0xFFFF); // Ground line
  tft.setCursor(2, 2);
  tft.setTextColor(0xFFFF);
  tft.setTextSize(1);
  tft.print(F("Score: "));
  tft.print(displayScore);
}

void updateScore() {
  int displayScore = score * scoreModifier;
  tft.fillRect(40, 2, 40, 8, 0x0000);
  tft.setCursor(40, 2);
  tft.setTextColor(0xFFFF);
  tft.setTextSize(1);
  tft.print(displayScore);
}

void triggerGameOver() {
  gameOver = true;
  int displayScore = score * scoreModifier;

  tft.fillScreen(0x0000);
  tft.setCursor(15, 50);
  tft.setTextColor(0xF800);
  tft.setTextSize(2);
  tft.print(F("You Died"));
  
  tft.setCursor(20, 80);
  tft.setTextColor(0xFFE0);
  tft.setTextSize(1);
  tft.print(F("Try again"));

  tft.setCursor(30, 100);
  tft.setTextColor(0xFFFF);
  tft.setTextSize(1);
  tft.print(displayScore);
}

void resetGame() {
  score = 0;
  obsX = 128;
  obsSpeed = 4;
  playerY = 96;
  isJumping = false;
  gameOver = false;
  tft.fillScreen(0x0000);
  drawHUD();
}
