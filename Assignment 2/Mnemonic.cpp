// Vaibhav Dhale
// Mnemonic / Opcode Validation

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>

using namespace std;

// Convert string to uppercase
string toUpperCase(string str)
{
    for (char &c : str)
        c = toupper(static_cast<unsigned char>(c));

    return str;
}

// Load opcode table from opcode.txt
bool loadOpcodeTable(const string &fileName,
                     vector<string> &opcodeTable)
{
    ifstream file(fileName);

    if (!file)
    {
        cout << "Error: Cannot open opcode file." << endl;
        return false;
    }

    string opcode;

    while (file >> opcode)
    {
        opcodeTable.push_back(toUpperCase(opcode));
    }

    file.close();

    return true;
}

// Validate mnemonic against opcode table
bool validateMnemonic(const string &mnemonic,
                      const vector<string> &opcodeTable)
{
    string input = toUpperCase(mnemonic);

    for (const string &opcode : opcodeTable)
    {
        if (input == opcode)
            return true;
    }

    return false;
}
