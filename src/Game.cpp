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
#include "Player.h"
#include "Computer.h"

Game::Game(fstream &setFile) {
    init();
    setupPile(setFile);
    setupGame();
}

void Game::setupGame() {
    const int start = 7;    // The number of cards each player should start with
    Vector<Card*> nPHand;   // Player hand
    Vector<Card*> nBHand;   // Bot hand
    for(int i = 0; i < start; i++) {    // Fills the computer's and player's hand with cards
        Card *card = drawCard(drwPile);
        nPHand.append(card);
        card = drawCard(drwPile);
        nBHand.append(card);
    }
    player->setHand(nPHand);
    compute->setHand(nBHand);
    Card *card = drawCard(drwPile);
    while(card->color == WILD) {        // If the first played card is a wild, return it to the draw pile and draw another
        drwPile.append(card);
        card = drawCard(drwPile);
    }
    lastPl = card;
    rest = card;
    turn = true;
}

Game::Game(GameS &save) {
    init();
    // Loads the save and converts it into something actually usable (the game struct)
    int colorS = 4;
    player->score = save.plScore;
    compute += save.cpScore;
    Vector<Card*> hand = Vector<Card*>(save.handS);     // To prevent having to reallocate when adding, just preallocate
    Vector<Card*> bHand = Vector<Card*>(save.bHandS);   // To prevent having to reallocate when adding, just preallocate
    turn = save.turn;
    unique = new Card*[54];
    for(int i = 0; i < 54; i++) {   // Recreates the unique cards
        unique[i] = new Card;
        unique[i]->color = save.cards[i].color;
        unique[i]->name = save.cards[i].name;
        for(int j = 0; j < save.draw[i]; j++) { // Adds the unique cards to the draw pile if needed
            drwPile.append(unique[i]);
        }
        for(int j = 0; j < save.play[i]; j++) { // Adds the unique cards to the player hand if needed
            hand.append(unique[i]);
        }
        for(int j = 0; j < save.bot[i]; j++) { // Adds the unique cards to the computer hand if needed
            bHand.append(unique[i]);
        }
        if(unique[i]->color == save.lastPl.color && unique[i]->name == save.lastPl.name) {  // Adds the unique card to the last played var if needed
            lastPl = unique[i];
        }
        if(unique[i]->color == save.rest.color && unique[i]->name == save.rest.name) {      // Adds the unique card to the card restriction var if needed
            rest = unique[i];
        }
    }
    // Passes new hands to player and bot object
    player->setHand(hand);
    compute->setHand(bHand);
    for(int i = 0; i < colorS; i++) {
        if(colors[i]->color == save.rest.color && colors[i]->name == save.rest.name) {  // This is incase the restriction var was a color card created by a wild card
            rest = colors[i];
        }
    }
}

void Game::init() {
    //Initialize Variables
    colorS = 0;
    colors = genColor();
    player = new Player();
    compute = new Computer();
    cout << "Game will have " << player->getCount() << " players/computers" << endl;
}

void Game::playGame() {
    bool uno = false, unoed = false, quit = false;  // Used to keep track of things in between turns
    while(player->score < 500 && compute->getScore() < 500) {   // A game ends when someone has 500 points or more
        Card *card;
        Card *res;
        if(turn) {
            cout << "Current card at play: " << lastPl->printCard() << endl;
            card = player->playCard(rest, drwPile, compute->getHand().size(), uno, unoed, quit, compute);
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
                    compute->draw(drwPile, 2);

                }
                if(card->name == PLUSF) {
                    if(rand()%2 == 1) { // The computer chooses randomly if it wants to challange you or not
                        cout << "The computer choose to not challenge you and will draw 4 cards now" << endl;
                        turn = true;
                        compute->draw(drwPile, 4);
                    } else {
                        cout << "The computer has choosen to challenge you." << endl;
                        if(player->challengeWin(rest)) {
                            cout << "The computer won since you had a matching color card. You now need to draw 4 cards" << endl;
                            player->draw(drwPile, 4);
                        } else {    // If the computer looses the challenge, draw 6 cards
                            cout << "The computer has lost. The computer now needs to draw 6 cards" << endl;
                            turn = true;
                            compute->draw(drwPile, 6);
                        }
                    }
                }
                if(card->name == SKIP) {
                    turn = true;
                }
                if(card->name == REV) {
                    turn = true;
                }
                if(player->getHand().size() == 1 && !unoed && !turn) {   // The logic for checking if the player should have called uno and if they did. Allows the computer to catch them
                    uno = true;
                } else {
                    uno = false;
                }
                unoed = false;
                drwPile.append(lastPl);
                lastPl = card;
                rest = res;
            } else {
                uno = false;
                unoed = false;
            }
        } else {
            card = compute->playCard(rest, drwPile, 0, uno, unoed, quit, player);
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
                    player->draw(drwPile, 2);

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
                        player->draw(drwPile, 4);
                    } else {
                        cout << "You have choosen to challenge the computer." << endl;
                        cout << "The computer hand is: ";   // This is one important change from the previous if statement. The rules say the challenger must be able to see the other's cards
                        for(int i = 0; i < compute->getHand().size(); i++) {
                            cout << printCard(compute->getHand()[i]) << "; ";
                        }
                        cout << endl;
                        if(compute->challengeWin(rest)) {
                            cout << "You won since the computer had a matching color card. The computer now needs to draw 4 cards" << endl;
                            compute->draw(drwPile, 4);
                        } else {
                            cout << "You lost the challenge. You now needs to draw 6 cards" << endl;
                            turn = false;
                            player->draw(drwPile, 6);
                        }
                    }
                }
                if(card->name == SKIP) {
                    turn = false;
                }
                if(card->name == REV) {
                    turn = false;
                }
                if(compute->getHand().size() == 1 && !unoed && turn) {   // This checks if the computer forgot to call uno and allows the user to catch them
                    uno = true;
                } else {
                    uno = false;
                }
                unoed = false;
                drwPile.append(lastPl);
                lastPl = card;
                rest = res;
            } else {
                uno = false;
                unoed = false;
            }
        }
        if(compute->getHand().size() == 0) {  // Checks if the computer ran out of cards and then calculates points earned if thats the case
            cout << "Computer has won!" << endl;
            Vector<Card*> plHand = player->getHand();
            compute += calcPoints(plHand);
            for(int i = 0; i < plHand.size(); i++) {
                drwPile.append(plHand[i]);
            }
            Vector<Card*> empty;
            player->setHand(empty);
            cout << "Current Score" << endl << "Player: " << player->score << endl << "Computer: " << compute->getScore() << endl;
            setupGame();
        } else if(player->getHand().size() == 0) {    // Checks if the player ran out of cards and then calculates points earned if thats the case
            cout << "You have won!" << endl;
            Vector<Card*> cHand = compute->getHand();
            player += calcPoints(cHand);
            for(int i = 0; i < cHand.size(); i++) {
                drwPile.append(cHand[i]);
            }
            Vector<Card*> empty;
            compute->setHand(empty);
            cout << "Current Score" << endl << "Player: " << player->score << endl << "Computer: " << compute->getScore() << endl;
            setupGame();
        }
    }
    if(player->score >= 500 || compute->getScore() >= 500) {    // If someone earns 500+ points, the game ends
        cout << "Game Complete! " << ((compute->getScore() >= 500) ? "Computer" : "Player") << " won!" << endl;
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

Card *Game::drawCard(Vector<Card*> &drwPile) {
    if(drwPile.size() == 0) {
        return nullptr;
    }
    int index = rand() % drwPile.size();
    Card *card = drwPile[index];
    drwPile.remove(index);
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

unsigned int Game::calcPoints(Vector<Card*> deck) {
    unsigned int points = 0;
    for(int i = 0; i < deck.size(); i++) {
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
    if(drwPile.size() == 0) {   // Makes sure this function doesn't run if the draw pile is already setup
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
        for(int i = 0; i < cardLimit; i++) {    // Copies the correct number of each unique card to the draw pile (only their pointers). Jumps to each line as necessary
            setup.seekg(indexes[i], ios::beg);
            int num = 0;
            setup >> num;
            setup >> num;
            setup >> num;
            for(int j = 0; j < num; j++) {
                drwPile.append(unique[i]);
            }
        }
        
    }
}

void Game::createSave(GameS &save) {
    // Saves everything
    save.plScore = player->score;
    save.cpScore = compute->getScore();
    Vector<Card*> comHand = compute->getHand();
    Vector<Card*> plHand = player->getHand();
    save.bHandS = comHand.size();
    save.handS = plHand.size();
    save.drwSize = drwPile.size();
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
        for(int j = 0; j < drwPile.size(); j++) {
            if((drwPile[j]->color == unique[i]->color) && (drwPile[j]->name == unique[i]->name)) {
                save.draw[i]++;
            }
        }
        for(int j = 0; j < plHand.size(); j++) {
            if((plHand[j]->color == unique[i]->color) && (plHand[j]->name == unique[i]->name)) {
                save.play[i]++;
            }
        }
        for(int j = 0; j < comHand.size(); j++) {
            if((comHand[j]->color == unique[i]->color) && (comHand[j]->name == unique[i]->name)) {
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
    delete player;
    delete compute;
}