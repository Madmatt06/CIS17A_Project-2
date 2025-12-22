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
    bool turn;                                  // True is for the player, false is for the computer
    Card *lastPl;                               // The last played card
    Card *rest;                                 // The current restriction on what can be played
    Vector<Card*> drwPile;                      // The cards waiting to be drawn that way a game never has more than the max number according to the uno rules
    Card **colors;                              // Wild Card restrictions
    Card **unique;                              // Instead of creating duplicate cards when there is more than one type of card, a single card is created and that pointer is duplicated the number of times needed in the draw pile.
    int colorS;                                 // The number of color cards in the color card array
    Player *player;                             // Player object
    Player *compute;                            // Computer object
    Card **genColor();                          // Generates the restriction cards used when a wild card is played. They only restrict by color
    void setupPile(fstream &);                  // Gets a file and reads from it to fill the draw pile with the correct cards. Makes modifying rules a bit easier
    void init();                                // Initializes variables to 0
    void setupGame();                           // Sets up a new game
    int strToCol(string);                       // Converts a string to a color enum
    Card *playWild(int, Card **);               // Asks the user what color to choose for the wild card
    Card *drawCard(Vector<Card*> &);            // Draws a card from an array and returns it's pointer
    string printCard(Card *);                   // Prints a card's color and type
    unsigned int calcPoints(Vector<Card*>);     // Calculates the number of points a person earns for winning
public:
    
    Game(fstream&);                             // Initializer
    Game(GameS&);                               // Start with save
    ~Game();                                    // Destructor
    void playGame();                            // The main game loop. Also handles special logic for cards such as +2, +4, and calls the playWild function when needed
    void createSave(GameS &);                   // Creates a save before the program terminates
};

#endif /* GAME_H */