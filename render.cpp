#include "render.h"


void modelRender::render(unsigned char* imgData){ 
    LOG_INFO("Begin to render one frame =========== ")
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    LOG_INFO("Begin to generate Texture");
    generateTexture(imgData);// Bind framebuffer to FBOID, and generate texture
    LOG_INFO("Finish generating Texture");

    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    LOG_INFO("Begin to generate Model");
    BindModel();
    LOG_INFO("Finish generating Model");

    shaderModel->use();
    glm::mat4 projection = glm::perspective(glm::radians(fov), (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(cameraPos, cameraTar, cameraUp);
    shaderModel->setMat4("projection", projection);
    shaderModel->setMat4("view", view);
    shaderModel->setMat4("model", glm::mat4(1.0f));

    glBindFramebuffer(GL_FRAMEBUFFER, FBOID);
    glDrawElements(GL_TRIANGLES, Fn, GL_UNSIGNED_INT, 0);

    eglSwapBuffers(eglDpy, eglSurf);

    readScreenPixel("result.png");
}


void modelRender::generateTexture(unsigned char* imgData){
    // load and create a texture
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (imgData){
        LOG_INFO("glTexImage2D");
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData);
        
        LOG_INFO("glTexImage2D finish");
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
        LOG_INFO("glFramebufferTexture2D finish");
        // glGenerateMipmap(GL_TEXTURE_2D);
    }
    else{
        std::cout << "Failed to load texture" << std::endl;
    }
}


void modelRender::generateModel(){
    glGenVertexArrays(1, &VAO[1]);
    glGenBuffers(1, &VBO[1]);
    glGenBuffers(1, &EBO[1]);

    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    LOG_INFO("transfer vertices data to GPU");
    glBufferData(GL_ARRAY_BUFFER, 6*4*Vn, vertices, GL_STATIC_DRAW);
    LOG_INFO("finish transfering vertices data to GPU");
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    LOG_INFO("transfer faces data to GPU");
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3*4*3*Fn, faces, GL_STATIC_DRAW);
    LOG_INFO("finish transferring faces data to GPU");

    // configure vertex attribute: position and color
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}


void modelRender::BindModel(){

    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);

    // configure vertex attribute: position and color
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

}


void modelRender::generateFrameBuffer(){
    glGenFramebuffers(1, &FBOID);
    glBindFramebuffer(GL_FRAMEBUFFER, FBOID);

    // create a color attachment texture
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

    // create depth and stencil attachment
    glGenRenderbuffers(1, &RBOID);
    glBindRenderbuffer(GL_RENDERBUFFER, RBOID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBOID); // now actually attach it

    // check frame buffer status
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE) {
        printf("Problem with OpenGL framebuffer after specifying color render buffer: \n%x\n", status);
    } else {
        printf("FBO creation succedded\n");
    }
}


void modelRender::glInit(){
    // 1. Initialize EGL
    eglDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    EGLint major, minor;
    if(!eglInitialize(eglDpy, &major, &minor)){
        std::printf("Failed to init\n");
    }

    // 2. Select an appropriate configuration
    EGLint numConfigs;
    EGLConfig eglCfg;

    eglChooseConfig(eglDpy, configAttribs, &eglCfg, 1, &numConfigs);

    eglSurf = eglCreatePbufferSurface(eglDpy, eglCfg, pbufferAttribs); // 3. Create a surface
    eglBindAPI(EGL_OPENGL_API); // 4. Bind the API

    // 5. Create a context and make it current
    eglCtx = eglCreateContext(eglDpy, eglCfg, EGL_NO_CONTEXT, NULL);

    eglMakeCurrent(eglDpy, eglSurf, eglSurf, eglCtx);
    
    if (!gladLoadGLLoader((GLADloadproc)eglGetProcAddress)){ // glad: load all OpenGL function pointers
        std::cout << "Failed to initialize GLAD" << std::endl;
    }  
    generateFrameBuffer();
}


void modelRender::processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 100/60;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if(glm::length(cameraPos - cameraTar) > cameraSpeed){
            cameraPos += cameraSpeed * glm::normalize(cameraTar - cameraPos);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPos -= cameraSpeed * glm::normalize(cameraTar - cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS | glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        glm::vec3 temp = cameraPos + cameraSpeed * glm::normalize(glm::cross(cameraUp, cameraTar-cameraPos));
        cameraPos = cameraTar + glm::length(cameraPos - cameraTar) * glm::normalize(temp - cameraTar);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS | glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        glm::vec3 temp = cameraPos - cameraSpeed * glm::normalize(glm::cross(cameraUp, cameraTar-cameraPos));
        cameraPos = cameraTar + glm::length(cameraPos - cameraTar) * glm::normalize(temp - cameraTar);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
        glm::vec3 right = glm::cross(cameraUp, cameraTar - cameraPos);
        glm::vec3 temp = (cameraPos + cameraSpeed * cameraUp);
        cameraPos = cameraTar + glm::length(cameraTar - cameraPos) * glm::normalize(temp - cameraTar);
        cameraUp = glm::normalize(glm::cross(cameraTar - cameraPos, right));
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
        glm::vec3 right = glm::cross(cameraUp, cameraTar - cameraPos);
        glm::vec3 temp = (cameraPos - cameraSpeed * cameraUp);
        cameraPos = cameraTar + glm::length(cameraTar - cameraPos) * glm::normalize(temp - cameraTar);
        cameraUp = glm::normalize(glm::cross(cameraTar - cameraPos, right));
    }
}


void modelRender::readScreenPixel(const char* filename){
    LOG_INFO("Begin to read screen pixel");
    unsigned int readBuffer;
    glGenBuffers(1, &readBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, readBuffer);
    LOG_INFO("glBufferData");
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * WIN_WIDTH * WIN_HEIGHT * 3, NULL, GL_DYNAMIC_DRAW);
    LOG_INFO("finish glBufferData");

    glBindBuffer(GL_PIXEL_PACK_BUFFER, readBuffer);
    LOG_INFO("glReadPixels");
    glReadPixels(0, 0, WIN_WIDTH, WIN_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    LOG_INFO("finish glReadPixels");

    uchar *data = (uchar *)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
    LOG_INFO("Finish rendering one frame ======");

    cv::Mat img(WIN_HEIGHT, WIN_WIDTH, CV_8UC3);
    for(int i=0; i<WIN_HEIGHT; i++){
        for(int j=0; j<WIN_WIDTH; j++){
            int pixIdx = (WIN_HEIGHT - i - 1) * WIN_WIDTH + j;
            img.at<cv::Vec3b>(i, j)[0] = data[3*pixIdx + 0];
            img.at<cv::Vec3b>(i, j)[1] = data[3*pixIdx + 1];
            img.at<cv::Vec3b>(i, j)[2] = data[3*pixIdx + 2];
        }
    }
    cv::imwrite(filename, img);
    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
}


modelRender::modelRender(float *vertices, unsigned int *faces, int Vn, int Fn){
    WIN_WIDTH = 3840;
    WIN_HEIGHT = 2160;
    SCR_WIDTH = WIN_WIDTH;
    SCR_HEIGHT = WIN_HEIGHT;
    this->textureID = 0;
    cameraPos = glm::vec3(16.0f, -11.0f, -3.9f);
    cameraTar = glm::vec3(-10.0f, 0.0f, 0.0f);
    cameraUp = glm::vec3(-0.0132f, -0.347f, 0.9379f);

    glInit();
#ifdef CMAKE
    shaderTexture = new Shader("../include/opengl/texture.vs", "../include/opengl/texture.fs"); 
    shaderModel = new Shader("../include/opengl/model.vs", "../include/opengl/model.fs");
#else
    shaderTexture = new Shader("./include/opengl/texture.vs", "./include/opengl/texture.fs"); 
    shaderModel = new Shader("./include/opengl/model.vs", "./include/opengl/model.fs");
#endif
    

    this->vertices = vertices;
    this->faces = faces;
    this->Vn = Vn;
    this->Fn = Fn;

    generateModel();
}


modelRender::~modelRender(){
    delete shaderModel;
    delete shaderTexture;
    eglTerminate(eglDpy);
}
