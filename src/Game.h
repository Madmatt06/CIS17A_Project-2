/* 
 * File:   Game.h
 * Author: Matthew Madrigal
 * Purpose:  Game class
 */

#ifndef GAME_H
#define GAME_H

#include "Card.h"

class Game{
private:
    enum colors {RED, GREEN, YELLOW, BLUE, WILD};
    enum types {ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, PLUST, REV, SKIP, PLUSF, CARD};
public:
    bool turn;      // True is for the player, false is for the computer
    int plScore;    // The score for the player
    int cpScore;    // The score for the computer
    Card *lastPl;   // The last played card
    Card *rest;     // The current restriction on what can be played
    int drwSize;    // Size of draw pile array
    int drwMax;
    Card **drwPile;  // The cards waiting to be drawn that way a game never has more than the max number according to the uno rules
    int handS;      // The size of the array of the cards the player currently has
    int handM;      // Max num of elements the array can hold right now
    Card **hand;     // The cards the player currently has
    int bHandS;     // The size of the array of the cards the computer currently has
    int bHandM;     // Max num of elements the array can hold right now
    Card **bHand;    // The cards the computer currently has
    void playGame(Game &, int, Card **);        // The main game loop. Also handles special logic for cards such as +2, +4, and calls the playWild function when needed
    int strToCol(string);                       // Converts a string to a color enum
    int strToNm(string);                        // Converts a string to a type enum
};

#endif /* GAME_H */