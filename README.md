# Shell
- This project involves building a simplified shell program in C, referred to as "Pioneer Shell" or "pish." The project focuses on key systems programming concepts such as string manipulation, file I/O, command-line argument parsing, process management using `fork()`, `exec()`, and `wait()`, and general terminal usage.
    
    The shell operates in two modes: **interactive mode** and **batch mode**. In interactive mode, the shell waits for user input, parses the command into arguments, and either executes built-in commands or spawns a child process to run external programs. Batch mode processes commands from a script file sequentially.
  Core features of this shell include:

- **Built-in Commands**: The shell supports `exit`, `cd`, and `history`. The `history` command stores and displays previously executed commands from the session.
- **Input Parsing**: Input is parsed into arguments using the `strtok()` function to handle whitespace and delimiters.
- **Process Management**: The shell creates child processes using `fork()` and executes programs with `execvp()`, mimicking how standard shells operate.
