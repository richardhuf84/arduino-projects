#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <NintendoExtensionCtrl.h>

SNESMiniController snes;

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 128

// Colors
#define DARKGREEN 0x03E0
#define PINK 0xF8FF
#define DARKCYAN 0x03EF
#define MAROON 0x7800
#define RED 0xF800
#define BLACK 0x0000


const unsigned short backgroundColor = DARKGREEN;
const unsigned short playerColor = DARKCYAN;
const unsigned short enemyColor = MAROON;

// Arduino Uno SPI pin definitions
#define SCLK_PIN 13
#define MOSI_PIN 11 // SDA
#define DC_PIN   8
#define CS_PIN   10
#define RST_PIN  7

// Useful for something?
#define BREADBOARD_BUTTON_PIN 2

// Initialize Adafruit SSD1351 over hardware SPI
Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

// Game variables
int groundPositionY = 96;


struct Player { 
  int positionX;
  int positionY;
  int width;
  int height;
  bool isJumping;
} player = {.positionX = 20, .positionY = 96, .width = 20, .height = 50, .isJumping = false };

struct Physics {
  float jumpVelocity;
  float gravity;
} physics = {.jumpVelocity = 0, .gravity = 1.2 };


struct Enemy {
  int positionX,
  positionY,
  width,
  height,
  speed;
} enemy = {.positionX = 128, .positionY = groundPositionY, .width = 20, .height = 20, .speed = 4 };

struct Enemy enemyA = {.positionX = 90, .positionY = groundPositionY };

bool gameOver = false;

void setup() {
  // SNES controller setup
  Serial.begin(115200);
	snes.begin();

	while (!snes.connect()) {
		Serial.println("Classic Controller not detected!");
		delay(1000);
	}
  tft.begin();
  tft.fillScreen(DARKGREEN); 
}

void loop() {
  // SNES loop
  boolean success = snes.update();  // Get new data from the controller

	if (success == true) {  
		snes.printDebug();
	}
	else {  
		Serial.println("Controller Disconnected!");
		delay(1000);
		snes.connect();
	}
  
  boolean aButton = snes.buttonA();
  boolean bButton = snes.buttonB();

  boolean startButton = snes.buttonStart();

  // D-pad
  boolean padUp = snes.dpadUp();
  boolean padDown = snes.dpadDown();
  boolean padLeft = snes.dpadLeft();
  boolean padRight = snes.dpadRight();

  // game loop
  if (gameOver) {
    if (startButton == true) {
      resetGame();
      delay(200);
    }
    return;
  }


  // Handle Button Jump
  if (aButton == true && !player.isJumping) {
    player.isJumping = true;
    physics.jumpVelocity = -10.0;
  }

  // Update Player Position
  if (player.isJumping) {
    // Clear previous player frame
    tft.fillRect(player.positionX, player.positionY, player.width, player.height, backgroundColor);
    player.positionY += physics.jumpVelocity;
    physics.jumpVelocity += physics.gravity;

    if (player.positionY >= 96) {
      player.positionY = 96;
      player.isJumping = false;
    }
  }

  // Update enemy position
  tft.fillRect(enemyA.positionX, enemyA.positionY, enemyA.width, enemyA.height, PINK); // Clear old obstacle
  enemyA.positionX -= enemyA.speed;

  if (enemyA.positionX < -enemyA.width)
    enemyA.positionX = 128;
  }

  // Draw Player
  tft.fillRect(player.positionX, player.positionY, player.width, player.height, playerColor); 

  // Draw enemyA 
  tft.fillRect(enemyA.positionX, enemyA.positionY, enemyA.width, enemyA.height, enemyColor);

  // Collision Detection
  if (enemyA.positionX < player.positionX + player.width && enemyA.positionX + enemyA.width > player.positionX &&
      enemyA.positionY < player.positionY + player.width && enemyA.positionY + enemyA.height > player.positionY) {
    triggerGameOver();
  }

  delay(30);
}

void drawHUD() {
  // Ground line
  tft.drawFastHLine(0, 107, 128, 0xFFFF); 
}

void triggerGameOver() {
  gameOver = true;

  tft.fillScreen(RED);
  tft.setCursor(15, 50);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.print(F("You Died"));
  
  tft.setCursor(20, 80);
  tft.setTextColor(0xFFE0);
  tft.setTextSize(1);
  tft.print(F("Press start to try again"));
}

void resetGame() {
  enemyA.positionX = 128;
  enemyA.speed = 4;
  player.positionY = 96;
  player.isJumping = false;
  gameOver = false;
  tft.fillScreen(backgroundColor);
  drawHUD();
}
