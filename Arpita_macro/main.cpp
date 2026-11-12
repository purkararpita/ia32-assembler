#include <algorithm>
#include <cctype>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;
namespace fs = std::filesystem;


struct MNTEntry
{
    string macroName;
    int parameterCount;
    int startIndex;
    int endIndex;
};


vector<string> MDT;
vector<MNTEntry> MNT;
string trim(const string& value)
{
    const size_t first = value.find_first_not_of(" \t\r\n");
    if (first == string::npos)
        return "";

    const size_t last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}


string removeComment(const string& line)
{
    bool inSingleQuote = false;
    bool inDoubleQuote = false;

    for (size_t i = 0; i < line.size(); ++i)
    {
        const char ch = line[i];

        if (ch == '\\' && i + 1 < line.size())
        {
            ++i;
            continue;
        }

        if (ch == '\'' && !inDoubleQuote)
            inSingleQuote = !inSingleQuote;
        else if (ch == '"' && !inSingleQuote)
            inDoubleQuote = !inDoubleQuote;
        else if (ch == ';' && !inSingleQuote && !inDoubleQuote)
            return line.substr(0, i);
    }

    return line;
}

string firstWord(const string& text)
{
    string word;
    stringstream stream(text);
    stream >> word;
    return word;
}

string toLower(string value)
{
    transform(value.begin(), value.end(), value.begin(),
              [](unsigned char ch) { return static_cast<char>(tolower(ch)); });
    return value;
}

int findMacro(const string& name)
{
   
    for (int i = static_cast<int>(MNT.size()) - 1; i >= 0; --i)
    {
        if (MNT[static_cast<size_t>(i)].macroName == name)
            return i;
    }
    return -1;
}

vector<string> splitArguments(const string& text)
{
    vector<string> arguments;
    const string cleaned = trim(text);
    if (cleaned.empty())
        return arguments;

    string current;
    bool inSingleQuote = false;
    bool inDoubleQuote = false;
    int squareDepth = 0;
    int roundDepth = 0;

    for (char ch : cleaned)
    {
        if (ch == '\'' && !inDoubleQuote)
            inSingleQuote = !inSingleQuote;
        else if (ch == '"' && !inSingleQuote)
            inDoubleQuote = !inDoubleQuote;
        else if (!inSingleQuote && !inDoubleQuote)
        {
            if (ch == '[') ++squareDepth;
            else if (ch == ']' && squareDepth > 0) --squareDepth;
            else if (ch == '(') ++roundDepth;
            else if (ch == ')' && roundDepth > 0) --roundDepth;
        }

        if (ch == ',' && !inSingleQuote && !inDoubleQuote &&
            squareDepth == 0 && roundDepth == 0)
        {
            arguments.push_back(trim(current));
            current.clear();
        }
        else
        {
            current += ch;
        }
    }

    arguments.push_back(trim(current));
    return arguments;
}


string replaceParameters(const string& instruction,
                         const vector<string>& arguments)
{
    static const regex parameterPattern(R"(%([0-9]+))");

    string result;
    string remaining = instruction;
    smatch match;

    while (regex_search(remaining, match, parameterPattern))
    {
        result += match.prefix().str();
        const int parameterNumber = stoi(match[1].str());

        if (parameterNumber >= 1 &&
            parameterNumber <= static_cast<int>(arguments.size()))
        {
            result += arguments[static_cast<size_t>(parameterNumber - 1)];
        }
        else
        {
            
            result += match.str();
        }

        remaining = match.suffix().str();
    }

    result += remaining;
    return result;
}
bool isKnownAssemblyWord(string word)
{
    word = toLower(word);

    static const unordered_set<string> knownWords = {
       
        "mov", "movzx", "movsx", "movsxd", "lea", "xchg", "bswap",

        "add", "adc", "sub", "sbb", "mul", "imul", "div", "idiv",
        "inc", "dec", "neg", "not",

        "and", "or", "xor", "shl", "shr", "sal", "sar", "rol", "ror",
        "rcl", "rcr",

        "cmp", "test",

        "push", "pop", "pushad", "popad", "pushf", "popf",

        "call", "ret", "retn", "leave", "enter", "jmp", "je", "jne",
        "jz", "jnz", "jg", "jge", "jl", "jle", "ja", "jae", "jb",
        "jbe", "jc", "jnc", "jo", "jno", "js", "jns", "jp", "jnp",
        "loop", "loope", "loopne",

        "nop", "hlt", "int", "iret", "iretq", "syscall", "sysenter",
        "sysexit", "cpuid", "cld", "std", "cli", "sti", "cdq", "cqo",
        "cbw", "cwde", "cwd", "wait", "pause",
        
        "db", "dw", "dd", "dq", "dt", "do", "dy", "resb", "resw",
        "resd", "resq", "equ", "times", "incbin", "section", "segment",
        "global", "extern", "bits", "default", "align", "alignb", "org",
        "cpu", "use16", "use32", "use64", "struc", "endstruc", "istruc",
        "iend", "at", "absolute"
    };

    return knownWords.count(word) != 0;
}

bool isKnownPreprocessorDirective(const string& word)
{
    static const unordered_set<string> directives = {
        "%include", "%define", "%xdefine", "%undef", "%assign",
        "%ifdef", "%ifndef", "%if", "%elif", "%else", "%endif",
        "%error", "%warning", "%pragma", "%line", "%arg", "%local"
    };
    return directives.count(word) != 0;
}

bool isValidAssemblyLine(const string& codeLine)
{
    const string code = trim(codeLine);
    if (code.empty())
        return true;

    string first, second;
    stringstream stream(code);
    stream >> first;


    if (!first.empty() && first.back() == ':')
        return true;

    if (isKnownAssemblyWord(first) || isKnownPreprocessorDirective(first))
        return true;

    stream >> second;


    if (!second.empty() &&
        (isKnownAssemblyWord(second) || isKnownPreprocessorDirective(second)))
        return true;


    if (first == ".data" || first == ".text" || first == ".bss" ||
        first == ".rodata")
        return true;

    return false;
}


void displayTables()
{
    cout << "\n========== MNT (Macro Name Table) ==========\n";
    cout << "Macro Name\tParameters\tStart\tEnd\n";

    for (const MNTEntry& entry : MNT)
    {
        cout << entry.macroName << "\t\t"
             << entry.parameterCount << "\t\t"
             << entry.startIndex << "\t"
             << entry.endIndex << '\n';
    }

    cout << "\n========== MDT (Macro Definition Table) ==========\n";
    cout << "Index\tInstruction\n";

    for (size_t i = 0; i < MDT.size(); ++i)
        cout << i << '\t' << MDT[i] << '\n';
}

int main(int argc, char* argv[])
{
    
    const string inputFile = (argc >= 2) ? argv[1] : "input.asm";
    fs::path outputPath(inputFile);
    outputPath.replace_extension(".i");
    const string outputFile = outputPath.string();
    const string temporaryFile = outputFile + ".tmp";

    bool preprocessingFlag = false;
    bool preprocessingError = false;

    vector<string> sourceLines;
    vector<bool> definitionLine;
    string line;

 
    std::remove(outputFile.c_str());
    std::remove(temporaryFile.c_str());

    
    ifstream source(inputFile);
    if (!source)
    {
        cerr << "Error: Cannot open input file '" << inputFile << "'.\n";
        return 1;
    }

    while (getline(source, line))
        sourceLines.push_back(line);
    source.close();

    definitionLine.assign(sourceLines.size(), false);

  
    cout << "\n========== PASS 1: COLLECT MACRO DEFINITIONS ==========\n";

    for (size_t i = 0; i < sourceLines.size(); ++i)
    {
        const string code = trim(removeComment(sourceLines[i]));
        if (firstWord(code) != "%macro")
            continue;

        preprocessingFlag = true;
        definitionLine[i] = true;

        string directive, macroName;
        int parameterCount = -1;
        stringstream header(code);

        if (!(header >> directive >> macroName >> parameterCount) ||
            parameterCount < 0)
        {
            cerr << "Error: Invalid macro definition at source line "
                 << (i + 1) << ".\n";
            preprocessingError = true;
            break;
        }

       
        if (findMacro(macroName) != -1)
        {
            cout << "Warning: Duplicate macro '" << macroName
                 << "'; the later definition will be used.\n";
        }

        const int startIndex = static_cast<int>(MDT.size());
        bool endFound = false;
        size_t endLine = i;

        for (size_t j = i + 1; j < sourceLines.size(); ++j)
        {
            const string bodyCode = trim(removeComment(sourceLines[j]));

            if (firstWord(bodyCode) == "%endmacro")
            {
                definitionLine[j] = true;
                endFound = true;
                endLine = j;
                break;
            }

            definitionLine[j] = true;
            MDT.push_back(sourceLines[j]);
        }

        if (!endFound)
        {
            cerr << "Error: %endmacro not found for macro '"
                 << macroName << "'.\n";
            preprocessingError = true;
            break;
        }

        
        const int endIndex = static_cast<int>(MDT.size()) - 1;
        MNT.push_back({macroName, parameterCount, startIndex, endIndex});

        cout << "Stored macro: " << macroName
             << " (" << parameterCount << " parameters)\n";

        i = endLine;
    }

    if (preprocessingError)
    {
        cout << "\nMacro preprocessing failed. No output file was generated.\n";
        return 1;
    }

    if (preprocessingFlag)
        displayTables();
    if (!preprocessingFlag)
    {
        cout << "\nNo macro definitions found.\n";
        cout << "No temporary .i file was generated.\n";
        cout << "File selected for assembler: " << inputFile << '\n';
        cout << "\nPreprocessor completed.\n";
        return 0;
    }

 
    cout << "\n========== PASS 2: EXPAND MACRO CALLS ==========\n";

    ofstream output(temporaryFile);
    if (!output)
    {
        cerr << "Error: Cannot create temporary output file.\n";
        return 1;
    }

    for (size_t lineIndex = 0; lineIndex < sourceLines.size(); ++lineIndex)
    {
        if (definitionLine[lineIndex])
            continue;

        const string originalLine = sourceLines[lineIndex];
        const string code = trim(removeComment(originalLine));

     
        if (code.empty())
        {
            output << originalLine << '\n';
            continue;
        }

        string token;
        string remaining;
        stringstream parser(code);
        parser >> token;
        getline(parser, remaining);

        const int macroPosition = findMacro(token);

        if (macroPosition >= 0)
        {
            const MNTEntry& macro = MNT[static_cast<size_t>(macroPosition)];
            const vector<string> arguments = splitArguments(removeComment(remaining));

            if (static_cast<int>(arguments.size()) != macro.parameterCount)
            {
                cerr << "Error: Parameter mismatch in macro '"
                     << macro.macroName << "'. Expected "
                     << macro.parameterCount << ", received "
                     << arguments.size() << ".\n";
                preprocessingError = true;
                continue;
            }

            cout << "Expanding macro call: " << macro.macroName << '\n';

            for (int mdtIndex = macro.startIndex;
                 mdtIndex <= macro.endIndex; ++mdtIndex)
            {
                string expanded = MDT[static_cast<size_t>(mdtIndex)];
                expanded = replaceParameters(expanded, arguments);
                output << expanded << '\n';
            }
        }
        else
        {
          
            if (!isValidAssemblyLine(code))
            {
                cerr << "Error: Undefined macro or unrecognized statement '"
                     << token << "' at source line "
                     << (lineIndex + 1) << ".\n";
                preprocessingError = true;
                continue;
            }

            output << originalLine << '\n';
        }
    }

    output.close();

    if (preprocessingError)
    {
        std::remove(temporaryFile.c_str());
        std::remove(outputFile.c_str());
        cerr << "\nMacro preprocessing failed.\n";
        cerr << "Assembler must not run because preprocessing failed.\n";
        return 1;
    }

    
    std::error_code renameError;
    fs::rename(temporaryFile, outputFile, renameError);
    if (renameError)
    {
        std::remove(temporaryFile.c_str());
        std::remove(outputFile.c_str());
        cerr << "Error: Could not finalize output file: "
             << renameError.message() << '\n';
        return 1;
    }

    
    cout << "\nMacro preprocessing completed successfully.\n";
    cout << "Temporary file generated: " << outputFile << '\n';
    cout << "File selected for assembler: " << outputFile << '\n';
    cout << "\nPreprocessor completed. Pass the selected file to the assembler.\n";

    return 0;
}
