#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include <SOIL.h>
#include <string>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "PlatformUtils.h"
#include "Camera.h"

int ScreenWidth = 1280;
int ScreenHeight = 960;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

GLfloat lastX = ScreenWidth / 2.0;
GLfloat lastY = ScreenHeight / 2.0;

Camera camera = Camera();
bool keys[1024];

void DoCameraMovements()
{
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

bool firstMouse = true;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = ypos - lastY;
    lastX = xpos;
    lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}


int main(int argc, char **argv)
{
    
    if(!glfwInit())
    {
        std::cout << "glfw init failed!" << std::endl;
        return 0;
    }
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow* window = glfwCreateWindow(ScreenWidth, ScreenHeight, "Hello OpenGL", NULL, NULL);
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
    
    int width,height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    
    // Use shader class
	std::string vs_path;
	std::string ps_path;
	vs_path = "advance.vs";
	ps_path = "advance.ps";
    Shader outShader(vs_path.c_str(), ps_path.c_str());

	std::string outline_ps_path;
	outline_ps_path = "lamp.ps";
	Shader outlineShader(vs_path.c_str(), outline_ps_path.c_str());
    
    // create texures
    int tWidth, tHeight;

	const char* texture_file[2] = {
        "metal.png",
        "container_diffuse.png"
    };

    int texture_length = 2;
    
    GLuint *textures = new GLuint[texture_length];
    glGenTextures(texture_length, textures);

    for (int i = 0; i < texture_length; ++i) {
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        // set texture wrapping mode
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        // set texture filtering mode
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		std::string path;
		common::GetTexturePath(path, texture_file[i]);
        
        unsigned char *image = SOIL_load_image(path.c_str(), &tWidth, &tHeight, 0, SOIL_LOAD_RGB);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tWidth, tHeight, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, image);
        std::cout << tWidth << " " << tHeight << std::endl;
        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(image);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    GLfloat cubeVertices[] = {
        // Positions          // Texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    GLfloat planeVertices[] = {
        // Positions            // Texture Coords (note we set these higher than 1 that together with GL_REPEAT as texture wrapping mode will cause the floor texture to repeat)
        5.0f,  -0.5f,  5.0f,  -0.5f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, -0.5f,
        
        5.0f,  -0.5f,  5.0f,  -0.5f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, -0.5f,
        5.0f,  -0.5f, -5.0f,  -0.5f, -0.5f
    };


    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    
    // uv attribution in vs shader
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    
    // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

	// create floor VAO
	GLuint floorVAO, floorVBO;
	glGenVertexArrays(1, &floorVAO);
	glBindVertexArray(floorVAO);
    
    glGenBuffers(1, &floorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	std::cout << "111" << camera.Front.x << " " << camera.Front.y << " " << camera.Front.z << std::endl;


   
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ScreenWidth, ScreenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout<< "frame buffer incomplete!" << std::endl;
        return 0;
    }
    
    glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    //glDepthFunc(GL_ALWAYS);

    while (!glfwWindowShouldClose(window)) {
		GLfloat current = glfwGetTime();
		deltaTime = current - lastFrame;
		lastFrame = current;

        glfwPollEvents();
		DoCameraMovements();
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        
        // bind textures
        // glActiveTexture(GL_TEXTURE0);
        // We omit the glActiveTexture part since TEXTURE0 is already the default active texture unit. (sampler used in fragment is set to 0 as well as default)
        //glUniform1i(glGetUniformLocation(outShader.Program,"texture0"), 0);

        glm::mat4 view;
		glm::mat4 projection;

 		outlineShader.Use();
 		GLint modelLoc0 = glGetUniformLocation(outlineShader.Program, "model");
		GLint viewLoc0 = glGetUniformLocation(outlineShader.Program, "view");
		GLint projectLoc0 = glGetUniformLocation(outlineShader.Program, "projection");

		view = camera.GetViewMatrix();
		projection = camera.GetProjectionMatrix();
		glUniformMatrix4fv(viewLoc0, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectLoc0, 1, GL_FALSE, glm::value_ptr(projection));

       
		       
        outShader.Use();
        
		GLint modelLoc = glGetUniformLocation(outShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(outShader.Program, "view");
		GLint projectLoc = glGetUniformLocation(outShader.Program, "projection");
		view = camera.GetViewMatrix();
		projection = camera.GetProjectionMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectLoc, 1, GL_FALSE, glm::value_ptr(projection));


		/*
		first pass: just draw as normal, except update stencil buffer
		*/

		glm::mat4 model;

		// floor plane, do not update stencil buffer
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);
        glBindVertexArray(floorVAO);
        model = glm::mat4();
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
 
		
		glEnable(GL_DEPTH_TEST);
		// the stencil buffer init state is 0, so just pass all cubes and updates its pixel point in stencil buffer to 1
        glBindTexture(GL_TEXTURE_2D, textures[1]);

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
        // cube1
		glBindVertexArray(VAO);
		model = glm::mat4();
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
        
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		//glDisable(GL_DEPTH_TEST);
		outlineShader.Use();

        // cube1 scale
		glBindVertexArray(VAO);
		GLfloat scale = 1.1f;
		model = glm::mat4();
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
		model = glm::scale(model, glm::vec3(scale, scale, scale));
		glUniformMatrix4fv(modelLoc0, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
        
		glStencilMask(0xFF);
		//glEnable(GL_DEPTH_TEST);

		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glClear(GL_STENCIL_BUFFER_BIT);
        
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
        // cube2
		outShader.Use();
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(-0.2f, 0.0f, 0.2f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		
		// second pass, draw larger cube only stencil buffer point is not 1, and use a different shader
		
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		//glDisable(GL_DEPTH_TEST);

		outlineShader.Use();

        // cube2 scale
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(-0.2f, 0.0f, 0.2f));
		model = glm::scale(model, glm::vec3(scale, scale, scale));
        glUniformMatrix4fv(modelLoc0, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
       
		// must reset mask to 1s
		glStencilMask(0xFF);
		//glEnable(GL_DEPTH_TEST);
        
        glfwSwapBuffers(window);
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    
    delete [] textures;
    
    glfwTerminate();
	return 0;
}

