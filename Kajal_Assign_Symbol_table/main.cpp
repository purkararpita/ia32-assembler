#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>

using namespace std;

// == SYMBOL STRUCTURE ==

struct Symbol
{
    int id;
    string name;
    int size;
    string value;
    string section;
    int address;
    string status;
    string reference;
};

// == PRINT ROW ==

void printRow(int id, string name, int size, string value,
              string section, int address, string status,
              string reference)
{
    cout << left
         << setw(5) << id
         << setw(12) << name
         << setw(8) << size
         << setw(12) << value
         << setw(12) << section;

    // Address
    cout << right
         << setfill('0')
         << setw(4)
         << uppercase
         << hex
         << address;

    // Status and Reference
    cout << setfill(' ')
         << "      "
         << left
         << setw(12) << status
         << setw(20) << reference
         << dec
         << endl;
}

// == MAIN ==

int main()
{
    ifstream file("program.asm");

    if (!file)
    {
        cout << "Error: program.asm not found!" << endl;
        return 1;
    }

    string line;

    vector<string> programLines;

    // Read complete program
    while (getline(file, line))
    {
        programLines.push_back(line);
    }

    file.close();

    vector<Symbol> symbols;

    // Stores label and its line number
    map<string, int> labelLine;

    int symbolID = 1;

    int dataAddress = 0;

    // =====
    // PASS 1
    // Find labels
    // =====

    for (int i = 0; i < programLines.size(); i++)
    {
        string line = programLines[i];

        // Remove comments
        size_t comment = line.find(';');

        if (comment != string::npos)
        {
            line = line.substr(0, comment);
        }

        // Remove leading spaces
        size_t first = line.find_first_not_of(" \t");

        if (first == string::npos)
        {
            continue;
        }

        line = line.substr(first);

        // Check for label
        size_t colon = line.find(':');

        if (colon != string::npos)
        {
            string label = line.substr(0, colon);

            // Remove trailing spaces
            size_t last = label.find_last_not_of(" \t");

            if (last != string::npos)
            {
                label = label.substr(0, last + 1);
            }

            labelLine[label] = i;
        }
    }

    // =====
    // PASS 1
    // Create DATA Symbol Table
    // ======

    for (int i = 0; i < programLines.size(); i++)
    {
        string line = programLines[i];

        // Remove comments
        size_t comment = line.find(';');

        if (comment != string::npos)
        {
            line = line.substr(0, comment);
        }

        if (line.empty())
        {
            continue;
        }

        stringstream ss(line);

        string name;
        string instruction;
        string value;

        ss >> name >> instruction;

        if (name.empty() || instruction.empty())
        {
            continue;
        }

        getline(ss, value);

        // == DB ==

        if (instruction == "db")
        {
            value.erase(0, value.find_first_not_of(" \t"));

            int size = 1;
            string displayValue;

            // DB string
            if (!value.empty() && value[0] == '"')
            {
                size_t end = value.find('"', 1);

                if (end != string::npos)
                {
                    displayValue = value.substr(1, end - 1);
                    size = displayValue.length();
                }
            }
            else
            {
                // DB number
                displayValue = value;
                size = 1;
            }

            symbols.push_back({
                symbolID,
                name,
                size,
                displayValue,
                ".data",
                dataAddress,
                "Defined",
                "-"
            });

            dataAddress += size;
            symbolID++;
        }

        //== DW ==

        else if (instruction == "dw")
        {
            value.erase(0, value.find_first_not_of(" \t"));

            symbols.push_back({
                symbolID,
                name,
                2,
                value,
                ".data",
                dataAddress,
                "Defined",
                "-"
            });

            dataAddress += 2;
            symbolID++;
        }

        // == DD ==

        else if (instruction == "dd")
        {
            value.erase(0, value.find_first_not_of(" \t"));

            symbols.push_back({
                symbolID,
                name,
                4,
                value,
                ".data",
                dataAddress,
                "Defined",
                "-"
            });

            dataAddress += 4;
            symbolID++;
        }
    }

    // ==
    // ADD TEXT LABELS TO SYMBOL TABLE
    // ==

    for (auto const &item : labelLine)
    {
        string label = item.first;
        int lineNumber = item.second;

        string reference = "-";

        // Find references to this label
        for (int i = 0; i < programLines.size(); i++)
        {
            string line = programLines[i];

            size_t comment = line.find(';');

            if (comment != string::npos)
            {
                line = line.substr(0, comment);
            }

            stringstream ss(line);

            string instruction;
            string operand;

            ss >> instruction >> operand;

            if (instruction == "jmp" && operand == label)
            {
                if (lineNumber > i)
                {
                    reference = "Forward";
                }
                else if (lineNumber < i)
                {
                    reference = "Backward";
                }
            }
        }

        // Add label to symbol table
        symbols.push_back({
            symbolID,
            label,
            0,
            "-",
            ".text",
            0,
            "Defined",
            reference
        });

        symbolID++;
    }

    // ==
    // SYMBOL TABLE OUTPUT
    // ==

    cout << "\n== SYMBOL TABLE ==\n";

    cout << left
         << setw(5) << "ID"
         << setw(12) << "Name"
         << setw(8) << "Size"
         << setw(12) << "Value"
         << setw(12) << "Section"
         << setw(10) << "Address"
         << setw(12) << "Status"
         << setw(20) << "Reference"
         << endl;

    cout << "----"
         << endl;

    // Print all symbols
    for (const auto &s : symbols)
    {
        printRow(
            s.id,
            s.name,
            s.size,
            s.value,
            s.section,
            s.address,
            s.status,
            s.reference
        );
    }

    cout << "--"
         << endl;

    return 0;
}
