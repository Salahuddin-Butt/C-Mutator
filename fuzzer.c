#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

// Function to read file into dynamically allocated memory
unsigned char* read_file(const char* filename, size_t* size) {
    FILE *f = fopen(filename, "rb");
    if (!f) return NULL;
    
    fseek(f, 0, SEEK_END);
    *size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    unsigned char *buffer = malloc(*size);
    fread(buffer, 1, *size, f);
    fclose(f);
    return buffer;
}

// Function to write memory back to a file
void write_file(const char* filename, unsigned char* buffer, size_t size) {
    FILE *f = fopen(filename, "wb");
    if (f) {
        fwrite(buffer, 1, size, f);
        fclose(f);
    }
}

int main(int argc, char *argv[]) {
    // 1. Argument Checking
    // Ensure the user provided exactly the fuzzer, the seed, and the target
    if (argc < 3) {
        printf("[-] Error: Missing arguments.\n");
        printf("[*] Usage: %s <seed_file> <target_executable>\n", argv[0]);
        printf("[*] Example: %s seed.txt ./target\n", argv[0]);
        return 1;
    }

    srand(time(NULL)); 
    size_t size = 0;
    int max_iterations = 10000; 
    
    printf("[*] Starting Fuzzer...\n");
    printf("[*] Seed File: %s\n", argv[1]);
    printf("[*] Target Binary: %s\n", argv[2]);

    for (int i = 0; i < max_iterations; i++) {
        // 2. Dynamic Seed Reading
        // Read from argv[1] instead of hardcoded "seed.txt"
        unsigned char *data = read_file(argv[1], &size);
        if (!data) {
            printf("[-] Failed to read seed file: %s. Exiting.\n", argv[1]);
            return 1;
        }

        if (size > 0) {
            size_t random_index = rand() % size;
            data[random_index] ^= 0xFF; 
        }

        write_file("mutated_input.txt", data, size);
        free(data); 

        pid_t pid = fork();

        if (pid == 0) {
            int null_fd = open("/dev/null", O_WRONLY);
            dup2(null_fd, STDOUT_FILENO);
            dup2(null_fd, STDERR_FILENO);
            close(null_fd);

            // 3. Dynamic Execution
            // Execute argv[2] instead of hardcoded "./target"
            execl(argv[2], argv[2], "mutated_input.txt", NULL);
            exit(1); 
        } else if (pid > 0) {
            int status;
            waitpid(pid, &status, 0);

            if (WIFSIGNALED(status)) {
                int fatal_signal = WTERMSIG(status);
                if (fatal_signal == 11) {
                    printf("\n[!!!] BOOM! CRASH DETECTED at iteration %d!\n", i);
                    printf("[*] Target %s crashed via Segmentation Fault.\n", argv[2]);
                    
                    rename("mutated_input.txt", "CRASH_TRIGGER.txt");
                    printf("[*] Winning payload saved as CRASH_TRIGGER.txt\n");
                    break;
                }
            }
            
            if (i % 500 == 0) {
                printf("[*] Fuzzing... Iteration %d\n", i);
            }
        } else {
            printf("[-] Fork failed.\n");
            return 1;
        }
    }

    printf("[*] Fuzzing run complete.\n");
    return 0;
}