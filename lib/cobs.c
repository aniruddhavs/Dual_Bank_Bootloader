#include "cobs.h"

CobsStatus_t cobs_encode(const uint8_t *input, size_t input_len,
                          uint8_t *output, size_t output_size,
                          size_t *output_len)
{
    if (!input || !output || !output_len) return COBS_ERR_NULL;
    // Worst case output: input_len + 2 (overhead byte + terminator)
    if (output_size < input_len + 2)     return COBS_ERR_OUTPUT_TOO_SMALL;

    size_t read_idx  = 0;
    size_t write_idx = 0;
    size_t code_idx  = 0;   // position of the current overhead byte
    uint8_t code     = 1;   // distance to next zero (starts at 1)

    code_idx = write_idx++;  // reserve space for first overhead byte

    while (read_idx < input_len) {
        if (input[read_idx] == 0x00) {
            output[code_idx] = code;  // write accumulated distance
            code_idx = write_idx++;   // reserve next overhead byte
            code = 1;
        } else {
            output[write_idx++] = input[read_idx];
            code++;
            if (code == 0xFF) {       // max block size reached
                output[code_idx] = code;
                code_idx = write_idx++;
                code = 1;
            }
        }
        read_idx++;
    }

    output[code_idx]  = code;    // write final overhead byte
    output[write_idx++] = 0x00;  // terminator

    *output_len = write_idx;
    return COBS_OK;
}

CobsStatus_t cobs_decode(const uint8_t *input, size_t input_len,
                          uint8_t *output, size_t output_size,
                          size_t *output_len)
{
    if (!input || !output || !output_len) return COBS_ERR_NULL;

    size_t read_idx  = 0;
    size_t write_idx = 0;

    while (read_idx < input_len) {
        uint8_t code = input[read_idx++];

        if (code == 0x00) return COBS_ERR_INVALID_FRAME; // 0x00 in payload = corrupt

        for (uint8_t i = 1; i < code; i++) {
            if (read_idx >= input_len)        return COBS_ERR_INVALID_FRAME;
            if (write_idx >= output_size)     return COBS_ERR_OUTPUT_TOO_SMALL;
            output[write_idx++] = input[read_idx++];
        }

        // Insert a zero byte unless this is the last block
        if (code < 0xFF && read_idx < input_len) {
            if (write_idx >= output_size)     return COBS_ERR_OUTPUT_TOO_SMALL;
            output[write_idx++] = 0x00;
        }
    }

    *output_len = write_idx;
    return COBS_OK;
}