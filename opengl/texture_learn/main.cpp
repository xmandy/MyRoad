#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include <SOIL.h>


void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main(int argc, char **argv)
{
    
    if(!glfwInit())
    {
        std::cout << "glfw init failed!" << std::endl;
        return 0;
    }
#ifdef MAC_OS_X
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//    char texture_path1[512] = "/Users/yangxue/workspace/MyRoad/opengl/textures/container.jpg";
//    char texture_path2[512] = "/Users/yangxue/workspace/MyRoad/opengl/textures/awesomeface.png";
    char* texture_path[2] = {
        "/Users/yangxue/workspace/MyRoad/opengl/textures/container.jpg",
        "/Users/yangxue/workspace/MyRoad/opengl/textures/awesomeface.png"
    };
#endif
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello OpenGL", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout<<"failed to init glew"<<std::endl;
        return -1;
    }
    
    std::cout << "OpenGL Vendor:" << glGetString(GL_VENDOR) << std::endl;
    std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version:" << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    
    glfwSetKeyCallback(window, key_callback);
    
    int width,height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    
    // Use shader class
    Shader outShader("/Users/yangxue/workspace/MyRoad/opengl/shaders/texture_learn.vs",
                     "/Users/yangxue/workspace/MyRoad/opengl/shaders/texture_learn.ps");
    
    // create texures
    int tWidth, tHeight;
    
    int texture_length = 2;
    
    GLuint *textures = new GLuint(texture_length);
    glGenTextures(texture_length, textures);
//    glBindTexture(GL_TEXTURE_2D, textures[0]);
//    // set texture wrapping mode
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    
//    // set texture filtering mode
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    
//    unsigned char *image = SOIL_load_image("/Users/yangxue/workspace/MyRoad/opengl/textures/container.jpg", &tWidth, &tHeight, 0, SOIL_LOAD_RGB);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tWidth, tHeight, 0, GL_RGB,
//                 GL_UNSIGNED_BYTE, image);
//    glGenerateMipmap(GL_TEXTURE_2D);
//    SOIL_free_image_data(image);
//    glBindTexture(GL_TEXTURE_2D, 0);
//    
    for (int i = 0; i < texture_length; ++i) {
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        // set texture wrapping mode
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        // set texture filtering mode
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        unsigned char *image = SOIL_load_image(texture_path[i], &tWidth, &tHeight, 0, SOIL_LOAD_RGB);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tWidth, tHeight, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(image);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
   
    GLfloat vertices[] = {
        // Positions            // colors               // uvs
        0.5f, 0.5f, 0.0f,       1.0f, 0.0f, 0.0f,       1.0f, 1.0f,
        0.5f, -0.5f, 0.0f,      0.0f, 1.0f, 0.0f,       1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, 1.0f,       0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,      0.0f, 0.0f, 0.0f,       0.0f, 1.0f,
    };
    GLuint indices[] = {
        0, 1, 3,
        1, 2, 3,
    };
    
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    // bind the array first and then bind other buffer
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // position attribution in vs shader
    // stride = 0 let opengl decide the stride, but usually we carefully set it by ourself
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    
    // color attribution in vs shader
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    
    // uv attribution in vs shader
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6*sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    
    
    // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindVertexArray(0);
    // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

   
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // bind textures
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glUniform1i(glGetUniformLocation(outShader.Program,"texture0"), 0);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        glUniform1i(glGetUniformLocation(outShader.Program,"texture1"), 1);
        
        outShader.Use();
        
        glBindVertexArray(VAO);
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        
        glfwSwapBuffers(window);
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    
    delete [] textures;
    
    glfwTerminate();
	return 0;
}

