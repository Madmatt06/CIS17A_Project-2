/* 
 * File:   Computer.h
 * Author: Matthew Madrigal
 * Purpose:  Computer Structure
 */

#ifndef COMPUTER_H
#define COMPUTER_H

#include "Player.h"

class Computer : public Player{
private:
public:
    virtual Card *playCard(Card*, Vector<Card*> &, int, bool, bool &, bool &, Player *prev);
    virtual void draw(Vector<Card*> &, int);
};

#endif /* COMPUTER_H */