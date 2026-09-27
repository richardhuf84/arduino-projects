#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <NintendoExtensionCtrl.h>

SNESMiniController snes;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128

// Colors
#define BLACK 0x0000
#define WHITE 0xFFFF

const unsigned short playerColor = WHITE;

// Arduino Uno SPI pin definitions
#define SCLK_PIN 13
#define MOSI_PIN 11 // SDA
#define DC_PIN   8
#define CS_PIN   10
#define RST_PIN  7

#define BREADBOARD_BUTTON_PIN 2

// Initialize Adafruit SSD1351 over hardware SPI
Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

// Game variables
int groundPositionY = 96;
int defaultPlayerHeight = 20;

struct Player { 
  int positionX;
  int positionY;
  int width;
  int height;
  uint8_t color;
  bool isJumping;
} player = {.positionX = 20, .positionY = 96 - defaultPlayerHeight, .width = 10, .height = defaultPlayerHeight, .color = WHITE, .isJumping = false };

struct Physics {
  float jumpVelocity;
  float gravity;
} physics = { .jumpVelocity = 0, .gravity = 1.2 };

struct Enemy {
  int positionX;
  int positionY;
  int width;
  int height;
  int speed;
  int clock;
} enemy = { .positionX = 128, .positionY = groundPositionY -10 - 1, .width = 10, .height = 10, .speed = 4, .clock = 0 };

bool gameOver = false;

void drawHUD() {
  if (!gameOver) {
    // Ground line
    tft.drawFastHLine(0, groundPositionY, SCREEN_WIDTH, WHITE);
  }
}

void setup() {
  // SNES controller setup
  Serial.begin(115200);
	snes.begin();

	while (!snes.connect()) {
		Serial.println("Classic Controller not detected!");
		delay(1000);
	}
  tft.begin();
  tft.fillScreen(BLACK); 
}

void clearPlayerPreviousFrame () {
  tft.drawRect(player.positionX, player.positionY, player.width, player.height, BLACK);
}

void clearEnemyPreviousFrame () {
  tft.fillRoundRect(enemy.positionX, enemy.positionY, enemy.width, enemy.height, 4, BLACK); 
}

void loop() {
  drawHUD();

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

  int playerSpeed = 2;

  // Handle movement
  if (padRight == true) {
    clearPlayerPreviousFrame();
    if (bButton == true) {
      playerSpeed = playerSpeed * 3;
    }
    player.positionX = player.positionX + playerSpeed;

    // move off edge of screen, reappear on other edge 
    if (player.positionX >= SCREEN_WIDTH - player.width) {
      player.positionX = 0 - player.width;
    }
  }
  if (padLeft == true) {
    clearPlayerPreviousFrame();
    if (bButton == true) {
      playerSpeed = playerSpeed * 3;
    }
    player.positionX = player.positionX - playerSpeed;

    // move off edge of screen, reappear on other edge 
    if (player.positionX <= 0 - player.width) {
      player.positionX = SCREEN_WIDTH + player.width;
    }
  }

  // Update Player Position
  if (player.isJumping) {
    // Clear previous player frame
    clearPlayerPreviousFrame();
    player.positionY += physics.jumpVelocity;
    physics.jumpVelocity += physics.gravity;

    if (player.positionY >= 96) {
      player.positionY = 96 - player.height - 1;
      player.isJumping = false;
    }
  }

  // Update enemy position
  clearEnemyPreviousFrame();
  enemy.clock += 1;
  if (enemy.clock % 20) {
    enemy.positionX -= 1;
  }

  if (enemy.positionX < -enemy.width) {
    enemy.positionX = 128;
  }

  // Draw Player
  tft.drawRect(player.positionX, player.positionY, player.width, player.height, WHITE); 

  // Draw enemy 
  tft.fillRoundRect(enemy.positionX, enemy.positionY, enemy.width, enemy.height, 4, WHITE);

  // Collision Detection
  if (enemy.positionX < player.positionX + player.width && enemy.positionX + enemy.width > player.positionX &&
      enemy.positionY < player.positionY + player.height && enemy.positionY + enemy.height > player.positionY) {
    triggerGameOver();
  }

  delay(20);
}

void triggerGameOver() {
  gameOver = true;
  tft.fillScreen(BLACK);
  tft.setCursor(15, 50);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print(F("You Died"));
  
  tft.setCursor(15, 80);
  tft.setTextSize(1);
  tft.println("Press start to ");
  tft.setCursor(15, 90);
  tft.println("try again.");
}

void resetGame() {
  enemy.positionX = 128;
  enemy.speed = 4;
  enemy.clock = 0;
  player.positionY = groundPositionY - player.height;
  player.positionX = 0;
  player.isJumping = false;
  tft.fillScreen(BLACK);
  gameOver = false;

  drawHUD();
}

