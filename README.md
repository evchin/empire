# EMPIRE

A C++ game where the player must defend the empire.

[**PLAY HERE**](https://repl.it/@EChin/empire#empire.cpp)

**How to Download and Play Locally**
1. Ensure you have a [C++ Compiler](https://isocpp.org/get-started) installed.
2. Download entire repository off GitHub and unzip the folder.
4. Build and run the empire.cpp file.

[**Game Demo**](https://www.youtube.com/watch?v=7oL-xQ-_spA)

[**Empire Preset File Template**](https://docs.google.com/spreadsheets/d/16vBtD3OjiTA5wkmSE2JrCejY7lrvCF9SScn6XFPewYc/edit?usp=sharing) - Create your own level. It is recommended that you fill in the excel sheet's numbers in a separate column, and then copy and paste them into a new notepad file. Otherwise, it becomes difficult to keep track of certain variables/game states. See provided preset files for examples on usage if confused.

**A note on 0_Test.txt**: Useful for simply testing functionality. All units have the same health (5) and damage (2) to make testing more efficient, except for the mine, which can only be used once and has 5 damage (so it should kill an enemy unit immediately). There are only two waves. The first wave requires 2 enemies to be killed. The second requires 3 (demonstrates the usage of increments over multiple waves). The minigame will be conducted after completion of the first wave.

**Score**: Score is calculated by multiplying 50 by the currentWave integer, and then adding it to the sum of all assets' costs and health. `score = 50 * current wave + all_assets(cost + health)`

