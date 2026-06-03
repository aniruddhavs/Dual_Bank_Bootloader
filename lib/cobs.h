#ifndef COBS_H
#define COBS_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
    COBS_OK,
    COBS_ERR_NULL,
    COBS_ERR_OUTPUT_TOO_SMALL,
    COBS_ERR_INVALID_FRAME
} CobsStatus_t;

CobsStatus_t cobs_encode(const uint8_t *input, size_t input_len,
                          uint8_t *output, size_t output_size,
                          size_t *output_len);

CobsStatus_t cobs_decode(const uint8_t *input, size_t input_len,
                          uint8_t *output, size_t output_size,
                          size_t *output_len);

#endif