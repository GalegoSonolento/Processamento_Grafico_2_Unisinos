#include "Terrain.h"
#include "Bmp.h"
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Terrain::Terrain(const std::string& bmpPath, GLuint shader)
    : shaderProgram(shader), lodLevel(1)
{
    heightmap = new Bmp(bmpPath.c_str());
    imageData = heightmap->getImage();
    width = heightmap->getWidth();
    height = heightmap->getHeight();
}

Terrain::~Terrain() {
    delete heightmap;
}

/*
void Terrain::setup(const glm::vec3& cameraPosition) {
    int blocksize = 32; // Tamanho de cada bloco (em pixels)
    for (int y = 0; y < height; y += blocksize) {
        for (int x = 0; x < width; x += blocksize) {
            Block block;
            block.lodLevel = 1; // LOD inicial
            generateBlockMesh(block, block.lodLevel, x, y, blocksize, blocksize);
            blocks.push_back(block);
        }
    }
    calculateBlockCenter();
}
*/

void Terrain::generateBlockMesh(Block& block, int lodLevel, int startX, int startY, int blockWidth, int blockHeight) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    unsigned char maxVal = 0;
    for (int i = 0; i < width * height * 3; i += 3) {
        if (imageData[i] > maxVal) maxVal = imageData[i];
    }
    float maxHeight = std::max(1.0f, static_cast<float>(maxVal));

    for (int y = startY; y <= startY + blockHeight; y += lodLevel) {
        for (int x = startX; x <= startX + blockWidth; x += lodLevel) {
            int index = (y * width + x) * 3;
            float intensity = imageData[index] / maxHeight;
            vertices.push_back(static_cast<float>(x));
            vertices.push_back(intensity * 20.0f);
            vertices.push_back(static_cast<float>(y));
            vertices.push_back((float)x / width);
            vertices.push_back((float)y / height);
        }
    }

    int w = (blockWidth / lodLevel) + 1;
    for (int y = 0; y < (blockHeight / lodLevel); ++y) {
        for (int x = 0; x < (blockWidth / lodLevel); ++x) {
            int i = y * w + x;
            indices.push_back(i);
            indices.push_back(i + 1);
            indices.push_back(i + w);
            indices.push_back(i + 1);
            indices.push_back(i + w + 1);
            indices.push_back(i + w);
        }
    }

    glGenVertexArrays(1, &block.vao);
    glGenBuffers(1, &block.vbo);
    glGenBuffers(1, &block.ebo);

    glBindVertexArray(block.vao);

    glBindBuffer(GL_ARRAY_BUFFER, block.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, block.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    block.indexCount = static_cast<int>(indices.size());
}



void Terrain::render(const glm::mat4& mvp, const glm::vec3& cameraPosition) {
    // Atualizar o LOD com base na posição da câmera
    //updateLOD(cameraPosition);

    // Configurar o shader e enviar a matriz MVP
    glUseProgram(shaderProgram);
    GLuint mvpLoc = glGetUniformLocation(shaderProgram, "mvp");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

    int blocksize = 32; // Tamanho de cada bloco (em pixels)
    for (int y = 0; y < height; y += blocksize) {
        for (int x = 0; x < width; x += blocksize) {
            Block block;

            // Calcular o LOD com base na distância da câmera
            float distance = glm::distance(cameraPosition, glm::vec3(x + blocksize / 2, 0, y + blocksize / 2));
            if (distance < 100.0f) {
                block.lodLevel = 1; // Alta resolução
            }
            else if (distance < 175.0f) {
                block.lodLevel = 2; // Média resolução
            }
            else {
                block.lodLevel = 4; // Baixa resolução
            }

            // Gerar a malha do bloco
            generateBlockMesh(block, block.lodLevel, x, y, blocksize, blocksize);
            blocks.push_back(block);
        }
    }
    calculateBlockCenter();

    for (auto& block : blocks) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glBindVertexArray(block.vao);
        glDrawElements(GL_TRIANGLES, block.indexCount, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

/*
void Terrain::updateLOD(const glm::vec3& cameraPosition) {
    // Centro do terreno
    glm::vec3 terrainCenter(width / 2.0f, 0, height / 2.0f);

    // Distância da câmera ao centro do terreno
    float distance = glm::distance(cameraPosition, terrainCenter);

    // Ajustar o nível de detalhe com base na distância
    if (distance < 200.0f) {
        lodLevel = 1; // Alta resolução
    }
    else if (distance < 300.0f) {
        lodLevel = 2; // Média resolução
    }
    else {
        lodLevel = 4; // Baixa resolução
    }

    // Regenerar a malha se o LOD mudar
    generateMesh();
}
*/

void Terrain::calculateBlockCenter() {
    for (auto& block : blocks) {
        block.center = glm::vec3(block.vao / 2.0f, 0.0f, block.ebo / 2.0f);
    }
}