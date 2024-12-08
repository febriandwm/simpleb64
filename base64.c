#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> 

//Base64 characters
static const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

//encode data to Base64
char *base64_encode(const unsigned char *data, size_t input_length) {
    size_t output_length = 4 * ((input_length + 2) / 3);
    char *encoded_data = malloc(output_length + 1);
    if (encoded_data == NULL) return NULL;

    for (size_t i = 0, j = 0; i < input_length;) {
        uint32_t octet_a = i < input_length ? data[i++] : 0;
        uint32_t octet_b = i < input_length ? data[i++] : 0;
        uint32_t octet_c = i < input_length ? data[i++] : 0;

        uint32_t combined = (octet_a << 16) | (octet_b << 8) | octet_c;

        encoded_data[j++] = base64_chars[(combined >> 18) & 0x3F];
        encoded_data[j++] = base64_chars[(combined >> 12) & 0x3F];
        encoded_data[j++] = i > input_length + 1 ? '=' : base64_chars[(combined >> 6) & 0x3F];
        encoded_data[j++] = i > input_length ? '=' : base64_chars[combined & 0x3F];
    }

    encoded_data[output_length] = '\0';
    return encoded_data;
}

//decode a Base64 character
static inline int base64_char_to_value(char c) {
    if ('A' <= c && c <= 'Z') return c - 'A';
    if ('a' <= c && c <= 'z') return c - 'a' + 26;
    if ('0' <= c && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;
}

//decode Base64 to original data
unsigned char *base64_decode(const char *data, size_t input_length, size_t *output_length) {
    if (input_length % 4 != 0) return NULL;

    size_t output_size = input_length / 4 * 3;
    if (data[input_length - 1] == '=') output_size--;
    if (data[input_length - 2] == '=') output_size--;

    unsigned char *decoded_data = malloc(output_size);
    if (decoded_data == NULL) return NULL;

    for (size_t i = 0, j = 0; i < input_length;) {
        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : base64_char_to_value(data[i++]);
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : base64_char_to_value(data[i++]);
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : base64_char_to_value(data[i++]);
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : base64_char_to_value(data[i++]);

        uint32_t combined = (sextet_a << 18) | (sextet_b << 12) | (sextet_c << 6) | sextet_d;

        if (j < output_size) decoded_data[j++] = (combined >> 16) & 0xFF;
        if (j < output_size) decoded_data[j++] = (combined >> 8) & 0xFF;
        if (j < output_size) decoded_data[j++] = combined & 0xFF;
    }

    *output_length = output_size;
    return decoded_data;
}

//loop for multiple operations
int main() {
    char choice;
    do {
        printf("\nChoose an option:\n");
        printf("1. Encode a string to Base64\n");
        printf("2. Decode a Base64 string\n");
        printf("3. \033[1;31mExit\033[0m\n");
        printf("Enter your choice (1, 2, or 3): ");
        scanf(" %c", &choice);
        getchar(); // To consume the newline character

        if (choice == '1') {
            // Encode
            char input[256];
            printf("Enter the string to encode: ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = '\0'; // Remove newline character

            char *encoded = base64_encode((const unsigned char *)input, strlen(input));
            printf("Encoded: %s\n", encoded);
            free(encoded);

        } else if (choice == '2') {
            // Decode
            char input[256];
            printf("Enter the Base64 string to decode: ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = '\0'; // Remove newline character

            size_t decoded_length;
            unsigned char *decoded = base64_decode(input, strlen(input), &decoded_length);
            if (decoded) {
                printf("Decoded: %.*s\n", (int)decoded_length, decoded);
                free(decoded);
            } else {
                printf("Invalid Base64 string.\n");
            }
        } else if (choice != '3') {
            printf("Invalid choice. Please try again.\n");
        }
    } while (choice != '3');

    printf("Goodbye!\n");
    return 0;
}

