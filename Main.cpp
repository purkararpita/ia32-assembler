#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

#include "Instr.h"

using namespace std;


string upperCase(string str)
{
    for (char& c : str)
    {
        c = toupper(static_cast<unsigned char>(c));
    }

    return str;
}



string cleanOperand(string operand)
{
    operand.erase(
        remove(operand.begin(), operand.end(), ','),
        operand.end()
    );

    return operand;
}



bool readOpcodeFile(const string& fileName,
                    vector<string>& opcodeTable)
{
    ifstream file(fileName);

    if (!file)
    {
        return false;
    }

    string mnemonic;

    while (file >> mnemonic)
    {
        opcodeTable.push_back(
            upperCase(mnemonic)
        );
    }

    file.close();

    return true;
}



void readSymbols(const string& fileName,
                 vector<string>& symbols)
{
    ifstream file(fileName);

    if (!file)
    {
        return;
    }

    string line;

    while (getline(file, line))
    {
        // Remove comments
        size_t comment =
            line.find(';');

        if (comment != string::npos)
        {
            line =
                line.substr(0, comment);
        }

        stringstream ss(line);

        string firstWord;

        if (!(ss >> firstWord))
        {
            continue;
        }

        // Check LABEL:
        if (!firstWord.empty() &&
            firstWord.back() == ':')
        {
            firstWord.pop_back();

            if (!firstWord.empty())
            {
                symbols.push_back(
                    upperCase(firstWord)
                );
            }
        }
    }

    file.close();
}



void displayOperand(const string& operand,
                    int number,
                    const vector<string>& symbols)
{
    string type =
        getOperandType(operand, symbols);

    cout << "\nOperand "
         << number
         << ": "
         << operand
         << endl;

    cout << "Operand Type: "
         << type
         << endl;


    if (type == "UNKNOWN")
    {
        cout << "Status: INVALID"
             << endl;

        cout << "Error Type: INVALID_OPERAND"
             << endl;
    }
    else
    {
        cout << "Status: VALID"
             << endl;
    }
}



void searchInstruction(
    const vector<string>& opcodeTable,
    const vector<string>& symbols)
{
    string input;

    cout << "\nEnter instruction "
         << "(Example: MOV EBX 5): ";

    getline(cin >> ws, input);


    stringstream ss(input);

    string mnemonic;
    string operand1;
    string operand2;

    ss >> mnemonic;
    ss >> operand1;
    ss >> operand2;


    if (mnemonic.empty())
    {
        cout << "\nError: Instruction cannot be empty."
             << endl;

        return;
    }


    operand1 = cleanOperand(operand1);
    operand2 = cleanOperand(operand2);


    bool mnemonicValid =
        validateMnemonic(
            mnemonic,
            opcodeTable
        );


    cout << "\n========== SEARCH RESULT =========="
         << endl;


    cout << "\nInstruction: "
         << upperCase(mnemonic)
         << endl;



    if (mnemonicValid)
    {
        cout << "Mnemonic: FOUND"
             << endl;
    }
    else
    {
        cout << "Mnemonic: NOT FOUND"
             << endl;

        cout << "Status: INVALID"
             << endl;

        cout << "Error Type: INVALID_MNEMONIC"
             << endl;
    }



    if (!operand1.empty())
    {
        displayOperand(
            operand1,
            1,
            symbols
        );
    }



    if (!operand2.empty())
    {
        displayOperand(
            operand2,
            2,
            symbols
        );
    }


    // Overall status

    bool operand1Valid =
        operand1.empty() ||
        validateOperand(
            operand1,
            symbols
        );


    bool operand2Valid =
        operand2.empty() ||
        validateOperand(
            operand2,
            symbols
        );


    cout << "\nOverall Status: ";

    if (mnemonicValid &&
        operand1Valid &&
        operand2Valid)
    {
        cout << "VALID INSTRUCTION"
             << endl;
    }
    else
    {
        cout << "INVALID INSTRUCTION"
             << endl;
    }


    cout << "\n---------------------------------------------"
         << endl;
}


// Validate complete assembly file

void validateAssemblyFile(
    const string& fileName,
    const vector<string>& opcodeTable,
    const vector<string>& symbols)
{
    ifstream file(fileName);

    if (!file)
    {
        cout << "\nError: Cannot open assembly file."
             << endl;

        return;
    }


    ofstream output("output.txt");

    string line;

    output << "=============================================\n";
    output << "             OPCODE CHECK\n";
    output << "=============================================\n\n";


    while (getline(file, line))
    {
        // Remove comments
        size_t comment =
            line.find(';');

        if (comment != string::npos)
        {
            line =
                line.substr(0, comment);
        }


        stringstream ss(line);

        string mnemonic;
        string operand1;
        string operand2;

        ss >> mnemonic;

        if (mnemonic.empty())
        {
            continue;
        }


        ss >> operand1;
        ss >> operand2;


        operand1 = cleanOperand(operand1);
        operand2 = cleanOperand(operand2);


        bool mnemonicValid =
            validateMnemonic(
                mnemonic,
                opcodeTable
            );


        output << "Instruction: "
               << upperCase(mnemonic)
               << "\n";


        if (mnemonicValid)
        {
            output << "Mnemonic: FOUND\n";
        }
        else
        {
            output << "Mnemonic: NOT FOUND\n";
            output << "Status: INVALID\n";
            output << "Error Type: INVALID_MNEMONIC\n";
        }


        // Operand 1

        if (!operand1.empty())
        {
            string type =
                getOperandType(
                    operand1,
                    symbols
                );

            output << "Operand: "
                   << operand1
                   << "\n";

            output << "Operand Type: "
                   << type
                   << "\n";


            if (type == "UNKNOWN")
            {
                output << "Status: INVALID\n";
                output << "Error Type: INVALID_OPERAND\n";
            }
            else
            {
                output << "Status: VALID\n";
            }
        }


        // Operand 2

        if (!operand2.empty())
        {
            string type =
                getOperandType(
                    operand2,
                    symbols
                );

            output << "Operand: "
                   << operand2
                   << "\n";

            output << "Operand Type: "
                   << type
                   << "\n";


            if (type == "UNKNOWN")
            {
                output << "Status: INVALID\n";
                output << "Error Type: INVALID_OPERAND\n";
            }
            else
            {
                output << "Status: VALID\n";
            }
        }


        output << "\n---------------------------------------------\n\n";
    }


    file.close();
    output.close();

    cout << "\nValidation completed."
         << endl;

    cout << "Result saved in output.txt"
         << endl;
}


// Main

int main()
{
    vector<string> opcodeTable;
    vector<string> symbols;


    string opcodeFile;
    string assemblyFile;


    cout << "Enter Opcode File Name: ";
    cin >> opcodeFile;


    cout << "Enter Assembly Program File Name: ";
    cin >> assemblyFile;


    // Read opcode file

    if (!readOpcodeFile(
            opcodeFile,
            opcodeTable))
    {
        cout << "\nError: Cannot open opcode file."
             << endl;

        return 1;
    }


    // Read symbols

    readSymbols(
        assemblyFile,
        symbols
    );


    int choice;


    while (true)
    {
        cout << "\n=="
             << endl;

        cout << "       IA-32 ASSEMBLER VALIDATION"
             << endl;

        cout << "=="
             << endl;

        cout << "\n1. Validate Assembly File"
             << endl;

        cout << "2. Search Instruction"
             << endl;

        cout << "3. Exit"
             << endl;


        cout << "\nEnter Choice: ";
        cin >> choice;


        if (choice == 1)
        {
            cout << "\nValidating assembly file..."
                 << endl;

            validateAssemblyFile(
                assemblyFile,
                opcodeTable,
                symbols
            );
        }


        else if (choice == 2)
        {
            searchInstruction(
                opcodeTable,
                symbols
            );
        }


        else if (choice == 3)
        {
            cout << "\nProgram terminated."
                 << endl;

            break;
        }


        else
        {
            cout << "\nInvalid choice."
                 << endl;
        }
    }


    return 0;
}
