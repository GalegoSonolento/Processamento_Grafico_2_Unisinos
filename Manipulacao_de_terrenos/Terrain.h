#ifndef TERRAIN_H
#define TERRAIN_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

class Bmp;

class Terrain {
public:
    Terrain(const std::string& bmpPath, GLuint shaderProgram);
    ~Terrain();

    void setup(const glm::vec3& cameraPosition);
    void render(const glm::mat4& mvp, const glm::vec3& cameraPosition);
    // void updateLOD(const glm::vec3& cameraPosition);

private:
    struct Block {
        GLuint vao, vbo, ebo;
        int lodLevel;
        glm::vec3 center;
        int indexCount;
    };

    std::vector<Block> blocks;
    GLuint shaderProgram;
    class Bmp* heightmap;
    unsigned char* imageData;
    int width, height;

    int lodLevel;

    void generateBlockMesh(Block& block, int lodLevel, int startX, int startY, int blockWidth, int blockHeight);
    void calculateBlockCenter();
};

#endif
