#ifndef CONTROLS_HPP
#define CONTROLS_HPP

void computeMatrices();
void computeMatricesFromInputs();
bool handleKeyboard(GLFWwindow* window);
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();

#endif