# CSOPESY  S15

# Authors: Gutierrez, Laroza, Leonida, Sinjian

Instructions to Run the Program
=========================================

1. **Compiler Required**:
   - This program is written in C++ and requires a C++ compiler like `g++` or `clang++`.
   - Recommended: g++ version 11 or higher.

2. **Build Instructions**:
   - Open Visual Studio 2022 and build the project
   - configure the config.txt file to your usage
   - NOTE: it is delay**s**-per-exec not delay-per-exec
     
3. **Run the Program**:
   - Once the program starts, type "initialize" to start the emulator
   - once the program runs, it should print on top the set config on initialize

4. **Backing Store**:
  - The program stores the backing store as **csopesy-backing-store.txt** located in the same directory.

5. **IMPORTANT NOTES**
  - `screen -ls` cannot be accessed from the screen -ls screen, it can only be accessed from the main menu thus can be a bit wonky with spamming
  - `vmstat` and `process-smi` can be spammed if needed
  - 
- The screen -c command was altered slightly
  - `screen -c faulty_process "DECLARE varA 10; DECLARE varB 5; ADD varA varA varB; WRITE 0x500 varA; READ varC 0x500; PRINT(\"Variable A: \" + varA); PRINT(\"Result: \" + varC)"`
  - would be instead
  - `screen -c faulty_process 256 "DECLARE varA 10; DECLARE varB 5; ADD varA varA varB; WRITE 0x500 varA; READ varC 0x500; PRINT (\"Variable A: \" + varA); PRINT (\"Result: \" + varC)"`

  _notice the added `256` (memory allocated to process) before the instructions list and the space before the parenthesis in the `print` instructions_

- The `main()` function is located in the following file: **main.cpp**
