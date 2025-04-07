#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const char* vertexShaderSource = R"(
    #version 400 core
    layout(location = 0) in vec2 aPos;
    layout(location = 1) in vec3 aColor;

    out vec3 vertexColor;
    uniform mat4 transform;

    void main() {
        gl_Position = transform * vec4(aPos, 0.0, 1.0);
        vertexColor = aColor;
    }
)";

const char* fragmentShaderSource = R"(
    #version 400 core
    in vec3 vertexColor;
    out vec4 FragColor;

    void main() {
        FragColor = vec4(vertexColor, 1.0);
    }
)";

void checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n" << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n" << std::endl;
        }
    }
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Erro ao inicializar GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Relógio", NULL, NULL);
    if (!window) {
        std::cerr << "Erro ao criar janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Erro ao inicializar GLEW" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 600);

    // Círculo do relógio
    std::vector<GLfloat> circleVertices;
    const int segments = 100;
    float radius = 0.8f;
    for (int i = 0; i < segments; ++i) {
        float angle = 2.0f * M_PI * i / segments;
        float x = cos(angle) * radius;
        float y = sin(angle) * radius;
        circleVertices.push_back(x);
        circleVertices.push_back(y);
        circleVertices.push_back(1.0f); // cor branca
        circleVertices.push_back(1.0f);
        circleVertices.push_back(1.0f);
    }

    // Ponteiro (linha vertical)
    GLfloat handVertices[] = {
        0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.6f, 1.0f, 0.0f, 0.0f
    };

    GLuint VAOs[2], VBOs[2];
    glGenVertexArrays(2, VAOs);
    glGenBuffers(2, VBOs);

    // Círculo
    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, circleVertices.size() * sizeof(GLfloat), &circleVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Ponteiro
    glBindVertexArray(VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(handVertices), handVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    checkCompileErrors(vertexShader, "VERTEX");

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkCompileErrors(fragmentShader, "FRAGMENT");

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkCompileErrors(shaderProgram, "PROGRAM");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    int transformLoc = glGetUniformLocation(shaderProgram, "transform");

    float hourAngle = 0.0f;
    float minuteAngle = 0.0f;
    float secondAngle = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Círculo
        glBindVertexArray(VAOs[0]);
        glm::mat4 identity = glm::mat4(1.0f);
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(identity));
        glDrawArrays(GL_LINE_LOOP, 0, segments);

		// Atualiza o ângulo de rotação dos ponteiros
		hourAngle += 0.001f;
        minuteAngle += 0.01f;
        secondAngle += 0.1f;

        // Ponteiro das horas
        glBindVertexArray(VAOs[1]);
        glm::mat4 hourTransform = glm::rotate(identity, hourAngle, glm::vec3(0, 0, 1));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(hourTransform));
        glDrawArrays(GL_LINES, 0, 2);

        // Ponteiro dos minutos
        glm::mat4 minTransform = glm::rotate(identity, minuteAngle, glm::vec3(0, 0, 1));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(minTransform));
        glDrawArrays(GL_LINES, 0, 2);

        // Ponteiro dos segundos
        glm::mat4 secTransform = glm::rotate(identity, secondAngle, glm::vec3(0, 0, 1));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(secTransform));
        glDrawArrays(GL_LINES, 0, 2);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
