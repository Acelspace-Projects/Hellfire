#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// BMP085 Calibration Values
int16_t ac1 = 408;
int16_t ac2 = -72;
int16_t ac3 = -14383;
uint16_t ac4 = 32741;
uint16_t ac5 = 32757;
uint16_t ac6 = 23153;
int16_t b1 = 6190;
int16_t b2 = 4;
int16_t mb = -32768;
int16_t mc = -8711;
int16_t md = 2868;

// Oversampling Setting (OSS), BMP085_ULTRALOWPOWER -> 0
#define OVERSAMPLING 0

// Compute pressure from UP
int32_t computePressure(int32_t UP) {
    int32_t T = 250; // Constant temperature in 0.1degrees
    int32_t B5 = (T * 16) - 8; 

    int32_t B6 = B5 - 4000;
    int32_t X1 = (b2 * ((B6 * B6) >> 12)) >> 11;
    int32_t X2 = (ac2 * B6) >> 11;
    int32_t X3 = X1 + X2;
    int32_t B3 = (((ac1 * 4 + X3) << OVERSAMPLING) + 2) / 4;

    X1 = (ac3 * B6) >> 13;
    X2 = (b1 * ((B6 * B6) >> 12)) >> 16;
    X3 = ((X1 + X2) + 2) >> 2;
    uint32_t B4 = (ac4 * (uint32_t)(X3 + 32768)) >> 15;
    uint32_t B7 = ((uint32_t)UP - B3) * (50000UL >> OVERSAMPLING);

    int32_t p;
    if (B7 < 0x80000000) {
        p = (B7 * 2) / B4;
    } else {
        p = (B7 / B4) * 2;
    }

    X1 = (p >> 8) * (p >> 8);
    X1 = (X1 * 3038) >> 16;
    X2 = (-7357 * p) >> 16;
    p = p + ((X1 + X2 + 3791) >> 4);

    return p;
}

// Read CSV file, process UP values, and write to output CSV
void processCSV(const char *inputFile, const char *outputFile) {
    FILE *input = fopen(inputFile, "r");
    FILE *output = fopen(outputFile, "w");

    if (!input || !output) {
        printf("Error: Unable to open input or output file\n");
        if (input) fclose(input);
        if (output) fclose(output);
        return;
    }

    char line[100];

    // Read and discard header from input file
    fgets(line, sizeof(line), input);

    // Write new header to output file
    fprintf(output, "timestamp,UP,Pressure (Pa)\n");

    while (fgets(line, sizeof(line), input)) {
        uint32_t timestamp;
        int32_t UP;

        // Read timestamp and UP from each line
        if (sscanf(line, "%u,%d", &timestamp, &UP) == 2) {
            int32_t pressure = computePressure(UP);
            fprintf(output, "%u,%d,%d\n", timestamp, UP, pressure); // Write to CSV
        }
    }

    fclose(input);
    fclose(output);
    printf("Processed data written to %s\n", outputFile);
}

int main() {
    processCSV("bmp-data.csv", "bmp-compensated.csv");
    return 0;
}