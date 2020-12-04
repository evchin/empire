#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <stdlib.h>
#include <ctime>
#include <string>

using namespace std;

/** FUNCTIONS  **/

// MAIN LEVEL
void welcome();                     // runs starting screen
void configGame(string fileName);   // set up a game given a file name
void runTutorial();                 // runs game tutorial
void runWave();                     // runs a single wave
void runMinigame();                 // runs minigame
void saveGame();                    // saves current game into text file specified by player
void gameOver();                    // screen when game is lost
void win();                         // screen when game is won

// MAP FUNCTIONS
void setBattlemap();                                        // sets up battlemap string from battlefield array 
void displayBattlemap();                                    // displays the battlemap
void setEnergyMap();                                        // sets up energy map for wizard recharges
void displayEnergyMap();                                    // displays the energy map
string getMiniBattleMap(int battlefield[][10], int rows);   // sets up and returns minigame's battlemap string

// STEP LEVEL
void buyAsset();                                        // buy an asset and place it on the battlemap
void removeAsset();                                     // remove an asset from the battlemap
void attack();                                          // enemies advance one step
void mineAndDefend();                                   // assets defend one step and miners mine one step
void generateEnemies();                                 // generate new wave of enemies each step
void generateGold();                                    // auto generate gold each step
void updateScore();                                     // update the score each step
void updateGameStates(int& currentStep);                // check for game over, update wizard recharges, current step, and incremented variables

// BASE LEVEL
string getFilePreset();                                         // get a valid name for the preset file from the user
string getSaveFileName();                                       // get a valid name for the save file from the user
int getTotalObjectsOnBattlefield();                             // returns total number of objects on battlefield
bool updatePosition(int row1, int col1, int row2, int col2);    // update the position of the object in (row1, col1) to (row2, col2)
void decreaseHealth(int row, int col, int damage);              // decrease health of object at (row, col) by <damage>
void spawnEnemy(int row, int col);                              // spawn a random enemy at (row, col)
bool isEnemy(int row, int col);                                 // return true if object at (row, col) is an enemy    
bool getYN(string question);                                    // return true if user answers yes to a given question
void printDivider(int length);                                  // print a divider of integer length
void newLine();                                                 // clear input

/** GLOBAL VARIABLES (indicated by the underscore in front) **/

// META
int _battlefield[500][11] = {};
int _health[500][11] = {};
int _energy[500][10] = {};
string _battleMap = "";
string _energyMap = "";
bool _gameOver = false;

// STATS
int _assetCosts[11] = {0, 25, 40, 25, 50, 25, 25, 75, 10, 20, 30};
int _assetHealth[11] = {0, 20, 20, 1, 5, 10, 15, 30, 10, 20, 30};
int _assetDamage[11] = {0, 0, 2, 10, 0, 4, 8, 10, 4, 8, 10};
int _minerEffectiveness = 5;
int _wizardRecharge = 3;

// IN GAME STATES
int _rows = 10;
int _gold = 0;
int _score = 0;
int _totalWaves = 0;
int _currentWave = 1;
int _goldPerWave = 100;
int _enemiesPerWave = 0;
int _enemiesPerWaveIncrement = 0;
int _enemiesPerWaveIncrementSteps = 0;
int _enemiesKilledInWave = 0;
int _enemiesLeftToSpawn = 0;
int _newEnemiesPerStep = 0;
int _newEnemiesPerStepIncrement = 0;
int _newEnemiesPerStepIncrementSteps = 0;
int _goldGeneration = 0;
int _minigameIncrement = 1;
int _totalCastleHealth = 50;
int _currentCastleHealth = 50;
int _enemySoldierProportion = 70;
int _enemyCaptainProportion = 20;
int _enemyGeneralProportion = 10;

int main()
{
    srand(time(0));

    welcome();

    string fileName = getFilePreset();
    configGame(fileName);
    setBattlemap();

    while (_currentWave <= _totalWaves)
    {
        runWave();

        if (_currentWave % _enemiesPerWaveIncrementSteps == 0)
            _enemiesPerWave += _enemiesPerWaveIncrement;

        if (_currentWave < _totalWaves)
        {
            _gold += _goldPerWave;
            cout << "Congratulations! You have survived another wave. " << _goldPerWave << " gold has been added to your wealth." << endl << endl;
            if (_currentWave % _minigameIncrement == 0) runMinigame();
            _currentWave++;
            saveGame();
            printDivider(100);
            cout << "You are about to enter wave " << _currentWave << "." << endl;
        }
        else _currentWave++;
    }

    win();
    return 0;
}

// MAIN FUNCTIONS

void welcome()
{
    cout << string("  ______ __  __ _____ _____ _____  ______ \n")+
                    " |  ____|  \\/  |  __ \\_   _|  __ \\|  ____|\n"+
                    " | |__  | \\  / | |__) || | | |__) | |__   \n"+
                    " |  __| | |\\/| |  ___/ | | |  _  /|  __|  \n"+
                    " | |____| |  | | |    _| |_| | \\ \\| |____ \n"+
                    " |______|_|  |_|_|   |_____|_|  \\_\\______|\n\n";

    cout << "IMPORTANT: Please maximize your window to make it full screen." << endl << endl;
    bool beenHere = getYN("Welcome to the Empire. Have you been here before?");

    if (!beenHere)
        runTutorial();
    else
        cout << "Excellent. Welcome back." << endl << "Now let us begin." << endl << endl;
}

void configGame(string fileName)
{
    for (int i = 0; i < _rows; i++)
    {
        for (int j = 0; j < 11; j++)
        {
            _battlefield[i][j] = 0;
            _health[i][j] = 0;
            _energy[i][j] = 0;
        }
    }
    
    ifstream inputFile;
    inputFile.open(fileName);

    while (inputFile.fail())
    {
        cout << "Cannot open file." << endl << endl;
        fileName = getFilePreset();
        inputFile.open(fileName);
    }

    inputFile >> _rows >> _totalWaves >> _currentWave >> _goldPerWave >> _minigameIncrement;
    inputFile >> _enemiesPerWave >> _enemiesPerWaveIncrement >> _enemiesPerWaveIncrementSteps >> _enemiesKilledInWave;
    inputFile >> _newEnemiesPerStep >> _newEnemiesPerStepIncrement >> _newEnemiesPerStepIncrementSteps;
    inputFile >> _enemySoldierProportion >> _enemyCaptainProportion >> _enemyGeneralProportion;

    inputFile >> _totalCastleHealth >> _currentCastleHealth >> _gold >> _goldGeneration;
    inputFile >> _minerEffectiveness >> _wizardRecharge;

    inputFile >> _assetHealth[1] >> _assetHealth[2] >> _assetHealth[4] >> _assetHealth[5] >> _assetHealth[6] >> _assetHealth[7] >> _assetHealth[8] >> _assetHealth[9] >> _assetHealth[10];
    inputFile >> _assetCosts[1] >> _assetCosts[2] >> _assetCosts[3] >> _assetCosts[4] >> _assetCosts[5] >> _assetCosts[6] >> _assetCosts[7] >> _assetCosts[8] >> _assetCosts[9] >> _assetCosts[10];
    inputFile >> _assetDamage[2] >> _assetDamage[3] >> _assetDamage[5] >> _assetDamage[6] >> _assetDamage[7] >> _assetDamage[8] >> _assetDamage[9] >> _assetDamage[10];

    int totalObjectsOnBattlefield;
    inputFile >> totalObjectsOnBattlefield;

    for (int i = 0; i < totalObjectsOnBattlefield; i++)
    {
        int code, row, column, health;
        inputFile >> code >> row >> column >> health;

        _battlefield[row][column] = code;

        if (health == -1)
            _health[row][column] = _assetHealth[code];
        else _health[row][column] = health;

        if (code == 7)
            _energy[row][column] = _wizardRecharge;
    }

    inputFile.close();
    updateScore();
    cout << endl;
}

void runTutorial()
{
    cout << "Welcome! Your mission is to defend the empire against the enemy, using your army, defensive units, and wits.\n";
    cout << "Let me show you around.\n\n";

    cout << "This is a sample map of a battlefield. \nThe map to the left is the BATTLEMAP, displaying your defenses and enemies.\n";
    cout << "The map to the right is your HEALTHMAP, displaying each assets' respective health values." << endl << endl;
    cout << "Notice how there are numbered labels at the left and the top for both maps. \n";
    cout << "These will serve as your coordinate system, where points are labeled as (row, column).\n\n";
    cout << "Our kingdom lies to the west. The enemy will come from the east.\n";
    cout << "The brick wall at the west is our castle. If the enemy reaches the final column of the west (column 0),\n";
    cout << "the castle will take damage. You must defeat all waves of the enemy before they destroy the castle.\n\n";

    configGame("1_Beginner.txt");
    setBattlemap();
    cout << _battleMap << endl;

    cout.setf(ios::right);
    cout << setw(150) << "Got it. Where's my army? (Press enter to continue.)\n";
    newLine();

    cout << "\n\nThese are the people available to serve the empire.\nIMPORTANT: The stats below are the default values, but may be modified through loaded presets." << endl << endl;
    cout << "The symbol below represents a MINER. \nOne miner will help you generate gold every step.\n\n---)\n\n";
    cout << "STATS:\nDamage: 0\nHealth: 5\nCost: 50 gold\n";
    printDivider(80);
    cout << "The symbol below represents an ARCHER. \nArchers help you attack enemies from a distance, but have less health and damage.\n\nD-->\n\n";
    cout << "STATS:\nDamage: 2\nHealth: 10\nCost: 25 gold\n";
    printDivider(80);
    cout << "The symbol below represents a SWORDSMAN. \nSwordsmen help you deal more damage to enemies, but lack the range of archers.\n\n-}--\n\n";
    cout << "STATS:\nDamage: 4\nHealth: 15\nCost: 25 gold\n";
    printDivider(80);
    cout << "The symbol below represents a WIZARD. \nWizards deal incredible damage, but take a long time to recharge their energy.\n\n~__~\n\n";
    cout << "STATS:\nDamage: 10\nHealth: 30\nCost: 75 gold\n";
    cout << endl;
    cout << setw(150) << "Okay. What does my defense look like? (Press enter to continue.)\n";
    newLine();

    cout << "These are the defenses at your disposal." << endl << endl;
    cout << "The symbol below represents a WALL. \nThese basic walls act as barriers to hold off enemies temporarily.\n\n|__|\n\n";
    cout << "STATS:\nDamage: 0\nHealth: 20\nCost: 25 gold\n";
    printDivider(80);
    cout << "The symbol below represents a PIKE WALL. \nThese sharp walls are similar to regular walls, except they also deal damage to enemies.\n\n////\n\n";
    cout << "STATS:\nDamage: 1\nHealth: 20\nCost: 40 gold\n";
    printDivider(80);
    cout << "The symbol below represents a MINE. \nMines can only be used once, but they deal intense damage to the unlucky enemy unit that steps on it.\n\n[[]]\n\n";
    cout << "STATS:\nDamage: 10\nHealth: 0\nCost: 25 gold\n";
    cout << endl;
    cout << setw(150) << "Awesome. What do I need to know about these enemies? (Press enter to continue.)\n";
    newLine();

    cout << "These are the enemies the empire faces." << endl << endl;
    cout << "The symbol below represents an ENEMY SOLDIER. \nThey are the most basic units, but in large numbers, they should not be trifled with.\n\n>__<\n\n";
    cout << "STATS:\nDamage: 2\nHealth: 10\n";
    printDivider(80);
    cout << "The symbol below represents an ENEMY CAPTAIN. \nThey deal a little more damage than a common soldier, and have more health.\n\nQ__Q\n\n";
    cout << "STATS:\nDamage: 5\nHealth: 20\n";
    printDivider(80);
    cout << "The symbol below represents an ENEMY GENERAL. \nOur only advice is to avoid them as much as possible...\n\nT__T\n\n";
    cout << "STATS:\nDamage: 10\nHealth: 30\n";
    cout << endl;
    cout << setw(150) << "...will keep that in mind. Anything else? (Press enter to continue.)\n";
    newLine();

    cout << "The enemy will come in waves. Each wave will be divided into steps as the enemy comes marching to the kingdom. \n";
    cout << "With each step, you will be given the option to take action or pass your turn.\n";
    cout << "Good luck. The future of the empire depends on you.\n\n";

    printDivider(150);
    cout.unsetf(ios::right);
}

void runWave()
{
    int currentStep = 1;
    _enemiesLeftToSpawn = _enemiesPerWave;
    _enemiesKilledInWave = 0;
    
    while (_enemiesPerWave > _enemiesKilledInWave)
    {
        printDivider(100);
        setBattlemap();
        displayBattlemap();

        bool pass = false;
        while (!pass)
        {
            int action;
            cout << "What would you like to do?\n";
            cout << "(1) Buy Asset" << endl;
            cout << "(2) Remove Asset" << endl;
            cout << "(3) See Energy Map" << endl;
            cout << "(4) Pass Turn" << endl;
            cin >> action;

            while (cin.fail() || action < 1 || action > 4)
            {
                cout << "Invalid Input!" << endl << "What would you like to do next? " << endl;
                cin.clear();
                newLine();
                cin >> action;
            }
            newLine();
            cout << endl;

            switch (action)
            {
                case 1:
                    buyAsset();
                    setBattlemap();
                    updateScore();
                    displayBattlemap();
                    break;
                case 2:
                    removeAsset();
                    setBattlemap();
                    updateScore();
                    displayBattlemap();
                    break;
                case 3:
                    setEnergyMap();
                    displayEnergyMap();
                    break;
                case 4:
                    cout << "You have passed your turn." << endl;
                    pass = true;
                    break;
            }

            if (!pass)
            {
                bool anotherAction;
                anotherAction = getYN("Would you like to take another action?");
                if (!anotherAction) pass = true;
            }
        }

        generateGold();
        mineAndDefend();
        attack();
        generateEnemies();

        updateGameStates(currentStep);
        if (_gameOver) return;
        updateScore();
    }
    cout << endl;
}

void runMinigame()
{
    cout << "Welcome to the Memory Minigame." << endl << "A map of five enemy soldiers will be shown. Your task is to memorize their coordinates as best as you can before time runs out!\n";
    cout << "With each coordinate point you get correct, you will earn 50 gold for your next wave!" << endl;
    cout << "You will have five seconds. Press enter when you're ready. ";
    newLine();
    cout << endl << endl;

    int battlefield[10][10] = {};
    for (int i = 0; i < 5; i++)
    {
        int row = rand() % 10;
        int col = rand() % 10;
        if (battlefield[row][col] != 0) i--;
        else battlefield[row][col] = 8;
    }

    string battlemap = getMiniBattleMap(battlefield, 10);
    cout << "Now quick! You have five seconds to memorize the locations of these five enemy soldiers." << endl << endl;
    cout << battlemap << endl;

    time_t t1 = time(0);
    time_t t2 = time(0);

    while (t2 - t1 <= 5)
    {
        t2 = time(0);
    }
    
    for (int i = 0; i < 1000; i++) cout << "\n";

    cout << "Time's up! Let's test your memory now. You cannot enter the same location twice." << endl << endl;
    for (int i = 0; i < 5; i++)
    {
        int row, col;
        cout << "Attempts Left: " << 5 - i << endl;
        cout << "What was one location of an enemy soldier? Enter in the format of <row> <col>. ";
        cin >> row >> col;

        while (cin.fail() || row < 0 || row > 9 || col < 0 || col > 9)
        {
            cout << "Invalid Input! What was one location of an enemy soldier? Enter in the format of <row> <col>. ";
            cin.clear();
            newLine();
            cin >> row >> col;
        }
        newLine();

        if (battlefield[row][col] == 8)
        {
            cout << "Correct! You've won 50 more gold pieces.";
            _gold += 50;
            battlefield[row][col] = -1;
        }
        else if (battlefield[row][col] == -1)
            cout << "You've already guessed this location!";
        else cout << "Incorrect!";

        cout << endl << endl;
    }
}

void saveGame()
{
    bool save = getYN("Would you like to save your game?");
    if (!save) return;

    ofstream outputFile;
    string fileName = getSaveFileName();
    outputFile.open(fileName);
    while (outputFile.fail())
    {
        cout << "Cannot open file." << endl << endl;
        fileName = getSaveFileName();
        outputFile.open(fileName);
    }

    outputFile << _rows << endl << _totalWaves << endl << _currentWave << endl << _goldPerWave << endl << _minigameIncrement << endl;
    outputFile << _enemiesPerWave << endl << _enemiesPerWaveIncrement << endl << _enemiesPerWaveIncrementSteps << endl;
    outputFile << _enemiesKilledInWave << endl << _newEnemiesPerStep << endl << _newEnemiesPerStepIncrement << endl << _newEnemiesPerStepIncrementSteps << endl;
    outputFile << _enemySoldierProportion << endl << _enemyCaptainProportion << endl << _enemyGeneralProportion << endl;
    outputFile << _totalCastleHealth << endl << _currentCastleHealth << endl << _gold << endl << _goldGeneration << endl << _minerEffectiveness << endl << _wizardRecharge << endl;
    outputFile << _assetHealth[1] << endl << _assetHealth[2] << endl << _assetHealth[4] << endl << _assetHealth[5] << endl;
    outputFile << _assetHealth[6] << endl << _assetHealth[7] << endl << _assetHealth[8] << endl << _assetHealth[9] << endl << _assetHealth[10] << endl;
    outputFile << _assetCosts[1] << endl << _assetCosts[2] << endl << _assetCosts[3] << endl << _assetCosts[4] << endl << _assetCosts[5] << endl << _assetCosts[6] << endl << _assetCosts[7] << endl;
    outputFile << _assetCosts[8] << endl << _assetCosts[9] << endl << _assetCosts[10] << endl;
    outputFile << _assetDamage[2] << endl << _assetDamage[3] << endl << _assetDamage[5] << endl << _assetDamage[6] << endl;
    outputFile << _assetDamage[7] << endl << _assetDamage[8] << endl << _assetDamage[9] << endl << _assetDamage[10] << endl;

    int totalObjectsOnBattlefield = getTotalObjectsOnBattlefield();
    outputFile << totalObjectsOnBattlefield << endl;

    for (int i = 0; i < _rows; i++)
    {
        for (int j = 0; j < 11; j++)
        {
            if (_battlefield[i][j] > 0)
            {
                outputFile << _battlefield[i][j] << " ";
                outputFile << i << " ";
                outputFile << j << " ";
                outputFile << _health[i][j] << endl;
            }
        }
    }

    outputFile.close();
    cout << endl;
    cout << "Successfully saved file " << fileName << endl << endl;
}

void gameOver()
{
    _gameOver = true;

    cout << string("      _____          __  __ ______    ______      ________ _____  \n") + 
                    "     / ____|   /\\   |  \\/  |  ____|  / __ \\ \\    / /  ____|  __ \\ \n" + 
                    "    | |  __   /  \\  | \\  / | |__    | |  | \\ \\  / /| |__  | |__) |\n" + 
                    "    | | |_ | / /\\ \\ | |\\/| |  __|   | |  | |\\ \\/ / |  __| |  _  / \n" + 
                    "    | |__| |/ ____ \\| |  | | |____  | |__| | \\  /  | |____| | \\ \\ \n" + 
                    "     \\_____/_/    \\_\\_|  |_|______|  \\____/   \\/   |______|_|  \\_\\ \n\n";

    cout << "Your castle has been destroyed!" << endl;
    bool startOver = getYN("Would you like to try again?");
    if (startOver)
        main();
    else exit(1);
    return;
}

void win()
{
    cout << string("   _____ ____  _   _  _____ _____         _______ _    _ _            _______ _____ ____  _   _  _____ \n") +
            "  / ____/ __ \\| \\ | |/ ____|  __ \\     /\\|__   __| |  | | |        /\\|__   __|_   _/ __ \\| \\ | |/ ____|\n" + 
            " | |   | |  | |  \\| | |  __| |__) |   /  \\  | |  | |  | | |       /  \\  | |    | || |  | |  \\| | (___  \n" + 
            " | |   | |  | | . ` | | |_ |  _  /   / /\\ \\ | |  | |  | | |      / /\\ \\ | |    | || |  | | . ` |\\___ \\ \n" +
            " | |___| |__| | |\\  | |__| | | \\ \\  / ____ \\| |  | |__| | |____ / ____ \\| |   _| || |__| | |\\  |____) |\n" +
            "  \\_____\\____/|_| \\_|\\_____|_|  \\_\\/_/    \\_\\_|   \\____/|______/_/    \\_\\_|  |_____\\____/|_| \\_|_____/ \n\n";
    cout << "You have successfully defended the empire." << endl << endl;

    bool again = getYN("Would you like to play again?");
    if (again) 
    {
        main();
        return;
    }
    else exit(1);
}

// MAP FUNCTIONS

void setBattlemap()
{
    _battleMap = "            00    01    02    03    04    05    06    07    08    09    \t\t\t\t";
    _battleMap += "    00    01    02    03    04    05    06    07    08    09    \n\n";

    for (int i = 0; i < _rows; i++)
    {
        if (i < 10) _battleMap += "[==]  0" + to_string(i) + "  ";
        else _battleMap += "[==]  " + to_string(i) + "  ";

        for (int j = 0; j < 11; j++)
        {
            _battleMap += " ";
            int code = _battlefield[i][j];
            switch(code)
            {
                case 1: _battleMap += "|__|"; break;
                case 2: _battleMap += "////"; break;
                case 3: _battleMap += "[[]]"; break;
                case 4: _battleMap += "---)"; break;
                case 5: _battleMap += "D-->"; break;
                case 6: _battleMap += "-}--"; break;
                case 7: _battleMap += "~__~"; break;
                case 8: _battleMap += ">__<"; break;
                case 9: _battleMap += "Q__Q"; break;
                case 10: _battleMap += "T__T"; break;
                default: _battleMap += "    ";
            }
            _battleMap += " ";
        }
        _battleMap += "\t\t\t";

        if (i < 10) _battleMap += "[==]  0" + to_string(i) + "  ";
        else _battleMap += "[==]  " + to_string(i) + "  ";

        for (int j = 0; j < 11; j++)
        {
            _battleMap += " ";

            if (_health[i][j] == 0)
                _battleMap += "    ";
            else if (_health[i][j] < 10)
                _battleMap += "[0" + to_string(_health[i][j]) + "]";
            else
                _battleMap += "[" + to_string(_health[i][j]) + "]";

            _battleMap += " ";
        }
        _battleMap += "\n\n";
    }
}

void displayBattlemap()
{
    cout << "\nWAVE: " << _currentWave << " / " << _totalWaves << endl;
    cout << "SCORE: " << _score << endl;
    cout << "CASTLE HEALTH: " << _currentCastleHealth << endl;
    cout << _gold << " GOLD" << endl << endl;
    cout << _battleMap;
}

void setEnergyMap()
{
    _energyMap = "        00    01    02    03    04    05    06    07    08    09    \n";

    for (int i = 0; i < _rows; i++)
    {
        if (i < 10) _energyMap += "  0" + to_string(i) + "  ";
        else _energyMap += "  " + to_string(i) + "  ";

        for (int j = 0; j < 10; j++)
        {
            _energyMap += " ";

            if (_battlefield[i][j] == 7)
            {
                if (_energy[i][j] < 10)
                    _energyMap += "[0" + to_string(_energy[i][j]) + "]";
                else
                    _energyMap += "[" + to_string(_energy[i][j]) + "]";
            }
            else _energyMap += "    ";

            _energyMap += " ";
        }
        _energyMap += "\n\n";
    }
}

void displayEnergyMap()
{
    cout << endl;
    cout << "Energy Map for Wizards in Charges" << endl;
    cout << "Full Charge: " << _wizardRecharge << endl << endl;
    cout << _energyMap;
}

string getMiniBattleMap(int battlefield[][10], int rows)
{
    string battlemap = "            00    01    02    03    04    05    06    07    08    09    \n";
    for (int i = 0; i < rows; i++)
    {
        if (i < 10) battlemap += "[==]  0" + to_string(i) + "  ";
        else battlemap += "[==]  " + to_string(i) + "  ";

        for (int j = 0; j < 10; j++)
        {
            battlemap += " ";
            int code = battlefield[i][j];
            switch(code)
            {
                case 1: battlemap += "|__|"; break;
                case 2: battlemap += "////"; break;
                case 3: battlemap += "[[]]"; break;
                case 4: battlemap += "---)"; break;
                case 5: battlemap += "D-->"; break;
                case 6: battlemap += "-}--"; break;
                case 7: battlemap += "~__~"; break;
                case 8: battlemap += ">__<"; break;
                case 9: battlemap += "Q__Q"; break;
                case 10: battlemap += "T__T"; break;
                default: battlemap += "    ";
            }
            battlemap += " ";
        }
        battlemap += "\n\n";
    }
    return battlemap;
}

// STEP LEVEL FUNCTIONS

void buyAsset()
{
    int assetCode;
    cout << "Which asset would you like to buy? \n";
    cout << "(1) Wall\t\t|__|\t\tCost: " << _assetCosts[1] << " Gold\t\tHealth: " << _assetHealth[1] << "\t\tDamage: " << _assetDamage[1] << endl;
    cout << "(2) Pike Wall\t\t////\t\tCost: " << _assetCosts[2] << " Gold\t\tHealth: " << _assetHealth[2] << "\t\tDamage: " << _assetDamage[2] << endl;
    cout << "(3) Mine\t\t[[]]\t\tCost: " << _assetCosts[3] << " Gold\t\tHealth: " << _assetHealth[3] << "\t\tDamage: " << _assetDamage[3] << endl;
    cout << "(4) Miner\t\t---)\t\tCost: " << _assetCosts[4] << " Gold\t\tHealth: " << _assetHealth[4] << "\t\tDamage: " << _assetDamage[4] << endl;
    cout << "(5) Archer\t\tD-->\t\tCost: " << _assetCosts[5] << " Gold\t\tHealth: " << _assetHealth[5] << "\t\tDamage: " << _assetDamage[5] << endl;
    cout << "(6) Swordsman\t\t-}--\t\tCost: " << _assetCosts[6] << " Gold\t\tHealth: " << _assetHealth[6] << "\t\tDamage: " << _assetDamage[6] << endl;
    cout << "(7) Wizard\t\t~__~\t\tCost: " << _assetCosts[7] << " Gold\t\tHealth: " << _assetHealth[7] << "\t\tDamage: " << _assetDamage[7] << "\t\tEnergy Recharge: " << _wizardRecharge << endl;
    cout << "(8) None, I've changed my mind.\n";
    cin >> assetCode;

    while (cin.fail() || assetCode <= 0 || assetCode > 8)
    {
        cout << "Invalid Input!" << endl << "Which asset would you like to buy? ";
        cin.clear();
        newLine();
        cin >> assetCode;
    }
    newLine();

    if (assetCode == 8)
        return;

    if (_gold < _assetCosts[assetCode])
    {
        cout << "Uh oh. Looks like you don't have enough gold to buy that asset. You currently have " << _gold << " gold." << endl << endl;
        buyAsset();
        return;
    }

    int row, col;
    cout << "\nWhere would you like to place this asset? Enter your coordinates in the format of <row> <column>." << endl;
    cout << "Enter -1 -1 if you would like to cancel this action. ";
    cin >> row >> col;

    if (row == -1 && col == -1)
        return;

    while (cin.fail() || row < 0 || row >= _rows || col < 0 || col > 9 || isEnemy(row, col))
    {
        if (isEnemy(row, col)) 
            cout << "You cannot place an asset on a coordinate that has an enemy." << endl;
        else 
            cout << "Invalid Input!" << endl;
        
        cout << "Where would you like to place this asset? Enter the coordinates in the format of <row> <column>. (Enter -1 -1 to cancel.) ";
        cin.clear();
        newLine();
        cin >> row >> col;

        if (row == -1 && col == -1)
            return;
    }
    newLine();

    if (_battlefield[row][col] != 0)
    {
        cout << "You have chosen a coordinate that already contains an asset: " << row << " " << col << endl;
        bool replaceAsset = getYN("Would you like to remove the previous asset and replace it with this one? \nWarning: You will not receive gold for the removal of an asset.");
        if (!replaceAsset) return;
    }

    _battlefield[row][col] = assetCode;
    _health[row][col] = _assetHealth[assetCode];
    if (assetCode == 7)
        _energy[row][col] = _wizardRecharge;
    _gold -= _assetCosts[assetCode];

    cout << endl;
}

void removeAsset()
{
    int row, col;
    cout << "Warning: You will not receive gold for the removal of an asset.\n";
    cout << "Where is the asset you want to remove? Enter your coordinates in the format of <row> <column>. " << endl;
    cout << "Enter -1 -1 if you would like to cancel this action. ";
    cin >> row >> col;

    if (row == -1 && col == -1) return;

    while (cin.fail() || row < 0 || row >= _rows || col < 0 || col > 9 || isEnemy(row, col))
    {
        cout << "Invalid Input!" << endl << "Where is the asset you want to remove? Enter the coordinates in the format of '<row> <column>'. (Enter -1 -1 to cancel.) ";
        cin.clear();
        newLine();
        cin >> row >> col;

        if (row == -1 && col == -1)
            return;
    }
    newLine();

    _battlefield[row][col] = 0;
    _health[row][col] = 0;
    _energy[row][col] = 0;
}

void attack()
{
    for (int i = 0; i < _rows; i++)
    {
        for (int j = 0; j < 11; j++)
        {
            if (isEnemy(i, j))
            {
                int enemy = _battlefield[i][j];
                if (j == 0)
                    _currentCastleHealth = max(_currentCastleHealth - _assetDamage[enemy], 0);
                else
                {
                    bool posUpdated = updatePosition(i, j, i, j - 1);
                    if (!posUpdated && !isEnemy(i, j - 1))
                        decreaseHealth(i, j-1, _assetDamage[enemy]);
                }
            }
        }
    }
}

void mineAndDefend()
{
    for (int i = 0; i < _rows; i++)
    {
        for (int j = 0; j < 11; j++)
        {
            switch(_battlefield[i][j])
            {
                case 2: // pike wall
                    if (isEnemy(i, j+1))
                        decreaseHealth(i, j+1, _assetDamage[2]);
                    break;
                case 3: // mine
                    if (isEnemy(i, j+1))
                    {
                        decreaseHealth(i, j+1, _assetDamage[3]);
                        decreaseHealth(i, j, _assetHealth[3]);
                    }
                    break;
                case 4: // miner
                    _gold += _minerEffectiveness;
                    break;
                case 5: // archer attacks enemy closest to castle in same row
                    for (int col = 0; col < 11; col++)
                    {
                        if (isEnemy(i, col))
                        {
                            decreaseHealth(i, col, _assetDamage[5]);
                            break;
                        }
                    }
                    break;
                case 6: // swordsman attacks anyone behind or in front of them on the same row, with the one closest to castle getting priority
                    {
                        int startingIndex = j - 1;
                        int finalIndex = j + 1;
                        if (j == 0) startingIndex = 0;
                        if (j == 10) finalIndex = 11;
                        for (int col = startingIndex; col <= finalIndex; col++)
                        {
                            if (isEnemy(i, col))
                            {
                                decreaseHealth(i, col, _assetDamage[6]);
                                break;
                            }
                        }
                        break;
                    }
                case 7: // wizard attacks all enemies in the same row if energy is fully recharged
                    if (_energy[i][j] != 0 && _energy[i][j] % _wizardRecharge == 0)
                    {
                        bool enemiesOnSameRow = false;
                        for (int col = 0; col < 11; col++)
                        {
                            if (isEnemy(i, col))
                            {
                                decreaseHealth(i, col, _assetDamage[7]);
                                enemiesOnSameRow = true;
                            }
                        }
                        if (enemiesOnSameRow) _energy[i][j] = -1;
                    }
                    break;
                default: break;
            }
        }
    }
}

void generateEnemies()
{
    // if we have already spawned all the enemies
    if (_enemiesLeftToSpawn <= 0) return;

    // count open spots available to spawn new enemies
    int openSpots = 0;
    for (int i = 0; i < _rows; i++)
    {
        if (_battlefield[i][10] == 0)
            openSpots++;
    }

    // if the enemies left to spawn is less than the new enemies we should spawn per step OR if the open spots are less than what we should spawn per step 
    // then just spawn in all open spots in last column
    if (_enemiesLeftToSpawn < _newEnemiesPerStep || openSpots <= _newEnemiesPerStep)
    {
        for (int i = 0; i < _rows; i++)
        {
            if (_battlefield[i][10] == 0)
            {
                spawnEnemy(i, 10);
                _enemiesLeftToSpawn--;
            }
        }
    }
    else
    {
        int enemiesSpawned = 0;
        while (enemiesSpawned < _newEnemiesPerStep)
        {
            int randomRow = rand() % _rows;
            if (_battlefield[randomRow][10] != 0)
                continue;
            spawnEnemy(randomRow, 10);
            _enemiesLeftToSpawn--;
            enemiesSpawned++;
        }
    }
}

void generateGold()
{
    _gold += _goldGeneration;
}

void updateScore()
{
    _score = _currentWave * 50;
    for (int i = 0; i < _rows; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            _score += _assetCosts[_battlefield[i][j]] + _health[i][j];
        }
    }
}

void updateGameStates(int& currentStep)
{
    // check for castle health / game over
    if (_currentCastleHealth <= 0) 
    {
        gameOver();
        return;
    }

    // update wizard energy levels
    for (int i = 0; i < _rows; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (_battlefield[i][j] == 7)
                _energy[i][j]++;
        }
    }

    // update _newEnemiesPerStep
    if (currentStep % _newEnemiesPerStepIncrementSteps == 0)
        _newEnemiesPerStep += _newEnemiesPerStepIncrement;

    currentStep++;
}

// BASE FUNCTIONS

string getFilePreset()
{
    bool preset = getYN("Do you want to load a game file? This can be a previous saved game or a customized preset.");
    string fileName;

    if (preset)
    {
        cout << "What is the name of your game preset file? ";
        cin >> fileName;

        while (cin.fail())
        {
            cout << "Sorry, couldn't understand you." << endl << "What is the name of your game preset file? ";
            cin.clear();
            newLine();
            cin >> fileName;
        }
        newLine();
    }
    else
    {
        int level;
        cout << "Select a Level: " << endl;
        cout << "(0) Test : For testing functionality." << endl;
        cout << "(1) Beginner : Just getting started." << endl;
        cout << "(2) Intermediate : Getting the hang of things..." << endl;
        cout << "(3) Advanced : Wow, this is challenging." << endl;
        cout << "(4) Expert : I'm going insane." << endl;
        cout << "(5) Endless : Endless waves and endless stress." << endl;
        cin >> level;

        while (cin.fail() || level > 5 || level < 0)
        {
            cout << "Invalid input." << endl << "Select a Level: ";
            cin.clear();
            newLine();
            cin >> level;
        }
        newLine();

        switch(level)
        {
            case 0: fileName = "0_Test.txt"; break;
            case 1: fileName = "1_Beginner.txt"; break;
            case 2: fileName = "2_Intermediate.txt"; break;
            case 3: fileName = "3_Advanced.txt"; break;
            case 4: fileName = "4_Expert.txt"; break;
            case 5: fileName = "5_Endless.txt"; break;
            default: fileName = ""; break;
        }
    }

    return fileName;
}

string getSaveFileName()
{
    string fileName;
    cout << "What would you like the name of your saved game file to be? Please make sure this file name does not already exist! ";
    cin >> fileName;

    while (cin.fail())
    {
        cout << "Sorry, couldn't understand you." << endl << "What would you like the name of your saved game file to be? Please make sure this file name does not already exist! ";
        cin.clear();
        newLine();
        cin >> fileName;
    }
    newLine();
    return fileName;
}

int getTotalObjectsOnBattlefield()
{
    int total = 0;
    for (int i = 0; i < _rows; i++)
    {
        for (int j = 0; j < 11; j++)
        {
            if (_battlefield[i][j] > 0)
                total++;
        }
    }
    return total;
}

bool updatePosition(int row1, int col1, int row2, int col2)
{
    if (_battlefield[row2][col2] == 0)
    {
        int code = _battlefield[row1][col1];
        _battlefield[row2][col2] = code;
        _battlefield[row1][col1] = 0;
        int health = _health[row1][col1];
        _health[row2][col2] = health;
        _health[row1][col1] = 0;
        return true;
    }
    return false;
}

void decreaseHealth(int row, int col, int damage)
{
    _health[row][col] -= damage;
    if (_health[row][col] <= 0)
    {
        int code = _battlefield[row][col];
        if (isEnemy(row, col)) 
        {
            _enemiesKilledInWave++;
            _gold += _assetCosts[code];
        }
        _battlefield[row][col] = 0;
        _health[row][col] = 0;
        _energy[row][col] = 0;
    }
}

void spawnEnemy(int row, int col)
{
    int randomEnemy = rand() % 100;

    if (randomEnemy < _enemySoldierProportion)
        randomEnemy = 8;
    else if (randomEnemy < _enemySoldierProportion + _enemyCaptainProportion)
        randomEnemy = 9;
    else
        randomEnemy = 10;

    _battlefield[row][col] = randomEnemy;
    _health[row][col] = _assetHealth[randomEnemy];
}

bool isEnemy(int row, int col)
{
    return _battlefield[row][col] > 7;
}

bool getYN(string question)
{
    char letter;
    cout << question << " (Y/N) ";
    cin >> letter;

    while (letter != 'y' && letter != 'Y' && letter != 'n' && letter != 'N')
    {
        cout << "Sorry, couldn't understand you." << endl << question << " (Y/N) ";
        cin.clear();
        newLine();
        cin >> letter;
    }
    newLine();
    cout << endl;

    if (letter == 'Y' || letter == 'y') return true;
    return false;
}

void printDivider(int length)
{
    for (int i = 0; i < length; i++)
    {
        cout << "-";
    }
    cout << endl;
}

void newLine()
{
    char c;
    do
    {
        cin.get(c);
    } while (c != '\n');
}