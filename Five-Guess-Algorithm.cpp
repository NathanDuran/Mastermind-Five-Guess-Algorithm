#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <ctime>

using namespace std;

vector<int> getRandomCode();

void createSet();

void combinationRecursive(int combinationLength, int position, vector<int> &current, vector<int> &elements);

string checkCode(vector<int> guess, vector<int> code);

void removeCode(vector<vector<int>> &set, vector<int> code);

void pruneCodes(vector<vector<int>> &set, vector<int> code, string currentResponse);

vector<vector<int>> minmax();

int getMaxScore(map<string, int> inputMap);

int getMinScore(map<vector<int>, int> inputMap);

vector<int> getNextGuess(vector<vector<int>> nextGuesses);

static const int NUM_COLOURS = 6;
static const int CODE_LENGTH = 4;
static vector<vector<int>> combinations; //Master set of combinations 1111 to 6666
static vector<vector<int>> candidateSolutions;
static vector<vector<int>> nextGuesses;
static vector<int> code;
static vector<int> currentGuess;
static string responsePegs;
static bool won;
static int turn;

int main() {

    turn = 1;
    won = false;

    code = getRandomCode();
    currentGuess = {1, 1, 2, 2}; //1122

    //Create the set of 1296 possible codes
    createSet();
    candidateSolutions.insert(candidateSolutions.end(), combinations.begin(), combinations.end());

    cout << "Code: ";
    for (int i = 0; i < code.size(); ++i) {
        cout << code[i] << " ";
    }
    cout << endl;

    while (!won) {

        //Remove currentGuess from possible solutions
        removeCode(combinations, currentGuess);
        removeCode(candidateSolutions, currentGuess);

        //Play the guess to get a response of colored and white pegs
        responsePegs = checkCode(currentGuess, code);

        cout << "Turn: " << turn << endl;
        cout << "Guess: ";
        for (int i = 0; i < currentGuess.size(); ++i) {
            cout << currentGuess[i] << " ";
        }
        cout << "= " << responsePegs << endl;

        //If the response is four colored pegs, the game is won
        if (responsePegs == "BBBB") {
            won = true;
            cout << "Game Won!" << endl;
            break;
        }

        //Remove from candidateSolutions,
        //any code that would not give the same response if it were the code
        pruneCodes(candidateSolutions, currentGuess, responsePegs);

        //Calculate Minmax scores
        nextGuesses = minmax();

        //Select next guess
        currentGuess = getNextGuess(nextGuesses);

        turn++;
    }//End while

    return 0;
}

vector<int> getRandomCode() {

    vector<int> code;
    int max = NUM_COLOURS;
    int min = 1;
    int random;
    //Returns the number of seconds since the UNIX epoch for a seed
    srand(time(0));

    for (int i = 0; i < CODE_LENGTH; ++i) {

        random = min + (rand() % (max - min + 1));
        code.push_back(random);
    }

    return code;
}

void createSet() {

    vector<int> current(CODE_LENGTH, 0);
    vector<int> elements;

    for (int i = 1; i <= NUM_COLOURS; ++i) {
        elements.push_back(i);
    }

    combinationRecursive(CODE_LENGTH, 0, current, elements);
}

void combinationRecursive(int combinationLength, int position, vector<int> &current, vector<int> &elements) {

    if (position >= combinationLength) {
        combinations.push_back(current);
        return;
    }

    for (int j = 0; j < elements.size(); ++j) {
        current[position] = elements[j];
        combinationRecursive(combinationLength, position + 1, current, elements);
    }
    return;
}

string checkCode(vector<int> guess, vector<int> code) {

    string result;

    //Get black/coloured
    for (int i = 0; i < CODE_LENGTH; ++i) {

        if (guess[i] == code[i]) {
            result.append("B");
            guess[i] *= -1;
            code[i] *= -1;
        }
    }

    //Get white
    for (int i = 0; i < CODE_LENGTH; ++i) {

        if (code[i] > 0) {

            vector<int>::iterator it = find(guess.begin(), guess.end(), code[i]);
            int index;
            if (it != guess.end()) {

                index = distance(guess.begin(), it);
                result.append("W");
                guess[index] *= -1;
            }
        }
    }

    return result;
}

void removeCode(vector<vector<int>> &set, vector<int> currentCode) {

    int index;
    for (auto it = set.begin(); it != set.end(); it++) {
        index = distance(set.begin(), it);

        if (set[index] == currentCode) {
            set.erase(set.begin() + index);
            break;
        }
    }
}

void pruneCodes(vector<vector<int>> &set, vector<int> currentCode, string currentResponse) {

    int index;
    vector<vector<int>>::iterator it = set.begin();

    while (it != set.end()) {
        index = distance(set.begin(), it);

        if (currentResponse != checkCode(currentCode, set[index])) {
            it = set.erase(set.begin() + index);
        } else {
            it++;
        }
    }
}

vector<vector<int>> minmax() {

    map<string, int> scoreCount;
    map<vector<int>, int> score;
    vector<vector<int>> nextGuesses;
    int max, min;

    for (int i = 0; i < combinations.size(); ++i) {

        for (int j = 0; j < candidateSolutions.size(); ++j) {

            string pegScore = checkCode(combinations[i], candidateSolutions[j]);
            if (scoreCount.count(pegScore) > 0) {
                scoreCount.at(pegScore)++;
            } else {
                scoreCount.emplace(pegScore, 1);
            }
        }

        max = getMaxScore(scoreCount);
        score.emplace(combinations[i], max);
        scoreCount.clear();
    }

    min = getMinScore(score);

    for (auto elem : score) {
        if (elem.second == min) {
            nextGuesses.push_back(elem.first);
        }
    }

    return nextGuesses;
}

int getMaxScore(map<string, int> inputMap) {

    int max = 0;
    for (auto elem : inputMap) {
        if (elem.second > max) {
            max = elem.second;
        }
    }

    return max;
}

int getMinScore(map<vector<int>, int> inputMap) {

    int min = numeric_limits<int>::max();
    for (auto elem : inputMap) {
        if (elem.second < min) {
            min = elem.second;
        }
    }

    return min;
}

vector<int> getNextGuess(vector<vector<int>> nextGuesses) {

    vector<int> nextGuess;

    for (int i = 0; i < nextGuesses.size(); ++i) {
        if (find(candidateSolutions.begin(), candidateSolutions.end(), nextGuesses[i]) != candidateSolutions.end()) {
            return nextGuesses[i];
        }
    }
    for (int j = 0; j < nextGuesses.size(); ++j) {
        if (find(combinations.begin(), combinations.end(), nextGuesses[j]) != combinations.end()) {
            return nextGuesses[j];
        }
    }

    return nextGuess;
}