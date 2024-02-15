/*============================================================================================================================================================
Battleships game

LED:
Board:                   D13 D12 D11 D10 D9  D8
                         D1  A   F   D2  D3  B
Pins:                    12  11  10  9   8   7        
                         |   |   |   |   |   |
                -----------------------------------------
                |    D1       D2       D3       D4      |
                |   AAAAA    AAAAA    AAAAA    AAAAA    |
                |  F     B  F     B  F     B  F     B   |
                |  F     B  F     B  F     B  F     B   |
                |   GGGGG    GGGGG    GGGGG    GGGGG    |
                |  E     C  E     C  E     C  E     C   |
                |  E     C  E     C  E     C  E     C   |
                |   DDDDD DP DDDDD DP DDDDD DP DDDDD DP |
                |             5 6 4 1 A S               |
                -----------------------------------------
                         |   |   |   |   |   |
Pins:                    1   2   3   4   5   6
                         E   D   DP  C   G   D4
Board:                   D2  D3  D4  D5  D6  D7


Buttons:  Pin
Number    (Left)        (Left)                  (Right)            //It doesn't matter if we place them on Right/Left pin.
1         A0(Board)     (Resistor 10k)->GND     VCC(5V)            //Just place the resistor and the wire(to board) on the same side(Left/Right).
2         A1(Board)     (Resistor 10k)->GND     VCC(5V)            //Also you have to place all 3 of them on the same side(Top/Bottom).
3         A2(Board)     (Resistor 10k)->GND     VCC(5V)
4         A3(Board)     (Resistor 10k)->GND     VCC(5V)


Examples for buttons:

             (1)                                             (2)

          GND                                            
          |                                              
          10k     VCC(5V)                                
          |       |                                      
  A0------|       |                                       |       |
          |       |                                       |       |
          |   \   |                                       |   \   |
          |--  \--|                                       |--  \--|
          |       |                                       |       |
          |   O   |                                       |   O   |
          |       |                                       |       |
          |--  /--|                                       |--  /--|
          |   /   |                                       |   /   |---------A2
          |       |                                       |       |
          |       |                                       |       |
                                                        VCC(5V)   10k
                                                                  |
                                                                  GND


============================================================================================================================================================*/

//For debugging purposes
//==========================================================================
//If we are debugging, then there will be debugging messages in the Serial Monitor. If not, then nothing will be printed in the Serial Monitor.
//Also, we are using it to show the ships position and other data
#define DEBUG 1
#if DEBUG == 1
#define debug(x) Serial.print(x);
#define debugln(x) Serial.println(x);
#else
#define debug(x)
#define debugln(x)
#endif
//==========================================================================





//Define the pins values for the LED and the 4 buttons
//==========================================================================
//LED:
//Pins
#define pin1 2
#define pin2 3
#define pin3 4
#define pin4 5
#define pin5 6
#define pin6 7
#define pin7 13
#define pin8 12
#define pin9 11
#define pin10 10
#define pin11 9
#define pin12 8
//Segments
#define A pin11
#define B pin7
#define C pin4
#define D pin2
#define E pin1
#define F pin10
#define G pin5
#define DP pin3
#define SEGMENT_ON  HIGH
#define SEGMENT_OFF LOW
//Digits
#define D1 pin12
#define D2 pin9
#define D3 pin8
#define D4 pin6
#define DIGIT_ON  LOW
#define DIGIT_OFF  HIGH


//Buttons pins
#define B1 A0
#define B2 A1
#define B3 A2
#define B4 A3
//==========================================================================





//Global variables
//==========================================================================
//Characters to draw
byte alphabet[27] = {
  //We don't need all of the alphabet in this project so we won't set all of them.
  //ABCDEFGx          //x represents the segment DP. I named it x so I can fit it in 1 space
  0b11101110, //A
  0b00111110, //b
  0b10011100, //C
  0b01111010, //d
  0b10011110, //E
  0b10001110, //F
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000, //THESE ARE NOT SET
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00011110, //t
  0b00111000, //u
  0b00000001, //v can't be drawn
  0b00000001, //w can't be drawn
  0b00000001, //x can't be drawn
  0b01100110, //Y
  0b00000001, //z can't be drawn
  0b00000000, //TURN OFF ALL SEGMENTS
};
byte digits[11] = {
  //ABCDEFGx          //x represents the segment DP. I named it x so I can fit it in 1 space
  0b11111100, //0
  0b01100000, //1
  0b11011010, //2
  0b11110010, //3
  0b01100110, //4
  0b10110110, //5
  0b10111110, //6
  0b11100000, //7
  0b11111110, //8
  0b11110110, //9
  0b00000000, //TURN OFF ALL SEGMENTS
};

//Structures
struct position{ //This structure holds a digit and a segment. It is used to hold ships position
  int digit; //1-4
  char segment; //A-G
};

struct ship{ //This structure holds the position of a ship
  position pos;
};

struct player{ //This structure represents each player.
  int ships_left = 4; //This will decrement till 0, when the game is over
  ship ships[4];

  //This will increment till 4, when the Human player has 4 ships and the game starts.
  //Same for AI so we won't generate 2 ships on the same space
  int number_of_ships = 0;

  //The map where each player hit. If an element is true, the player can't attack again there.
  //The rows represent the digits and the columns represent the segment.
  //This also serves if the player already hit the specific ship
  bool alreadyHitSpace[4][7];
  int wins=0; // Optional, maybe we want to count the wins later.
};

player Human; //Human
player AI; //Robot
bool gameGenerated; //It holds if the game(AI ships) is generated.
bool humanTurn; //True if it's human's turn, false if it's AI's turn.
uint32_t seed; //randomSeed. It will be generated when the player pressed Button4 to start the game
uint32_t globalTime_millis = 0; //Time since the program started(milliseconds)
uint32_t lastPressTime; //Last time since the button was pressed(milliseconds)
uint32_t lastReleaseTime; //Last time since the button was released(milliseconds)
int lastButtonState;//It holds the last state of the button pressed(LOW/HIGH)
int buttonState; //It holds the current state of the button pressed(LOW/HIGH)
int hitAShip; //It holds if the player hit a ship or not. 0=The player didn't hit a ship, 1=The player hit a ship.
int currentDigitSelected; //The current selected digit: 1-4
char currentSegmentSelected; //The current selected segment: A-G
//==========================================================================





void setup() {
  setupTimer();
  setupPins();
  Serial.begin(9600);
  debugln();
  reset_game();
}
 
void loop() {
  generate_game();
  if (humanTurn) {
    humanAction();
    }
  else {
    AIAction();
  }
  showCurrentDigitAndSegmentSelected();
}





//Players action
//==========================================================================
void humanAction(){
  buttonAction(B1);
  buttonAction(B2);
  buttonAction(B3);
}

void AIAction(){
  AIAttacks();
}
//==========================================================================





//Attack logic for each player
//==========================================================================
void humanAttacks(){
  hitAShip = 0;
  //Human already hit this space
  if (Human.alreadyHitSpace[currentDigitSelected-1][int(currentSegmentSelected-'A')]){ //This also serves if the player already hit the specific ship
      debug("Human already hit this space: ");
      debug(currentDigitSelected);
      debugln(currentSegmentSelected);
  }
  //Human didn't hit this space yet
  else{
    for (int i=0; i<4; i++){
      //If Human hit a ship
      if(AI.ships[i].pos.digit == currentDigitSelected && AI.ships[i].pos.segment == currentSegmentSelected){
        debug("Human HIT a ship on this space: ");
        debug(currentDigitSelected);
        debugln(currentSegmentSelected);
        AI.ships_left--;
        Human.alreadyHitSpace[currentDigitSelected-1][int(currentSegmentSelected-'A')] = true;
        //If Human won
        if(AI.ships_left==0){
          debugln("Human won!");
          Human.wins++;
          reset_game();
        }
        //Else Human didn't won yet
        else {
          hitAShip = 1;
          Button4_action();
        }
        break;
      }
    }
    //If Human didn't hit any ship
    if(hitAShip == 0){
      debug("Human missed: ");
      debug(currentDigitSelected);
      debugln(currentSegmentSelected);
      Human.alreadyHitSpace[currentDigitSelected-1][int(currentSegmentSelected-'A')] = true;
      Button4_action();
    }
  }
}

void AIAttacks(){
  //Select a space that wasn't already attacked by the AI
  do{
    currentDigitSelected = random(1,5); //1-4
    currentSegmentSelected = 'A' + random(0,7);  //A-G
  }while(AI.alreadyHitSpace[currentDigitSelected-1][int(currentSegmentSelected-'A')]); //This also serves if the AI already hit the ship on this space.

  do{
    showCurrentDigitAndSegmentSelectedWhileBlinking();

    //If AI hit a ship
    if(digitalRead(B1) == HIGH){
      debug("AI HIT a ship on this space: ");
      debug(currentDigitSelected);
      debugln(currentSegmentSelected);
      Human.ships_left--;
      AI.alreadyHitSpace[currentDigitSelected-1][int(currentSegmentSelected-'A')] = true;
      if(Human.ships_left==0){
        debugln("AI won!");
        AI.wins++;
        reset_game();
        break;
      }
      else {
        hitAShip = 1;
        Button4_action();
        break;
      }
    }
    //else if AI didn't hit a ship
    else if(digitalRead(B2) == HIGH){
      debug("AI missed: ");
      debug(currentDigitSelected);
      debugln(currentSegmentSelected);
      AI.alreadyHitSpace[currentDigitSelected-1][int(currentSegmentSelected-'A')] = true;
      hitAShip = 0;
      Button4_action();
      break;
    }
  }while(digitalRead(B1) == LOW && digitalRead(B2) == LOW);
}

void response(){
  humanTurn = !humanTurn;

  turnOffAllDigitsExcept(4);
  lightDigit(hitAShip);

  //The response won't happen more than once(button bouncing) because after we switch the turn from one player to the other this method won't be called again(until it has to).
  while(digitalRead(B4) == LOW){
    if(digitalRead(B4) == HIGH) break;
  }while(digitalRead(B4) == HIGH){
    if(digitalRead(B4) == LOW) break;
  }

  if(humanTurn) {
    debugln("Human turn!");
    currentDigitSelected = 1;
    currentSegmentSelected = 'A';
  }
  else {
    debugln("AI turn!");
  }
}

void reset_game(){
  debugln("Press Button 4 to begin!");

  //Reset the players. We don't have to reset the AI ships because we will do that in generateAI(). As for the Human, we will set our ships on paper
  Human.ships_left = 4;
  AI.ships_left = 4;
  for (int i=0; i<4; i++){
    for (int j=0; j<7; j++){
      Human.alreadyHitSpace[i][j] = false;
      AI.alreadyHitSpace[i][j] = false;
    }
  }

  humanTurn = true; //True if it's human's turn, false if it's AI's turn.
  currentDigitSelected = 1; //The current selected digit: 1-4
  currentSegmentSelected = 'A'; //The current selected segment: A-G
  gameGenerated = false; //It holds if the game(AI ships) is generated.
  lastPressTime = 0; //Last time since the button was pressed(milliseconds)
  lastReleaseTime = 0; //Last time since the button was released(milliseconds)
  lastButtonState = LOW;//It holds the last state of the button pressed(LOW/HIGH)
  //We don't have to reset the buttonState, because it represents the current state of the button pressed/released
  //buttonState = LOW; //It holds the current state of the button pressed(LOW/HIGH)
}
//==========================================================================





//Generating the game(AI ships) and the seed, by Human input(Press Button4)
//==========================================================================
void generate_game(){
  while(!gameGenerated){
    start_message_press_button4();
    seed+=TCNT1;
    if (digitalRead(B4) == HIGH){
      gameGenerated = true;
      debug("Seed: ");
      debugln(seed);
      randomSeed(seed);
      turnOffAllDigits();
      generateAI();
    }
    //Update the time("globalTime_millis")
    cli();
    sei();
  }
}

void generateAI(){
  debugln("AI ships:");
  AI.number_of_ships=0;
  for (int i=0; i<4; i++){
    do{
      currentDigitSelected = random(1,5);
      currentSegmentSelected = 'A' + random(0,7);
    } while(AIShipAlreadyThere());
    AI.ships[i].pos.digit = currentDigitSelected;
    AI.ships[i].pos.segment = currentSegmentSelected;
    AI.number_of_ships++;
    debug("Ship ");
    debug(i);
    debug(": ");
    debug(AI.ships[i].pos.digit);
    debugln(AI.ships[i].pos.segment);

    uint32_t time = globalTime_millis;
    uint32_t rand = random(1,200);
    while((globalTime_millis - time) < rand){
      //Update the time("globalTime_millis")
      cli();
      sei();
    }

    seed+=TCNT1;
    randomSeed(seed);
  }
  currentDigitSelected = 1;
  currentSegmentSelected = 'A';
  debugln("Human turn!");
}

bool AIShipAlreadyThere(){
  for (int i=0; i<AI.number_of_ships; i++){
    if(AI.ships[i].pos.digit == currentDigitSelected && AI.ships[i].pos.segment == currentSegmentSelected){
        debug("AI already has a ship on this space: ");
        debug(currentDigitSelected);
        debugln(currentSegmentSelected);
        return true;
    }
  }
  return false;
}
//==========================================================================





//Buttons logic:
//==========================================================================
//We take as input the port for a specific button
//For example, if we take B2 as input, we will operate with Button2
void buttonAction(int button){
  //Firstly, we have to take the input of the button only 1 time.
  //The input is taken if the button was pressed for at least 50 ms, then released for at least 50ms
  //Basically, debouncing the button
  if ((globalTime_millis - lastPressTime) >= 50){
    buttonState = digitalRead(button);
    if(buttonState != lastButtonState){
      if((globalTime_millis-lastReleaseTime) >= 50){
        lastPressTime = globalTime_millis;
        lastButtonState = buttonState;
        if(buttonState == HIGH){
          //lastReleaseTime = globalTime_millis;
          if (button == B1) {
            Button1_action();
          }
          else if (button == B2) {
            Button2_action();
            }
          else if (button == B3) {
            Button3_action();
            }
        }
      }
      if(buttonState == HIGH) {
        lastReleaseTime = globalTime_millis;
      }
    }
  }
  else {
    lastReleaseTime = globalTime_millis;
  }
}

//Switch to next digit: 1-4. Button 1 is pressed too when the Human tells that the AI hit a ship, but the logic for that is not present here.
void Button1_action(){
  currentDigitSelected++;
  currentDigitSelected%=5;
  if(currentDigitSelected == 0) {
    currentDigitSelected++;
  }
  showCurrentDigitAndSegmentSelected();
}

//Switch to next segment: A-G. Button 2 is pressed too when the Human tells that the AI didn't hit a ship, but the logic for that is not present here.
void Button2_action(){
  currentSegmentSelected++;
  currentSegmentSelected = (currentSegmentSelected-'A')%7;
  currentSegmentSelected+='A';
  if(currentSegmentSelected == 0) {
    currentSegmentSelected++;
  }
  showCurrentDigitAndSegmentSelected();
}

//Human attacks
void Button3_action(){
  humanAttacks();
}

//Confirm the result of attacking a space by a player. Button 4 is pressed too when generating the game, but the logic for that is not present here
void Button4_action(){
  response();
}
//==========================================================================





//Printing on the LED methods
//==========================================================================
//A basic message to the player ("but4") that tells him to press Button 4 to start the game
void start_message_press_button4(){
  if(globalTime_millis%20<5){
    turnOffAllDigitsExcept(1);
    lightLetter('b');
  }
  else if(globalTime_millis%20<10){
    turnOffAllDigitsExcept(2);
    lightLetter('u');
  }
  else if(globalTime_millis%20<15){
    turnOffAllDigitsExcept(3);
    lightLetter('t');
  }
  else{
    turnOffAllDigitsExcept(4);
    lightDigit(4);
  }
}

//Lights (the current segment that was attacked by the AI) or (the current segment selected by Human that is going to be attacked when confirmed) on the LED.
void showCurrentDigitAndSegmentSelected(){
  turnOffAllDigitsExcept(currentDigitSelected);
  lightSegment(currentSegmentSelected);
}

//This method show the current digit and segment selected by the player, while blinking
void showCurrentDigitAndSegmentSelectedWhileBlinking(){
  if(globalTime_millis%500<250){
    showCurrentDigitAndSegmentSelected();
  }
  else {
    turnOffAllDigits();
  }
  //In case we are in a while loop whilst using this function we have to update(get) "globalTime_millis" variable.
  cli();
  sei();
}

void turnOffAllDigits(){
  digitalWrite(D1, DIGIT_OFF);
  digitalWrite(D2, DIGIT_OFF);
  digitalWrite(D3, DIGIT_OFF);
  digitalWrite(D4, DIGIT_OFF);
}

//Turns off all 4 digits, excepting the digit that we transfer
void turnOffAllDigitsExcept(int exception){
  turnOffAllDigits();
  switch (exception){
    case 1:
      digitalWrite(D1, DIGIT_ON);
      break;
    case 2:
      digitalWrite(D2, DIGIT_ON);
      break;
    case 3:
      digitalWrite(D3, DIGIT_ON);
      break;
    case 4:
      digitalWrite(D4, DIGIT_ON);
      break;
  }
}

//Lights (the current segment that was attacked by the AI) or (the current segment selected by Human that is going to be attacked when confirmed) on the LED.
//The digit where this happens is the variable "currentDigitSelected". For more details, see the method "showCurrentDigitAndSegmentSelected()".
void lightSegment(char SEGMENT) {
  digitalWrite(A, SEGMENT_OFF);
  digitalWrite(B, SEGMENT_OFF);
  digitalWrite(C, SEGMENT_OFF);
  digitalWrite(D, SEGMENT_OFF);
  digitalWrite(E, SEGMENT_OFF);
  digitalWrite(F, SEGMENT_OFF);
  digitalWrite(G, SEGMENT_OFF);
  digitalWrite(DP, SEGMENT_OFF);
  switch (SEGMENT){
    case 'A':
      digitalWrite(A, SEGMENT_ON);
      break;
    case 'B':
      digitalWrite(B, SEGMENT_ON);
      break;
    case 'C':
      digitalWrite(C, SEGMENT_ON);
      break;
    case 'D':
      digitalWrite(D, SEGMENT_ON);
      break;
    case 'E':
      digitalWrite(E, SEGMENT_ON);
      break;
    case 'F':
      digitalWrite(F, SEGMENT_ON);
      break;
    case 'G':
      digitalWrite(G, SEGMENT_ON);
      break;
    case '.':
      digitalWrite(DP, SEGMENT_ON);
      break;
  }
}

//Lights a digit on the LED
void lightDigit(int n) {
  //For n=10 we turn off all segments
  if (n<0 || n>9) {
    n=10;
  }
  digitalWrite(A, bitRead(digits[n],7));
  digitalWrite(B, bitRead(digits[n],6));
  digitalWrite(C, bitRead(digits[n],5));
  digitalWrite(D, bitRead(digits[n],4));
  digitalWrite(E, bitRead(digits[n],3));
  digitalWrite(F, bitRead(digits[n],2));
  digitalWrite(G, bitRead(digits[n],1));
  digitalWrite(DP, bitRead(digits[n],0));
}

//Lights a letter on the LED
void lightLetter(char c) {
  int n;
  //We will get the index of the character in the alphabet array. It is as following: 0=A, 1=B, 2=C, ..., 25=Z
  //Some letters can't be drawn on the digit and will be shown as .
  //If the character is not a letter we will turn off all segments.
  //If it's not a letter
  if(c<'A' && c>'Z' || c<'a' && c>'z') {
    n=26;
  }
  //else if it's a lowercase letter we convert it to an uppercase letter
  else if(c>='a' && c<='z') {
    c = c - 'a' + 'A';
    n = c - 'A';
  }
  else {
    n = c-'A';
  }
  digitalWrite(A, bitRead(alphabet[n],7));
  digitalWrite(B, bitRead(alphabet[n],6));
  digitalWrite(C, bitRead(alphabet[n],5));
  digitalWrite(D, bitRead(alphabet[n],4));
  digitalWrite(E, bitRead(alphabet[n],3));
  digitalWrite(F, bitRead(alphabet[n],2));
  digitalWrite(G, bitRead(alphabet[n],1));
  digitalWrite(DP, bitRead(alphabet[n],0));
}
//==========================================================================





//Basic setups for timer and pins
//==========================================================================
void setupTimer(){
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCCR1B |= (1 << WGM12);
  // Set CS10 bit for prescaler 1
  TCCR1B |= (1 << CS10);
 
  //Set threshhold value
  TCNT1=0;
  OCR1A=16000;
  TIMSK1 = (1 << OCIE1A);
  sei();//enable global interrupts
}

void setupPins(){
  //LED
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  pinMode(E, OUTPUT);
  pinMode(F, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(DP, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);

  //Buttons
  pinMode(B1, INPUT);
  pinMode(B2, INPUT);
  pinMode(B3, INPUT);
  pinMode(B4, INPUT);
}

ISR(TIMER1_COMPA_vect){
  TCNT1=0;
  globalTime_millis++;
}
//==========================================================================