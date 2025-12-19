/* 
 * File:   Card.h
 * Author: Matthew Madrigal
 * Purpose:  Card Structure
 */

#ifndef CARD_H
#define CARD_H

using namespace std;
#include <string>

class Card{
private:
    enum colors {RED, GREEN, YELLOW, BLUE, WILD};
    enum types {ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, PLUST, REV, SKIP, PLUSF, CARD};
public:
    int color;
    int name;
    string printCard();   // Prints a card's color and type
};

#endif /* CARD_H */