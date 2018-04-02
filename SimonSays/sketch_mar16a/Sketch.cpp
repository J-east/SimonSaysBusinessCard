/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

#include <ADCTouch.h>
//Beginning of Auto generated function prototypes by Atmel Studio
//End of Auto generated function prototypes by Atmel Studio

#define ROUNDS_TO_WIN      13 //Number of rounds to successfully remember before you win. 13 is do-able.
#define ENTRY_TIME_LIMIT   3000 //Amount of time to press a button before game times out. 3000ms = 3 sec

#define CHOICE_OFF      0 //Used to control LEDs
#define CHOICE_NONE     0 //Used to check buttons
// bitwise QoL operators
#define CHOICE_RED		(1 << 0)
#define CHOICE_GREEN	(1 << 1)
#define CHOICE_BLUE		(1 << 2)
#define CHOICE_YELLOW	(1 << 3)

// LED pin definitions
#define LED_RED     12
#define LED_GREEN   11
#define LED_BLUE    10
#define LED_YELLOW  9
  
// Button pin definitions
#define BUTTON_RED    A0
#define BUTTON_GREEN  A1
#define BUTTON_BLUE   A2
#define BUTTON_YELLOW A3


int ref0, ref1, ref2, ref3;       //reference values to remove offset
const int led =13;
byte gameBoard[32]; //Contains the combination of buttons as we advance
byte gameRound = 0; //Counts the number of successful rounds the player has made it through

byte Button0Val = 0;
byte Button1Val = 0;
byte Button2Val = 0;
byte Button3Val = 0;

boolean playMemory();
void playWinner();
void playLoser();
void addToMoves();
void playMoves();
byte waitForButton();
byte checkButton();
void setLEDs(byte);
void UpdateButtonVals();
void attractMode(void);

 
void setup() { 
    ref0 = ADCTouch.read(A0, 500);    //create reference values to
    ref1 = ADCTouch.read(A1, 500);      //account for the capacitance of the pad
	ref2 = ADCTouch.read(A2, 500);      //account for the capacitance of the pad
	ref3 = ADCTouch.read(A3, 500);      //account for the capacitance of the pad
    pinMode(9, OUTPUT);
	pinMode(10, OUTPUT);
	pinMode(11, OUTPUT);
	pinMode(12, OUTPUT);
    digitalWrite(9,LOW);
	digitalWrite(10,LOW);
	digitalWrite(11,LOW);
	digitalWrite(12,LOW);
}
 
void loop() { 
	for(int i = 0; i < 10; i++){
		UpdateButtonVals();
	}	

	attractMode();
	// Indicate the start of game play
	setLEDs(CHOICE_RED | CHOICE_GREEN | CHOICE_BLUE | CHOICE_YELLOW); // Turn all LEDs on
	delay(1000);
	setLEDs(CHOICE_OFF); // Turn off LEDs
	delay(250);

    // Play memory game and handle result
    if (playMemory() == true)
		playWinner(); // Player won, play winner tones
    else {
		playLoser(); // Player lost, play loser tones    
		delay(300);
	}
}

// Play the regular memory game
// Returns 0 if player loses, or 1 if player wins
boolean playMemory(void) {
	randomSeed(millis()+analogRead(A4)); // Seed the random generator with random amount of millis()
	gameRound = 0; // Reset the game to the beginning

	while (gameRound < ROUNDS_TO_WIN) {
		addToMoves(); // Add a button to the current moves, then play them back

		playMoves(); // Play back the current game board

		// Then require the player to repeat the sequence.
		for (byte currentMove = 0 ; currentMove < gameRound ; currentMove++) {
			byte choice = waitForButton(); // See what button the user presses
			if (choice == 0) 
				return false; // If wait timed out, player loses
			if (choice != gameBoard[currentMove]) 
				return false; // If the choice is incorrect, player loses
		}

		delay(1000); // Player was correct, delay before playing moves
	}

	return true; // Player made it through all the rounds to win!
}

// Plays the current contents of the game moves
void playMoves(void) {
	for (byte currentMove = 0 ; currentMove < gameRound ; currentMove++) {
		setLEDs(gameBoard[currentMove]); //Turn on a given LED
		delay(150);
		setLEDs(CHOICE_OFF); // Turn off all LEDs

		delay(150); // 150 works well. 75 gets fast.
	}
}

// Wait for a button to be pressed.
// Returns one of LED colors (LED_RED, etc.) if successful, 0 if timed out
byte waitForButton(void) {
	long startTime = millis(); // Remember the time we started the this loop

	// Loop until too much time has passed 
	while ( (millis() - startTime) < ENTRY_TIME_LIMIT) {
		UpdateButtonVals();
		byte button = checkButton();

		if (button != CHOICE_NONE) {
			setLEDs(button); //Turn on a given LED
			delay(150);
			setLEDs(CHOICE_OFF); // Turn off all LEDs
			
			while(checkButton() != CHOICE_NONE) {UpdateButtonVals();}  // Now let's wait for user to release button			
			delay(10); 

			return button;
		}
	}

	return CHOICE_NONE; // If we get here, we've timed out!
}

// Adds a new random button to the game sequence, by sampling the timer
void addToMoves(void) {
	byte newButton = random(0, 4); //min (included), max (excluded)

	// We have to convert this number, 0 to 3, to CHOICEs
	if(newButton == 0) 
		newButton = CHOICE_RED;
	else if(newButton == 1) 
		newButton = CHOICE_GREEN;
	else if(newButton == 2) 
		newButton = CHOICE_BLUE;
	else if(newButton == 3) 
		newButton = CHOICE_YELLOW;

	gameBoard[gameRound++] = newButton; // Add this new button to the game array
}

// in order to remove noise and mis presses we sample twice
int value0 = 0;
int value1 = 0;
int value2 = 0;
int value3 = 0;
void UpdateButtonVals(){
    value0 = ADCTouch.read(BUTTON_RED, 20);   //no second parameter
    value1 = ADCTouch.read(BUTTON_GREEN, 20);     //   --> 100 samples
    value2 = ADCTouch.read(BUTTON_BLUE, 20);     //   --> 100 samples
    value3 = ADCTouch.read(BUTTON_YELLOW, 20);     //   --> 100 samples
    
    value0 -= ref0;       //remove offset
    value1 -= ref1;
    value2 -= ref2;       //remove offset
    value3 -= ref3;

    if (value0 > 30)
	    Button0Val < 10 ? Button0Val++ :0;
    else 
	    Button0Val = 0;

    if (value1 > 30)
	    Button1Val < 10 ? Button1Val++ :0;
    else 
	    Button1Val = 0;

    if (value2 > 30)
	    Button2Val < 10 ? Button2Val++ :0;
    else 
	    Button2Val = 0;

    if (value3 > 30)
	    Button3Val < 10 ? Button3Val++ :0;
    else 
	    Button3Val = 0;
}

byte checkButton() {
	if (value0 > 9) 
		return(CHOICE_RED);
	else if (value1 > 9) 
		return(CHOICE_GREEN);
	else if (value2 > 9) 
		return(CHOICE_BLUE);
	else if (value3 > 9) 
		return(CHOICE_YELLOW);

	return(CHOICE_NONE); // If no button is pressed, return none	
}



// Lights a given LEDs
// Pass in a byte that is made up from CHOICE_RED, CHOICE_YELLOW, etc
void setLEDs(byte leds) {
	if ((leds & CHOICE_RED) != 0)
		digitalWrite(LED_RED, HIGH);
	else
		digitalWrite(LED_RED, LOW);

	if ((leds & CHOICE_GREEN) != 0)
		digitalWrite(LED_GREEN, HIGH);
	else
		digitalWrite(LED_GREEN, LOW);

	if ((leds & CHOICE_BLUE) != 0)
		digitalWrite(LED_BLUE, HIGH);
	else
		digitalWrite(LED_BLUE, LOW);

	if ((leds & CHOICE_YELLOW) != 0)
		digitalWrite(LED_YELLOW, HIGH);
	else
		digitalWrite(LED_YELLOW, LOW);
}

// Play the winner sound and lights
void playWinner(void) {
	setLEDs(CHOICE_GREEN | CHOICE_BLUE);
	delay(20);
	setLEDs(CHOICE_RED | CHOICE_YELLOW);
	delay(20);
	setLEDs(CHOICE_GREEN | CHOICE_BLUE);
	delay(20);
	setLEDs(CHOICE_RED | CHOICE_YELLOW);
	delay(20);
}

void playLoser(void) {
	setLEDs(CHOICE_RED | CHOICE_GREEN);
	delay(250);
	setLEDs(CHOICE_BLUE | CHOICE_YELLOW);
	delay(250);
	setLEDs(CHOICE_RED | CHOICE_GREEN);
	delay(250);
	setLEDs(CHOICE_BLUE | CHOICE_YELLOW);
	delay(250);
}

// Show an "attract mode" display while waiting for user to press button.
void attractMode(void) {
  while(1) {
	UpdateButtonVals();
    setLEDs(CHOICE_RED);
    delay(100);
    if (checkButton() != CHOICE_NONE) return;
	UpdateButtonVals();
    setLEDs(CHOICE_BLUE);
    delay(100);
    if (checkButton() != CHOICE_NONE) return;
	UpdateButtonVals();
    setLEDs(CHOICE_GREEN);
    delay(100);
    if (checkButton() != CHOICE_NONE) return;
	UpdateButtonVals();
    setLEDs(CHOICE_YELLOW);
    delay(100);
    if (checkButton() != CHOICE_NONE) return;
  }
}
