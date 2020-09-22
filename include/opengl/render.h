#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/opencv.hpp>

#include "shader.h"
#include <sys/time.h>
#include <EGL/egl.h>

#include "../log/log.h"

static const EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, 
        EGL_PBUFFER_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, 8,
        EGL_RENDERABLE_TYPE, 
        EGL_OPENGL_BIT,
        EGL_NONE
};    

static const int pbufferWidth = 3840;
static const int pbufferHeight = 2160;

static const EGLint pbufferAttribs[] = {
    EGL_WIDTH, pbufferWidth,
    EGL_HEIGHT, pbufferHeight,
    EGL_NONE,
};


class modelRender{
private:
    unsigned int WIN_WIDTH;
    unsigned int SCR_WIDTH;
    unsigned int WIN_HEIGHT;
    unsigned int SCR_HEIGHT;
    unsigned int VAO[2]; // vertex array object
    unsigned int VBO[2]; // vertex buffer object
    unsigned int EBO[2]; // element buffer object

    unsigned int textureID;
    unsigned int FBOID;
    unsigned int RBOID;

    GLFWwindow* window;
    float fov =  45.0f;
    glm::vec3 cameraPos;
    glm::vec3 cameraTar;
    glm::vec3 cameraUp;

    void processInput(GLFWwindow *window);
    void readScreenPixel(const char* filename);

    Shader *shaderTexture;
    Shader *shaderModel;
    timeval start, end;

    EGLDisplay eglDpy;
    EGLSurface eglSurf;
    EGLContext eglCtx;
    float *vertices;
    unsigned int *faces;
    int Vn;
    int Fn;
    
public:
    modelRender(float *vertices, unsigned int *faces, int Vn, int Fn);
    ~modelRender();

public:
    void glInit();
    void generateFrameBuffer();
    void generateModel();
    void generateTexture(unsigned char* imgData);
    void render(unsigned char *imgData);
    void BindModel();
};