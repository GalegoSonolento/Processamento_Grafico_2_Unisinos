// OpenGL 4 com GLFW, GLEW, VAO/VBO, Shaders e GLM

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <windows.h>
#include "Bmp.h"

#define SCREEN_X 800
#define SCREEN_Y 600

GLuint textureID;
GLuint vao, vbo, ebo;
GLuint shaderProgram;

Bmp* img1;
unsigned char* data;

const char* vertexShaderSource = R"(
#version 400 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
uniform mat4 mvp;
out vec2 TexCoord;
void main() {
    TexCoord = aTexCoord;
    gl_Position = mvp * vec4(aPos, 1.0);
})";

const char* fragmentShaderSource = R"(
#version 400 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D ourTexture;
void main() {
    FragColor = texture(ourTexture, TexCoord);
})";

void buildTexture()
{
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, img1->getWidth(), img1->getHeight(), GL_RGB, GL_UNSIGNED_BYTE, data);
}

GLuint compileShader(const char* src, GLenum type)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    return shader;
}

void setupShaders()
{
    GLuint vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void setupBuffers()
{
    float vertices[] = {
        // positions          // texture coords
        // Face traseira
        -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,  0.0f, 1.0f,

        // Face frontal
        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f, 1.0f,

        // Face esquerda
        -1.0f,  1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,  0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,  1.0f, 1.0f,

        // Face direita
         1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
         1.0f,  1.0f, -1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f,  0.0f, 1.0f,

         // Face inferior
         -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
          1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
          1.0f, -1.0f,  1.0f,  1.0f, 1.0f,
         -1.0f, -1.0f,  1.0f,  0.0f, 1.0f,

         // Face superior
         -1.0f,  1.0f, -1.0f,  0.0f, 0.0f,
          1.0f,  1.0f, -1.0f,  1.0f, 0.0f,
          1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
         -1.0f,  1.0f,  1.0f,  0.0f, 1.0f,
    };

    GLuint indices[] = {
        0, 1, 2, 2, 3, 0,   // Face traseira
        4, 5, 6, 6, 7, 4,   // Face frontal
        8, 9, 10, 10, 11, 8, // Face esquerda
        12, 13, 14, 14, 15, 12, // Face direita
        16, 17, 18, 18, 19, 16, // Face inferior
        20, 21, 22, 22, 23, 20  // Face superior
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void display(GLFWwindow* window)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Projeção com perspectiva corrigida
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_X / SCREEN_Y, 0.1f, 100.0f);

    // Move a "câmera" para trás
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5));

    // Rotação do cubo
    glm::mat4 model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 mvp = projection * view * model;

    glUseProgram(shaderProgram);
    GLuint mvpLoc = glGetUniformLocation(shaderProgram, "mvp");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

    glBindTexture(GL_TEXTURE_2D, textureID);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

int main()
{
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_X, SCREEN_Y, "Texture Demo", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    glewExperimental = true;
    if (glewInit() != GLEW_OK) return -1;

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 800, 600);

    img1 = new Bmp("./images/normal_1.bmp");
    img1->convertBGRtoRGB();
    data = img1->getImage();

    if (data)
    {
        buildTexture();
        setupShaders();
        setupBuffers();
    }

    while (!glfwWindowShouldClose(window)){
        display(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
