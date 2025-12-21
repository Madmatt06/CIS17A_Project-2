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

Game::Game(fstream &setFile) {
    init();
    setupPile(setFile);
    setupGame();
}

void Game::setupGame() {
    const int start = 7;    // The number of cards each player should start with
    handS = start;
    if(hand == nullptr) {          // Prevents accidental memory leaks when called after a previous game
        handM = start;
        hand = new Card*[start];
    }
    bHandS = start;
    if(bHand == nullptr) {         // Prevents accidental memory leaks when called after a previous game
        bHandM = start;
        bHand = new Card*[start];
    }
    for(int i = 0; i < start; i++) {    // Fills the computer's and player's hand with cards
        Card *card = drawCard(drwSize, drwPile);
        hand[i] = card;
        card = drawCard(drwSize, drwPile);
        bHand[i] = card;
    }
    Card *card = drawCard(drwSize, drwPile);
    while(card->color == WILD) {        // If the first played card is a wild, return it to the draw pile and draw another
        addCard(drwSize, drwMax, &drwPile, card);
        card = drawCard(drwSize, drwPile);
    }
    lastPl = card;
    rest = card;
    turn = true;
}

Game::Game(GameS &save) {
    init();
    // Loads the save and converts it into something actually usable (the game struct)
    int colorS = 4;
    plScore = save.plScore;
    cpScore = save.cpScore;
    bHandS = save.bHandS;
    bHandM = save.bHandS;
    handS = save.handS;
    handM = save.handS;
    drwSize = save.drwSize;
    drwMax = save.handS + save.bHandS + save.drwSize;
    drwPile = new Card*[drwMax];
    bHand = new Card*[bHandM];
    hand = new Card*[handM];
    turn = save.turn;
    Card **unique = new Card*[54];
    int indexDr = 0;
    int indexPl = 0;
    int indexCp = 0;
    for(int i = 0; i < 54; i++) {   // Recreates the unique cards
        unique[i] = new Card;
        unique[i]->color = save.cards[i].color;
        unique[i]->name = save.cards[i].name;
        for(int j = 0; j < save.draw[i]; j++) { // Adds the unique cards to the draw pile if needed
            drwPile[indexDr] = unique[i];
            indexDr++;
        }
        for(int j = 0; j < save.play[i]; j++) { // Adds the unique cards to the player hand if needed
            hand[indexPl] = unique[i];
            indexPl++;
        }
        for(int j = 0; j < save.bot[i]; j++) { // Adds the unique cards to the computer hand if needed
            bHand[indexCp] = unique[i];
            indexCp++;
        }
        if(unique[i]->color == save.lastPl.color && unique[i]->name == save.lastPl.name) {  // Adds the unique card to the last played var if needed
            lastPl = unique[i];
        }
        if(unique[i]->color == save.rest.color && unique[i]->name == save.rest.name) {      // Adds the unique card to the card restriction var if needed
            rest = unique[i];
        }
    }
    for(int i = 0; i < colorS; i++) {
        if(colors[i]->color == save.rest.color && colors[i]->name == save.rest.name) {  // This is incase the restriction var was a color card created by a wild card
            rest = colors[i];
        }
    }
}

void Game::init() {
    //Initialize Variables
    hand = nullptr;
    bHand = nullptr;
    drwPile = nullptr;
    drwSize = 0;
    drwMax = 0;
    plScore = 0;
    cpScore = 0;
    colorS = 0;
    colors = genColor();
}

void Game::playGame() {
    bool uno = false, unoed = false, quit = false;  // Used to keep track of things in between turns
    while(plScore < 500 && cpScore < 500) {   // A game ends when someone has 500 points or more
        Card *card;
        Card *res;
        if(turn) {
            cout << "Current card at play: " << lastPl->printCard() << endl;
            card = playCard(uno, unoed, quit);
            if(quit) {  // Allows the player to exit the game
                break;
            }
            res = card;
            turn = false;
            if(card != nullptr) {   // This is to catch when the player draws a card and doesn't play it (or if they couldn't do anything)
                // The following if statements handle the special cards (wild and draw cards)
                if(card->color == WILD) {
                    res = playWild(colorS, colors);
                }
                if(card->name == PLUST) {
                    cout << "The computer will draw 2 cards now" << endl;
                    turn = true;
                    int drawnC = 0;
                    for(int i = 0; i < 2; i++) {
                        Card *drawn = drawCard(drwSize, drwPile);
                        if(drawn != nullptr) {
                            drawnC++;
                            addCard(bHandS, bHandM, &bHand, drawn);
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
                        turn = true;
                        int drawnC = 0;
                        for(int i = 0; i < 4; i++) {
                            Card *drawn = drawCard(drwSize, drwPile);
                            if(drawn != nullptr) {
                                drawnC++;
                                addCard(bHandS, bHandM, &bHand, drawn);
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
                        if(challengeWin(rest, hand, handS)) {
                            cout << "The computer won since you had a matching color card. You now need to draw 4 cards" << endl;
                            int drawnC = 0;
                            for(int i = 0; i < 4; i++) {
                                Card *drawn = drawCard(drwSize, drwPile);
                                if(drawn != nullptr) {  // If for some reason a card could not be drawn (running out of cards) keep track of it and don't add a nullptr
                                    drawnC++;
                                    cout << "You drew a " << printCard(drawn) << endl;
                                    addCard(handS, handM, &hand, drawn);
                                }
                            }
                            if(drawnC < 4) {
                                cout << "Only " << drawnC << " cards were drawn." << endl;
                            }
                            if(drwSize == 0) {
                                cout << "The draw pile is now empty." << endl;
                            }
                        } else {    // If the computer looses the challenge, draw 6 cards
                            cout << "The computer has lost. The computer now needs to draw 6 cards" << endl;
                            turn = true;
                            int drawnC = 0;
                            for(int i = 0; i < 6; i++) {
                                Card *drawn = drawCard(drwSize, drwPile);
                                if(drawn != nullptr) {  // If for some reason a card could not be drawn (running out of cards) keep track of it and don't add a nullptr
                                    drawnC++;
                                    addCard(bHandS, bHandM, &bHand, drawn);
                                }
                            }
                            if(drawnC < 6) {
                                cout << "Only " << drawnC << " cards were drawn." << endl;
                            }
                            if(drwSize == 0) {
                                cout << "The draw pile is now empty." << endl;
                            }
                        }
                    }
                }
                if(card->name == SKIP) {
                    turn = true;
                }
                if(card->name == REV) {
                    turn = true;
                }
                if(handS == 1 && !unoed && !turn) {   // The logic for checking if the player should have called uno and if they did. Allows the computer to catch them
                    uno = true;
                } else {
                    uno = false;
                }
                unoed = false;
                addCard(drwSize, drwMax, &drwPile, lastPl);
                lastPl = card;
                rest = res;
            } else {
                uno = false;
                unoed = false;
            }
        } else {
            card = playCom(uno, unoed);
            res = card;
            turn = true;
            if(card != nullptr) {   // Alot of this branch is just like the player one. (catches when the computer doesn't play a card)
                // The following if statements handle the special cards (wild and draw cards)
                if(card->color == WILD) {
                    res = colors[rand()%colorS];
                    cout << "The next played card now must be a " << printCard(res) << endl;
                }
                if(card->name == PLUST) {
                    cout << "You will draw 2 cards now" << endl;
                    turn = false;
                    int drawnC = 0;
                    for(int i = 0; i < 2; i++) {
                        Card *drawn = drawCard(drwSize, drwPile);
                        if(drawn != nullptr) {
                            drawnC++;
                            cout << "You drew a " << printCard(drawn) << endl;
                            addCard(handS, handM, &hand, drawn);
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
                        turn = false;
                        int drawnC = 0;
                        for(int i = 0; i < 4; i++) {
                            Card *drawn = drawCard(drwSize, drwPile);
                            if(drawn != nullptr) {
                                drawnC++;
                                cout << "You drew a " << printCard(drawn) << endl;
                                addCard(handS, handM, &hand, drawn);
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
                        for(int i = 0; i < bHandS; i++) {
                            cout << printCard(bHand[i]) << "; ";
                        }
                        cout << endl;
                        if(challengeWin(rest, bHand, bHandS)) {
                            cout << "You won since the computer had a matching color card. The computer now needs to draw 4 cards" << endl;
                            int drawnC = 0;
                            for(int i = 0; i < 4; i++) {
                                Card *drawn = drawCard(drwSize, drwPile);
                                if(drawn != nullptr) {
                                    drawnC++;
                                    addCard(bHandS, bHandM, &bHand, drawn);
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
                            turn = false;
                            int drawnC = 0;
                            for(int i = 0; i < 6; i++) {
                                Card *drawn = drawCard(drwSize, drwPile);
                                if(drawn != nullptr) {
                                    drawnC++;
                                    cout << "You drew a " << printCard(drawn) << endl;
                                    addCard(handS, handM, &hand, drawn);
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
                    turn = false;
                }
                if(card->name == REV) {
                    turn = false;
                }
                if(bHandS == 1 && !unoed && turn) {   // This checks if the computer forgot to call uno and allows the user to catch them
                    uno = true;
                } else {
                    uno = false;
                }
                unoed = false;
                addCard(drwSize, drwMax, &drwPile, lastPl);
                lastPl = card;
                rest = res;
            } else {
                uno = false;
                unoed = false;
            }
        }
        if(bHandS == 0) {  // Checks if the computer ran out of cards and then calculates points earned if thats the case
            cout << "Computer has won!" << endl;
            cpScore += calcPoints(hand, handS);
            for(int i = 0; i < handS; i++) {
                addCard(drwSize, drwMax, &drwPile, hand[i]);
            }
            handS = 0;
            cout << "Current Score" << endl << "Player: " << plScore << endl << "Computer: " << cpScore << endl;
            setupGame();
        } else if(handS == 0) {    // Checks if the player ran out of cards and then calculates points earned if thats the case
            cout << "You have won!" << endl;
            plScore += calcPoints(bHand, bHandS);
            for(int i = 0; i < bHandS; i++) {
                addCard(drwSize, drwMax, &drwPile, bHand[i]);
            }
            bHandS = 0;
            cout << "Current Score" << endl << "Player: " << plScore << endl << "Computer: " << cpScore << endl;
            setupGame();
        }
    }
    if(plScore >= 500 || cpScore >= 500) {    // If someone earns 500+ points, the game ends
        cout << "Game Complete! " << ((cpScore >= 500) ? "Computer" : "Player") << " won!" << endl;
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

Card *Game::playCard(bool uno, bool &unoed, bool &quit) {
    bool choosen = false;
    Card *usrCard = nullptr;
    while(!choosen) {                               // This forces the user to play a valid card before continuing
        int validS;
        Card **valid = getPlayable(handS, hand, rest, validS);
        if(validS != 0 || drwSize != 0) {      // If its possible for the player to do something, allow them to play
            cout << "The computer has " << bHandS << " cards" << endl;
            printDeck(handS, hand, rest);
            cout << "Enter the card you would like to play (Type it as listed)" << ((drwSize > 0) ?" or type draw to draw a card" : "") << (uno? ". Since the computer didn't call uno, you can type uno to force them to draw" : "") << ": ";
            string input, wordF, wordS;
            getline(cin, input);                    // Gets the card the player wants to play
            bool next = false;
            for(int i = 0; i < input.length(); i++) {
                input[i] = tolower(input[i]);
            }
            if(input == "quit") {                   // Special case for quiting the game
                quit = true;
                delete []valid;
                return nullptr;
            }
            if(input == "draw") {                   // Special case for drawing from the pile
                usrCard = drawCard(drwSize, drwPile);
                if(usrCard == nullptr) {            // In case the user draws a card when there is nothing to draw (asks what to do again)
                    cout << "There are no cards to draw." << endl;
                } else {                            // Tell the user what card they got. If its playable, ask if they want to play it
                    cout << "You got a " << printCard(usrCard) << endl;
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
                            addCard(handS, handM, &hand, usrCard);
                            usrCard = nullptr;
                        }
                    } else {    // Add to hand if not playable
                        addCard(handS, handM, &hand, usrCard);
                        usrCard = nullptr;
                    }
                    choosen = true;
                }
            } else if(input == "uno") { // Handles uno logic
                if(!uno) {  // Checks if it was entered to catch the computer or protect themself
                    if(handS == 2 && validS >= 1) {    // Makes sure they can call uno
                        unoed = true;
                        cout << "You called uno!" << endl;
                    } else {
                        cout << "You can't call uno yet!" << endl;
                    }
                } else {    // Makes the computer draw 4 cards
                    cout << "The computer will draw 4 cards" << endl;
                    int drawnC = 0;
                    for(int i = 0; i < 4; i++) {
                        Card *drawn = drawCard(drwSize, drwPile);
                        if(drawn != nullptr) {
                            drawnC++;
                            addCard(bHandS, bHandM, &bHand, drawn);
                        }
                    }
                    if(drawnC != 4) {
                        cout << "The computer could only draw " << drawnC << " cards." << endl;
                    }
                    if(drwSize == 0) {
                        cout << "The draw pile is now empty." << endl;
                    }
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
                    for(int i = 0; i < validS; i++) {
                        Card *card = valid[i];
                        if(card->color == color && card->name == name) {
                            choosen = true;
                            usrCard = card;
                            removeCard(handS, hand, card);
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
        delete []valid;
    }
    return usrCard;
    
}

Card *Game::playCom(bool uno, bool &unoed) {
    if(uno) {   // If the player forgot to call uno, there is a 50% change the computer catches it
        if(rand()%2 == 1) {
            cout << "The computer caught you with one card!" << endl;
            int drawnC = 0;
            for(int i = 0; i < 4; i++) {
                Card *drawn = drawCard(drwSize, drwPile);
                if(drawn != nullptr) {
                    drawnC++;
                    addCard(handS, handM, &hand, drawn);
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
    Card **valid = getPlayable(bHandS, bHand, rest, validS);
    if(validS != 0 || drwSize != 0) {  // If the computer can do something
        if(validS != 0) {                   // If the computer doesn't have to draw, play a card
            int index = rand()%validS;
            usrCard = valid[index];
            removeCard(bHandS, bHand, usrCard);
            cout << "The computer played the card " << printCard(usrCard) << endl;
        } else {
            cout << "The computer drawed a card" << endl;
            Card *drawn = drawCard(drwSize, drwPile);
            addCard(bHandS, bHandM, &bHand, drawn);
        }
        if(bHandS == 1 && rand()%2 == 1) { // There is a 50% chance the computer remembers to call uno. I know this computer is really dumb
            cout << "The computer called uno!" << endl;
            unoed = true;
        }
    } else {    // If the computer can't play anything and for some reason the draw pile is completly empty, skip their turn
        cout << "The computer couldn't play anything and the draw pile was empty." << endl;
        unoed = true;   // Just incase
    }
    delete []valid;
    return usrCard;
}

Card *Game::playWild(int size, Card **colors) {
    bool picked = false;
    Card *cardR;    // Card used to restrict to a color
    while(!picked) {    // keeps looping until a color is obtained
        cout << "Choose a color (Red, Green, Yellow, Blue): ";
        string input;
        getline(cin, input);
        for(int i = 0; i < input.length(); i++) {   // Lower case for easier parsing
            input[i] = tolower(input[i]);
        }
        int color;
        color = strToCol(input);
        if(color != -1 && color != WILD) {
            for(int i = 0; i < size; i++) {
                Card *card = colors[i];
                if(card->color == color) {
                    picked = true;
                    cardR = card;
                    break;
                }
            }
            if(!picked) {
                cout << "ERROR: Could not find card color..." << endl;
            }
        } else {
            cout << "Invalid input" << endl;
        }
    }
    return cardR;
}

Card *Game::drawCard(int &size, Card **draw) {
    if(size == 0) {
        return nullptr;
    }
    int index = rand() % size;
    Card *card = draw[index];
    removeCard(size, draw, card);
    return card;
}

void Game::addCard(int &size, int &maxSize, Card ***hand, Card *card) {
    if(size >= maxSize) {   // If the array is not large enough, replace it with a bigger one
        maxSize += 10;
        Card **handN = new Card*[maxSize];
        copy(size, *hand, handN);
        delete [](*hand);
        *hand = handN;
    }
    (*hand)[size] = card;
    size++;
}

bool Game::challengeWin(Card *check, Card **deck, int size) {
    for(int i = 0; i < size; i++) {
        Card *card = deck[i];
        if(check->color == card->color) {       // If there was a playable card with a matching color, then the challenger wins
            return true;
        }
    }
    return false;
}

void Game::printDeck(int size, Card **deck, Card *active) {
    cout << "Cards in hand: ";
    for(int i = 0; i < size; i++) {
        cout << printCard(deck[i]) << "; ";     // Prints all the cards in the array
    }
    cout << endl;
    Card **play;
    int playS;
    play = getPlayable(size, deck, active, playS);
    cout << "Playable cards in hand: ";
    for(int i = 0; i < playS; i++) {
        cout << printCard(play[i]) << "; ";     // Prints all playable cards
    }
    cout << endl;
    delete []play;      // Frees memory
}

string Game::printCard(Card *card) {
    string display;
    // Converts the color enum into a human readable string
    switch(card->color) {
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
    switch(card->name) {
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
        display += to_string(card->name);
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

unsigned int Game::calcPoints(Card **deck, int size) {
    unsigned int points = 0;
    for(int i = 0; i < size; i++) {
        Card *card = deck[i];
        switch(card->name) {
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
            points += card->name;   // Cards 0-9 are valued at their number
            break;
            case PLUST:
            case REV:
            case SKIP:
            points += 20;           // Special non wild cards are valued at 20 points each
            break;
            case CARD:
            case PLUSF:
            points += 50;           // Wild cards (type CARD and PLUSF will always be a wild) are valued at 50 points each
            break;
            default:
            break;
        }
    }
    return points;
}

void Game::setupGame(Game &game) {
    const int start = 7;    // The number of cards each player should start with
    game.handS = start;
    if(game.hand == nullptr) {          // Prevents accidental memory leaks when called after a previous game
        game.handM = start;
        game.hand = new Card*[start];
    }
    game.bHandS = start;
    if(game.bHand == nullptr) {         // Prevents accidental memory leaks when called after a previous game
        game.bHandM = start;
        game.bHand = new Card*[start];
    }
    for(int i = 0; i < start; i++) {    // Fills the computer's and player's hand with cards
        Card *card = drawCard(game.drwSize, game.drwPile);
        game.hand[i] = card;
        card = drawCard(game.drwSize, game.drwPile);
        game.bHand[i] = card;
    }
    Card *card = drawCard(game.drwSize, game.drwPile);
    while(card->color == WILD) {        // If the first played card is a wild, return it to the draw pile and draw another
        addCard(game.drwSize, game.drwMax, &game.drwPile, card);
        card = drawCard(game.drwSize, game.drwPile);
    }
    game.lastPl = card;
    game.rest = card;
    game.turn = true;
}

Card **Game::getPlayable(int size, Card **deck, Card *active, int &plSize) {
    Card **play;                        // Playable cards
    int playS = 0;                      // Playable cards size
    for(int i = 0; i < size; i++) {     // Calculates how large the new array needs to be to hold the playable cards
        Card *card = deck[i];
        if(card->color == WILD || card->color == active->color || card->name == active->name) {
            playS++;
        }
    }
    play = new Card*[playS];            // Creates the array with the correct size
    plSize = playS;
    int plIndex = 0;
    for(int i = 0; i < size; i++) {
        if(plIndex >= playS) {          // Ends loop early when it should be done
            break;
        }
        Card *card = deck[i];
        // Adds all playable cards to the list
        if(card->color == WILD || card->color == active->color || card->name == active->name) {
            play[plIndex] = card;
            plIndex++;
        }
    }
    return play;
}

void Game::copy(int size, Card **arr1, Card **arr2) {
    for(int i = 0; i < size; i++) {
        arr2[i] = arr1[i];
    }
}

void Game::removeCard(int &size, Card **hand, Card *card) {
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

Card **Game::genColor() {
    colors = new Card*[4];
    colors[0] = new Card{RED, CARD};
    colors[1] = new Card{GREEN, CARD};
    colors[2] = new Card{YELLOW, CARD};
    colors[3] = new Card{BLUE, CARD};
    colorS = 4;
    return colors;
}

void Game::setupPile(fstream &setup) {
    if(drwPile == nullptr) {   // Makes sure this function doesn't run if the draw pile is already setup
        const int cardLimit = 54;   // Uno only has 54 unique cards under normal rules
        long int indexes[cardLimit] = {};   // Used to jump around to the correct spots in the file
        unique = new Card*[cardLimit]; 
        int total = 0;
        for(int i = 1; i < cardLimit; i++) {    // Finds where the new lines are to get the position each card is located at in the file (first one is alwasy 0)
            string line;
            getline(setup, line);
            indexes[i] = setup.tellg();
        }
        for(int i = 0; i < cardLimit; i++) {    // Starts by jumping to each index to create each unique card and counts how large the draw pile needs to be
            setup.seekg(indexes[i], ios::beg);
            Card *card = new Card;
            int num = 0;
            setup >> card->color;
            setup >> card->name;
            setup >> num;
            total += num;
            unique[i] = card;
        }
        // Creates the draw pile
        drwMax = total;
        drwSize = total;
        drwPile = new Card*[total];
        int drwInd = 0;
        for(int i = 0; i < cardLimit; i++) {    // Copies the correct number of each unique card to the draw pile (only their pointers). Jumps to each line as necessary
            setup.seekg(indexes[i], ios::beg);
            int num = 0;
            setup >> num;
            setup >> num;
            setup >> num;
            for(int j = 0; j < num; j++) {
                drwPile[drwInd] = unique[i];
                drwInd++;
            }
        }
        
    }
}

void Game::createSave(GameS &save) {
    // Saves everything
    save.plScore = plScore;
    save.cpScore = cpScore;
    save.bHandS = bHandS;
    save.handS = handS;
    save.drwSize = drwSize;
    save.turn = turn;
    save.lastPl.color = lastPl->color;
    save.lastPl.name  = lastPl->name;
    save.rest.color = rest->color;
    save.rest.name  = rest->name;
    for (int i = 0; i < 54; i++) {  // Searches for each card type, counts them, and then saves them to the correct variable
        save.draw[i] = 0;
        save.play[i] = 0;
        save.bot[i]  = 0;
        save.cards[i].color = unique[i]->color;
        save.cards[i].name = unique[i]->name;
        for(int j = 0; j < drwSize; j++) {
            if((drwPile[j]->color == unique[i]->color) && (drwPile[j]->name == unique[i]->name)) {
                save.draw[i]++;
            }
        }
        for(int j = 0; j < handS; j++) {
            if((hand[j]->color == unique[i]->color) && (hand[j]->name == unique[i]->name)) {
                save.play[i]++;
            }
        }
        for(int j = 0; j < bHandS; j++) {
            if((bHand[j]->color == unique[i]->color) && (bHand[j]->name == unique[i]->name)) {
                save.bot[i]++;
            }
        }
    }
}

Game::~Game() {
    for(int i = 0; i < 54; i++) {
        delete unique[i];
    }
    for(int i = 0; i < 4; i++) {
        delete colors[i];
    }
    delete []unique;
    delete []colors;
    delete []drwPile;
    delete []bHand;
    delete []hand;
}