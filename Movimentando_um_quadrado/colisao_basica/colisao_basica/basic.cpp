#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

// Vertex Shader Source Code
const char* vertexShaderSource = R"(
    #version 400 core
    layout(location = 0) in vec2 aPos;
    layout(location = 1) in vec3 aColor;

    out vec3 vertexColor;
    uniform vec2 offset;

    void main() {
        gl_Position = vec4(aPos + offset, 0.0, 1.0);
        vertexColor = aColor;
    }
)";

// Fragment Shader Source Code
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
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                << infoLog << "\n" << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                << infoLog << "\n" << std::endl;
        }
    }
}

// Estrutura para armazenar o tri�ngulo em movimento
struct Square {
    float x, y;        // Posi��o
    float speedX, speedY; // Velocidade
    float size = 0.2f; // Tamanho do tri�ngulo
};

// Fun��o para verificar colis�o entre dois quadrados
bool checkCollision(const Square& a, const Square& b) {
    return (fabs(a.x - b.x) * 2 < (a.size + b.size)) &&
        (fabs(a.y - b.y) * 2 < (a.size + b.size));
};

int main() {
    // Inicializa GLFW
    if (!glfwInit()) {
        std::cerr << "Erro ao inicializar GLFW" << std::endl;
        return -1;
    }

    // Configura��o da vers�o do OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Cria uma janela GLFW
    GLFWwindow* window = glfwCreateWindow(800, 600, "Tri�ngulo em Movimento", NULL, NULL);
    if (!window) {
        std::cerr << "Erro ao criar janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Inicializa GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Erro ao inicializar GLEW" << std::endl;
        return -1;
    }

    // Define a �rea de renderiza��o
    glViewport(0, 0, 800, 600);

    // Dados dos v�rtices: Posi��o (x, y) e Cor (r, g, b)
    GLfloat vertices[] = {
        // Posi��es     // Cores
        -0.1f, -0.1f,  1.0f, 0.0f, 0.0f,  // Inferior esquerdo (Vermelho)
         0.1f, -0.1f,  0.0f, 1.0f, 0.0f,  // Inferior direito (Verde)
        -0.1f,  0.1f,  0.0f, 0.0f, 1.0f,  // Superior (Azul)
		 0.1f,  0.1f,  0.5f, 0.4f, 1.0f   // Superior direito (Roxo)
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Compilar shaders
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

    // Posi��o inicial e velocidade do tri�ngulo
    Square square = { 0.5f, 0.5f, 0.005f, 0.003f };
    Square square2 = { -0.5f, 0.5f, 0.010f, 0.006f };

    int offsetLocation = glGetUniformLocation(shaderProgram, "offset");

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        // Atualiza a posi��o do quadrado
        square.x += square.speedX;
        square.y += square.speedY;

        square2.x += square2.speedX;
        square2.y += square2.speedY;

        // Verifica colis�o com as bordas da tela
        if (fabs(square.x) + square.size > 1.0f) square.speedX *= -1;
        if (fabs(square.y) + square.size > 1.0f) square.speedY *= -1;

        if (fabs(square2.x) + square2.size > 1.0f) square2.speedX *= -1;
        if (fabs(square2.y) + square2.size > 1.0f) square2.speedY *= -1;

        // Verifica colis�o entre os quadrados
        if (checkCollision(square, square2)) {
            // Inverte a dire��o dos quadrados em caso de colis�o
            if (fabs(square.x - square2.x) * 2 < (square.size + square2.size)) {
                square.speedX *= -1;
                square2.speedX *= -1;
            }
            if (fabs(square.y - square2.y) * 2 < (square.size + square2.size)) {
                square.speedY *= -1;
                square2.speedY *= -1;
            };
        };
        
        // Passa o deslocamento para o shader
        glUniform2f(offsetLocation, square.x, square.y);
        // Renderiza o quadrado
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// Precisa colocar o glUniform2f novamente para o segundo quadrado - sempre juntos
        glUniform2f(offsetLocation, square2.x, square2.y);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
