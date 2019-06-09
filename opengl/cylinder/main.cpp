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
#include <vector>

int ScreenWidth = 640;
int ScreenHeight = 480;

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

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	Vertex(GLfloat PosX, GLfloat PosY, GLfloat PosZ,
		GLfloat NormalX, GLfloat NormalY, GLfloat NormalZ,
		GLfloat TexX, GLfloat TexY)
	{
		Position.x = PosX;
		Position.y = PosY;
		Position.z = PosZ;
		Normal.x = NormalX;
		Normal.y = NormalY;
		Normal.z = NormalZ;
		TexCoords.x = TexX;
		TexCoords.y = TexY;
	}

	friend std::ostream& operator<<(std::ostream &os, Vertex &v)
	{
		return os << "(" << v.Position.x << ", " << v.Position.y << ", " << v.Position.z << ")" << std::endl;
	}
};

int main(int argc, char **argv)
{
    
    if(!glfwInit())
    {
        std::cout << "glfw init failed!" << std::endl;
        return 0;
    }
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    const char* texture_file[2] = {
        "container.jpg",
        "awesomeface.png"
    };
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
	common::GetShaderPath(vs_path, "3d.vs");
	common::GetShaderPath(ps_path, "texture_learn.ps");
    Shader outShader(vs_path.c_str(), ps_path.c_str());
    
    // create texures
    int tWidth, tHeight;
    
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
        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(image);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
   
//    GLfloat vertices[] = {
//        // Positions            // colors               // uvs
//        0.5f, 0.5f, 0.0f,       1.0f, 0.0f, 0.0f,       1.0f, 1.0f,
//        0.5f, -0.5f, 0.0f,      0.0f, 1.0f, 0.0f,       1.0f, 0.0f,
//        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, 1.0f,       0.0f, 0.0f,
//        -0.5f, 0.5f, 0.0f,      0.0f, 0.0f, 0.0f,       0.0f, 1.0f,
//    };
//    GLuint indices[] = {
//        0, 1, 3,
//        1, 2, 3,
//    };
    GLfloat vertices[] = {
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

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, 5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f, 3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f, 2.0f, -2.5f),
		glm::vec3(1.5f, 0.2f, -1.5f),
		glm::vec3(-1.3f, 1.0f, -1.5f)
	};

    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
//    glGenBuffers(1, &EBO);
    
    // bind the array first and then bind other buffer
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // position attribution in vs shader
    // stride = 0 let opengl decide the stride, but usually we carefully set it by ourself
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    
    // color attribution in vs shader
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
//    glEnableVertexAttribArray(1);
    
    // uv attribution in vs shader
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    
    
    // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindVertexArray(0);

	std::cout << "111" << camera.Front.x << " " << camera.Front.y << " " << camera.Front.z << std::endl;

	/******set up cylinder vertices******/
	std::vector<Vertex> TopVertices;
	std::vector<Vertex> BottomVertices;
	std::vector<Vertex> SideVertices;

	const float PI = 3.1415926535897932384626433832795;
	float HalfHight = 0.5f;

	Vertex TopCenter = Vertex(0.0f, HalfHight, 0.0f, 0.0f, 0.0f, 1.0f, 0.5, 0.5);
	Vertex BottomCenter = Vertex(0.0f, -HalfHight, 0.0f, 0.0f, 0.0f, 1.0f, 0.5, 0.5);
	TopVertices.push_back(TopCenter);
	BottomVertices.push_back(BottomCenter);


	GLint SideCount = 36;

	for (int i = 0; i <= SideCount; ++i)
	{
		// gen top mesh
		GLfloat angle = i * PI * 2 / SideCount;
		Vertex vertex(cos(angle), HalfHight, sin(angle), 0.0f, 1.0f, 0.0f,
			(cos(angle) + 1) / 2.0, (sin(angle) / 2.0));
		TopVertices.push_back(vertex);

		// gen middle rectangle
		Vertex vertexDown(cos(angle), -HalfHight, sin(angle), cos(angle), 0, sin(angle),
			i / (float)SideCount, 0.0f);
		Vertex vertexUp(cos(angle), HalfHight, sin(angle), cos(angle), 0, sin(angle),
			i / (float)SideCount, 1.0f);

		std::cout << vertexDown << vertexUp;
		SideVertices.push_back(vertexDown);
		SideVertices.push_back(vertexUp);

		// gen bottom mesh
		angle = -angle;
		Vertex bvertex(cos(angle), -HalfHight, sin(angle), 0.0f, 0.0f, 1.0f,
			(cos(angle) + 1) / 2.0, (sin(angle) / 2.0));
		BottomVertices.push_back(bvertex);

	}

 
	//SideVertices.push_back(Vertex(0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
	//SideVertices.push_back(Vertex(0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
	//SideVertices.push_back(Vertex(-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));

	//SideVertices.push_back(Vertex(0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
	//SideVertices.push_back(Vertex(-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
	//SideVertices.push_back(Vertex(-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));

	std::cout << TopVertices.size() << " " << SideVertices.size() << " " << BottomVertices.size() << std::endl;

	/******end set up cylinder vertices******/

	GLuint TopVAO, BottomVAO, SideVAO;
	GLuint TopVBO, BottomVBO, SideVBO;
	glGenVertexArrays(1, &TopVAO);
	glGenVertexArrays(1, &BottomVAO);
	glGenVertexArrays(1, &SideVAO);

	glGenBuffers(1, &TopVBO);
	glGenBuffers(1, &BottomVBO);
	glGenBuffers(1, &SideVBO);

	// Top
	glBindVertexArray(TopVAO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, TopVBO);
	glBufferData(GL_ARRAY_BUFFER, TopVertices.size() * sizeof(Vertex), &TopVertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	glBindVertexArray(0);


	glBindVertexArray(BottomVAO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, BottomVBO);
	glBufferData(GL_ARRAY_BUFFER, BottomVertices.size() * sizeof(Vertex), &BottomVertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	glBindVertexArray(0);


	glBindVertexArray(SideVAO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, SideVBO);
	glBufferData(GL_ARRAY_BUFFER, SideVertices.size() * sizeof(Vertex), &SideVertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	glBindVertexArray(0);

	std::string cyvs_path = common::GetShaderPath("cylinder.vs");
	std::string cyps_path = common::GetShaderPath("cylinder.ps");

	Shader cylinderShader(cyvs_path.c_str(), cyps_path.c_str());
	
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
   
    while (!glfwWindowShouldClose(window)) {
		GLfloat current = glfwGetTime();
		deltaTime = current - lastFrame;
		lastFrame = current;

        glfwPollEvents();
		DoCameraMovements();
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cylinderShader.Use();

		GLint modelLoc = glGetUniformLocation(cylinderShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(cylinderShader.Program, "view");
		GLint projectLoc = glGetUniformLocation(cylinderShader.Program, "projection");
        glm::mat4 view;
		view = camera.GetViewMatrix();
		glm::mat4 projection = camera.GetProjectionMatrix();

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectLoc, 1, GL_FALSE, glm::value_ptr(projection));

		
		glm::mat4 model;
		//model = glm::rotate(model, (GLfloat)glfwGetTime()*0.5f, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(TopVAO);
		glDrawArrays(GL_TRIANGLE_FAN, 0, TopVertices.size());
		glBindVertexArray(0);
	
		//glBindVertexArray(SideVAO);
		//glDrawArrays(GL_TRIANGLE_STRIP, 0, SideVertices.size());
		//glBindVertexArray(0);

		glBindVertexArray(BottomVAO);
		glDrawArrays(GL_TRIANGLE_FAN, 0, BottomVertices.size());
		glBindVertexArray(0);

        // bind textures
		/*
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glUniform1i(glGetUniformLocation(outShader.Program,"texture0"), 0);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        glUniform1i(glGetUniformLocation(outShader.Program,"texture1"), 1);
        
        outShader.Use();
        
		GLint modelLoc = glGetUniformLocation(outShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(outShader.Program, "view");
		GLint projectLoc = glGetUniformLocation(outShader.Program, "projection");
        glm::mat4 view;
		view = camera.GetViewMatrix();
		glm::mat4 projection = camera.GetProjectionMatrix();

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectLoc, 1, GL_FALSE, glm::value_ptr(projection));
		*/
	/*
		for (int i = 0; i < sizeof(cubePositions) / sizeof(cubePositions[0]); ++i)
		{ 
			//std::cout << "this length" << sizeof(cubePositions) / sizeof(cubePositions[0]) << std::endl;
			glm::mat4 model;
			model = glm::translate(model, cubePositions[i]);
			model = glm::rotate(model, (GLfloat)glfwGetTime()*0.5f, glm::vec3(0.5f, 1.0f, 1.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glBindVertexArray(VAO);
			
	//        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			}
        glBindVertexArray(0);
		*/
		
        
        
        glfwSwapBuffers(window);
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
//    glDeleteBuffers(1, &EBO);
    
    delete [] textures;
    
    glfwTerminate();
	return 0;
}
