/* 
 * File:   GameS.h
 * Author: Matthew Madrigal
 * Purpose:  This structure is used to save the game state to a file
 */

#ifndef GAMES_H
#define GAMES_H

#include "Card.h"

struct GameS{
    bool turn;      // True is for the player, false is for the computer
    int plScore;    // The score for the player
    int cpScore;    // The score for the computer
    int drwSize;    // Size of draw pile array
    int handS;      // The size of the array of the cards the player currently has
    int bHandS;     // The size of the array of the cards the computer currently has
    Card rest;      // The recent restriction on cards to play (useful for wild cards)
    Card lastPl;    // The last played card
    Card cards[54]; // The list of unique cards
    int draw[54];   // The amount of each unique card in the draw pile (index is the same as the index of the unique card in cards)
    int play[54];   // The amount of each unique card in the player hand (index is the same as the index of the unique card in cards
    int bot[54];    // The amount of each unique card in the computer hand (index is the same as the index of the unique card in cards
};

#endif /* Game_S_H */