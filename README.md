#Simple Shell

A lightweight Unix-style shell implemented in C, designed to support interactive command execution, job control, internal commands, and command history management.

##Overview

Simple Shell is a minimal command-line interpreter that mimics the core functionalities of modern shells like bash or zsh. This shell was built as a standalone project to deepen my understanding of systems programming, process control, signal handling, and user-space command execution in Unix-like operating systems.

##Features

✅ Command Execution
Executes user-entered commands in both foreground and background modes using fork(), execvp(), and waitpid().
Background jobs are cleaned using non-blocking wait logic to prevent zombie processes.
✅ Shell Prompt
Dynamic prompt that displays the current working directory using getcwd().
✅ Internal Commands
exit: Terminates the shell.
pwd: Displays the current working directory.
cd: Changes the working directory with support for:
cd ~: Home directory.
cd -: Previous directory.
cd path: Absolute or relative path.
help: Displays usage information for internal commands.
All internal commands return proper error messages on invalid usage.
✅ History Feature
Maintains a record of the 10 most recent commands.
history: Displays the latest commands with their command number.
!!: Re-executes the last command.
!n: Re-executes the command with command number n.
✅ Signal Handling
Implements custom handler for SIGINT (Ctrl+C).
Prevents shell termination on interrupt and displays help info instead.
Handles system call interruptions correctly by checking errno == EINTR.
✅ Testing Support
Designed with Google Test in mind (gtest/ directory).
Modular structure with tests for command parsing, execution, and history logic.
