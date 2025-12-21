/* 
 * File:   Player.h
 * Author: Matthew Madrigal
 * Purpose:  Player Class
 */

#ifndef PLAYER_H
#define PLAYER_H

#include "Vector.h"
#include "Card.h"

class Player{
private:
    static int players;                     // Keeps track of the number of players
    int score;                              // Keeps track of score
    int strToCol(string);                   // Converts a string to a color enum
    int strToNm(string);                    // Converts a string to a type enum
    void printDeck(Card *);                 // Prints what cards are in the array and what cards can be played.
protected:
    enum colors {RED, GREEN, YELLOW, BLUE, WILD};
    enum types {ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, PLUST, REV, SKIP, PLUSF, CARD};
    Vector<Card*> hand;
    Vector<Card*> getPlayable(Card*);
    Card *drawCard(int &, Card **);                     // Draws a card from an array and returns it's pointer
    void addCard(Card *);       // Adds a card to an array. If needed, it will grow the array size
    void removeCard(int &, Card **, Card *);            // Removes a card given its pointer from an array and decreases its size by one
    void removeCard(Vector<Card*> &, Card *);             // Works with the Vector object

public:
    Player() {score = 0; players++;};
    virtual Card *playCard(Card*, Card**, int &, int, bool, bool &, bool &);
    Vector<Card*> getHand() const;
    void setHand(Vector<Card*> &);
    int getScore() const;
    void addScore(const int);
    static int getCount() {return players;}
};

#endif /* PLAYER_H */