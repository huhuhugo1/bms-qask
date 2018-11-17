/* 
 * File:   bms1B.cpp
 */

#include <cstdlib>
#include <math.h>

#include "sndfile.hh"
#include <iostream>
#include <algorithm>
constexpr auto AMPLITUDE = 0x7F000000;


using namespace std;

int main(int argc, char** argv) {    
    uint16_t samples_per_baud = 0;
    int zero_sample;
    int samples[2];

    SndfileHandle inputFile = SndfileHandle(argv[1]);

    /*try {
    } catch () {

    } */

    // Prepare output file name
    std::string output_file_name = argv[1];
    auto pos = output_file_name.rfind('.');
    if (pos != std::string::npos || (pos == 0) || (pos == 1 ))
        output_file_name.erase(pos);
    output_file_name += ".wav";

    // Count number of zero samples at begin
    // This number may be one greater than samples per baud
    while(inputFile.read(&zero_sample, 1) && zero_sample == 0)
        ++samples_per_baud;

    // Move to the expected last sample of the second baud
    inputFile.seek((samples_per_baud << 1) - 1, SEEK_SET);

    // Read expected last sample of the 2nd baud and 1st sample of the 3rd baud
    inputFile.read(samples, 2);
    // If there is no change from nonzero sample to zero sample,
    // expected sample per baud number is wrong and needs to be fixed
    if (!(samples[0] != 0 && samples[1] == 0))
        --samples_per_baud;

    // Move behind the sync sequnce
    inputFile.seek(samples_per_baud << 2, SEEK_SET);

    int baud[samples_per_baud];
    while(inputFile.read(baud, samples_per_baud)) {
        const auto max = *std::max_element(baud, baud + samples_per_baud);
        if (max > AMPLITUDE >> 1) {
            if (max > 3 * (AMPLITUDE >> 2))
                std::cout << "11";
            else
                std::cout << "10";
        } else {
            if (max > AMPLITUDE >> 2)
                std::cout << "01";
            else
                std::cout << "00";
        }
    };
    
    return 0;
}

