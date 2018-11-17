/* 
 * File:   bms1B.cpp
 */

#include <cstdlib>
#include <math.h>

#include "sndfile.hh"
#include <iostream>
#include <algorithm>
#include <fstream>

constexpr auto AMPLITUDE = 0x7F000000;

int main(int argc, char** argv) {    
    if (argc < 2) {
        std::cerr << "Input file path missing!" << std::endl;
        return 1; 
    }

    if (!std::ifstream(argv[1]).good()) {
        std::cerr << "Unable to open input file!" << std::endl;
        return 1;
    }

    SndfileHandle input_file = SndfileHandle(argv[1]);

    // Prepare output file name
    std::string output_file_name = argv[1];
    auto pos = output_file_name.rfind('.');
    if (!(pos == std::string::npos || pos == 0 || (pos == 1 && output_file_name[0] == '.')))
        output_file_name.erase(pos);
    output_file_name += ".txt";

    // Prepare output file
    auto output_file = std::ofstream(output_file_name);
    if (!output_file.good()) {
        std::cerr << "Unable to create output file!" << std::endl;
        return 1;
    }

    uint16_t samples_per_baud = 0;
    int zero_sample;
    int samples[2];

    // Count number of zero samples at begin
    // This number may be one greater than samples per baud
    while(input_file.read(&zero_sample, 1) && zero_sample == 0)
        ++samples_per_baud;

    // Move to the expected last sample of the second baud
    input_file.seek((samples_per_baud << 1) - 1, SEEK_SET);

    // Read expected last sample of the 2nd baud and 1st sample of the 3rd baud
    input_file.read(samples, 2);
    // If there is no change from nonzero sample to zero sample,
    // expected sample per baud number is wrong and needs to be fixed
    if (!(samples[0] != 0 && samples[1] == 0))
        --samples_per_baud;

    // Move behind the sync sequnce
    input_file.seek(samples_per_baud << 2, SEEK_SET);

    int baud[samples_per_baud];
    while(input_file.read(baud, samples_per_baud)) {
        const auto max = *std::max_element(baud, baud + samples_per_baud);
        if (max > AMPLITUDE >> 1) {
            if (max > 3 * (AMPLITUDE >> 2))
                output_file << "11";
            else
                output_file << "10";
        } else {
            if (max > AMPLITUDE >> 2)
                output_file << "01";
            else
                output_file << "00";
        }
    };
    
    output_file << std::endl;
    return 0;
}

