#include <vector>
#include <cstdio>

#include <GL/glew.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "shader.hpp"
#include "overlay.hpp"

unsigned int Overlay2DVertexBufferID;
unsigned int Overlay2DShaderID;

void initOverlay2D() {
    glGenBuffers(1, &Overlay2DVertexBufferID);
    Overlay2DShaderID = LoadShaders("overlay.vertexshader",
                                    "overlay.fragmentshader");
}

void drawRectangle2D(int x, int y, int width, int height) {
    printf("Got: %d %d %d %d\n", x,y,width,height);
    std::vector<glm::vec2> vertices;
    glm::vec2 vertex_up_left = glm::vec2(x, y);
    glm::vec2 vertex_up_right = glm::vec2(x+width, y);
    glm::vec2 vertex_down_left = glm::vec2(x, y+height);
    glm::vec2 vertex_down_right = glm::vec2(x+width, y+height);

    vertices.push_back(vertex_up_left);
    printf("%f %f\n", vertex_down_right.x, vertex_down_right.y);
    vertices.push_back(vertex_down_left);
    vertices.push_back(vertex_up_right);

    vertices.push_back(vertex_down_right);
    vertices.push_back(vertex_up_right);
    vertices.push_back(vertex_down_left);

    glBindBuffer(GL_ARRAY_BUFFER, Overlay2DVertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2),
                 &vertices[0], GL_STATIC_DRAW);

    glUseProgram(Overlay2DShaderID);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, Overlay2DVertexBufferID);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(0);
}

void cleanupOverlay2D() {
    glDeleteBuffers(1, &Overlay2DVertexBufferID);
    glDeleteProgram(Overlay2DShaderID);
}