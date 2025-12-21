/* 
 * File:   main.cpp
 * Author: Matthew Madrigal
 * Created on October 31st, 2025, 3:52 pm,
 * Purpose:  To create an Uno game in C++ using what has been learned so far. These are the rules used for this game: https://service.mattel.com/instruction_sheets/42001pr.pdf
 *           The blank cards are not included since they are for custom rules or replacing lost cards.
 */

//System Libraries
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
using namespace std;

//User Libraries
#include "Card.h"
#include "Game.h"
#include "GameS.h"
enum colors {RED, GREEN, YELLOW, BLUE, WILD};
enum types {ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, PLUST, REV, SKIP, PLUSF, CARD};

//Global Constants - Math/Science/Conversions only

//Function Prototypes
void printDeck(int, Card **, Card *);               // Prints what cards are in the array and what cards can be played.
string printCard(Card *);                           // Prints a card's color and type
Card **getPlayable(int, Card **, Card *, int &);    // Creates a new dynamically allocated array with the cards that can be played given a restriction card
bool challengeWin(Card *, Card **, int);            // Checks if a hand has a card with a matching color to the restriction. Needed for challenges
unsigned int calcPoints(Card **, int);              // Calculates the number of points a person earns for winning
int strToCol(string);                               // Converts a string to a color enum
int strToNm(string);                                // Converts a string to a type enum
void removeCard(int &, Card **, Card *);            // Removes a card given its pointer from an array and decreases its size by one
Card *playWild(int, Card **);                       // Asks the user what color to choose for the wild card
Card *drawCard(int &, Card **);                     // Draws a card from an array and returns it's pointer
Card **genColor();                                  // Generates the restriction cards used when a wild card is played. They only restrict by color
void copy(int, Card **, Card **);                   // Copys an array to another array
void addCard(int &, int &, Card ***, Card *);       // Adds a card to an array. If needed, it will grow the array size
void setupGame(Game &);                             // Sets up a new game
Card *playCard(Game &, bool , bool &, bool &);      // Asks the user what card to play and then returns it. Also allows the user to quit, call uno, or call a missed uno
Card *playCom(Game &, bool, bool &);                // Plays a random legal card and has a chance to call a missed uno
void playGame(Game &, int, Card **);                // The main game loop. Also handles special logic for cards such as +2, +4, and calls the playWild function when needed
Card **setupPile(Game &, fstream &);                // Gets a file and reads from it to fill the draw pile with the correct cards. Makes modifying rules a bit easier
Card **loadSave(Game &, GameS &, Card **, int);     // Loads the game state from a previous save if the user quit the game before it ended
void createSave(Game &, GameS &, Card **);          // Creates a save before the program terminates


//Execution Begins Here
int main(int argc, char** argv) {
    //Set random seed
    srand(time(0));
    //Declare Variables
    Game *game;
    GameS saved;
    Card **unique;  // Instead of creating duplicate cards when there is more than one type of card, a single card is created and that pointer is duplicated the number of times needed in the draw pile.
    Card **colors;  // Restriction cards for playing wild cards
    fstream setup;  // File used to setup the draw pile for a new game
    fstream save;   // File used to restore the state of the program from when it was last ran
    
    //Initialize Variables
    
    //The Process -> Map Inputs to Outputs
    // Checks if a save file exists and if the game didn't end yet
    save.open("save.data", ios::in | ios::out | ios::binary);
    if(save.is_open()) {
        cout << "Save data found. Resuming using save data." << endl;
        save.read(reinterpret_cast<char *>(&saved), sizeof(saved));
        if(saved.cpScore >= 500 || saved.plScore >= 500) {    // The game ends if someone scores 500 points. Create a new game if that happened
            cout << "Save data is from a completed game, creating new game." << endl;
            setup.open("start.txt", ios::in);
            game = new Game(setup);
            setup.close();
        } else {    // Load previous state for game
            game = new Game(saved);
        }
    } else {
        cout << "No save data found. Starting a new game." << endl;
        // Creates a save file
        save.close();
        save.open("save.data", ios::out | ios::binary);
        save.close();
        save.open("save.data", ios::in | ios::out | ios::binary);
        // Reads file for what cards to include in a new game
        setup.open("start.txt", ios::in);
        game = new Game(setup);
        setup.close();
    }

    //Display Inputs/Outputs
    // Gives instructions and plays game
    cout << "Welcome to Uno. When you are asked to play a card, you can either play one or draw. When you are going to have one card left, you must first type and enter uno before entering the card you wish to play. If the computer forgets to call uno, you can penalize them by entering uno before playing a card. To save and quit, type quit when asked to play a card." << endl;
    game->playGame();

    // Saves game state
    save.seekp(0L, ios::beg);
    game->createSave(saved);
    save.write(reinterpret_cast<char*>(&saved), sizeof(saved));

    //Exit the Program
    // Clean up memory
    delete game;
    save.close();
    return 0;
}

void printDeck(int size, Card **deck, Card *active) {
    cout << "Cards in hand: ";
    for(int i = 0; i < size; i++) {
        cout << printCard(deck[i]) << "; ";     // Prints all the cards in the array
    }
    cout << endl;
    Card **play;
    int playS;
    play = getPlayable(size, deck, active, playS);
    cout << "Playable cards in hand: ";
    for(int i = 0; i < playS; i++) {
        cout << printCard(play[i]) << "; ";     // Prints all playable cards
    }
    cout << endl;
    delete []play;      // Frees memory
}

string printCard(Card *card) {
    string display;
    // Converts the color enum into a human readable string
    switch(card->color) {
        case RED:
        display += "Red ";
        break;
        case GREEN:
        display += "Green ";
        break;
        case YELLOW:
        display += "Yellow ";
        break;
        case BLUE:
        display += "Blue ";
        break;
        case WILD:
        display += "Wild ";
        break;
        default:
        break;
    }
    // Converts the type enum into a human readable string
    switch(card->name) {
        case ZERO:
        case ONE:
        case TWO:
        case THREE:
        case FOUR:
        case FIVE:
        case SIX:
        case SEVEN:
        case EIGHT:
        case NINE:
        display += to_string(card->name);
        break;
        case PLUST:
        display += "Draw Two";
        break;
        case PLUSF:
        display += "Draw Four";
        break;
        case REV:
        display += "Reverse";
        break;
        case SKIP:
        display += "Skip";
        break;
        case CARD:
        default:
        display += "Card";
        break;
    }
    return display;

}

Card **getPlayable(int size, Card **deck, Card *active, int &plSize) {
    Card **play;                        // Playable cards
    int playS = 0;                      // Playable cards size
    for(int i = 0; i < size; i++) {     // Calculates how large the new array needs to be to hold the playable cards
        Card *card = deck[i];
        if(card->color == WILD || card->color == active->color || card->name == active->name) {
            playS++;
        }
    }
    play = new Card*[playS];            // Creates the array with the correct size
    plSize = playS;
    int plIndex = 0;
    for(int i = 0; i < size; i++) {
        if(plIndex >= playS) {          // Ends loop early when it should be done
            break;
        }
        Card *card = deck[i];
        // Adds all playable cards to the list
        if(card->color == WILD || card->color == active->color || card->name == active->name) {
            play[plIndex] = card;
            plIndex++;
        }
    }
    return play;
}

bool challengeWin(Card *check, Card **deck, int size) {
    for(int i = 0; i < size; i++) {
        Card *card = deck[i];
        if(check->color == card->color) {       // If there was a playable card with a matching color, then the challenger wins
            return true;
        }
    }
    return false;
}

unsigned int calcPoints(Card **deck, int size) {
    unsigned int points = 0;
    for(int i = 0; i < size; i++) {
        Card *card = deck[i];
        switch(card->name) {
            case ZERO:
            case ONE:
            case TWO:
            case THREE:
            case FOUR:
            case FIVE:
            case SIX:
            case SEVEN:
            case EIGHT:
            case NINE:
            points += card->name;   // Cards 0-9 are valued at their number
            break;
            case PLUST:
            case REV:
            case SKIP:
            points += 20;           // Special non wild cards are valued at 20 points each
            break;
            case CARD:
            case PLUSF:
            points += 50;           // Wild cards (type CARD and PLUSF will always be a wild) are valued at 50 points each
            break;
            default:
            break;
        }
    }
    return points;
}

Card *playCard(Game &game, bool uno, bool &unoed, bool &quit) {
    bool choosen = false;
    Card *usrCard = nullptr;
    while(!choosen) {                               // This forces the user to play a valid card before continuing
        int validS;
        Card **valid = getPlayable(game.handS, game.hand, game.rest, validS);
        if(validS != 0 || game.drwSize != 0) {      // If its possible for the player to do something, allow them to play
            cout << "The computer has " << game.bHandS << " cards" << endl;
            printDeck(game.handS, game.hand, game.rest);
            cout << "Enter the card you would like to play (Type it as listed)" << ((game.drwSize > 0) ?" or type draw to draw a card" : "") << (uno? ". Since the computer didn't call uno, you can type uno to force them to draw" : "") << ": ";
            string input, wordF, wordS;
            getline(cin, input);                    // Gets the card the player wants to play
            bool next = false;
            for(int i = 0; i < input.length(); i++) {
                input[i] = tolower(input[i]);
            }
            if(input == "quit") {                   // Special case for quiting the game
                quit = true;
                delete []valid;
                return nullptr;
            }
            if(input == "draw") {                   // Special case for drawing from the pile
                usrCard = drawCard(game.drwSize, game.drwPile);
                if(usrCard == nullptr) {            // In case the user draws a card when there is nothing to draw (asks what to do again)
                    cout << "There are no cards to draw." << endl;
                } else {                            // Tell the user what card they got. If its playable, ask if they want to play it
                    cout << "You got a " << printCard(usrCard) << endl;
                    if((usrCard->color == WILD) || (usrCard->color == game.rest->color || usrCard->name == game.rest->name)) {
                        cout << "Would you like to play it? (yes or No)" << endl;
                        string toPlay;
                        while (toPlay != "yes" && toPlay != "no") {
                            getline(cin, toPlay);
                            int strSize = toPlay.size();
                            char *playCSt = new char[strSize];    // This isn't needed but I am using it to make sure I use a cstring at least once
                            strncpy(playCSt, toPlay.c_str(), strSize);
                            for(int i = 0; i < strSize; i++) {
                                playCSt[i] = tolower(playCSt[i]);
                            }
                            if(strcmp(playCSt, "yes") != 0 && strcmp(playCSt,"no") != 0) {
                                cout << "Invalid input" << endl;
                            }
                            delete []playCSt;
                        }
                        if(toPlay != "yes") {
                            addCard(game.handS, game.handM, &game.hand, usrCard);
                            usrCard = nullptr;
                        }
                    } else {    // Add to hand if not playable
                        addCard(game.handS, game.handM, &game.hand, usrCard);
                        usrCard = nullptr;
                    }
                    choosen = true;
                }
            } else if(input == "uno") { // Handles uno logic
                if(!uno) {  // Checks if it was entered to catch the computer or protect themself
                    if(game.handS == 2 && validS >= 1) {    // Makes sure they can call uno
                        unoed = true;
                        cout << "You called uno!" << endl;
                    } else {
                        cout << "You can't call uno yet!" << endl;
                    }
                } else {    // Makes the computer draw 4 cards
                    cout << "The computer will draw 4 cards" << endl;
                    int drawnC = 0;
                    for(int i = 0; i < 4; i++) {
                        Card *drawn = drawCard(game.drwSize, game.drwPile);
                        if(drawn != nullptr) {
                            drawnC++;
                            addCard(game.bHandS, game.bHandM, &game.bHand, drawn);
                        }
                    }
                    if(drawnC != 4) {
                        cout << "The computer could only draw " << drawnC << " cards." << endl;
                    }
                    if(game.drwSize == 0) {
                        cout << "The draw pile is now empty." << endl;
                    }
                }
            }else { // Splits the input into two for parsing of card color and type
                for(int i = 0; i < input.length(); i++) {
                    if(input[i] == ' ' && !next) {
                        next = true;
                    } else {
                        if(!next) {
                            wordF += input[i];
                        } else {
                            wordS += input[i];
                        }
                    }
                }
                int color, name;
                color = strToCol(wordF);
                name = strToNm(wordS);
                if(color != -1 && name != -1) { // Tries to find the card in the playable array. Removes it if found and returns it
                    for(int i = 0; i < validS; i++) {
                        Card *card = valid[i];
                        if(card->color == color && card->name == name) {
                            choosen = true;
                            usrCard = card;
                            removeCard(game.handS, game.hand, card);
                            break;
                        }
                    }
                }
                if(!choosen) {
                    cout << "Can not play card" << endl;
                }
            }
        } else {    // Fail safe if there are no cards playable and the draw pile is empty
            cout << "You have no playable cards and the draw pile was empty. Your turn will be skipped." << endl;
            choosen = true;
        }
        delete []valid;
    }
    return usrCard;
    
}

int strToCol(string input) {
    if(input == "red") {
        return RED;
    }
    if(input == "green") {
        return GREEN;
    }
    if(input == "yellow") {
        return YELLOW;
    }
    if(input == "blue") {
        return BLUE;
    }
    if(input == "wild") {
        return WILD;
    }
    return -1;
}

int strToNm(string input) {
    if(input.length() == 1 && isdigit(input[0])) {
        return static_cast<int>(input[0] - '0');
    } else {
        if(input == "draw two") {
            return PLUST;
        }
        if(input == "draw four") {
            return PLUSF;
        }
        if(input == "reverse") {
            return REV;
        }
        if(input == "skip") {
            return SKIP;
        }
        if(input == "card") {
            return CARD;
        }
    }
    return -1;
}

void removeCard(int &size, Card **hand, Card *card) {
    int index = 0;      // Finds the first instance of a card with a matching pointer and removes it
    for(int i = 0; i < size; i++) {
        if(hand[i] == card) {
            index = i;
            break;
        }
    }
    for(int i = index+1; i < size; i++) {
        hand[i-1] = hand[i];
    }
    size--;
}

Card *playWild(int size, Card **colors) {
    bool picked = false;
    Card *cardR;    // Card used to restrict to a color
    while(!picked) {    // keeps looping until a color is obtained
        cout << "Choose a color (Red, Green, Yellow, Blue): ";
        string input;
        getline(cin, input);
        for(int i = 0; i < input.length(); i++) {   // Lower case for easier parsing
            input[i] = tolower(input[i]);
        }
        int color;
        color = strToCol(input);
        if(color != -1 && color != WILD) {
            for(int i = 0; i < size; i++) {
                Card *card = colors[i];
                if(card->color == color) {
                    picked = true;
                    cardR = card;
                    break;
                }
            }
        } else {
            cout << "Invalid input" << endl;
        }
    }
    return cardR;
}

Card *drawCard(int &size, Card **draw) {
    if(size == 0) {
        return nullptr;
    }
    int index = rand() % size;
    Card *card = draw[index];
    removeCard(size, draw, card);
    return card;
}

Card **genColor() {
    Card **colors = new Card*[4];
    colors[0] = new Card{RED, CARD};
    colors[1] = new Card{GREEN, CARD};
    colors[2] = new Card{YELLOW, CARD};
    colors[3] = new Card{BLUE, CARD};
    return colors;
}

void copy(int size, Card **arr1, Card **arr2) {
    for(int i = 0; i < size; i++) {
        arr2[i] = arr1[i];
    }
}

void addCard(int &size, int &maxSize, Card ***hand, Card *card) {
    if(size >= maxSize) {   // If the array is not large enough, replace it with a bigger one
        maxSize += 10;
        Card **handN = new Card*[maxSize];
        copy(size, *hand, handN);
        delete [](*hand);
        *hand = handN;
    }
    (*hand)[size] = card;
    size++;
}

void setupGame(Game &game) {
    const int start = 7;    // The number of cards each player should start with
    game.handS = start;
    if(game.hand == nullptr) {          // Prevents accidental memory leaks when called after a previous game
        game.handM = start;
        game.hand = new Card*[start];
    }
    game.bHandS = start;
    if(game.bHand == nullptr) {         // Prevents accidental memory leaks when called after a previous game
        game.bHandM = start;
        game.bHand = new Card*[start];
    }
    for(int i = 0; i < start; i++) {    // Fills the computer's and player's hand with cards
        Card *card = drawCard(game.drwSize, game.drwPile);
        game.hand[i] = card;
        card = drawCard(game.drwSize, game.drwPile);
        game.bHand[i] = card;
    }
    Card *card = drawCard(game.drwSize, game.drwPile);
    while(card->color == WILD) {        // If the first played card is a wild, return it to the draw pile and draw another
        addCard(game.drwSize, game.drwMax, &game.drwPile, card);
        card = drawCard(game.drwSize, game.drwPile);
    }
    game.lastPl = card;
    game.rest = card;
    game.turn = true;
}

Card *playCom(Game &game, bool uno, bool &unoed) {
    if(uno) {   // If the player forgot to call uno, there is a 50% change the computer catches it
        if(rand()%2 == 1) {
            cout << "The computer caught you with one card!" << endl;
            int drawnC = 0;
            for(int i = 0; i < 4; i++) {
                Card *drawn = drawCard(game.drwSize, game.drwPile);
                if(drawn != nullptr) {
                    drawnC++;
                    addCard(game.handS, game.handM, &game.hand, drawn);
                }
            }
            if(drawnC != 4) {
                cout << "You could only draw " << drawnC << " cards." << endl;
            }
            if(game.drwSize == 0) {
                cout << "The draw pile is now empty." << endl;
            }
        }
    }
    Card *usrCard = nullptr;
    int validS;
    Card **valid = getPlayable(game.bHandS, game.bHand, game.rest, validS);
    if(validS != 0 || game.drwSize != 0) {  // If the computer can do something
        if(validS != 0) {                   // If the computer doesn't have to draw, play a card
            int index = rand()%validS;
            usrCard = valid[index];
            removeCard(game.bHandS, game.bHand, usrCard);
            cout << "The computer played the card " << printCard(usrCard) << endl;
        } else {
            cout << "The computer drawed a card" << endl;
            Card *drawn = drawCard(game.drwSize, game.drwPile);
            addCard(game.bHandS, game.bHandM, &game.bHand, drawn);
        }
        if(game.bHandS == 1 && rand()%2 == 1) { // There is a 50% chance the computer remembers to call uno. I know this computer is really dumb
            cout << "The computer called uno!" << endl;
            unoed = true;
        }
    } else {    // If the computer can't play anything and for some reason the draw pile is completly empty, skip their turn
        cout << "The computer couldn't play anything and the draw pile was empty." << endl;
        unoed = true;   // Just incase
    }
    delete []valid;
    return usrCard;
}

void playGame(Game &game, int colorS, Card **colors) {
    bool uno = false, unoed = false, quit = false;  // Used to keep track of things in between turns
    while(game.plScore < 500 && game.cpScore < 500) {   // A game ends when someone has 500 points or more
        Card *card;
        Card *res;
        if(game.turn) {
            cout << "Current card at play: " << printCard(game.lastPl) << endl;
            card = playCard(game, uno, unoed, quit);
            if(quit) {  // Allows the player to exit the game
                break;
            }
            res = card;
            game.turn = false;
            if(card != nullptr) {   // This is to catch when the player draws a card and doesn't play it (or if they couldn't do anything)
                // The following if statements handle the special cards (wild and draw cards)
                if(card->color == WILD) {
                    res = playWild(colorS, colors);
                }
                if(card->name == PLUST) {
                    cout << "The computer will draw 2 cards now" << endl;
                    game.turn = true;
                    int drawnC = 0;
                    for(int i = 0; i < 2; i++) {
                        Card *drawn = drawCard(game.drwSize, game.drwPile);
                        if(drawn != nullptr) {
                            drawnC++;
                            addCard(game.bHandS, game.bHandM, &game.bHand, drawn);
                        }
                    }
                    if(drawnC < 2) {
                        cout << "Only " << drawnC << " cards were drawn." << endl;
                    }
                    if(drawnC == 0) {
                        cout << "The draw pile is now empty." << endl;
                    }

                }
                if(card->name == PLUSF) {
                    if(rand()%2 == 1) { // The computer chooses randomly if it wants to challange you or not
                        cout << "The computer choose to not challenge you and will draw 4 cards now" << endl;
                        game.turn = true;
                        int drawnC = 0;
                        for(int i = 0; i < 4; i++) {
                            Card *drawn = drawCard(game.drwSize, game.drwPile);
                            if(drawn != nullptr) {
                                drawnC++;
                                addCard(game.bHandS, game.bHandM, &game.bHand, drawn);
                            }
                        }
                        if(drawnC < 4) {
                            cout << "Only " << drawnC << " cards were drawn." << endl;
                        }
                        if(drawnC == 0) {
                            cout << "The draw pile is now empty." << endl;
                        }
                    } else {
                        cout << "The computer has choosen to challenge you." << endl;
                        if(challengeWin(game.rest, game.hand, game.handS)) {
                            cout << "The computer won since you had a matching color card. You now need to draw 4 cards" << endl;
                            int drawnC = 0;
                            for(int i = 0; i < 4; i++) {
                                Card *drawn = drawCard(game.drwSize, game.drwPile);
                                if(drawn != nullptr) {  // If for some reason a card could not be drawn (running out of cards) keep track of it and don't add a nullptr
                                    drawnC++;
                                    cout << "You drew a " << printCard(drawn) << endl;
                                    addCard(game.handS, game.handM, &game.hand, drawn);
                                }
                            }
                            if(drawnC < 4) {
                                cout << "Only " << drawnC << " cards were drawn." << endl;
                            }
                            if(game.drwSize == 0) {
                                cout << "The draw pile is now empty." << endl;
                            }
                        } else {    // If the computer looses the challenge, draw 6 cards
                            cout << "The computer has lost. The computer now needs to draw 6 cards" << endl;
                            game.turn = true;
                            int drawnC = 0;
                            for(int i = 0; i < 6; i++) {
                                Card *drawn = drawCard(game.drwSize, game.drwPile);
                                if(drawn != nullptr) {  // If for some reason a card could not be drawn (running out of cards) keep track of it and don't add a nullptr
                                    drawnC++;
                                    addCard(game.bHandS, game.bHandM, &game.bHand, drawn);
                                }
                            }
                            if(drawnC < 6) {
                                cout << "Only " << drawnC << " cards were drawn." << endl;
                            }
                            if(game.drwSize == 0) {
                                cout << "The draw pile is now empty." << endl;
                            }
                        }
                    }
                }
                if(card->name == SKIP) {
                    game.turn = true;
                }
                if(card->name == REV) {
                    game.turn = true;
                }
                if(game.handS == 1 && !unoed && !game.turn) {   // The logic for checking if the player should have called uno and if they did. Allows the computer to catch them
                    uno = true;
                } else {
                    uno = false;
                }
                unoed = false;
                addCard(game.drwSize, game.drwMax, &game.drwPile, game.lastPl);
                game.lastPl = card;
                game.rest = res;
            } else {
                uno = false;
                unoed = false;
            }
        } else {
            card = playCom(game, uno, unoed);
            res = card;
            game.turn = true;
            if(card != nullptr) {   // Alot of this branch is just like the player one. (catches when the computer doesn't play a card)
                // The following if statements handle the special cards (wild and draw cards)
                if(card->color == WILD) {
                    res = colors[rand()%colorS];
                    cout << "The next played card now must be a " << printCard(res) << endl;
                }
                if(card->name == PLUST) {
                    cout << "You will draw 2 cards now" << endl;
                    game.turn = false;
                    int drawnC = 0;
                    for(int i = 0; i < 2; i++) {
                        Card *drawn = drawCard(game.drwSize, game.drwPile);
                        if(drawn != nullptr) {
                            drawnC++;
                            cout << "You drew a " << printCard(drawn) << endl;
                            addCard(game.handS, game.handM, &game.hand, drawn);
                        }
                    }
                    if(drawnC < 2) {
                        cout << "Only " << drawnC << " cards were drawn." << endl;
                    }
                    if(drawnC == 0) {
                        cout << "The draw pile is now empty." << endl;
                    }

                }
                if(card->name == PLUSF) {
                    string input;
                    while(input != "yes" && input != "no") {    // Allows the user to challange a draw 4 card. Keeps asking until it recieves valid input
                        cout << "Would you like to challenge the card? (Yes or No)" << endl;
                        getline(cin, input);
                        for(int i = 0; i < input.size(); i++) {
                            input[i] = tolower(input[i]);
                        }
                        if(input != "yes" && input != "no") {
                            cout << "Invalid input" << endl;
                        }
                    }
                    // The following if statements do the same thing as the if statement that randomly chooses or doesn't choose to challange the player from before
                    if(input == "no") {
                        cout << "You choose to not challenge the computer and will draw 4 cards now" << endl;
                        game.turn = false;
                        int drawnC = 0;
                        for(int i = 0; i < 4; i++) {
                            Card *drawn = drawCard(game.drwSize, game.drwPile);
                            if(drawn != nullptr) {
                                drawnC++;
                                cout << "You drew a " << printCard(drawn) << endl;
                                addCard(game.handS, game.handM, &game.hand, drawn);
                            }
                        }
                        if(drawnC < 4) {
                            cout << "Only " << drawnC << " cards were drawn." << endl;
                        }
                        if(drawnC == 0) {
                            cout << "The draw pile is now empty." << endl;
                        }
                    } else {
                        cout << "You have choosen to challenge the computer." << endl;
                        cout << "The computer hand is: ";   // This is one important change from the previous if statement. The rules say the challenger must be able to see the other's cards
                        for(int i = 0; i < game.bHandS; i++) {
                            cout << printCard(game.bHand[i]) << "; ";
                        }
                        cout << endl;
                        if(challengeWin(game.rest, game.bHand, game.bHandS)) {
                            cout << "You won since the computer had a matching color card. The computer now needs to draw 4 cards" << endl;
                            int drawnC = 0;
                            for(int i = 0; i < 4; i++) {
                                Card *drawn = drawCard(game.drwSize, game.drwPile);
                                if(drawn != nullptr) {
                                    drawnC++;
                                    addCard(game.bHandS, game.bHandM, &game.bHand, drawn);
                                }
                            }
                            if(drawnC < 4) {
                                cout << "Only " << drawnC << " cards were drawn." << endl;
                            }
                            if(drawnC == 0) {
                                cout << "The draw pile is now empty." << endl;
                            }
                        } else {
                            cout << "You lost the challenge. You now needs to draw 6 cards" << endl;
                            game.turn = false;
                            int drawnC = 0;
                            for(int i = 0; i < 6; i++) {
                                Card *drawn = drawCard(game.drwSize, game.drwPile);
                                if(drawn != nullptr) {
                                    drawnC++;
                                    cout << "You drew a " << printCard(drawn) << endl;
                                    addCard(game.handS, game.handM, &game.hand, drawn);
                                }
                            }
                            if(drawnC < 6) {
                                cout << "Only " << drawnC << " cards were drawn." << endl;
                            }
                            if(drawnC == 0) {
                                cout << "The draw pile is now empty." << endl;
                            }
                        }
                    }
                }
                if(card->name == SKIP) {
                    game.turn = false;
                }
                if(card->name == REV) {
                    game.turn = false;
                }
                if(game.bHandS == 1 && !unoed && game.turn) {   // This checks if the computer forgot to call uno and allows the user to catch them
                    uno = true;
                } else {
                    uno = false;
                }
                unoed = false;
                addCard(game.drwSize, game.drwMax, &game.drwPile, game.lastPl);
                game.lastPl = card;
                game.rest = res;
            } else {
                uno = false;
                unoed = false;
            }
        }
        if(game.bHandS == 0) {  // Checks if the computer ran out of cards and then calculates points earned if thats the case
            cout << "Computer has won!" << endl;
            game.cpScore += calcPoints(game.hand, game.handS);
            for(int i = 0; i < game.handS; i++) {
                addCard(game.drwSize, game.drwMax, &game.drwPile, game.hand[i]);
            }
            game.handS = 0;
            cout << "Current Score" << endl << "Player: " << game.plScore << endl << "Computer: " << game.cpScore << endl;
            setupGame(game);
        } else if(game.handS == 0) {    // Checks if the player ran out of cards and then calculates points earned if thats the case
            cout << "You have won!" << endl;
            game.plScore += calcPoints(game.bHand, game.bHandS);
            for(int i = 0; i < game.bHandS; i++) {
                addCard(game.drwSize, game.drwMax, &game.drwPile, game.bHand[i]);
            }
            game.bHandS = 0;
            cout << "Current Score" << endl << "Player: " << game.plScore << endl << "Computer: " << game.cpScore << endl;
            setupGame(game);
        }
    }
    if(game.plScore >= 500 || game.cpScore >= 500) {    // If someone earns 500+ points, the game ends
        cout << "Game Complete! " << ((game.cpScore >= 500) ? "Computer" : "Player") << " won!" << endl;
    }
    cout << "Game will save" << endl;
}

Card **setupPile(Game &game, fstream &setup) {
    if(game.drwPile == nullptr) {   // Makes sure this function doesn't run if the draw pile is already setup
        const int cardLimit = 54;   // Uno only has 54 unique cards under normal rules
        long int indexes[cardLimit] = {};   // Used to jump around to the correct spots in the file
        Card **cardTypes = new Card*[cardLimit]; 
        int total = 0;
        for(int i = 1; i < cardLimit; i++) {    // Finds where the new lines are to get the position each card is located at in the file (first one is alwasy 0)
            string line;
            getline(setup, line);
            indexes[i] = setup.tellg();
        }
        for(int i = 0; i < cardLimit; i++) {    // Starts by jumping to each index to create each unique card and counts how large the draw pile needs to be
            setup.seekg(indexes[i], ios::beg);
            Card *card = new Card;
            int num = 0;
            setup >> card->color;
            setup >> card->name;
            setup >> num;
            total += num;
            cardTypes[i] = card;
        }
        // Creates the draw pile
        game.drwMax = total;
        game.drwSize = total;
        game.drwPile = new Card*[total];
        int drwInd = 0;
        for(int i = 0; i < cardLimit; i++) {    // Copies the correct number of each unique card to the draw pile (only their pointers). Jumps to each line as necessary
            setup.seekg(indexes[i], ios::beg);
            int num = 0;
            setup >> num;
            setup >> num;
            setup >> num;
            for(int j = 0; j < num; j++) {
                game.drwPile[drwInd] = cardTypes[i];
                drwInd++;
            }
        }
        return cardTypes;
    }
    return nullptr;
}

Card **loadSave(Game &game, GameS &save, Card **colors, int colorS) {
    // Loads the save and converts it into something actually usable (the game struct)
    game.plScore = save.plScore;
    game.cpScore = save.cpScore;
    game.bHandS = save.bHandS;
    game.bHandM = save.bHandS;
    game.handS = save.handS;
    game.handM = save.handS;
    game.drwSize = save.drwSize;
    game.drwMax = save.handS + save.bHandS + save.drwSize;
    game.drwPile = new Card*[game.drwMax];
    game.bHand = new Card*[game.bHandM];
    game.hand = new Card*[game.handM];
    game.turn = save.turn;
    Card **unique = new Card*[54];
    int indexDr = 0;
    int indexPl = 0;
    int indexCp = 0;
    for(int i = 0; i < 54; i++) {   // Recreates the unique cards
        unique[i] = new Card;
        unique[i]->color = save.cards[i].color;
        unique[i]->name = save.cards[i].name;
        for(int j = 0; j < save.draw[i]; j++) { // Adds the unique cards to the draw pile if needed
            game.drwPile[indexDr] = unique[i];
            indexDr++;
        }
        for(int j = 0; j < save.play[i]; j++) { // Adds the unique cards to the player hand if needed
            game.hand[indexPl] = unique[i];
            indexPl++;
        }
        for(int j = 0; j < save.bot[i]; j++) { // Adds the unique cards to the computer hand if needed
            game.bHand[indexCp] = unique[i];
            indexCp++;
        }
        if(unique[i]->color == save.lastPl.color && unique[i]->name == save.lastPl.name) {  // Adds the unique card to the last played var if needed
            game.lastPl = unique[i];
        }
        if(unique[i]->color == save.rest.color && unique[i]->name == save.rest.name) {      // Adds the unique card to the card restriction var if needed
            game.rest = unique[i];
        }
    }
    for(int i = 0; i < colorS; i++) {
        if(colors[i]->color == save.rest.color && colors[i]->name == save.rest.name) {  // This is incase the restriction var was a color card created by a wild card
            game.rest = colors[i];
        }
    }
    return unique;
}

void createSave(Game &game, GameS &save, Card **unique) {
    // Saves everything
    save.plScore = game.plScore;
    save.cpScore = game.cpScore;
    save.bHandS = game.bHandS;
    save.handS = game.handS;
    save.drwSize = game.drwSize;
    save.turn = game.turn;
    save.lastPl.color = game.lastPl->color;
    save.lastPl.name  = game.lastPl->name;
    save.rest.color = game.rest->color;
    save.rest.name  = game.rest->name;
    for (int i = 0; i < 54; i++) {  // Searches for each card type, counts them, and then saves them to the correct variable
        save.draw[i] = 0;
        save.play[i] = 0;
        save.bot[i]  = 0;
        save.cards[i].color = unique[i]->color;
        save.cards[i].name = unique[i]->name;
        for(int j = 0; j < game.drwSize; j++) {
            if((game.drwPile[j]->color == unique[i]->color) && (game.drwPile[j]->name == unique[i]->name)) {
                save.draw[i]++;
            }
        }
        for(int j = 0; j < game.handS; j++) {
            if((game.hand[j]->color == unique[i]->color) && (game.hand[j]->name == unique[i]->name)) {
                save.play[i]++;
            }
        }
        for(int j = 0; j < game.bHandS; j++) {
            if((game.bHand[j]->color == unique[i]->color) && (game.bHand[j]->name == unique[i]->name)) {
                save.bot[i]++;
            }
        }
    }
}