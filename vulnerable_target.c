#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function to read file into memory
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

int main() {
    srand(time(NULL)); // Seed the random number generator
    size_t size = 0;
    
    // 1. Read a dummy seed file (you need to create a simple 'seed.txt' with some text)
    unsigned char *data = read_file("seed.txt", &size);
    if (!data) {
        printf("Failed to read seed file.\n");
        return 1;
    }

    // 2. The Mutation: Pick a random byte and corrupt it
    if (size > 0) {
        size_t random_index = rand() % size;
        data[random_index] ^= 0xFF; // Bitwise XOR to flip the bits
        printf("Corrupted byte at index: %zu\n", random_index);
    }

    // 3. Write the corrupted data to a new file
    write_file("mutated_input.txt", data, size);
    
    printf("Mutation complete. Check mutated_input.txt\n");
    free(data);
    return 0;
}