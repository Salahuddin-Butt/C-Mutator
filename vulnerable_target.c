#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// By moving the flaw here, we guarantee a Return Address overwrite (Signal 11)
void process_payload(char *payload) {
    char tiny_buffer[50];
    
    // THE FATAL FLAW: Copying a massive payload into a 50-byte buffer
    strcpy(tiny_buffer, payload); 
}

int main(int argc, char *argv[]) {
    // 1. Open the mutated file passed by the fuzzer
    FILE *f = fopen(argv[1], "rb");
    if (!f) {
        return 1;
    }

    // 2. Read the payload
    char payload[1000] = {0};
    fread(payload, 1, 999, f);
    fclose(f);

    // 3. Trigger the vulnerability
    process_payload(payload);

    return 0;
}