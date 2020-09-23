#include <iostream>
#include "YUV_IO.hpp"
#include "PLYIO.h"
#include "render.h"

#define width 3840
#define height 2160

int main(int argc, char *argv[])
{
  // Load uyvy frame and convert to RGB
    LOG_INFO("main entry");
    uint8_t *data_uyvy = new uint8_t[width * height * 2];
#ifdef CMAKE
    Load_uyvy("../datas/test.yuv", data_uyvy);
#else
    Load_uyvy("datas/test.yuv", data_uyvy);
#endif

    unsigned char *data = new unsigned char[width * height * 3];
    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            int y = i*width*2 + j*2 + 1;
            // int u = (j%2) ? (y+1) : (y-1);
            int u = (j%2) ? (y-1) : (y-3);
            // int v = (j%2) ? (y-1) : (y+1);
            int v = (j%2) ? (y+1) : (y-1);
            int idx = (height - i - 1) * width +  j;
            data[3*idx] = data_uyvy[y] + 1.770*(data_uyvy[u] - 128); //B
            data[3*idx + 1] = data_uyvy[y] - 0.343*(data_uyvy[u] - 128) - 0.714*(data_uyvy[v] - 128); //G
            data[3*idx + 2] = data_uyvy[y] + 1.403*(data_uyvy[v] - 128); //R
        }
    }

    // load 3D model
    unsigned int Vn = 0, Fn = 0;
    unsigned int *faces = new unsigned int[5000000];
    float *vertices = new float[5000000];
#ifdef CMAKE
    loadPlyFile("../datas/AH-64.ply", vertices, faces, Vn, Fn);
#else
    loadPlyFile("./datas/AH-64.ply", vertices, faces, Vn, Fn);
#endif

    // from now on use your OpenGL context
    modelRender render(vertices, faces, Vn, Fn);
    render.render(data);

    delete vertices;
    delete faces;
    return 0;
}