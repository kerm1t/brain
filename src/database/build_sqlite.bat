rem To build a DLL of SQLite for use in Windows, first acquire the appropriate amalgamated source code files, sqlite3.c and sqlite3.h. These can either be downloaded from the SQLite website or custom generated from sources as shown above.

rem With source code files in the working directory, a DLL can be generated using MSVC with the following command:

cl sqlite3.c -link -dll -out:sqlite3.dll
rem The above command should be run from the MSVC Native Tools Command Prompt.