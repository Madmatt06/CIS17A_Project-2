/* 
 * File:   Game.cpp
 * Author: Matthew Madrigal
 * Created on December 12th, 2025, 11:39 am,
 * Purpose:  To define the functions in the Game class
 */

//System Libraries
#include <iostream>
using namespace std;

//User Libraries
#include "Game.h"
#include "Card.h"

void Game::playGame(Game &game, int colorS, Card **colors) {
    bool uno = false, unoed = false, quit = false;  // Used to keep track of things in between turns
    while(game.plScore < 500 && game.cpScore < 500) {   // A game ends when someone has 500 points or more
        Card *card;
        Card *res;
        if(turn) {
            cout << "Current card at play: " << lastPl->printCard() << endl;
            card = playCard(game, uno, unoed, quit);
            if(quit) {  // Allows the player to exit the game
                break;
            }
            res = card;
            game.turn = false;
            if(card != nullptr) {   // This is to catch when the player draws a card and doesn't play it (or if they couldn't do anything)
                // The following if statements handle the special cards (wild and draw cards)
                if(card->color == WILD) {
                    res = playWild(colorS, colors);
                }
                if(card->name == PLUST) {
                    cout << "The computer will draw 2 cards now" << endl;
                    game.turn = true;
                    int drawnC = 0;
                    for(int i = 0; i < 2; i++) {
                        Card *drawn = drawCard(game.drwSize, game.drwPile);
                        if(drawn != nullptr) {
                            drawnC++;
                            addCard(game.bHandS, game.bHandM, &game.bHand, drawn);
                        }
                    }
                    if(drawnC < 2) {
                        cout << "Only " << drawnC << " cards were drawn." << endl;
                    }
                    if(drawnC == 0) {
                        cout << "The draw pile is now empty." << endl;
                    }

                }
                if(card->name == PLUSF) {
                    if(rand()%2 == 1) { // The computer chooses randomly if it wants to challange you or not
                        cout << "The computer choose to not challenge you and will draw 4 cards now" << endl;
                        game.turn = true;
                        int drawnC = 0;
                        for(int i = 0; i < 4; i++) {
                            Card *drawn = drawCard(game.drwSize, game.drwPile);
                            if(drawn != nullptr) {
                                drawnC++;
                                addCard(game.bHandS, game.bHandM, &game.bHand, drawn);
                            }
                        }
                        if(drawnC < 4) {
                            cout << "Only " << drawnC << " cards were drawn." << endl;
                        }
                        if(drawnC == 0) {
                            cout << "The draw pile is now empty." << endl;
                        }
                    } else {
                        cout << "The computer has choosen to challenge you." << endl;
                        if(challengeWin(game.rest, game.hand, game.handS)) {
                            cout << "The computer won since you had a matching color card. You now need to draw 4 cards" << endl;
                            int drawnC = 0;
                            for(int i = 0; i < 4; i++) {
                                Card *drawn = drawCard(game.drwSize, game.drwPile);
                                if(drawn != nullptr) {  // If for some reason a card could not be drawn (running out of cards) keep track of it and don't add a nullptr
                                    drawnC++;
                                    cout << "You drew a " << printCard(drawn) << endl;
                                    addCard(game.handS, game.handM, &game.hand, drawn);
                                }
                            }
                            if(drawnC < 4) {
                                cout << "Only " << drawnC << " cards were drawn." << endl;
                            }
                            if(game.drwSize == 0) {
                                cout << "The draw pile is now empty." << endl;
                            }
                        } else {    // If the computer looses the challenge, draw 6 cards
                            cout << "The computer has lost. The computer now needs to draw 6 cards" << endl;
                            game.turn = true;
                            int drawnC = 0;
                            for(int i = 0; i < 6; i++) {
                                Card *drawn = drawCard(game.drwSize, game.drwPile);
                                if(drawn != nullptr) {  // If for some reason a card could not be drawn (running out of cards) keep track of it and don't add a nullptr
                                    drawnC++;
                                    addCard(game.bHandS, game.bHandM, &game.bHand, drawn);
                                }
                            }
                            if(drawnC < 6) {
                                cout << "Only " << drawnC << " cards were drawn." << endl;
                            }
                            if(game.drwSize == 0) {
                                cout << "The draw pile is now empty." << endl;
                            }
                        }
                    }
                }
                if(card->name == SKIP) {
                    game.turn = true;
                }
                if(card->name == REV) {
                    game.turn = true;
                }
                if(game.handS == 1 && !unoed && !game.turn) {   // The logic for checking if the player should have called uno and if they did. Allows the computer to catch them
                    uno = true;
                } else {
                    uno = false;
                }
                unoed = false;
                addCard(game.drwSize, game.drwMax, &game.drwPile, game.lastPl);
                game.lastPl = card;
                game.rest = res;
            } else {
                uno = false;
                unoed = false;
            }
        } else {
            card = playCom(game, uno, unoed);
            res = card;
            game.turn = true;
            if(card != nullptr) {   // Alot of this branch is just like the player one. (catches when the computer doesn't play a card)
                // The following if statements handle the special cards (wild and draw cards)
                if(card->color == WILD) {
                    res = colors[rand()%colorS];
                    cout << "The next played card now must be a " << printCard(res) << endl;
                }
                if(card->name == PLUST) {
                    cout << "You will draw 2 cards now" << endl;
                    game.turn = false;
                    int drawnC = 0;
                    for(int i = 0; i < 2; i++) {
                        Card *drawn = drawCard(game.drwSize, game.drwPile);
                        if(drawn != nullptr) {
                            drawnC++;
                            cout << "You drew a " << printCard(drawn) << endl;
                            addCard(game.handS, game.handM, &game.hand, drawn);
                        }
                    }
                    if(drawnC < 2) {
                        cout << "Only " << drawnC << " cards were drawn." << endl;
                    }
                    if(drawnC == 0) {
                        cout << "The draw pile is now empty." << endl;
                    }

                }
                if(card->name == PLUSF) {
                    string input;
                    while(input != "yes" && input != "no") {    // Allows the user to challange a draw 4 card. Keeps asking until it recieves valid input
                        cout << "Would you like to challenge the card? (Yes or No)" << endl;
                        getline(cin, input);
                        for(int i = 0; i < input.size(); i++) {
                            input[i] = tolower(input[i]);
                        }
                        if(input != "yes" && input != "no") {
                            cout << "Invalid input" << endl;
                        }
                    }
                    // The following if statements do the same thing as the if statement that randomly chooses or doesn't choose to challange the player from before
                    if(input == "no") {
                        cout << "You choose to not challenge the computer and will draw 4 cards now" << endl;
                        game.turn = false;
                        int drawnC = 0;
                        for(int i = 0; i < 4; i++) {
                            Card *drawn = drawCard(game.drwSize, game.drwPile);
                            if(drawn != nullptr) {
                                drawnC++;
                                cout << "You drew a " << printCard(drawn) << endl;
                                addCard(game.handS, game.handM, &game.hand, drawn);
                            }
                        }
                        if(drawnC < 4) {
                            cout << "Only " << drawnC << " cards were drawn." << endl;
                        }
                        if(drawnC == 0) {
                            cout << "The draw pile is now empty." << endl;
                        }
                    } else {
                        cout << "You have choosen to challenge the computer." << endl;
                        cout << "The computer hand is: ";   // This is one important change from the previous if statement. The rules say the challenger must be able to see the other's cards
                        for(int i = 0; i < game.bHandS; i++) {
                            cout << printCard(game.bHand[i]) << "; ";
                        }
                        cout << endl;
                        if(challengeWin(game.rest, game.bHand, game.bHandS)) {
                            cout << "You won since the computer had a matching color card. The computer now needs to draw 4 cards" << endl;
                            int drawnC = 0;
                            for(int i = 0; i < 4; i++) {
                                Card *drawn = drawCard(game.drwSize, game.drwPile);
                                if(drawn != nullptr) {
                                    drawnC++;
                                    addCard(game.bHandS, game.bHandM, &game.bHand, drawn);
                                }
                            }
                            if(drawnC < 4) {
                                cout << "Only " << drawnC << " cards were drawn." << endl;
                            }
                            if(drawnC == 0) {
                                cout << "The draw pile is now empty." << endl;
                            }
                        } else {
                            cout << "You lost the challenge. You now needs to draw 6 cards" << endl;
                            game.turn = false;
                            int drawnC = 0;
                            for(int i = 0; i < 6; i++) {
                                Card *drawn = drawCard(game.drwSize, game.drwPile);
                                if(drawn != nullptr) {
                                    drawnC++;
                                    cout << "You drew a " << printCard(drawn) << endl;
                                    addCard(game.handS, game.handM, &game.hand, drawn);
                                }
                            }
                            if(drawnC < 6) {
                                cout << "Only " << drawnC << " cards were drawn." << endl;
                            }
                            if(drawnC == 0) {
                                cout << "The draw pile is now empty." << endl;
                            }
                        }
                    }
                }
                if(card->name == SKIP) {
                    game.turn = false;
                }
                if(card->name == REV) {
                    game.turn = false;
                }
                if(game.bHandS == 1 && !unoed && game.turn) {   // This checks if the computer forgot to call uno and allows the user to catch them
                    uno = true;
                } else {
                    uno = false;
                }
                unoed = false;
                addCard(game.drwSize, game.drwMax, &game.drwPile, game.lastPl);
                game.lastPl = card;
                game.rest = res;
            } else {
                uno = false;
                unoed = false;
            }
        }
        if(game.bHandS == 0) {  // Checks if the computer ran out of cards and then calculates points earned if thats the case
            cout << "Computer has won!" << endl;
            game.cpScore += calcPoints(game.hand, game.handS);
            for(int i = 0; i < game.handS; i++) {
                addCard(game.drwSize, game.drwMax, &game.drwPile, game.hand[i]);
            }
            game.handS = 0;
            cout << "Current Score" << endl << "Player: " << game.plScore << endl << "Computer: " << game.cpScore << endl;
            setupGame(game);
        } else if(game.handS == 0) {    // Checks if the player ran out of cards and then calculates points earned if thats the case
            cout << "You have won!" << endl;
            game.plScore += calcPoints(game.bHand, game.bHandS);
            for(int i = 0; i < game.bHandS; i++) {
                addCard(game.drwSize, game.drwMax, &game.drwPile, game.bHand[i]);
            }
            game.bHandS = 0;
            cout << "Current Score" << endl << "Player: " << game.plScore << endl << "Computer: " << game.cpScore << endl;
            setupGame(game);
        }
    }
    if(game.plScore >= 500 || game.cpScore >= 500) {    // If someone earns 500+ points, the game ends
        cout << "Game Complete! " << ((game.cpScore >= 500) ? "Computer" : "Player") << " won!" << endl;
    }
    cout << "Game will save" << endl;
}

int Game::strToCol(string input) {
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

int Game::strToNm(string input) {
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