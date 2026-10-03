#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <NintendoExtensionCtrl.h>

SNESMiniController snes;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128

// Colors
#define BLACK 0x0000
#define GREEN 0x07E0
#define RED 0xF800
#define WHITE 0xFFFF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define ORANGE 0xF360

// Arduino Uno SPI pin definitions
#define DC_PIN   8
#define CS_PIN   10
#define RST_PIN  7

#define BREADBOARD_BUTTON_PIN 2
#define LED_PIN 4

const unsigned short playerColor = WHITE;
const unsigned short enemyColor = GREEN;
const int defaultRoundness = 4;
const uint16_t screenBgColor = BLACK;
const uint16_t textDefaultColor = RED;

// TODO make an array of colors for enemy, and randomly assign enemy color.

// Initialize Adafruit SSD1351 over hardware SPI
Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

// Game variables
bool gameOver = false;
int groundPositionY = 96;
int defaultPlayerHeight = 20;

struct Player { 
  int positionX;
  int positionY;
  int width;
  int height;
  uint16_t color;
  bool isJumping;
  bool isAttacking;
} player = { 
  .positionX = 20, 
  .positionY = groundPositionY - defaultPlayerHeight, 
  .width = 10, 
  .height = defaultPlayerHeight, 
  .color = playerColor, 
  .isJumping = false, 
  .isAttacking = false 
};

struct Enemy {
  int positionX;
  int positionY;
  int width;
  int height;
  int speed;
  int clock;
  uint16_t color;
  int roundness;
} enemy = { 
  .positionX = 128, 
  .positionY = groundPositionY -enemy.height - 1, // 1px offset to draw above the ground line 
  .width = 8, 
  .height = 8, 
  .speed = 4, 
  .clock = 0, 
  .color = enemyColor, 
  .roundness = defaultRoundness 
};

struct Physics {
  float jumpVelocity;
  float gravity;
  float velocityModifier;
} physics = { 
  .jumpVelocity = 0, 
  .gravity = 1.3,
  .velocityModifier = -12.0 
};


void clearPlayerPreviousFrame () {
  tft.drawRect(player.positionX, player.positionY, player.width, player.height, BLACK);
}

void clearEnemyPreviousFrame () {
  tft.drawRoundRect(enemy.positionX, enemy.positionY, enemy.width, enemy.height, enemy.roundness, screenBgColor); 
}

bool isJumpButtonPressed (bool isJumpButtonPressed) {
  if (isJumpButtonPressed == true) {
    return true;
  }
  return false;
}

void isPlayerCollidingWithEnemy () {
  if (enemy.positionX < player.positionX + player.width && enemy.positionX + enemy.width > player.positionX &&
    enemy.positionY < player.positionY + player.height && enemy.positionY + enemy.height > player.positionY) {

    if (player.isAttacking) {
      clearEnemyPreviousFrame();
      enemy.positionX = 128 + enemy.width;
      return;
    }

    triggerGameOver();
  }
}

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

  pinMode(LED_PIN, OUTPUT);

	while (!snes.connect()) {
		Serial.println("Classic Controller not detected!");
		delay(1000);
	}

  tft.begin();
  tft.setTextSize(2);
  tft.fillScreen(screenBgColor); 
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
  
  // input buttons
  boolean yButton = snes.buttonY();
  boolean xButton = snes.buttonX();
  boolean aButton = snes.buttonA();
  boolean bButton = snes.buttonB();
  boolean startButton = snes.buttonStart();

  // Named button constants
  boolean actionButton = xButton;
  boolean jumpButton = aButton || bButton; // TODO use this

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

  // Handle Jump
  if (isJumpButtonPressed(jumpButton == true) && !player.isJumping) {
    player.isJumping = true;
    physics.jumpVelocity = physics.velocityModifier;
  }

  int playerSpeed = 2;

  // Handle movement
  if (padRight == true) {
    clearPlayerPreviousFrame();
    if (yButton == true) {
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
    if (yButton == true) {
      playerSpeed = playerSpeed * 3;
    }
    player.positionX = player.positionX - playerSpeed;

    // move off edge of screen, reappear on other edge 
    if (player.positionX <= 0 - player.width) {
      player.positionX = SCREEN_WIDTH + player.width;
    }
  }

  // Handle action button
  if (actionButton == true) {
    digitalWrite(LED_PIN, HIGH);
    player.color = RED;
    player.isAttacking = true;
  }
  else {
    digitalWrite(LED_PIN, LOW);
    player.color = WHITE;
    player.isAttacking = false;
  }


  // Update Player Position
  if (player.isJumping) {
    // Clear previous player frame
    clearPlayerPreviousFrame();
    player.positionY += physics.jumpVelocity;
    physics.jumpVelocity += physics.gravity;

    if (player.positionY >= groundPositionY) {
      player.positionY = groundPositionY - player.height - 1;
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
  if (player.isAttacking == true) {
    tft.drawRect(player.positionX, player.positionY, player.width, player.height, player.color); 
  }
  else {
    tft.drawRect(player.positionX, player.positionY, player.width, player.height, player.color); 
  }

  // Draw enemy 
  tft.fillRoundRect(enemy.positionX, enemy.positionY, enemy.width, enemy.height, enemy.roundness, enemy.color);

  // Collision Detection
  isPlayerCollidingWithEnemy();

  delay(20);
}

void triggerGameOver() {
  gameOver = true;
  tft.fillScreen(screenBgColor);
  tft.setCursor(15, 50);
  tft.setTextColor(textDefaultColor);
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
  tft.fillScreen(screenBgColor);
  gameOver = false;

  drawHUD();
}

