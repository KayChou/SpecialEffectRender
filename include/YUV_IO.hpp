#pragma once

#include <stdio.h>
#include <fstream>
#include <iostream>
#include "stdint.h"


bool Load_uyvy(const char* filename, uint8_t *data){
    FILE * in = fopen (filename, "r");
    if (!in){
        std::cerr << "Error opening file" << std::endl;
        return false;
    }

    fread(data, sizeof(uint8_t), 3840 * 2160 * 2, in);

    fclose(in);

    return true;
}