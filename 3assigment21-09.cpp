#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>

using namespace std;

string convertToHex(int number, int bytes) {
    stringstream ss;
    ss << uppercase << hex << setfill('0') << setw(bytes * 2) << number;
    return ss.str();
}

string convertToLittleEndian(int number, int bytes) {
    string result = "";
    for (int i = 0; i < bytes; i++) {
        int currentByte = (number >> (i * 8)) & 0xFF;
        if (i > 0) result += " ";
        result += convertToHex(currentByte, 1);
    }
    return result;
}

int main() {
    ifstream asmFile("program.asm");
    if (!asmFile.is_open()) {
        cout << "Error: Could not open program.asm file!" << endl;
        return 1;
    }

    string line;
    while (getline(asmFile, line)) {
        line = line.substr(0, line.find(';'));

        stringstream ss(line);
        string label, directive, value;
        ss >> label >> directive;
        getline(ss, value);

        size_t startPos = value.find_first_not_of(" \t");
        if (startPos == string::npos) continue;
        value = value.substr(startPos);

        if (directive == "db") {
            if (value[0] == '"') {
                size_t endQuote = value.find('"', 1);
                if (endQuote != string::npos) {
                    string str = value.substr(1, endQuote - 1);
                    cout << "String : " << str << endl << "ASCII  : ";
                    for (size_t i = 0; i < str.length(); i++) {
                        cout << (int)(unsigned char)str[i] << (i < str.length() - 1 ? " " : "");
                    }
                    cout << endl << "Hex    : ";
                    for (size_t i = 0; i < str.length(); i++) {
                        cout << convertToHex((unsigned char)str[i], 1) << (i < str.length() - 1 ? " " : "");
                    }
                    cout << endl << endl;
                }
            } else {
                int num = stoi(value);
                cout << "Decimal : " << num << endl;
                cout << "Hexadecimal : " << convertToHex(num, 1) << endl << endl;
            }
        }
        else if (directive == "dw" || directive == "dd") {
            int bytes = (directive == "dw") ? 2 : 4;
            int num = stoi(value);
            cout << "Decimal : " << num << endl;
            cout << "Hexadecimal : " << convertToHex(num, bytes) << endl;
            cout << "Little Endian Format : " << convertToLittleEndian(num, bytes) << endl << endl;
        }
    }

    asmFile.close();
    return 0;
}
