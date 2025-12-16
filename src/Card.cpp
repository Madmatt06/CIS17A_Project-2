/* 
 * File:   Card.cpp
 * Author: Matthew Madrigal
 * Created on December 12th, 2025, 11:47 am,
 * Purpose:  To define the functions in the Card Class
 */

//System Libraries
#include <iostream>
using namespace std;

//User Libraries
#include "Card.h"

string Card::printCard() {
    string display;
    // Converts the color enum into a human readable string
    switch(color) {
        case RED:
        display += "Red ";
        break;
        case GREEN:
        display += "Green ";
        break;
        case YELLOW:
        display += "Yellow ";
        break;
        case BLUE:
        display += "Blue ";
        break;
        case WILD:
        display += "Wild ";
        break;
        default:
        break;
    }
    // Converts the type enum into a human readable string
    switch(name) {
        case ZERO:
        case ONE:
        case TWO:
        case THREE:
        case FOUR:
        case FIVE:
        case SIX:
        case SEVEN:
        case EIGHT:
        case NINE:
        display += to_string(name);
        break;
        case PLUST:
        display += "Draw Two";
        break;
        case PLUSF:
        display += "Draw Four";
        break;
        case REV:
        display += "Reverse";
        break;
        case SKIP:
        display += "Skip";
        break;
        case CARD:
        default:
        display += "Card";
        break;
    }
    return display;

}
