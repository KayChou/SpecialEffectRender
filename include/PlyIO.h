#ifndef PLYIO_H_
#define PLYIO_H_
#include <stdio.h>
#include <cmath>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include "typedefs.h"

static const unsigned int MAX_COMMENT_SIZE = 256;

template <class T>
void bigLittleEndianSwap (T * v, unsigned int numOfElements) {
    char * tmp = (char*)v;
    for (unsigned int j = 0; j < numOfElements; j++){
        unsigned int offset = 4*j;
        char c = tmp[offset];
        tmp[offset] =  tmp[offset+3];
        tmp[offset+3] = c;
        c = tmp[offset+1];
        tmp[offset+1] = tmp[offset+2];
        tmp[offset+2] = c;
    }
}

//==========================================================================
// parase plyFile header
//==========================================================================
bool parasePlyHeader(std::ifstream &in, const char* filename, unsigned int &vn, unsigned int &fn, 
                   PLYFormat &format, unsigned int &numVertProperties, bool &hasColor, int &headerSize){
    vn = 0;
    fn = 0;
    numVertProperties = 0;
    headerSize = 0;
    hasColor = false;
    std::string current, currentelement;

    in >> current;
    if (current != "ply"){
        std::cerr << "(PLY) not a PLY file" << std::endl;
        return 0;
    }

    in >> current;
    int lid = 0;
    while (current != "end_header") {
        if (current == "format") {
            in >> current;
            if (current == "binary_big_endian") {
                in >> current;
                if (current == "1.0")
                    format = BINARY_BIG_ENDIAN_1;
                else{
                    std::cerr << "(PLY) error parsing header - bad binary big endian version" << std::endl;
                    return 0;
                }
            } 
            else if (current == "binary_little_endian") {
                in >> current;
                if (current == "1.0")
                    format = BINARY_LITTLE_ENDIAN_1;
                else{
                    std::cerr << "(PLY) error parsing header - bad binary little endian version" << std::endl;
                    return 0;
                }
            } 
            else if (current == "ascii") {
                in >> current;
                if (current == "1.0")
                    format = ASCII_1;
                else{
                    std::cerr << "(PLY) error parsing header - bad ascii version" << std::endl;
                    return 0;
                }
            } 
            else {
                std::cerr << "(PLY) error parsing header (format)" << std::endl;
                return 0;
            }
        } 
        else if (current == "element") {
            in >> current;
            if (current == "vertex"){
                currentelement = current;
                in >> vn;
            }
            else if (current == "face"){
                currentelement = current;
                in >> fn;
            }
            else{
                std::cerr << "(PLY) ignoring unknown element " << current << std::endl;
                currentelement = "";
            }
        } 
        else if (currentelement != "" && current == "property") {
            in >> current;
            if (current == "float" || current == "double") {
                numVertProperties++;
                in >> current;
            }
            else if (current == "uchar") { // color
                numVertProperties++;
                hasColor = true;
                in >> current;
            }
            else if (current == "list") {
                in >> current;
                in >> current;
                in >> current;
            } 
            else {
                std::cerr << "(PLY) error parsing header (property)" << std::endl;
                return 0;
            }
        } 
        else if ( (current == "comment") || (current.find("obj_info") != std::string::npos) ) {
            char comment[MAX_COMMENT_SIZE];
            in.getline (comment, MAX_COMMENT_SIZE);
        } 
        else {}
        in >> current;
        lid++;
    }
    headerSize = in.tellg();
    return true;
}



//==========================================================================
// read plyFile: parase header and read datas
//==========================================================================
bool readPlyFile(const char* filename, float *vertices, unsigned int *faces, unsigned int &Vn, unsigned int &Fn)
{
    unsigned int numVertProperties=0;
    int headerSize=0;
    PLYFormat format;
    bool hasColor = false;
    std::string current, currentelement;

    // open file
    std::ifstream in(filename);
    if (!in){
        std::cerr << "(PLY) error opening file" << std::endl;
        return false;
    }

    //parase plyFile header
    if(!parasePlyHeader(in, filename, Vn, Fn, format, numVertProperties, hasColor, headerSize)){
        std::cerr << "Parase header failed\n";
        return false;
    }
    in.close();
    
    // start to read data
    Point3f tempV, tempNormal;
    RGB tempColor;
    float curvature;
    int cnt = 0;

    if(format == ASCII_1){
        FILE * in = fopen(filename, "r");
        if (!in){
            std::cerr << "(PLY) error opening file" << std::endl;
            return false;
        }
        char c;
        for(int i=0; i<headerSize; i++){ fread(&c, 1, 1, in); } // skip the header

        int R, G, B, A, K;
        if(numVertProperties == 10){ // x, y, z, R, G, B, nx, ny, nz, curvature
            for(unsigned int i=0; i<Vn; i++){ 
                fscanf(in, "%f %f %f", &vertices[6*i], &vertices[6*i+1], &vertices[6*i+2]);
                fscanf(in, "%i %i %i", &R, &G, &B);
                fscanf(in, "%f %f %f %f", &tempNormal.X, &tempNormal.Y, &tempNormal.Z, &curvature);
                vertices[6*i + 3] = (float)R / 255.0f;
                vertices[6*i + 4] = (float)G / 255.0f;
                vertices[6*i + 5] = (float)B / 255.0f;
            }
        }
        else if(numVertProperties == 7){ // x, y, z, R, G, B, A
            for(unsigned int i=0; i<Vn; i++){
                fscanf(in, "%f %f %f", &vertices[6*i], &vertices[6*i+1], &vertices[6*i+2]);
                fscanf(in, "%i %i %i %i", &R, &G, &B, &A);
                vertices[6*i + 3] = (float)R / 255.0f;
                vertices[6*i + 4] = (float)G / 255.0f;
                vertices[6*i + 5] = (float)B / 255.0f;
            }
            for(unsigned int i=0; i<Fn; i++){
                fscanf(in, "%u %u %u %u", &K, &faces[3*i], &faces[3*i+1], &faces[3*i+2]);
            }
            return true;
        }

        else if(numVertProperties == 6 && hasColor){ // x, y, z, R, G, B
            for(unsigned int i=0; i<Vn; i++){
                fscanf(in, "%f %f %f", &vertices[6*i], &vertices[6*i+1], &vertices[6*i+2]);
                fscanf(in, "%i %i %i", &R, &G, &B);
                vertices[6*i + 3] = (float)R / 255.0f;
                vertices[6*i + 4] = (float)G / 255.0f;
                vertices[6*i + 5] = (float)B / 255.0f;
            }
            return true;
        }

        
        else if(numVertProperties == 6){ // x, y, z, nx, ny, nz
            for(unsigned int i=0; i<Vn; i++){
                fscanf(in, "%f %f %f", &vertices[6*i], &vertices[6*i+1], &vertices[6*i+2]);
                fscanf(in, "%f %f %f", &tempNormal.X, &tempNormal.Y, &tempNormal.Z);
                vertices[6*i + 3] = 0;
                vertices[6*i + 4] = 0;
                vertices[6*i + 5] = 0;
            }
            return true;
        }
        else if(numVertProperties == 3){ // x, y, z
            for(unsigned int i=0; i<Vn; i++){
                fscanf(in, "%f %f %f", &vertices[6*i], &vertices[6*i+1], &vertices[6*i+2]);
                vertices[6*i + 3] = 0;
                vertices[6*i + 4] = 0;
                vertices[6*i + 5] = 0;
            }
            return true;
        }
        fclose(in);
    }
    else{
        std::cerr << "(PLY) Unknowed format\n" << std::endl;
        return false;
    }

    return true;
}

#endif
