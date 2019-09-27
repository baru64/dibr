#ifndef CONTROLS_HPP
#define CONTROLS_HPP

void computeMatricesFromInputs(GLFWwindow* window, int width, int height);
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();

#endif