#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define B64_GROUP_BYTES 3
#define B64_GROUP_CHARS 4
#define B64_BITS_6 6
#define B64_MASK_63 0x3F
#define B64_MASK_255 0xFF

static const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char *base64_encode(const unsigned char *data, size_t input_length, size_t *output_length) {
    *output_length = B64_GROUP_CHARS * ((input_length + (B64_GROUP_BYTES - 1)) / B64_GROUP_BYTES);
    char *encoded_data = malloc(*output_length + 1);
    if (encoded_data == NULL) return NULL;

    for (size_t i = 0, j = 0; i < input_length; ) {
        uint32_t octet_a = i < input_length ? data[i++] : 0;
        uint32_t octet_b = i < input_length ? data[i++] : 0;
        uint32_t octet_c = i < input_length ? data[i++] : 0;
        uint32_t triple = (octet_a << 16) + (octet_b << 8) + octet_c;

        encoded_data[j++] = base64_chars[(triple >> 3 * B64_BITS_6) & B64_MASK_63];
        encoded_data[j++] = base64_chars[(triple >> 2 * B64_BITS_6) & B64_MASK_63];
        encoded_data[j++] = base64_chars[(triple >> 1 * B64_BITS_6) & B64_MASK_63];
        encoded_data[j++] = base64_chars[(triple >> 0 * B64_BITS_6) & B64_MASK_63];
    }

    for (size_t i = 0; i < (B64_GROUP_BYTES - input_length % B64_GROUP_BYTES) % B64_GROUP_BYTES; i++)
        encoded_data[*output_length - 1 - i] = '=';

    encoded_data[*output_length] = '\0';
    return encoded_data;
}

unsigned char *base64_decode(const char *data, size_t input_length, size_t *output_length) {
    if (input_length % B64_GROUP_CHARS != 0) return NULL;
    *output_length = input_length / B64_GROUP_CHARS * B64_GROUP_BYTES;
    if (input_length >= 1 && data[input_length - 1] == '=') (*output_length)--;
    if (input_length >= 2 && data[input_length - 2] == '=') (*output_length)--;

    unsigned char *decoded_data = malloc(*output_length == 0 ? 1 : *output_length);
    if (decoded_data == NULL) return NULL;

    static int decoding_table[256];
    static int table_built = 0;
    if (!table_built) {
        for (int i = 0; i < 256; i++) decoding_table[i] = -1;
        for (int i = 0; i < 64; i++) decoding_table[(unsigned char)base64_chars[i]] = i;
        table_built = 1;
    }

    int padding = 0;
    for (size_t i = 0; i < input_length; i++) {
        unsigned char c = (unsigned char)data[i];
        if (c == '=') {
            padding++;
            if (i < input_length - 2 || padding > 2) {
                free(decoded_data);
                return NULL;
            }
        } else if (padding > 0 || decoding_table[c] < 0) {
            free(decoded_data);
            return NULL;
        }
    }

    for (size_t i = 0, j = 0; i < input_length; ) {
        uint32_t sextet_a = data[i] == '=' ? (i++, 0u) : (uint32_t)decoding_table[(unsigned char)data[i++]];
        uint32_t sextet_b = data[i] == '=' ? (i++, 0u) : (uint32_t)decoding_table[(unsigned char)data[i++]];
        uint32_t sextet_c = data[i] == '=' ? (i++, 0u) : (uint32_t)decoding_table[(unsigned char)data[i++]];
        uint32_t sextet_d = data[i] == '=' ? (i++, 0u) : (uint32_t)decoding_table[(unsigned char)data[i++]];
        uint32_t triple = (sextet_a << 3 * B64_BITS_6) + (sextet_b << 2 * B64_BITS_6) + (sextet_c << 1 * B64_BITS_6) + (sextet_d << 0 * B64_BITS_6);

        if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & B64_MASK_255;
        if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & B64_MASK_255;
        if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & B64_MASK_255;
    }

    return decoded_data;
}
