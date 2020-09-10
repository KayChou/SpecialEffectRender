#include "YUV_IO.hpp"
#include "PlyIO.h"
#include "render.h"

#define width 3840
#define height 2160

timeval start, end;


int main(){
    // Load uyvy frame and convert to RGB
    LOG_INFO("main entry");
    uint8_t *data_uyvy = new uint8_t[width * height * 2];
    Load_uyvy("datas/test.yuv", data_uyvy);

    unsigned char *data = new unsigned char[width * height * 3];
    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            int y = (i*width + j)*2 + 1;
            int u = (j%2) ? (y+1) : (y-1);
            int v = (j%2) ? (y-1) : (y+1);
            int idx = (height - i - 1) * width +  j;
            data[3*idx] = data_uyvy[y] + 1.770*(data_uyvy[u] - 128); //B
            data[3*idx + 1] = data_uyvy[y] - 0.343*(data_uyvy[u] - 128) - 0.714*(data_uyvy[v] - 128); //G
            data[3*idx + 2] = data_uyvy[y] + 1.403*(data_uyvy[v] - 128); //R
        }
    }

    // load 3D model
    std::vector<Point3f> verts;
    std::vector<RGB> colors;
    unsigned int *faces = new unsigned int[500000];
    //readPlyFile("./datas/pointcloud.ply", verts, colors);
    readPlyFile("./datas/AH-64-vertColor.ply", verts, colors, faces);

    float *vertices = new float[6 * verts.size()];
    for(int i=0; i<verts.size(); i++){
        vertices[6*i] = verts[i].X;
        vertices[6*i + 1] = verts[i].Y;
        vertices[6*i + 2] = verts[i].Z;
        if(colors.size() == verts.size()){
            vertices[6*i + 3] = (float)(colors[i].R) / 255.0f;
            vertices[6*i + 4] = (float)(colors[i].G) / 255.0f;
            vertices[6*i + 5] = (float)(colors[i].B) / 255.0f;
        }
        else{
            vertices[6*i + 3] = 1.0;
            vertices[6*i + 4] = 0.0;
            vertices[6*i + 5] = 0.0;
        }
    }
    modelRender render;
    render.render(data, vertices, faces, verts.size(), colors.size());

    delete data_uyvy;
    delete faces;
}