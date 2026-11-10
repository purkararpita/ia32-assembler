#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

using namespace std;

struct Macro {
    string name;
    vector<string> parameters;
    vector<string> body;
    int mdtPointer;
};

vector<Macro> macros;
vector<string> intermediateCode;
vector<string> MDT;

string trim(string s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos)
        return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

vector<string> split(string s) {
    vector<string> tokens;
    stringstream ss(s);
    string word;
    while (ss >> word)
        tokens.push_back(word);
    return tokens;
}

string replaceAll(string text, string oldWord, string newWord) {
    if (oldWord.empty())
        return text;
    size_t pos = 0;
    while ((pos = text.find(oldWord, pos)) != string::npos) {
        text.replace(pos, oldWord.length(), newWord);
        pos += newWord.length();
    }
    return text;
}

void pass1() {
    ifstream input("input.asm");
    if (!input) {
        cout << "Error: Cannot open input.asm\n";
        return;
    }

    string line;
    bool insideMacro = false;
    Macro currentMacro;

    while (getline(input, line)) {
        line = trim(line);
        if (line.empty())
            continue;

        if (line == "MACRO") {
            insideMacro = true;
            currentMacro = Macro();
            continue;
        }

        if (insideMacro) {
            if (line == "MEND") {
                currentMacro.mdtPointer = MDT.size();
                for (auto &instruction : currentMacro.body)
                    MDT.push_back(instruction);
                MDT.push_back("MEND");
                macros.push_back(currentMacro);
                insideMacro = false;
                continue;
            }

            vector<string> tokens = split(line);
            if (currentMacro.name.empty()) {
                currentMacro.name = tokens[0];
                for (int i = 1; i < (int)tokens.size(); i++)
                    currentMacro.parameters.push_back(tokens[i]);
            } else {
                currentMacro.body.push_back(line);
            }
        } else {
            intermediateCode.push_back(line);
        }
    }
    input.close();
    cout << "\nPASS 1 COMPLETED SUCCESSFULLY!\n";
}

void pass2() {
    ofstream output("input.i");
    if (!output) {
        cout << "Error: Cannot create input.i\n";
        return;
    }

    for (string line : intermediateCode) {
        vector<string> tokens = split(line);
        if (tokens.empty())
            continue;

        bool found = false;
        for (auto &macro : macros) {
            if (tokens[0] == macro.name) {
                found = true;
                vector<string> actualParameters;
                for (int i = 1; i < (int)tokens.size(); i++)
                    actualParameters.push_back(tokens[i]);

                if (actualParameters.size() != macro.parameters.size()) {
                    cout << "Error: Incorrect parameters for " << macro.name << endl;
                    break;
                }

                for (string instruction : macro.body) {
                    for (int i = 0; i < (int)macro.parameters.size(); i++) {
                        instruction = replaceAll(instruction, macro.parameters[i], actualParameters[i]);
                    }
                    output << instruction << endl;
                }
                break;
            }
        }
        if (!found)
            output << line << endl;
    }
    output.close();
    cout << "\nPASS 2 COMPLETED SUCCESSFULLY!\n";
    cout << "Expanded code generated in input.i\n";
}

void displayMNT() {
    cout << "\n========== MACRO NAME TABLE (MNT) ==========\n";
    cout << "Macro Name\tMDT Pointer\n";
    for (auto &macro : macros) {
        cout << macro.name << "\t\t" << macro.mdtPointer << endl;
    }
}

void displayMDT() {
    cout << "\n========== MACRO DEFINITION TABLE (MDT) ==========\n";
    cout << "Index\tInstruction\n";
    for (int i = 0; i < (int)MDT.size(); i++) {
        cout << i << "\t" << MDT[i] << endl;
    }
}

int main() {
    cout << "====================================\n";
    cout << "        TWO PASS MACRO PROCESSOR\n";
    cout << "====================================\n";
    
    pass1();
    
    if (macros.empty()) {
        cout << "No macro definitions found!\n";
        return 0;
    }
    
    displayMNT();
    displayMDT();
    
    pass2();
    
    cout << "\n====================================\n";
    cout << "       MACRO PROCESSING FINISHED\n";
    cout << "====================================\n";
    return 0;
}