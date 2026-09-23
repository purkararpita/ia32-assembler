#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>

using namespace std;

string toHex(int value, int bytes)
{
    stringstream ss;

    ss << uppercase
       << hex
       << setfill('0')
       << setw(bytes * 2)
       << value;

    return ss.str();
}

string littleEndian(int value, int bytes)
{
    string result;

    for (int i = 0; i < bytes; i++)
    {
        int byteValue = (value >> (8 * i)) & 0xFF;

        stringstream ss;

        ss << uppercase
           << hex
           << setfill('0')
           << setw(2)
           << byteValue;

        if (i > 0)
            result += " ";

        result += ss.str();
    }

    return result;
}

string asciiToHex(char ch)
{
    stringstream ss;

    ss << uppercase
       << hex
       << setfill('0')
       << setw(2)
       << (int)(unsigned char)ch;

    return ss.str();
}

int main()
{
    ifstream file("program.asm");

    if (!file)
    {
        cout << "Error: program.asm not found!" << endl;
        return 1;
    }

    string line;

    while (getline(file, line))
    {
        line = line.substr(0, line.find(';'));

        stringstream ss(line);

        string label;
        string instruction;
        string values;

        ss >> label >> instruction;
        getline(ss, values);

        if (instruction == "db")
        {
            values.erase(0, values.find_first_not_of(" \t"));

            // String
            if (!values.empty() && values[0] == '"')
            {
                size_t end = values.find('"', 1);

                if (end != string::npos)
                {
                    string str = values.substr(1, end - 1);

                    cout << "String : " << str << endl;

                    cout << "ASCII  : ";

                    for (char ch : str)
                    {
                        cout << (int)(unsigned char)ch << " ";
                    }

                    cout << endl;

                    cout << "Hex    : ";

                    for (char ch : str)
                    {
                        cout << asciiToHex(ch) << " ";
                    }

                    cout << endl << endl;
                }
            }
            // Decimal DB
            else
            {
                stringstream valueStream(values);
                string value;

                while (getline(valueStream, value, ','))
                {
                    value.erase(0, value.find_first_not_of(" \t"));

                    if (value.empty())
                        continue;

                    int number = stoi(value);

                    cout << "Decimal : " << number << endl;
                    cout << "Hexadecimal : "
                         << toHex(number, 1) << endl;

                    cout << endl;
                }
            }
        }

        else if (instruction == "dw")
        {
            string value = values;

            value.erase(0, value.find_first_not_of(" \t"));

            if (!value.empty())
            {
                int number = stoi(value);

                cout << "Decimal : " << number << endl;

                cout << "Hexadecimal : "
                     << toHex(number, 2) << endl;

                cout << "Little Endian Format : "
                     << littleEndian(number, 2) << endl;

                cout << endl;
            }
        }

        else if (instruction == "dd")
        {
            stringstream valueStream(values);
            string value;

            while (getline(valueStream, value, ','))
            {
                value.erase(0, value.find_first_not_of(" \t"));

                if (value.empty())
                    continue;

                int number = stoi(value);

                cout << "Decimal : " << number << endl;

                cout << "Hexadecimal : "
                     << toHex(number, 4) << endl;

                cout << "Little Endian Format : "
                     << littleEndian(number, 4) << endl;

                cout << endl;
            }
        }
    }

    file.close();

    return 0;
}
