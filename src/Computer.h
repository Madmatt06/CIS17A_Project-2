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
    virtual Card *playCard(Card*, Card**, int &, int, bool, bool &, bool &);
};

#endif /* COMPUTER_H */