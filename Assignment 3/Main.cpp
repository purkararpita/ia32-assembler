// Arpita Purkar
// Main Program, Menu & Output/Report

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Display validation result
void displayResult(const string &mnemonic,
                   bool mnemonicValid,
                   const string &operand1,
                   bool operand1Valid,
                   const string &operand2,
                   bool operand2Valid)
{
    cout << "\n===================================="
         << endl;

    cout << "Instruction: " << mnemonic << endl;

    cout << "Mnemonic: "
         << (mnemonicValid ? "VALID" : "INVALID")
         << endl;

    if (!operand1.empty())
    {
        cout << "Operand 1: " << operand1 << endl;
        cout << "Status: "
             << (operand1Valid ? "VALID" : "INVALID")
             << endl;
    }

    if (!operand2.empty())
    {
        cout << "Operand 2: " << operand2 << endl;
        cout << "Status: "
             << (operand2Valid ? "VALID" : "INVALID")
             << endl;
    }

    cout << "===================================="
         << endl;
}

// Write result to output file
void writeOutput(const string &instruction,
                 const string &result)
{
    ofstream file("output.txt", ios::app);

    if (!file)
    {
        cout << "Error: Cannot create output file."
             << endl;
        return;
    }

    file << "Instruction: " << instruction << endl;
    file << "Result: " << result << endl;
    file << "------------------------------------"
         << endl;

    file.close();
}

// Main program and menu
int main()
{
    int choice;

    while (true)
    {
        cout << "\n===================================="
             << endl;
        cout << "       IA-32 ASSEMBLER VALIDATOR"
             << endl;
        cout << "===================================="
             << endl;

        cout << "1. Validate Assembly File" << endl;
        cout << "2. Search Instruction" << endl;
        cout << "3. Exit" << endl;

        cout << "\nEnter choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1:
                cout << "\nAssembly file validation selected."
                     << endl;
                break;

            case 2:
                cout << "\nInstruction search selected."
                     << endl;
                break;

            case 3:
                cout << "\nProgram terminated."
                     << endl;
                return 0;

            default:
                cout << "\nInvalid choice."
                     << endl;
        }
    }

    return 0;
}
