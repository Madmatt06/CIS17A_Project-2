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

Card *Computer::playCard(Card* rest, Card** drwPile, int &drwSize, int bHandS, bool uno, bool &unoed, bool &quit) {
    if(uno) {   // If the player forgot to call uno, there is a 50% change the computer catches it
        if(rand()%2 == 1) {
            cout << "The computer caught you with one card!" << endl;
            int drawnC = 0;
            for(int i = 0; i < 4; i++) {
                Card *drawn = drawCard(drwSize, drwPile);
                if(drawn != nullptr) {
                    drawnC++;
                    addCard(drawn);
                }
            }
            if(drawnC != 4) {
                cout << "You could only draw " << drawnC << " cards." << endl;
            }
            if(drwSize == 0) {
                cout << "The draw pile is now empty." << endl;
            }
        }
    }
    Card *usrCard = nullptr;
    int validS;
    Vector<Card*> valid = getPlayable(rest);
    if(validS != 0 || drwSize != 0) {  // If the computer can do something
        if(validS != 0) {                   // If the computer doesn't have to draw, play a card
            int index = rand()%validS;
            usrCard = valid[index];
            removeCard(hand, usrCard);
            cout << "The computer played the card " << usrCard->printCard() << endl;
        } else {
            cout << "The computer drawed a card" << endl;
            Card *drawn = drawCard(drwSize, drwPile);
            addCard(drawn);
        }
        if(bHandS == 1 && rand()%2 == 1) { // There is a 50% chance the computer remembers to call uno. I know this computer is really dumb
            cout << "The computer called uno!" << endl;
            unoed = true;
        }
    } else {    // If the computer can't play anything and for some reason the draw pile is completly empty, skip their turn
        cout << "The computer couldn't play anything and the draw pile was empty." << endl;
        unoed = true;   // Just incase
    }
    return usrCard;
}