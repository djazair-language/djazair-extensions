#include "vm/vm.h"
#include "vm/registry.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "sha256.h"
#include "aes.h"

#define AES_BLOCK_SIZE 16
#define AES_256_KEY_SIZE 32
#define AES_IV_SIZE 16
#define SHA256_HASH_LEN 32
#define SHA256_HEX_LEN 64

// Forward declarations for base64 (defined in base64.c)
char *base64_encode(const unsigned char *data, size_t input_length, size_t *output_length);
unsigned char *base64_decode(const char *data, size_t input_length, size_t *output_length);

// SHA-256 Binding
static Value cryptoSha256(djazairVM *vm, int argCount, Value *args) {
    if (argCount != 1) { 
        runtimeError(vm, "crypto.sha256(): Expected 1 argument but got %d.", argCount); 
        return NULL_VAL; 
    }
    if (!IS_STRING(args[0])) { 
        runtimeError(vm, "crypto.sha256(): Argument must be a string."); 
        return NULL_VAL; 
    }

    const char* input = AS_STRING(args[0])->chars;
    size_t len = (size_t)AS_STRING(args[0])->length;
    
    SHA256_CTX ctx;
    uint8_t hash[SHA256_HASH_LEN];
    sha256_init(&ctx);
    sha256_update(&ctx, (uint8_t*)input, len);
    sha256_final(&ctx, hash);

    char hex[SHA256_HEX_LEN + 1];
    for (int i = 0; i < SHA256_HASH_LEN; i++) {
        snprintf(hex + (i * 2), 3, "%02x", hash[i]);
    }
    hex[SHA256_HEX_LEN] = '\0';

    return OBJ_VAL(copyString(vm, hex, (int)strlen(hex)));
}

// Base64 Encode Binding
static Value cryptoBase64Encode(djazairVM *vm, int argCount, Value *args) {
    if (argCount != 1) { 
        runtimeError(vm, "crypto.base64Encode(): Expected 1 argument but got %d.", argCount); 
        return NULL_VAL; 
    }
    if (!IS_STRING(args[0])) { 
        runtimeError(vm, "crypto.base64Encode(): Argument must be a string."); 
        return NULL_VAL; 
    }

    const char* input = AS_STRING(args[0])->chars;
    size_t len = (size_t)AS_STRING(args[0])->length;
    
    size_t out_len;
    char* encoded = base64_encode((unsigned char*)input, len, &out_len);
    if (!encoded) return NULL_VAL;
    if (out_len > INT_MAX) {
        free(encoded);
        runtimeError(vm, "crypto.base64Encode(): Result is too large.");
        return NULL_VAL;
    }

    Value result = OBJ_VAL(copyString(vm, encoded, (int)out_len));
    free(encoded);
    return result;
}

// Base64 Decode Binding
static Value cryptoBase64Decode(djazairVM *vm, int argCount, Value *args) {
    if (argCount != 1) { 
        runtimeError(vm, "crypto.base64Decode(): Expected 1 argument but got %d.", argCount); 
        return NULL_VAL; 
    }
    if (!IS_STRING(args[0])) { 
        runtimeError(vm, "crypto.base64Decode(): Argument must be a string."); 
        return NULL_VAL; 
    }

    const char* input = AS_STRING(args[0])->chars;
    size_t len = (size_t)AS_STRING(args[0])->length;
    
    size_t out_len;
    unsigned char* decoded = base64_decode(input, len, &out_len);
    if (!decoded) return NULL_VAL;
    if (out_len > INT_MAX) {
        free(decoded);
        runtimeError(vm, "crypto.base64Decode(): Result is too large.");
        return NULL_VAL;
    }

    Value result = OBJ_VAL(copyString(vm, (const char*)decoded, (int)out_len));
    free(decoded);
    return result;
}

// AES Encrypt (CBC-256 with PKCS7 padding)
static Value cryptoAesEncrypt(djazairVM *vm, int argCount, Value *args) {
    if (argCount != 3) { 
        runtimeError(vm, "crypto.aesEncrypt(): Expected 3 arguments but got %d.", argCount); 
        return NULL_VAL; 
    }
    if (!IS_STRING(args[0])) { 
        runtimeError(vm, "crypto.aesEncrypt(): Argument 1 (data) must be a string."); 
        return NULL_VAL; 
    }
    if (!IS_STRING(args[1])) { 
        runtimeError(vm, "crypto.aesEncrypt(): Argument 2 (key) must be a string."); 
        return NULL_VAL; 
    }
    if (!IS_STRING(args[2])) { 
        runtimeError(vm, "crypto.aesEncrypt(): Argument 3 (iv) must be a string."); 
        return NULL_VAL; 
    }

    size_t data_len = (size_t)AS_STRING(args[0])->length;
    size_t key_len = (size_t)AS_STRING(args[1])->length;
    size_t iv_len = (size_t)AS_STRING(args[2])->length;
    const char* data = AS_CSTRING(args[0]);
    const char* key = AS_CSTRING(args[1]);
    const char* iv = AS_CSTRING(args[2]);

    if (key_len != AES_256_KEY_SIZE) {
        runtimeError(vm, "crypto.aesEncrypt(): Key must be %d bytes.", AES_256_KEY_SIZE); 
        return NULL_VAL;
    }
    if (iv_len != AES_IV_SIZE) {
        runtimeError(vm, "crypto.aesEncrypt(): IV must be %d bytes.", AES_IV_SIZE); 
        return NULL_VAL;
    }
    if (data_len > (size_t)INT_MAX - AES_BLOCK_SIZE) {
        runtimeError(vm, "crypto.aesEncrypt(): Data is too large.");
        return NULL_VAL;
    }

    // PKCS7 Padding
    size_t padding = AES_BLOCK_SIZE - (data_len % AES_BLOCK_SIZE);
    size_t padded_len = data_len + padding;
    uint8_t* buffer = malloc(padded_len);
    if (buffer == NULL) {
        runtimeError(vm, "crypto.aesEncrypt(): Out of memory.");
        return NULL_VAL;
    }
    memcpy(buffer, data, data_len);
    for (size_t i = data_len; i < padded_len; i++) buffer[i] = (uint8_t)padding;

    struct AES_ctx ctx;
    AES_init_ctx_iv(&ctx, (uint8_t*)key, (uint8_t*)iv);
    AES_CBC_encrypt_buffer(&ctx, buffer, padded_len);

    Value result = OBJ_VAL(copyString(vm, (const char*)buffer, (int)padded_len));
    free(buffer);
    return result;
}

// AES Decrypt (CBC-256 with PKCS7 unpadding)
static Value cryptoAesDecrypt(djazairVM *vm, int argCount, Value *args) {
    if (argCount != 3) { 
        runtimeError(vm, "crypto.aesDecrypt(): Expected 3 arguments but got %d.", argCount); 
        return NULL_VAL; 
    }
    if (!IS_STRING(args[0])) { 
        runtimeError(vm, "crypto.aesDecrypt(): Argument 1 (data) must be a string."); 
        return NULL_VAL; 
    }
    if (!IS_STRING(args[1])) { 
        runtimeError(vm, "crypto.aesDecrypt(): Argument 2 (key) must be a string."); 
        return NULL_VAL; 
    }
    if (!IS_STRING(args[2])) { 
        runtimeError(vm, "crypto.aesDecrypt(): Argument 3 (iv) must be a string."); 
        return NULL_VAL; 
    }

    size_t data_len = (size_t)AS_STRING(args[0])->length;
    size_t key_len = (size_t)AS_STRING(args[1])->length;
    size_t iv_len = (size_t)AS_STRING(args[2])->length;
    const char* data = AS_CSTRING(args[0]);
    const char* key = AS_CSTRING(args[1]);
    const char* iv = AS_CSTRING(args[2]);

    if (data_len == 0 || data_len % AES_BLOCK_SIZE != 0) {
        runtimeError(vm, "crypto.aesDecrypt(): Invalid ciphertext length."); 
        return NULL_VAL;
    }
    if (data_len > (size_t)INT_MAX) {
        runtimeError(vm, "crypto.aesDecrypt(): Data is too large.");
        return NULL_VAL;
    }
    if (key_len != AES_256_KEY_SIZE) {
        runtimeError(vm, "crypto.aesDecrypt(): Key must be %d bytes.", AES_256_KEY_SIZE); 
        return NULL_VAL;
    }
    if (iv_len != AES_IV_SIZE) {
        runtimeError(vm, "crypto.aesDecrypt(): IV must be %d bytes.", AES_IV_SIZE); 
        return NULL_VAL;
    }

    uint8_t* buffer = malloc(data_len);
    if (buffer == NULL) {
        runtimeError(vm, "crypto.aesDecrypt(): Out of memory.");
        return NULL_VAL;
    }
    memcpy(buffer, data, data_len);

    struct AES_ctx ctx;
    AES_init_ctx_iv(&ctx, (uint8_t*)key, (uint8_t*)iv);
    AES_CBC_decrypt_buffer(&ctx, buffer, data_len);

    // Remove PKCS7 Padding in constant time
    uint8_t padding = buffer[data_len - 1];
    int padding_invalid = 0;
    
    for (size_t i = 1; i <= AES_BLOCK_SIZE; i++) {
        uint8_t expected = padding;
        if (i > padding) expected = buffer[data_len - i];
        if (buffer[data_len - i] != expected) {
            padding_invalid = 1;
        }
    }
    if (padding > AES_BLOCK_SIZE || padding == 0) {
        padding_invalid = 1;
    }
    if (padding_invalid) {
        free(buffer);
        runtimeError(vm, "crypto.aesDecrypt(): Decryption failed — invalid padding."); 
        return NULL_VAL;
    }
    size_t original_len = data_len - padding;

    Value result = OBJ_VAL(copyString(vm, (const char*)buffer, (int)original_len));
    free(buffer);
    return result;
}

/* Register the crypto module functions available to the core */
static NativeMethod crypto_nativeMethods[] = {
    {"sha256",        cryptoSha256,        1},
    {"base64Encode", cryptoBase64Encode,  1},
    {"base64Decode", cryptoBase64Decode,  1},
    {"aesEncrypt",   cryptoAesEncrypt,    3},
    {"aesDecrypt",   cryptoAesDecrypt,    3},
    {NULL, NULL, 0}
};

/* Initialize and register the std_crypto_native module in the Djazair engine */
void init_crypto_module(djazairVM *vm) {
    djazair_register_native_module(vm, "std_crypto_native", crypto_nativeMethods);
}
