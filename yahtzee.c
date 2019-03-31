/*By Joel Hochstetter
Scores a round of yahtzee including rerolls*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int getInput(int array[], int min, int max, int range);
int chooseFromList();
int rerollChoice();
void scoreChoice(int numbersInput[]);
void scoreDice(int numbersInput[], int combos[]);
int mostInARow (int count[7]);
int rerollSome(int numbersInput[]);
int getDiceSum(int numbersInput[]);
void printDiceValues(int numbersInput[]);
int getSomeDice(int toReroll[], int numbersInput[]);

#define NUM_OF_DICE_ROLLED 5

//Used for enterring dice values
#define INCORRECT_FORMAT 0
#define CORRECT_INPUT 1
#define OUT_OF_RANGE 2
#define INCORRECT_NUM_OF_VALUES 3

#define TRUE 1
#define FALSE 0

//used for rerolling choice
#define REROLL_SOME 1
#define REROLL_ALL 2
#define KEEP_DICE 3
#define INVALID_CHOICE -1

//used in array of combinations
#define THREE_OF_KIND 0
#define FOUR_OF_KIND 1
#define FULL_HOUSE 2
#define SMALL_STRAIGHT 3
#define STRAIGHT 4
#define YAHTZEE 5
#define CHANCE 6


int main (int argc, char *argv[]) {
   int numbersInput[NUM_OF_DICE_ROLLED] = {0};
   int numRerolls = 0;
   int choice; //reroll choice
   printf("\nPlease enter dice values:\n");
   int isValidInput = getInput(numbersInput, 5, 5, 6);
   if (isValidInput == CORRECT_INPUT) {
      printDiceValues(numbersInput);
   }   
   while (numRerolls < 2 && isValidInput == CORRECT_INPUT) {
      choice = rerollChoice();
      if (choice == KEEP_DICE) {
         numRerolls = 2; //breaks loop condition
      } else if (choice == REROLL_ALL) {
         printf("\nPlease enter dice values:\n");
         isValidInput = getInput(numbersInput, 5, 5, 6);
      } else if (choice == REROLL_SOME){
         isValidInput = rerollSome(numbersInput);
      } else {
         printf("\nInvalid Choice\n");
         isValidInput = INVALID_CHOICE;
         numRerolls = 2; //breaks loop condition
      }
      if (isValidInput == CORRECT_INPUT && choice != KEEP_DICE) {
         printDiceValues(numbersInput); //to ensure not printing twice
      }
      numRerolls++;
   }
   if (isValidInput == CORRECT_INPUT) {
      scoreChoice(numbersInput);
   } else if (isValidInput == INCORRECT_FORMAT) {
      printf("\nIncorrect Input Format.\n");   
   } else if (isValidInput == OUT_OF_RANGE) {
      printf("\nValue Out of Range.\n");
   } else if (isValidInput == INCORRECT_NUM_OF_VALUES) {
      printf("\nIncorrect Number of Values.\n");
   }
   return EXIT_SUCCESS;
}

int getInput(int array[], int min, int max, int range) {
/*min and max are lowest and highest expected number of inputs
range is maximum number accepted as an input
If range is 5 then then the program is getting dice to reroll so
deals with inputs differently

Calling instructions:
Get 5 dice values: getInput(numbersInput, 5, 5, 6)
Get n dice values after reroll: getInput(newInputs, n, n, 6)
Get which dice to reroll: getInput(toReroll, 1, 5, 5) */
   range = range + '0'; //converts range to ascii
   int isValid = CORRECT_INPUT;
   int expectingNumber = TRUE; //Is number or space the expected input
   int numOfValidValues = 0; //number of valid values entered so far
   int ch = getchar();
   while(ch != '\n' && isValid != INCORRECT_FORMAT) {
   //^ incorrect input is the highest priority error
      if (ch >= '1' && ch <= range && expectingNumber == TRUE && 
      numOfValidValues < max) { /*if dice is in valid 
      range cases for expecting number and not expecting number*/
         if (range == '6') { //if for getting dice values
            array[numOfValidValues] = ch - '0';
            numOfValidValues++; //increments count for all inputs
         } else { //if for getting which dice to reroll
            if (array[ch - '1'] == 0) {
               array[ch - '1'] = 1;
               numOfValidValues++; //only increments count if new input  
            }   
         }
         expectingNumber = FALSE; //next valid input must be a space  
      } else if (ch != ' ' && ch != '\n' && (ch > '9' || ch < '0')) {
         isValid = INCORRECT_FORMAT;
      } else if (expectingNumber == FALSE && ch <= '9' && ch >= '0') {
         isValid = INCORRECT_FORMAT;
      } else if (ch == ' ') {
         expectingNumber = TRUE;
      } else if (expectingNumber == TRUE && 
      (ch == '0' || (ch >= range + 1 && ch <= '9'))) { 
      //no incorrect format yet so no if condition needed
            isValid = OUT_OF_RANGE;
      } else if (numOfValidValues == max) { 
      //^handles if 6th valid input is enterred
         if (isValid == CORRECT_INPUT) {
            isValid = INCORRECT_NUM_OF_VALUES;
         }
      }
      ch = getchar();
   }
   if (isValid == CORRECT_INPUT && (numOfValidValues < min 
   || numOfValidValues > max)) { //continued from ^
      isValid = INCORRECT_NUM_OF_VALUES;
   }
   return isValid;
}

int chooseFromList() {
   int userChoice = getchar();
   while (userChoice == ' ') { //accepts first non-space character
      userChoice = getchar();
   }
   int currentChar = userChoice;
   while (currentChar != '\n') { //clears the input buffer
      currentChar = getchar();
   }
   userChoice = userChoice - '0'; //gives the integer not ascii
   return userChoice;
}

int rerollChoice() {
   printf("\nPlease choose:\n");
   printf(" 1 -> Reroll some dice\n");
   printf(" 2 -> Reroll all dice\n");
   printf(" 3 -> Keep dice\n");
   int nextMoveChoice = chooseFromList();
   if (nextMoveChoice  > 3 || nextMoveChoice <= 0) {
      nextMoveChoice = INVALID_CHOICE;
   }
   return nextMoveChoice;
}

int rerollSome(int numbersInput[]) {
   printf("\nPlease enter dice to reroll (1 - 5):\n");
   int toReroll[6] = {0}; //stores 1 if dice will be rerolled. Else 0
   int isValid;
   isValid = getInput(toReroll, 1, 5, 5);
   if (isValid == INCORRECT_FORMAT) {
      printf("Incorrect Input Format.");
      isValid = INVALID_CHOICE; //ensures no output for rest of program
   } else if (isValid == OUT_OF_RANGE) {
      printf("Dice Number Out of Range.");
      isValid = INVALID_CHOICE; //ensures no output for rest of program
   } else {
      isValid = getSomeDice(toReroll, numbersInput);
   }
   return isValid;
}

void scoreChoice(int numbersInput[]) {
   int combos[7] = {0}; //stores # of pts for each type of combo
   //combos[i] is 0 if the combo isn't achieved
   scoreDice(numbersInput, combos);

   char scoreOut[7][20]; //array of possible outputs
   strcpy(scoreOut[THREE_OF_KIND], "Three of a Kind");
   strcpy(scoreOut[FOUR_OF_KIND], "Four of a Kind");
   strcpy(scoreOut[FULL_HOUSE], "Full House");
   strcpy(scoreOut[SMALL_STRAIGHT], "Small Straight");
   strcpy(scoreOut[STRAIGHT], "Straight");
   strcpy(scoreOut[YAHTZEE], "Yahtzee");
   strcpy(scoreOut[CHANCE], "Chance");

   int *optionIndex = malloc(sizeof(int) * 6);//creates array of size 6
   //this was done as some characters in scoreOut were being overriden
   if(optionIndex == NULL) {
      printf("Out of memory.\n");
      exit(EXIT_FAILURE); //copied from lecture slides. So can use this
   }
   /*stores the index of the combo at the address
   of the index displayed. E.g. If small straight is the first scoring
   type displayed then optionIndex[0] = SMALL_STRAIGHT , etc.  */
   printf("\nYour score options are:\n");
   int j = 1; //display index. So options shown 1, 2, 3, etc.
   int i;
   for (i = 0; i <= 6; i++){
      if (combos[i] > 0) {
         printf(" %d -> %s (%d points)\n", j, scoreOut[i], combos[i]);
         optionIndex[j] = i;
         j++;
      }  
   }
   int decision = chooseFromList();
   int points;
   if (decision > 0 && decision <= j - 1) {
      points = combos[optionIndex[decision]];
      printf("\nYour score is: %d points\n", points);
   } else {
      printf("\nInvalid Choice.\n");
   }
}

void scoreDice(int numbersInput[], int combos[]) {
   int count[7] = {0}; //1 is dice 1, 2 is dice 2, etc. 
   int numPairs = 0;  //used for keeping track of full house
   int triple = FALSE;
   int numOnCurrent; /*stores number of times most recently checked
   dice value has been rolled to easily update scoreOutions */
   int diceSum = getDiceSum(numbersInput);
   int diceValue;
   int i;
   for (i = 0; i < NUM_OF_DICE_ROLLED; i++){
      diceValue = numbersInput[i];
      count[diceValue] = count[diceValue] + 1;
      numOnCurrent = count[diceValue];
      if (numOnCurrent == 2) {
         numPairs++;
      } else if (numOnCurrent == 3) {
         combos[THREE_OF_KIND] = diceSum; 
         triple = TRUE;
      } else if (numOnCurrent == 4) {
         combos[FOUR_OF_KIND] = diceSum; 
      } else if (numOnCurrent == 5) {
         combos[YAHTZEE] = 50; 
      }
   }   
   combos[CHANCE] = diceSum; //assigns chance its value
   
   if (triple == TRUE && numPairs == 2) { /* checks if full house
   numPairs needs to be 2 because a triple is counted as a pair */
      combos[FULL_HOUSE] = 25;
   }
   if (numPairs <= 1 && triple == FALSE) {
      int inARow = mostInARow(count);
      if (inARow >= 4) {
         combos[SMALL_STRAIGHT] = 30;
         if (inARow == 5) {
            combos[STRAIGHT] = 40;   
         }
      }
   }
}

int mostInARow (int count[7]) {
   int inARow = 0;
   int i;
   for (i = 0; i <= 6; i++) {
      if (count[i] > 0) {
         inARow++;
      } else if (inARow < 4) {
         inARow = 0;
      } else {
         i = 7; //breaks loop if already have a run and reach a zero
      }
   }
   return inARow;
}

int getDiceSum(int numbersInput[]) {
   int sum = 0;
   int i;
   for (i = 0; i < NUM_OF_DICE_ROLLED; i++) {
      sum = sum + numbersInput[i];
   }
   return sum;
}

void printDiceValues(int numbersInput[]) {
   printf("\nYour dice values are:");
   int i;
   for (i = 0; i < NUM_OF_DICE_ROLLED; i++) {
      printf(" %d", numbersInput[i]);
   }
   printf("\n");
}

int getSomeDice(int toReroll[], int numbersInput[]) {
   int numDiceReroll = 0;
   int i;
   for (i = 0; i < NUM_OF_DICE_ROLLED; i++) {
      if (toReroll[i] == 1) {
         numDiceReroll++;
      }  
   }
   int newInputs[5] = {0}; //stores the values of the rerolled die
   printf("\nPlease enter %d ", numDiceReroll);
   if (numDiceReroll > 1) { //different output for plural
      printf("values:\n");
   } else {
      printf("value:\n");
   }
   int isValid = getInput(newInputs, numDiceReroll, numDiceReroll, 6);
   if (isValid == CORRECT_INPUT) {
      i = 0;
      int newCounter = 0; //counts number of elements replaced 
      while (i < NUM_OF_DICE_ROLLED) {
         if (toReroll[i] == 1) {
            numbersInput[i] = newInputs[newCounter];
            newCounter++;
         }  
         i++;
      }
   }
   return isValid;
}
