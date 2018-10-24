/*
 * File:   bms1A.cpp
 */

#include "sndfile.hh"
#include <array>
#include <cmath>
#include <iostream>

// Signal parameters
constexpr auto LEVELS = 4;
constexpr auto AMPLITUDE = 0x7F000000;
constexpr auto FREQ_REAL = 1000;
constexpr auto SAMPLE_RATE = 32000;

// File parameters
constexpr auto CHANELS = 1;
constexpr auto FORMAT = SF_FORMAT_WAV | SF_FORMAT_PCM_24;

// Help constants
constexpr auto FREQ_NORM = 2.0 * M_PI * FREQ_REAL / SAMPLE_RATE;
constexpr auto PERIOD_SIZE = SAMPLE_RATE / FREQ_REAL;

// Pregenerated samples of one sine period
// 2D array, 1st index - amplitude level, 2nd index - sample number
constexpr auto sine = [] {
    std::array<std::array<int, PERIOD_SIZE>, LEVELS> a = {0};
    for (size_t level = 0; level < LEVELS; ++level)
        for (size_t idx = 0; idx < PERIOD_SIZE; ++idx)
            a[level][idx] = (AMPLITUDE / (LEVELS - 1.0)) * level * std::sin(FREQ_NORM * idx);
    return a;
}();

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Input file path missing!" << std::endl;
        return 1; 
    }

    // Create output file
    SndfileHandle outputFile = SndfileHandle("sine.waw", SFM_WRITE, FORMAT, CHANELS, SAMPLE_RATE);

    // Write sync sequence
    outputFile.write(sine[0].data(), PERIOD_SIZE);
    outputFile.write(sine[3].data(), PERIOD_SIZE);
    outputFile.write(sine[0].data(), PERIOD_SIZE);
    outputFile.write(sine[3].data(), PERIOD_SIZE);

    // Open input file for read
    auto file = fopen(argv[1], "r");
    if (!file) {
        std::cerr << "Unable to open input file!" << std::endl;
        return 1;
    }

    // In each iteration read two bits and write to file one period of sine of corespondig amplitude level
    while(true) {
        uint8_t level = 0;
        auto c = fgetc(file);
        if (c == EOF)
            break;
        else if (c == '1')
            level += 2;

        c = fgetc(file);
        if (c == EOF)
            break;
        else if (c == '1')
            level += 1;

        outputFile.write(sine[level].data(), PERIOD_SIZE);
    }

    // Close file
    fclose(file);
    return 0;
}
