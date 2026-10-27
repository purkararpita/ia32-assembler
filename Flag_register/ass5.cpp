#include <iostream>
#include <cstdint>
#include <string>
#include <limits>
#include <iomanip>
#include <cctype>

using namespace std;
//============================================
// ASSIGNMENT 5: IA-32 EFLAGS REGISTER SIMULATOR
// This program simulates selected IA-32 flag behaviors.
// It does not modify the real processor's EFLAGS register
// ===========================================
struct Flags {
    bool CF = false;  // Carry Flag
    bool PF = false;  // Parity Flag
    bool AF = false;  // Auxiliary Carry Flag
    bool ZF = false;  // Zero Flag
    bool SF = false;  // Sign Flag
    bool OF = false;  // Overflow Flag
    bool DF = false;  // Direction Flag
};

struct SystemFlags {
    bool TF = false;  // Trap Flag
    bool IF = false;  // Interrupt Enable Flag
    int IOPL = 0;     // I/O Privilege Level (2 bits)
    bool NT = false;  // Nested Task Flag
    bool RF = false;  // Resume Flag
    bool VM = false;  // Virtual-8086 Mode Flag
    bool AC = false;  // Alignment Check
    bool VIF = false; // Virtual Interrupt Flag
    bool VIP = false; // Virtual Interrupt Pending
    bool ID = false;  // CPUID Identification Flag
};

Flags flags;
SystemFlags systemFlags;

// Read and validate an integer.
int readInteger(const string& prompt) {
    int value;

    while (true) {
        cout << prompt;

        if (cin >> value) {
            return value;
        }

        cout << "Invalid input. Please enter an integer.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// Calculate even parity of the low byte.
bool evenParity(uint8_t value) {
    int ones = 0;

    for (int i = 0; i < 8; i++) {
        if ((value >> i) & 1)
            ones++;
    }

    return (ones % 2 == 0);
}

// Update flags for 8-bit addition.
uint8_t performADD(uint8_t a, uint8_t b) {
    unsigned int fullResult =
        static_cast<unsigned int>(a) + b;

    uint8_t result = static_cast<uint8_t>(fullResult);

    flags.CF = fullResult > 0xFF;

    flags.AF =
        ((a & 0x0F) + (b & 0x0F)) > 0x0F;

    flags.ZF = (result == 0);
    flags.SF = (result & 0x80) != 0;

    flags.OF =
        ((~(a ^ b) & (a ^ result) & 0x80) != 0);

    flags.PF = evenParity(result);

    return result;
}

// Update flags for 8-bit subtraction.
uint8_t performSUB(uint8_t a, uint8_t b) {
    int fullResult =
        static_cast<int>(a) - static_cast<int>(b);

    uint8_t result = static_cast<uint8_t>(fullResult);

    flags.CF = (a < b);

    flags.AF =
        ((a & 0x0F) < (b & 0x0F));

    flags.ZF = (result == 0);
    flags.SF = (result & 0x80) != 0;

    flags.OF =
        (((a ^ b) & (a ^ result) & 0x80) != 0);

    flags.PF = evenParity(result);

    return result;
}

// Display the arithmetic
void displayArithmeticFlags() {
    cout << "\n--- Arithmetic / Status Flags ---\n";
    cout << "CF (Carry Flag)           = " << flags.CF << '\n';
    cout << "PF (Parity Flag)          = " << flags.PF << '\n';
    cout << "AF (Auxiliary Carry Flag) = " << flags.AF << '\n';
    cout << "ZF (Zero Flag)            = " << flags.ZF << '\n';
    cout << "SF (Sign Flag)            = " << flags.SF << '\n';
    cout << "OF (Overflow Flag)        = " << flags.OF << '\n';
}

// Convert an unsigned 8-bit pattern to a signed value.
int signed8(uint8_t value) {
    if (value < 128)
        return value;

    return static_cast<int>(value) - 256;
}

// Accept two 8-bit operands.
bool readOperands(uint8_t& a, uint8_t& b) {
    int first = readInteger("Enter first operand (0-255): ");
    int second = readInteger("Enter second operand (0-255): ");

    if (first < 0 || first > 255 ||
        second < 0 || second > 255) {
        cout << "Error: Both operands must be between 0 and 255.\n";
        return false;
    }

    a = static_cast<uint8_t>(first);
    b = static_cast<uint8_t>(second);

    return true;
}

// ADD, SUB, and CMP operations.
void arithmeticMenu() {
    cout << "\n--- Arithmetic Instructions ---\n";
    cout << "1. ADD\n";
    cout << "2. SUB\n";
    cout << "3. CMP\n";

    int choice = readInteger("Select instruction: ");

    if (choice < 1 || choice > 3) {
        cout << "Invalid instruction choice.\n";
        return;
    }

    uint8_t a, b;

    if (!readOperands(a, b))
        return;

    uint8_t result = 0;

    if (choice == 1) {
        result = performADD(a, b);

        cout << "\nInstruction: ADD\n";
        cout << "Result (unsigned) = "
             << static_cast<int>(result) << '\n';
        cout << "Result (signed 8-bit) = "
             << signed8(result) << '\n';
    }
    else if (choice == 2) {
        result = performSUB(a, b);

        cout << "\nInstruction: SUB\n";
        cout << "Result (unsigned) = "
             << static_cast<int>(result) << '\n';
        cout << "Result (signed 8-bit) = "
             << signed8(result) << '\n';
    }
    else {
        // CMP updates flags like subtraction but does not
        // store the subtraction result in a destination.
        result = performSUB(a, b);

        cout << "\nInstruction: CMP\n";
        cout << "Internal subtraction result = "
             << static_cast<int>(result) << '\n';

        cout << "Unsigned comparison: ";

        if (a == b)
            cout << "Equal\n";
        else if (a > b)
            cout << "Greater\n";
        else
            cout << "Less\n";

        int sa = signed8(a);
        int sb = signed8(b);

        cout << "Signed comparison: ";

        if (sa == sb)
            cout << "Equal\n";
        else if (sa > sb)
            cout << "Greater\n";
        else
            cout << "Less\n";
    }

    displayArithmeticFlags();
}

// Direction Flag demonstration.
void directionFlagDemo() {
    cout << "\n--- Direction Flag (DF) Demonstration ---\n";
    cout << "1. Forward processing (DF = 0)\n";
    cout << "2. Backward processing (DF = 1)\n";

    int choice = readInteger("Choose direction: ");

    if (choice == 1) {
        flags.DF = false;
    }
    else if (choice == 2) {
        flags.DF = true;
    }
    else {
        cout << "Invalid choice.\n";
        return;
    }

    string data = "ABCDE";

    cout << "String: " << data << '\n';
    cout << "DF = " << flags.DF << '\n';

    cout << "Processing order: ";

    if (!flags.DF) {
        for (char ch : data)
            cout << ch << ' ';
    }
    else {
        for (int i = static_cast<int>(data.size()) - 1;
             i >= 0; i--) {
            cout << data[i] << ' ';
        }
    }

    cout << "\n";
    cout << "Note: This demonstrates direction conceptually;\n";
    cout << "it does not execute real IA-32 string instructions.\n";
}

// Display selected system flags and descriptions.
void displaySystemFlags() {
    cout << "\n--- Simulated System Flags ---\n";

    cout << "TF   = " << systemFlags.TF
         << "  (Trap Flag: single-step debugging)\n";

    cout << "IF   = " << systemFlags.IF
         << "  (Interrupt Enable Flag)\n";

    cout << "IOPL = " << systemFlags.IOPL
         << "  (I/O Privilege Level, range 0-3)\n";

    cout << "NT   = " << systemFlags.NT
         << "  (Nested Task Flag)\n";

    cout << "RF   = " << systemFlags.RF
         << "  (Resume Flag)\n";

    cout << "VM   = " << systemFlags.VM
         << "  (Virtual-8086 Mode Flag)\n";

    cout << "AC   = " << systemFlags.AC
         << "  (Alignment Check)\n";

    cout << "VIF  = " << systemFlags.VIF
         << "  (Virtual Interrupt Flag)\n";

    cout << "VIP  = " << systemFlags.VIP
         << "  (Virtual Interrupt Pending)\n";

    cout << "ID   = " << systemFlags.ID
         << "  (CPUID Identification Flag)\n";

    cout << "\nThese are simulated values, not actual CPU flags.\n";
}

// Allow the user to change simulated system-flag values.
void systemFlagMenu() {
    while (true) {
        displaySystemFlags();

        cout << "\n--- System Flag Simulation Menu ---\n";
        cout << "1. Set TF\n";
        cout << "2. Set IF\n";
        cout << "3. Set IOPL (0-3)\n";
        cout << "4. Set NT\n";
        cout << "5. Set RF\n";
        cout << "6. Set VM\n";
        cout << "7. Set AC\n";
        cout << "8. Set VIF\n";
        cout << "9. Set VIP\n";
        cout << "10. Set ID\n";
        cout << "0. Return to main menu\n";

        int choice = readInteger("Enter choice: ");

        if (choice == 0)
            return;

        if (choice == 3) {
            int value = readInteger("Enter IOPL (0-3): ");

            if (value >= 0 && value <= 3)
                systemFlags.IOPL = value;
            else
                cout << "IOPL must be between 0 and 3.\n";

            continue;
        }

        if (choice < 1 || choice > 10) {
            cout << "Invalid choice.\n";
            continue;
        }

        int value = readInteger("Enter simulated value (0 or 1): ");

        if (value != 0 && value != 1) {
            cout << "Value must be 0 or 1.\n";
            continue;
        }

        bool state = (value == 1);

        switch (choice) {
            case 1: systemFlags.TF = state; break;
            case 2: systemFlags.IF = state; break;
            case 4: systemFlags.NT = state; break;
            case 5: systemFlags.RF = state; break;
            case 6: systemFlags.VM = state; break;
            case 7: systemFlags.AC = state; break;
            case 8: systemFlags.VIF = state; break;
            case 9: systemFlags.VIP = state; break;
            case 10: systemFlags.ID = state; break;
        }
    }
}

// Display reserved bit positions.
void displayReservedBits() {
    cout << "\n--- EFLAGS Reserved / Special Bit Positions ---\n";

    cout << "Bit 1      : Fixed/reserved position; conventionally reads as 1.\n";
    cout << "Bits 3, 5  : Reserved.\n";
    cout << "Bit 15     : Reserved.\n";
    cout << "Bits 22-31 : Reserved.\n";

    cout << "\nReserved bits must not be used as ordinary flags.\n";
    cout << "Follow the processor manual for reserved-bit behavior.\n";
}

// Display all flag groups.
void displayAllFlags() {
    displayArithmeticFlags();

    cout << "\nDF (Direction Flag) = " << flags.DF << '\n';

    displaySystemFlags();
    displayReservedBits();
}

int main() {
    int choice;

    do {
        cout << "\n================\n";
        cout << "   IA-32 EFLAGS REGISTER SIMULATOR\n";
        cout << "=================\n";

        cout << "1. Arithmetic instructions (ADD/SUB/CMP)\n";
        cout << "2. Direction Flag (DF) demonstration\n";
        cout << "3. System flags simulation\n";
        cout << "4. Display all flags and reserved bits\n";
        cout << "5. Exit\n";

        choice = readInteger("Enter choice: ");

        switch (choice) {
            case 1:
                arithmeticMenu();
                break;

            case 2:
                directionFlagDemo();
                break;

            case 3:
                systemFlagMenu();
                break;

            case 4:
                displayAllFlags();
                break;

            case 5:
                cout << "Assignment 5 simulator terminated.\n";
                break;

            default:
                cout << "Invalid choice. Try again.\n";
        }

    } while (choice != 5);

    return 0;
}
