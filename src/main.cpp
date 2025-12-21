/* 
 * File:   main.cpp
 * Author: Matthew Madrigal
 * Created on October 31st, 2025, 3:52 pm,
 * Purpose:  To create an Uno game in C++ using what has been learned so far. These are the rules used for this game: https://service.mattel.com/instruction_sheets/42001pr.pdf
 *           The blank cards are not included since they are for custom rules or replacing lost cards.
 */

//System Libraries
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
using namespace std;

//User Libraries
#include "Card.h"
#include "Game.h"
#include "GameS.h"

//Global Constants - Math/Science/Conversions only

//Function Prototypes


//Execution Begins Here
int main(int argc, char** argv) {
    //Set random seed
    srand(time(0));
    //Declare Variables
    Game *game;
    GameS saved;
    Card **unique;  // Instead of creating duplicate cards when there is more than one type of card, a single card is created and that pointer is duplicated the number of times needed in the draw pile.
    Card **colors;  // Restriction cards for playing wild cards
    fstream setup;  // File used to setup the draw pile for a new game
    fstream save;   // File used to restore the state of the program from when it was last ran
    
    //Initialize Variables
    
    //The Process -> Map Inputs to Outputs
    // Checks if a save file exists and if the game didn't end yet
    save.open("save.data", ios::in | ios::out | ios::binary);
    if(save.is_open()) {
        cout << "Save data found. Resuming using save data." << endl;
        save.read(reinterpret_cast<char *>(&saved), sizeof(saved));
        if(saved.cpScore >= 500 || saved.plScore >= 500) {    // The game ends if someone scores 500 points. Create a new game if that happened
            cout << "Save data is from a completed game, creating new game." << endl;
            setup.open("start.txt", ios::in);
            game = new Game(setup);
            setup.close();
        } else {    // Load previous state for game
            game = new Game(saved);
        }
    } else {
        cout << "No save data found. Starting a new game." << endl;
        // Creates a save file
        save.close();
        save.open("save.data", ios::out | ios::binary);
        save.close();
        save.open("save.data", ios::in | ios::out | ios::binary);
        // Reads file for what cards to include in a new game
        setup.open("start.txt", ios::in);
        game = new Game(setup);
        setup.close();
    }

    //Display Inputs/Outputs
    // Gives instructions and plays game
    cout << "Welcome to Uno. When you are asked to play a card, you can either play one or draw. When you are going to have one card left, you must first type and enter uno before entering the card you wish to play. If the computer forgets to call uno, you can penalize them by entering uno before playing a card. To save and quit, type quit when asked to play a card." << endl;
    game->playGame();

    // Saves game state
    save.seekp(0L, ios::beg);
    game->createSave(saved);
    save.write(reinterpret_cast<char*>(&saved), sizeof(saved));

    //Exit the Program
    // Clean up memory
    delete game;
    save.close();
    return 0;
}

