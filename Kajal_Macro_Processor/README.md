
This project implements a basic two-pass macro preprocessor for assembly source files, based on the class explanation:

- The macro preprocessor runs before the assembler.
- PASS 1 reads `%macro` definitions and stores their bodies in a dynamically sized Macro Definition Table (MDT).
- The Macro Name Table (MNT) stores each macro name, parameter count, and MDT start/end indexes.
- PASS 2 copies ordinary assembly lines, removes macro definitions, and expands macro calls by replacing `%1`, `%2`, etc. with actual arguments.
- If at least one macro definition is present and preprocessing succeeds, a temporary `.i` file is generated.
- If there are no macro definitions, no `.i` file is generated; the original `.asm` file is selected for the assembler.
- If preprocessing fails, the partial `.i` file is removed and the assembler must not run.


## MNT and MDT

MNT fields:

1. Macro Name
2. Parameter Count
3. Start Index in MDT
4. End Index in MDT

MDT stores one source line per index. It uses `std::vector<string>`, so it grows dynamically as macro bodies are read; it is not a fixed-size table.

## Duplicate macro names
The program allows duplicate names. Both definitions remain visible in MNT, and the last definition is used when the macro is called. This is an explicit simple policy for the assignment implementation.
