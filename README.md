# C-Mutator: Automated Systems Vulnerability Hunter

![Language](https://img.shields.io/badge/Language-C-blue.svg)
![Platform](https://img.shields.io/badge/Platform-Linux-lightgrey.svg)
![Focus](https://img.shields.io/badge/Focus-Systems_Security-red.svg)

## Overview
**C-Mutator** is a custom-built, mutation-based fuzzer engineered entirely in C. It is designed to automatically hunt for memory corruption vulnerabilities—specifically Buffer Overflows—in compiled binaries. 

Instead of manual reverse-engineering, this tool rapidly mutates input data at the byte level, automatically feeds it into a target process, and monitors the operating system for fatal crash signals (`SIGSEGV`).

This project was built to demonstrate advanced low-level systems programming, memory management, process cloning, and system signal handling.

## Core Architecture
The fuzzer operates on a highly optimized Parent-Child process model:

1. **Memory Mutation Engine:** Dynamically allocates memory to read a user-provided seed file. It utilizes bitwise XOR operations (`^ 0xFF`) to corrupt specific bytes, generating a unique payload for every iteration.
2. **Process Cloning:** Utilizes the Linux `fork()` system call to spawn an exact child process of the fuzzer.
3. **Execution Replacement:** The child process uses `execl()` to replace its own execution thread with the target binary, passing the mutated payload as the argument. Standard output is suppressed to `/dev/null` to maintain terminal speed.
4. **Automated Crash Detection:** The parent process uses `waitpid()` to monitor the child's exit status. By passing the status integer through the `WIFSIGNALED()` and `WTERMSIG()` macros, the fuzzer detects if the child died due to Signal 11 (Segmentation Fault). 
5. **Payload Preservation:** Upon detecting a crash, the execution loop halts and the exact payload that triggered the memory corruption is saved as `CRASH_TRIGGER.txt` for further reverse-engineering.

## The Vulnerable Target
To test the fuzzer, an intentionally vulnerable target (`vulnerable_target.c`) is included in the repository. 
* **The Flaw:** It utilizes the notoriously unsafe `strcpy()` function to copy input into a fixed-size 64-byte character array without bounds checking.
* **The Compilation:** The `Makefile` intentionally compiles this target without modern stack protections (`-fno-stack-protector -z execstack`) to allow the fuzzer to successfully trigger code-execution conditions on the stack.

---

## Build Instructions

A `Makefile` is included for automated, professional compilation.

```bash
# 1. Clone the repository
git clone [https://github.com/](https://github.com/)[Your GitHub Username]/C-Mutator.git
cd C-Mutator

# 2. Compile the fuzzer and the vulnerable target
make
Usage
The fuzzer requires two arguments to run: a seed file and the target executable.

Bash
./fuzzer <seed_file> <target_executable>
Example Execution
Bash
# Create a basic seed file
echo "TEST_PAYLOAD_STRING" > seed.txt

# Fire the fuzzer against the vulnerable target
./fuzzer seed.txt ./target
Expected Output
The fuzzer will run until it successfully forces a memory fault in the target binary:

Plaintext
[*] Starting Fuzzer...
[*] Seed File: seed.txt
[*] Target Binary: ./target
[*] Fuzzing... Iteration 0
[*] Fuzzing... Iteration 500

[!!!] BOOM! CRASH DETECTED at iteration 742!
[*] Target ./target crashed via Segmentation Fault.
[*] Winning payload saved as CRASH_TRIGGER.txt
Disclaimer
This tool is built strictly for educational purposes and academic security research. It should only be run against binaries and systems where you have explicit permission to conduct vulnerability assessments.