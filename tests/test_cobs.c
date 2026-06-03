#include <stdio.h>
#include <string.h>
#include "../lib/cobs.h"

static int tests_run = 0;
static int tests_passed = 0;

void run_test(const char *label, const uint8_t *input, size_t input_len) {
    tests_run++;
    uint8_t encoded[512];
    uint8_t decoded[512];
    size_t  encoded_len = 0;
    size_t  decoded_len = 0;

    // Encode
    CobsStatus_t enc_status = cobs_encode(input, input_len, encoded, sizeof(encoded), &encoded_len);
    if (enc_status != COBS_OK) {
        printf("  FAIL: %s — encode error %d\n", label, enc_status);
        return;
    }

    // Verify no 0x00 in payload (only the terminator at the end)
    for (size_t i = 0; i < encoded_len - 1; i++) {
        if (encoded[i] == 0x00) {
            printf("  FAIL: %s — 0x00 found in payload at index %zu\n", label, i);
            return;
        }
    }
    if (encoded[encoded_len - 1] != 0x00) {
        printf("  FAIL: %s — frame not terminated with 0x00\n", label);
        return;
    }

    // Decode (exclude the 0x00 terminator)
    CobsStatus_t dec_status = cobs_decode(encoded, encoded_len - 1, decoded, sizeof(decoded), &decoded_len);
    if (dec_status != COBS_OK) {
        printf("  FAIL: %s — decode error %d\n", label, dec_status);
        return;
    }

    // Verify round-trip
    if (decoded_len != input_len || memcmp(decoded, input, input_len) != 0) {
        printf("  FAIL: %s — round-trip mismatch\n", label);
        return;
    }

    printf("  PASS: %s\n", label);
    tests_passed++;
}

int main(void) {
    printf("=== COBS Tests ===\n");

    // Basic cases
    run_test("single non-zero byte",
        (uint8_t[]){0x11}, 1);

    run_test("single zero byte",
        (uint8_t[]){0x00}, 1);

    run_test("no zeros in payload",
        (uint8_t[]){0x11, 0x22, 0x33, 0x44}, 4);

    run_test("zero in middle",
        (uint8_t[]){0x11, 0x00, 0x22}, 3);

    run_test("consecutive zeros",
        (uint8_t[]){0x00, 0x00, 0x00}, 3);

    run_test("zeros at boundaries",
        (uint8_t[]){0x00, 0x11, 0x22, 0x00}, 4);

    run_test("empty input",
        (uint8_t[]){}, 0);

    // Stress: 254 non-zero bytes (max block boundary)
    uint8_t big[254];
    for (int i = 0; i < 254; i++) big[i] = i + 1;
    run_test("254 non-zero bytes (max block)", big, 254);

    // Stress: 255 non-zero bytes (forces block split)
    uint8_t bigger[255];
    for (int i = 0; i < 255; i++) bigger[i] = i + 1;
    run_test("255 non-zero bytes (block split)", bigger, 255);

    printf("\n%d/%d tests passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}