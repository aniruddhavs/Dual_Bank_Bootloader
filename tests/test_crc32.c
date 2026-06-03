#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../lib/crc32.h"

// Known correct CRC32 values — verify against https://crccalc.com
static int tests_run = 0;
static int tests_passed = 0;

void assert_crc(const char *label, const uint8_t *data, size_t len, uint32_t expected) {
    tests_run++;
    uint32_t result = crc32_calculate(data, len);
    if (result == expected) {
        printf("  PASS: %s\n", label);
        tests_passed++;
    } else {
        printf("  FAIL: %s — got 0x%08X, expected 0x%08X\n", label, result, expected);
    }
}

int main(void) {
    printf("=== CRC32 Tests ===\n");

    assert_crc("empty string",
        (uint8_t*)"", 0, 0x00000000);

    assert_crc("'123456789'",
        (uint8_t*)"123456789", 9, 0xCBF43926);

    assert_crc("single 0xFF byte",
        (uint8_t*)"\xFF", 1, 0xFF000000);

    uint8_t zeros[256] = {0};
    assert_crc("256 zero bytes",
        zeros, 256, 0xd968558);

    printf("\n%d/%d tests passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}