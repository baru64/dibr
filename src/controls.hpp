#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include "sprites.hpp"

void computeMatrices();
void computeMatricesFromInputs();
bool handleKeyboard(GLFWwindow* window, SpriteGenerator* sprites, bool u);
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();

#endif