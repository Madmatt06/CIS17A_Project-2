/* 
 * File:   Player.cpp
 * Author: Matthew Madrigal
 * Created on December 20th, 2025, 7:41 pm,
 * Purpose:  To define the functions in the player class
 */

//System Libraries
#include <iostream>
#include <algorithm>
#include <string.h>
using namespace std;

//User Libraries
#include "Player.h"

int Player::players = 0;

Card *Player::playCard(Card* rest, Vector<Card*> &drwPile, int bHandS, bool uno, bool &unoed, bool &quit, Player *prev) {
    bool choosen = false;
    Card *usrCard = nullptr;
    while(!choosen) {                               // This forces the user to play a valid card before continuing
        Vector<Card*> valid = getPlayable(rest);
        if(valid.size() != 0 || drwPile.size() != 0) {  // If its possible for the player to do something, allow them to play
            cout << "The computer has " << bHandS << " cards" << endl;
            printDeck(rest);
            cout << "Enter the card you would like to play (Type it as listed)" << ((drwPile.size() > 0) ?" or type draw to draw a card" : "") << (uno? ". Since the computer didn't call uno, you can type uno to force them to draw" : "") << ": ";
            string input, wordF, wordS;
            getline(cin, input);                    // Gets the card the player wants to play
            bool next = false;
            for(int i = 0; i < input.length(); i++) {
                input[i] = tolower(input[i]);
            }
            if(input == "quit") {                   // Special case for quiting the game
                quit = true;
                return nullptr;
            }
            if(input == "draw") {                   // Special case for drawing from the pile
                usrCard = drawCard(drwPile);
                if(usrCard == nullptr) {            // In case the user draws a card when there is nothing to draw (asks what to do again)
                    cout << "There are no cards to draw." << endl;
                } else {                            // Tell the user what card they got. If its playable, ask if they want to play it
                    cout << "You got a " << usrCard->printCard() << endl;
                    if((usrCard->color == WILD) || (usrCard->color == rest->color || usrCard->name == rest->name)) {
                        cout << "Would you like to play it? (yes or No)" << endl;
                        string toPlay;
                        while (toPlay != "yes" && toPlay != "no") {
                            getline(cin, toPlay);
                            int strSize = toPlay.size();
                            char *playCSt = new char[strSize];    // This isn't needed but I am using it to make sure I use a cstring at least once
                            strncpy(playCSt, toPlay.c_str(), strSize);
                            for(int i = 0; i < strSize; i++) {
                                playCSt[i] = tolower(playCSt[i]);
                            }
                            if(strcmp(playCSt, "yes") != 0 && strcmp(playCSt,"no") != 0) {
                                cout << "Invalid input" << endl;
                            }
                            delete []playCSt;
                        }
                        if(toPlay != "yes") {
                            addCard(usrCard);
                            usrCard = nullptr;
                        }
                    } else {    // Add to hand if not playable
                        addCard(usrCard);
                        usrCard = nullptr;
                    }
                    choosen = true;
                }
            } else if(input == "uno") { // Handles uno logic
                if(!uno) {  // Checks if it was entered to catch the computer or protect themself
                    if(hand.size() == 2 && valid.size() >= 1) {    // Makes sure they can call uno
                        unoed = true;
                        cout << "You called uno!" << endl;
                    } else {
                        cout << "You can't call uno yet!" << endl;
                    }
                } else {    // Makes the computer draw 4 cards
                    uno = true;
                    cout << "The computer will draw 4 cards" << endl;
                    prev->draw(drwPile, 4);
                    bHandS += 4;
                }
            }else { // Splits the input into two for parsing of card color and type
                for(int i = 0; i < input.length(); i++) {
                    if(input[i] == ' ' && !next) {
                        next = true;
                    } else {
                        if(!next) {
                            wordF += input[i];
                        } else {
                            wordS += input[i];
                        }
                    }
                }
                int color, name;
                color = strToCol(wordF);
                name = strToNm(wordS);
                if(color != -1 && name != -1) { // Tries to find the card in the playable array. Removes it if found and returns it
                    for(int i = 0; i < valid.size(); i++) {
                        Card *card = valid[i];
                        if(card->color == color && card->name == name) {
                            choosen = true;
                            usrCard = card;
                            removeCard(hand, usrCard);
                            break;
                        }
                    }
                }
                if(!choosen) {
                    cout << "Can not play card" << endl;
                }
            }
        } else {    // Fail safe if there are no cards playable and the draw pile is empty
            cout << "You have no playable cards and the draw pile was empty. Your turn will be skipped." << endl;
            choosen = true;
        }
    }
    return usrCard;
    
}

int Player::strToCol(string input) {
    if(input == "red") {
        return RED;
    }
    if(input == "green") {
        return GREEN;
    }
    if(input == "yellow") {
        return YELLOW;
    }
    if(input == "blue") {
        return BLUE;
    }
    if(input == "wild") {
        return WILD;
    }
    return -1;
}

int Player::strToNm(string input) {
    if(input.length() == 1 && isdigit(input[0])) {
        return static_cast<int>(input[0] - '0');
    } else {
        if(input == "draw two") {
            return PLUST;
        }
        if(input == "draw four") {
            return PLUSF;
        }
        if(input == "reverse") {
            return REV;
        }
        if(input == "skip") {
            return SKIP;
        }
        if(input == "card") {
            return CARD;
        }
    }
    return -1;
}

Card *Player::drawCard(Vector<Card*> &drwPile) {
    if(drwPile.size() == 0) {
        return nullptr;
    }
    int index = rand() % drwPile.size();
    Card *card = drwPile[index];
    drwPile.remove(index);
    return card;
}

void Player::addCard(Card *card) {
    hand.append(card);
    sort(hand.begin(), hand.end());
}

void Player::removeCard(int &size, Card **hand, Card *card) {
    int index = 0;      // Finds the first instance of a card with a matching pointer and removes it
    for(int i = 0; i < size; i++) {
        if(hand[i] == card) {
            index = i;
            break;
        }
    }
    for(int i = index+1; i < size; i++) {
        hand[i-1] = hand[i];
    }
    size--;
}

void Player::removeCard(Vector<Card*> &cards, Card *card) {
    for(int i = 0; i < cards.size(); i++) {
        if(cards[i] == card) {
            cards.remove(i);
            break;
        }
    }
}

void Player::printDeck(Card *active) {
    cout << "Cards in hand: ";
    for(int i = 0; i < hand.size(); i++) {
        cout << hand[i]->printCard() << "; ";     // Prints all the cards in the array
    }
    cout << endl;
    Vector<Card*> play;
    play = getPlayable(active);
    cout << "Playable cards in hand: ";
    for(int i = 0; i < play.size(); i++) {
        cout << play[i]->printCard() << "; ";     // Prints all playable cards
    }
    cout << endl;
}

Vector<Card*> Player::getHand() const {
    return hand;
}
void Player::setHand(Vector<Card*> &nHand) {
    hand = nHand;
}

Player Player::operator+=(const int amnt) {
    score += amnt;
    return *this;
}

Vector<Card*> Player::getPlayable(Card* active) {
    Vector<Card*> play;
    for(int i = 0; i < hand.size(); i++) {
        Card *card = hand[i];
        // Adds all playable cards to the list
        if(card->color == WILD || card->color == active->color || card->name == active->name) {
            play.append(card);
        }
    }
    return play;
}

void Player::draw(Vector<Card*> &drwPile, int num) {
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
        cout << "You drew a " << drawn->printCard() << endl;
    }
    if(drawnC < num) {
        cout << "Only " << drawnC << " cards were drawn." << endl;
    }
    if(drawnC == 0) {
        cout << "The draw pile was empty." << endl;
    } 
}

bool Player::challengeWin(Card *check) {
    for(int i = 0; i < hand.size(); i++) {
        Card *card = hand[i];
        if(check->color == card->color) {       // If there was a playable card with a matching color, then the challenger wins
            return true;
        }
    }
    return false;
}
