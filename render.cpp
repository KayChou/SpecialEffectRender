#include "render.h"


void modelRender::render(unsigned char* imgData, float *vertices, unsigned int *faces, int Vn, int Fn){
    LOG_INFO("Begin to render one frame");
    generateTexture(imgData);
    
    generateModel(vertices, faces, Vn, Fn);
    glBindTexture(GL_TEXTURE_2D, textureID); // bind Texture
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // bind frame buffer to default display

    //while (!glfwWindowShouldClose(window))
    //{
        //processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // texture render
        shaderTexture->use();
        glBindVertexArray(VAO[0]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);
        
        // model render
        shaderModel->use();
        glBindVertexArray(VAO[1]);
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraTar, cameraUp);
        shaderModel->setMat4("projection", projection);
        shaderModel->setMat4("view", view);
        shaderModel->setMat4("model", glm::mat4(1.0f));

        glBindBuffer(GL_ELEMENT_ARRAY_BARRIER_BIT, EBO[1]);
        glDrawElements(GL_TRIANGLES, Fn, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        //break;
    //}
    //readScreenPixel();

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO[1]);
    glDeleteBuffers(1, &VBO[1]);
    glDeleteBuffers(1, &EBO[1]);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    //glfwTerminate();
}


void modelRender::generateTexture(unsigned char* imgData){
    LOG_INFO("Start generating texture");
    float vertices[] = {
        // positions          // colors           // texture coords
         1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    glGenVertexArrays(1, &VAO[0]);
    glGenBuffers(1, &VBO[0]);
    glGenBuffers(1, &EBO[0]);
    LOG_INFO("Finish generating buffers");

    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    LOG_INFO("glBUfferData VBO");
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    LOG_INFO("finish glBUfferData VBO");
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    LOG_INFO("glBUfferData EBO");
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    LOG_INFO("finish glBUfferData EBO");
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // load and create a texture 
    // -------------------------
    LOG_INFO("Start to gen texure");
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    LOG_INFO("Set texture parameter");

    int width = 3840;
    int height = 2160;

    if (imgData){
        LOG_INFO("glTexImage2D");
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData);
        LOG_INFO("Finish glTexImage2D");
        //glGenerateMipmap(GL_TEXTURE_2D);
    }
    else{
        std::cout << "Failed to load texture" << std::endl;
    }
    LOG_INFO("Finish generating texture");
}


void modelRender::generateModel(float *vertices, unsigned int *faces, int Vn, int Fn){
    glGenVertexArrays(1, &VAO[1]);
    glGenBuffers(1, &VBO[1]);
    glGenBuffers(1, &EBO[1]);

    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    LOG_INFO("Model vertices glBufferData");
    glBufferData(GL_ARRAY_BUFFER, 6*4*Vn, vertices, GL_STATIC_DRAW);
    
    LOG_INFO("Model faces glBufferData");
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3*4*3*Fn, faces, GL_STATIC_DRAW);
    LOG_INFO("Model glBufferData finish");

    // configure vertex attribute: position and color
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}


void modelRender::glInit(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

    // glfw window creation
    window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
    }
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


void modelRender::readScreenPixel(){
    unsigned int readBuffer;
    glGenBuffers(1, &readBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, readBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * WIN_WIDTH * WIN_HEIGHT * 3, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_PIXEL_PACK_BUFFER, readBuffer);
    glReadPixels(0, 0, WIN_WIDTH, WIN_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    uchar *data = (uchar *)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
    cv::Mat img(WIN_HEIGHT, WIN_WIDTH, CV_8UC3);
    for(int i=0; i<WIN_HEIGHT; i++){
        for(int j=0; j<WIN_WIDTH; j++){
            int pixIdx = (WIN_HEIGHT - i - 1) * WIN_WIDTH + j;
            //std::printf("%d %d | %u %u %u\n", i, j, data[3*pixIdx + 0], data[3*pixIdx + 1], data[3*pixIdx + 2]);
            img.at<cv::Vec3b>(i, j)[0] = data[3*pixIdx + 0];
            img.at<cv::Vec3b>(i, j)[1] = data[3*pixIdx + 1];
            img.at<cv::Vec3b>(i, j)[2] = data[3*pixIdx + 2];
        }
    }
    cv::imwrite("img.png", img);
    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);

    //glReadPixels(0, 0, WIN_WIDTH, WIN_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, (uchar *)dataRead);
}


modelRender::modelRender(){
    WIN_WIDTH = 800;
    WIN_HEIGHT = 600;
    this->textureID = 0;
    cameraPos   = glm::vec3(0.0f, 0.0f, 20.0f);
    cameraTar = glm::vec3(0.0f, 0.0f, 0.0f);
    cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

    glInit();
    shaderTexture = new Shader("./include/opengl/texture.vs", "./include/opengl/texture.fs"); 
    shaderModel = new Shader("./include/opengl/model.vs", "./include/opengl/model.fs");
}


modelRender::~modelRender(){
    delete shaderModel;
    delete shaderTexture;
}
