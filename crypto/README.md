# crypto — Cryptographic Primitives

The `crypto` library provides SHA-256 hashing, Base64 encoding/decoding, and AES-256-CBC encryption/decryption.

## Quick Start

```dz
use crypto

# Hash
let hash = crypto.sha256("password")

# Base64
let encoded = crypto.base64Encode("secret")
let decoded = crypto.base64Decode(encoded)

# AES-256-CBC
let key = "12345678901234561234567890123456"  # 32 bytes
let iv  = "1234567890123456"                  # 16 bytes
let encrypted = crypto.aesEncrypt("data", key, iv)
let plaintext = crypto.aesDecrypt(encrypted, key, iv)
```

## API

### `crypto.sha256(text) → string`

Computes the SHA-256 hash of a string. Returns a 64-character lowercase hex digest.

```dz
crypto.sha256("Hello")
# → "185f8db32271fe25f561a6fc938b2e264306ec304eda518007d1764826381969"
```

### `crypto.base64Encode(text) → string`

Encodes a string to Base64 (RFC 4648).

```dz
crypto.base64Encode("Hello World")
# → "SGVsbG8gV29ybGQ="
```

### `crypto.base64Decode(encoded) → string`

Decodes a Base64-encoded string back to its original form.

```dz
crypto.base64Decode("SGVsbG8gV29ybGQ=")
# → "Hello World"
```

### `crypto.aesEncrypt(data, key, iv) → string`

Encrypts data using AES-256-CBC with PKCS7 padding.

- `data`: plaintext string
- `key`: 32-byte encryption key
- `iv`: 16-byte initialization vector

Returns the ciphertext as a raw string (may contain non-printable bytes).

### `crypto.aesDecrypt(data, key, iv) → string`

Decrypts AES-256-CBC ciphertext back to plaintext.

- `data`: ciphertext string (output of `aesEncrypt`)
- `key`: same 32-byte key used for encryption
- `iv`: same 16-byte IV used for encryption

Throws an error if decryption fails (wrong key, corrupted data, or invalid padding).

## Implementation Details

| Algorithm | Mode | Key Size | Block Size |
|-----------|------|----------|------------|
| SHA-256 | — | — | — |
| AES-256 | CBC | 32 bytes | 16 bytes |
| Base64 | — | — | — |

- AES uses PKCS7 padding for block alignment
- AES decryption validates padding in constant time
- Base64 follows RFC 4648 standard alphabet
- SHA-256 produces a 64-character hex digest

## Error Handling

All functions validate their inputs:

| Error | Cause |
|-------|-------|
| `crypto.aesEncrypt(): Key must be 32 bytes.` | Wrong key length |
| `crypto.aesEncrypt(): IV must be 16 bytes.` | Wrong IV length |
| `crypto.aesDecrypt(): Decryption failed — invalid padding.` | Wrong key or corrupted data |
| `crypto.sha256(): Argument must be a string.` | Non-string input |

## Notes

- AES ciphertext contains binary data — use `base64Encode()` to store or transmit safely
- Always use a unique IV for each encryption operation
- The same key + IV + data always produces the same ciphertext (deterministic)
- Djazair strings cannot represent embedded null bytes (`\0`), so the reported length of binary ciphertext may differ from the actual byte count. Decryption still works correctly since the C layer tracks length separately.
