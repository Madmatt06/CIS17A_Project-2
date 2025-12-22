/* 
 * File:   Game.h
 * Author: Matthew Madrigal
 * Purpose:  Game class
 */

#ifndef GAME_H
#define GAME_H

#include "Card.h"
#include "GameS.h"
#include "Player.h"
#include <fstream>
using namespace std;

class Game{
private:
    enum colors {RED, GREEN, YELLOW, BLUE, WILD};
    enum types {ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, PLUST, REV, SKIP, PLUSF, CARD};
    bool turn;      // True is for the player, false is for the computer
    Card *lastPl;   // The last played card
    Card *rest;     // The current restriction on what can be played
    Vector<Card*> drwPile;  // The cards waiting to be drawn that way a game never has more than the max number according to the uno rules
    Card **colors;  // Wild Card restrictions
    Card **unique;  // Instead of creating duplicate cards when there is more than one type of card, a single card is created and that pointer is duplicated the number of times needed in the draw pile.
    int colorS;
    Player *player;
    Player *compute;
    Card **genColor();  // Generates the restriction cards used when a wild card is played. They only restrict by color
    void setupPile(fstream &);                // Gets a file and reads from it to fill the draw pile with the correct cards. Makes modifying rules a bit easier
    void init();        // Initializes variables to 0
    void setupGame();                             // Sets up a new game
    int strToCol(string);                       // Converts a string to a color enum
    int strToNm(string);                        // Converts a string to a type enum
    Card *playWild(int, Card **);                       // Asks the user what color to choose for the wild card
    Card *drawCard(Vector<Card*> &);                    // Draws a card from an array and returns it's pointer
    void addCard(int &, int &, Card ***, Card *);       // Adds a card to an array. If needed, it will grow the array size
    bool challengeWin(Card *, Card **, int);            // Checks if a hand has a card with a matching color to the restriction. Needed for challenges
    string printCard(Card *);                           // Prints a card's color and type
    unsigned int calcPoints(Vector<Card*>);             // Calculates the number of points a person earns for winning
    void setupGame(Game &);                             // Sets up a new game
    void printDeck(int, Card **, Card *);               // Prints what cards are in the array and what cards can be played.
    Card **getPlayable(int, Card **, Card *, int &);    // Creates a new dynamically allocated array with the cards that can be played given a restriction card
    void copy(int, Card **, Card **);                   // Copys an array to another array
    void removeCard(int &, Card **, Card *);            // Removes a card given its pointer from an array and decreases its size by one
public:
    
    Game(fstream&);         // Initializer
    Game(GameS&);    // Start with save
    ~Game();
    void playGame();        // The main game loop. Also handles special logic for cards such as +2, +4, and calls the playWild function when needed
    void createSave(GameS &);          // Creates a save before the program terminates
};

#endif /* GAME_H */