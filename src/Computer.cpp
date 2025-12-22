/* 
 * File:   Computer.cpp
 * Author: Matthew Madrigal
 * Created on December 21st, 2025, 10:10 am,
 * Purpose:  To define the functions in the Computer class
 */

//System Libraries
#include <iostream>
using namespace std;

//User Libraries
#include "Computer.h"
#include "Vector.h"

Card *Computer::playCard(Card* rest, Vector<Card*> &drwPile, int bHandS, bool uno, bool &unoed, bool &quit, Player *prev) {
    if(uno) {   // If the player forgot to call uno, there is a 50% change the computer catches it
        if(rand()%2 == 1) {
            cout << "The computer caught you with one card!" << endl;
            prev->draw(drwPile, 1);
        }
    }
    Card *usrCard = nullptr;
    Vector<Card*> valid = getPlayable(rest);
    if(valid.size() != 0 || drwPile.size() != 0) {  // If the computer can do something
        if(valid.size() != 0) {                   // If the computer doesn't have to draw, play a card
            int index = rand()%valid.size();
            usrCard = valid[index];
            removeCard(hand, usrCard);
            cout << "The computer played the card " << usrCard->printCard() << endl;
        } else {
            cout << "The computer drawed a card" << endl;
            Card *drawn = drawCard(drwPile);
            addCard(drawn);
        }
        if(hand.size() == 1 && rand()%2 == 1) { // There is a 50% chance the computer remembers to call uno. I know this computer is really dumb
            cout << "The computer called uno!" << endl;
            unoed = true;
        }
    } else {    // If the computer can't play anything and for some reason the draw pile is completly empty, skip their turn
        cout << "The computer couldn't play anything and the draw pile was empty." << endl;
        unoed = true;   // Just incase
    }
    return usrCard;
}

void Computer::draw(Vector<Card*> &drwPile, int num) {
    int drawnC = 0;
    for(int i = 0; i < num; i++) {
        if(drwPile.size() == 0) {
            break;
        }
        int index = rand() % drwPile.size();
        Card *drawn = drwPile[index];
        drwPile.remove(index);
        hand.append(drawn);
        drawnC++;
    }
    if(drawnC < num) {
        cout << "Only " << drawnC << " cards were drawn." << endl;
    }
    if(drawnC == 0) {
        cout << "The draw pile was empty." << endl;
    } 
}