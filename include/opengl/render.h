#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "shader.h"
#include <sys/time.h>

#include "../log/log.h"

class modelRender{
private:
    unsigned int WIN_WIDTH;
    unsigned int WIN_HEIGHT;
    unsigned int VAO[2]; // vertex array object
    unsigned int VBO[2]; // vertex buffer object
    unsigned int EBO[2]; // element buffer object

    unsigned int textureID;

    GLFWwindow* window;
    float fov =  45.0f;
    glm::vec3 cameraPos;
    glm::vec3 cameraTar;
    glm::vec3 cameraUp;

    void processInput(GLFWwindow *window);
    void readScreenPixel();

    Shader *shaderTexture;
    Shader *shaderModel;
    timeval start, end;
    
public:
    modelRender();
    ~modelRender();

public:
    void glInit();
    void generateTexture(unsigned char* imgData);
    void generateModel(float *vertices, unsigned int *faces, int Vn, int Fn);
    void render(unsigned char *imgData, float *vertices, unsigned int *faces, int Vn, int Fn);

    void readPixel();
};