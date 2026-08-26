// Ajay Pathade
// Assembly File Processing & Line Parsing

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// Read assembly file
bool readAssemblyFile(const string &fileName,
                      vector<string> &lines)
{
    ifstream file(fileName);

    if (!file)
    {
        cout << "Error: Cannot open assembly file."
             << endl;
        return false;
    }

    string line;

    while (getline(file, line))
    {
      
        size_t comment = line.find(';');

        if (comment != string::npos)
            line = line.substr(0, comment);

        if (!line.empty())
            lines.push_back(line);
    }

    file.close();

    return true;
}

// Parse instruction line
void parseInstruction(const string &line,
                      string &mnemonic,
                      string &operand1,
                      string &operand2)
{
    stringstream ss(line);

    mnemonic.clear();
    operand1.clear();
    operand2.clear();

    ss >> mnemonic;
    ss >> operand1;
    ss >> operand2;

    // Remove commas
    if (!operand1.empty() && operand1.back() == ',')
        operand1.pop_back();

    if (!operand2.empty() && operand2.back() == ',')
        operand2.pop_back();
}

// Parse operands
void parseOperands(const string &line,
                   vector<string> &operands)
{
    stringstream ss(line);

    string mnemonic;
    string operand;

    ss >> mnemonic;

    while (ss >> operand)
    {
        if (!operand.empty() && operand.back() == ',')
            operand.pop_back();

        operands.push_back(operand);
    }
}
