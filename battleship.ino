/*--------------------------------------------------------------------------
| Battleship game
| Made by Benjamin Bach Jensen
| Last modified 03-01-2018
|--------------------------------------------------------------------------*/

// Defining various pins
#define submitButton 18
#define buzzer 53

// Define joystick pins
#define joyButton 20
#define joy_X A6
#define joy_Y A7

// Define all 8x8 display rows
#define row_1 2
#define row_2 3
#define row_3 4
#define row_4 5
#define row_5 6
#define row_6 7
#define row_7 8
#define row_8 9
const int rows[8] = {row_1, row_2, row_3, row_4, row_5, row_6, row_7, row_8};

// Define all 8x8 display cols
#define col_1 10
#define col_2 11
#define col_3 12
#define col_4 13
#define col_5 A0
#define col_6 A1
#define col_7 A2
#define col_8 A3
const int cols[8] = {col_1, col_2, col_3, col_4, col_5, col_6, col_7, col_8};

// Defining ship struct
typedef struct {
  char *shipName;
  bool shipDirection; // true(1) = vertical, false(0) = horizontal
  int shipLength;
  int startPos[2];
  bool shipPlaced; // true(1), false(0)
} ship;
ship patrolBoat[2] = {
  {"Patrol Boat", true, 2, {0,0}, false},
  {"Patrol Boat", true, 2, {0,0}, false}
};
ship submarine[2] = {
  {"Submarine", true, 3, {0,0}, false},
  {"Submarine", true, 3, {0,0}, false}
};
ship battleship[2] = {
  {"Battleship", true, 4, {0,0}, false},
  {"Battleship", true, 4, {0,0}, false}
};
ship aircraftCarrier[2] = {
  {"Aircraft Carrier", true, 5, {0,0}, false},
  {"Aircraft Carrier", true, 5, {0,0}, false}
};

struct battleGround {
  int YCord[64];
  int XCord[64];
} grid[2];

// Globals variables
int noHit = 0;
int shipDestroyStatus[2][4] = {{0,0,0,0},{0,0,0,0}};
int pixelPos[8][8];
int turnCount = 0;
const int playerOne = 0;
const int playerTwo = 1;
bool joystickCheck = false;

/*--------------------------------------------------------------------------
| Helper functions
|--------------------------------------------------------------------------*/

// Clears the 8x8 display pixels
void clearDisplay() {
  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      pixelPos[x][y] = HIGH;
    }
  }
}

void refreshScreen() {
  for (int row = 0; row < 8; row++) {
    digitalWrite(rows[row], HIGH);
    for (int col = 0; col < 8; col++) {
      int thisPixel = pixelPos[row][col];
      digitalWrite(cols[col], thisPixel);
      if (thisPixel == LOW) {
        digitalWrite(cols[col], HIGH);
      }
    }
    digitalWrite(rows[row], LOW);
  }
}

int returnSomething(int val) {
    if (joystickCheck == false){
      joystickCheck = true;
      return val;
    } else {
      return 0;
    }
}

int joystickControl() {
  int xVal = analogRead(joy_X);
  int yVal = analogRead(joy_Y);
  int joystickButton = digitalRead(joyButton);
  int placeButton = digitalRead(submitButton);

  if (placeButton == 1) {
    if (joystickButton == 1) {
      if (xVal > 1000 - 100 && xVal < 1000 + 100 && yVal > 500 - 100 && yVal < 500 + 100)
        return returnSomething(1); // Detect up
      else if (xVal > 500 - 100 && xVal < 500 + 100 && yVal > 1000 - 100 && yVal < 1000 + 100)
        return returnSomething(2); // Detect right
      else if (xVal > 0 - 100 && xVal <  0 + 100 && yVal > 500 - 100 && yVal < 500 + 100)
        return returnSomething(3); // Detect bottom
      else if (xVal > 500 - 100 && xVal <  500 + 100 && yVal > 0 - 100 && yVal < 0 + 100)
        return returnSomething(4); // Detect left
      else {
        joystickCheck = false;
        return 0;
      }
    } else {
      return returnSomething(5);
    }
  } else {
    return returnSomething(6);
  }
}

void pressToStart() {
  clearDisplay();
  Serial.println("Welcome to Battleships");
  Serial.println("Please press the select button to start!");
  
  bool endLoop = false;
  while (endLoop == false) {
    switch (joystickControl()) {
      case 6:
        endLoop = true;
      break;
    }
  }
}

void renderSingleShip(ship& shipName) {
  int Y = shipName.startPos[0];
  int X = shipName.startPos[1];
  if (shipName.shipPlaced == 1) {
    if (shipName.shipDirection == true) {
      for (int j = 0; j < shipName.shipLength; j++)
        pixelPos[Y + j][X] = LOW;
    } else {
      for (int j = 0; j < shipName.shipLength; j++)
        pixelPos[Y][X + j] = LOW;
    }
  }
}

void renderShips(int player) {
  if (player == 0) {
    renderSingleShip(patrolBoat[0]);
    renderSingleShip(submarine[0]);
    renderSingleShip(battleship[0]);
    renderSingleShip(aircraftCarrier[0]);
  } else {
    renderSingleShip(patrolBoat[1]);
    renderSingleShip(submarine[1]);
    renderSingleShip(battleship[1]);
    renderSingleShip(aircraftCarrier[1]);
  }
}

void placeShip(ship& shipType, int player) {
  int limit = 8 - shipType.shipLength;
  char *shipName = shipType.shipName;
  int startY = 0;
  int startX = 0;
  int refreshOld = 0;
  bool isVertical = true;
  bool isPlaced = false;

  while (isPlaced == false) {
    switch (joystickControl()) {
      // Move ship up
      case 1:
        if (isVertical == true) {
          if (startY != limit) {
            startY = startY + 1;
          }
        } else {
          if (startY != 7) {
            startY = startY + 1;
          }
        }
        delay(50);
      break;
      // Move ship right
      case 2:
        if (isVertical == true) {
          if (startX != 7) {
            startX = startX + 1;
          }
        } else {
          if (startX != limit) {
            startX = startX + 1;
          }
        }
        delay(50);
      break;
      // Move ship down
      case 3:
        if (startY != 0) {
          startY = startY - 1;
        }
        delay(50);
      break;
      // Move ship left
      case 4:
        if (startX != 0) {
          startX = startX - 1;
        }
        delay(50);
      break;
      // Toogle isVertical
      case 5:
        if (isVertical == true) {
          if (startX > limit) {
            startX = limit;
          }
          isVertical = false;
        } else {
          if (startY > limit) {
            startY = limit;
          }
          isVertical = true;
        }
        delay(50);
      break;
      case 6:
        shipType.startPos[0] = startY;
        shipType.startPos[1] = startX;
        shipType.shipDirection = isVertical;
        shipType.shipPlaced = true;
        Serial.print("> ");
        Serial.print(shipName);
        Serial.println(" sucessfully placed!");
        
        isPlaced = true;
      break;
    }
    
    clearDisplay();
    
    if (isVertical == true) {
      for (int i = 0; i < shipType.shipLength; i++) {
         pixelPos[startY + i][startX] = LOW;
      }
    } else {
      for (int i = 0; i < shipType.shipLength; i++) {
         pixelPos[startY][startX + i] = LOW;
      }
    }

    // Render old ship placements on the map
    if (refreshOld > 9) {
      renderShips(player);
      refreshOld = 0;
    }
    
    refreshOld++;
    refreshScreen();
  }
}

void selectBombLocation(int player, int turnCount) {
  int playerNumber;
  if (player == 0)
    playerNumber = 1;
  else
    playerNumber = 2;
  
  Serial.println();
  Serial.print("Player ");
  Serial.print(playerNumber);
  Serial.println("'s turn");
  
  bool isPlaced = false;
  int startY = 0;
  int startX = 0;
  int refreshOld = 0;
  
  while (isPlaced == false) {
    switch (joystickControl()) {
      case 1: // Move pixel up
        if (startY != 7)
            startY = startY + 1;
        delay(50);
      break;
      case 2: // Move pixel right
        if (startX != 7)
          startX = startX + 1;
        delay(50);
      break;
      case 3: // Move pixel down
        if (startY != 0)
          startY = startY - 1;
        delay(50);
      break;
      case 4: // Move pixel left
        if (startX != 0)
          startX = startX - 1;
        delay(50);
      break;
      case 6:
        int inUse = 0;
        for (int i = 0; i < turnCount; i++) {
          int oldYCord = grid[player].YCord[i];
          int oldXCord = grid[player].XCord[i];
          
          if (oldYCord == startY && oldXCord == startX)
            inUse = 1;
        }
        
        if (inUse == 0) {
          grid[player].YCord[turnCount] = startY;
          grid[player].XCord[turnCount] = startX;
          
          int cords[2] = {startY,startX};
          scanForHit(player, cords);
          
          isPlaced = true;
        } else {
          Serial.println("> Already bombed!");
          tone(buzzer, 500, 300);
        }
      break;
    }
    clearDisplay();
    pixelPos[startY][startX] = LOW;
    
    // Render old placed bombs
    if (refreshOld > 10) {
      for (int i = 0; i < turnCount; i++) {
        int oldYCord = grid[player].YCord[i];
        int oldXCord = grid[player].XCord[i];
        pixelPos[oldYCord][oldXCord] = LOW;
      }
      refreshOld = 0;
    }

    refreshOld++;
    
    refreshScreen();
  }
}

int invertPlayer(int player) {
  if (player == 0)
    return 1;
  else
    return 0;
}

void scanForHit(int player, int cords[2]) {
  int otherPlayer = invertPlayer(player);
  
  scanForHitSingle(patrolBoat[otherPlayer], cords);
  scanForHitSingle(submarine[otherPlayer], cords);
  scanForHitSingle(battleship[otherPlayer], cords);
  scanForHitSingle(aircraftCarrier[otherPlayer], cords);
  
  noHit = 0;
}

void scanForHitSingle(ship& shipType, int cords[2]) {
  int shipYCord = cords[0];
  int shipXCord = cords[1];
  
  int YCord = shipType.startPos[0];
  int XCord = shipType.startPos[1];
  
  int shipLength = shipType.shipLength;
  int shipDirection = shipType.shipDirection;
  char *shipName = shipType.shipName;
  
  int YCordNew;
  int XCordNew;
  
  for (int i = 0; i < shipLength; i++) {
    if (shipDirection == 1){
      YCordNew = YCord + i;
      XCordNew = XCord;
    } else {
      XCordNew = XCord + i;
      YCordNew = YCord;
    }

    if (shipYCord == YCordNew && shipXCord == XCordNew) {
      tone(buzzer, 16, 500);
      Serial.print("> Enemy ");
      Serial.print(shipName);
      Serial.println(" got hit!");
      noHit--;
      continue;
    }      
  }

  noHit++;
  if (noHit == 4)
    Serial.println("> Splash!");
}

void checkGameState(int player, int turnCount) {
  int otherPlayer = invertPlayer(player);

  int ship1 = isDestroyed(patrolBoat[otherPlayer], player, turnCount);
  int ship2 = isDestroyed(submarine[otherPlayer], player, turnCount);
  int ship3 = isDestroyed(battleship[otherPlayer], player, turnCount);
  int ship4 = isDestroyed(aircraftCarrier[otherPlayer], player, turnCount);

  annonceDestruction(0, player, ship1, "Patrol Boat");
  annonceDestruction(1, player, ship2, "Submarine");
  annonceDestruction(2, player, ship3, "Battleship");
  annonceDestruction(3, player, ship4, "Aircraft Carrier");
  
  if (shipDestroyStatus[player][0] == 1 && shipDestroyStatus[player][1] == 1 && shipDestroyStatus[player][2] == 1 && shipDestroyStatus[player][3] == 1) {
    Serial.println(" ");
    Serial.print("Player ");
    Serial.print(player);
    Serial.print(" has destroyed all enemy ships, and has won the game!");
    Serial.println(" ");
    Serial.println("Please restart arduino!");
    while(true){}
  }

}

void annonceDestruction(int shipArray, int player, int shipStatus, char *shipName) {
  if (shipDestroyStatus[player][shipArray] == 0 && shipStatus == 1) {
    Serial.print("> Enemy ");
    Serial.print(shipName);
    Serial.println(" destroyed!");
    
    shipDestroyStatus[player][shipArray] = 1;
  }
}

int isDestroyed(ship& shipType, int player, int turnCount) {
  int hitCounter = 0;
  int YCord = shipType.startPos[0];
  int XCord = shipType.startPos[1];

  int shipLength = shipType.shipLength;
  int shipDirection = shipType.shipDirection;
  char *shipName = shipType.shipName;
  
  int YCordNew;
  int XCordNew;
        
  for (int i = 0; i < shipLength; i++) {
    if (shipDirection == 1){
      YCordNew = YCord + i;
      XCordNew = XCord;
    } else {
      XCordNew = XCord + i;
      YCordNew = YCord;
    }

    for (int j = 0; j < turnCount + 1; j++) {
      int placedYCord = grid[player].YCord[j];
      int placedXCord = grid[player].XCord[j];

      if (placedYCord == YCordNew && placedXCord == XCordNew)
        hitCounter++;
    }
  }

  if (shipLength == hitCounter)
    return 1;
  else
    return 0; 
}

/*--------------------------------------------------------------------------
| Run game setup
|--------------------------------------------------------------------------*/

void setup() {
  // Calling the serial monitor
  Serial.begin(9600);

  pinMode(submitButton, INPUT_PULLUP);
  pinMode(buzzer, INPUT);
  
  // Activating the Joystick pins
  pinMode(joyButton, INPUT);
  pinMode(joy_X, INPUT);
  pinMode(joy_Y, INPUT);

  // Activating all 8x8 display pins
  for (int i = 0; i < 8; i++) {
    pinMode(cols[i], OUTPUT);
    pinMode(rows[i], OUTPUT);
    digitalWrite(cols[i], HIGH);
  }

  pressToStart();

  // Player 1 places ships
  Serial.println();
  Serial.println("Player 1: Place your ships!");
  placeShip(patrolBoat[0], playerOne);
  placeShip(submarine[0], playerOne);
  placeShip(battleship[0], playerOne);
  placeShip(aircraftCarrier[0], playerOne);

  clearDisplay();
  delay(1000);
  
  // Player 2 places ships
  Serial.println();
  Serial.println("Player 2: Place your ships!");
  placeShip(patrolBoat[1], playerTwo);
  placeShip(submarine[1], playerTwo);
  placeShip(battleship[1], playerTwo);
  placeShip(aircraftCarrier[1], playerTwo);

  delay(1000);
}

/*--------------------------------------------------------------------------
| Run main game loop
|--------------------------------------------------------------------------*/
void loop() {
  
  // Player 1
  selectBombLocation(playerOne, turnCount);
  checkGameState(playerOne, turnCount);
  
  // Player 2
  selectBombLocation(playerTwo, turnCount);
  checkGameState(playerTwo, turnCount);

  turnCount++;
}
