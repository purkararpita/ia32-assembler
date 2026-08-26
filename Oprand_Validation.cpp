// Kajal Kamble
// Operand Validation & Error Detection

#include <iostream>
#include <string>
#include <vector>
#include <cctype>

using namespace std;

// Validate 32-bit register
bool validateRegister(const string &reg)
{
    string registers[] =
    {
        "EAX", "EBX", "ECX", "EDX",
        "ESP", "EBP", "ESI", "EDI"
    };

    string input = reg;

    for (char &c : input)
        c = toupper(static_cast<unsigned char>(c));

    for (const string &validReg : registers)
    {
        if (input == validReg)
            return true;
    }

    return false;
}

// Identify operand type
string identifyOperandType(const string &operand)
{
    if (validateRegister(operand))
        return "REGISTER";

    // Decimal constant
    bool decimal = true;

    for (char c : operand)
    {
        if (!isdigit(static_cast<unsigned char>(c)))
        {
            decimal = false;
            break;
        }
    }

    if (decimal && !operand.empty())
        return "CONSTANT";

    // Memory operand
    if (operand.front() == '[' &&
        operand.back() == ']')
    {
        return "MEMORY";
    }

    return "UNKNOWN";
}

// Validate operand
bool validateOperand(const string &operand)
{
    return identifyOperandType(operand) != "UNKNOWN";
}

// Identify error type
string identifyErrorType(const string &operand)
{
    if (validateOperand(operand))
        return "NO_ERROR";

    string upper = operand;

    for (char &c : upper)
        c = toupper(static_cast<unsigned char>(c));

    // 16-bit registers
    string invalid16[] =
    {
        "AX", "BX", "CX", "DX",
        "SI", "DI", "BP", "SP"
    };

    for (const string &reg : invalid16)
    {
        if (upper == reg)
            return "INVALID_16_BIT_REGISTER";
    }

    // 8-bit registers
    string invalid8[] =
    {
        "AL", "BL", "CL", "DL",
        "AH", "BH", "CH", "DH"
    };

    for (const string &reg : invalid8)
    {
        if (upper == reg)
            return "INVALID_8_BIT_REGISTER";
    }

    return "INVALID_OPERAND";
}
