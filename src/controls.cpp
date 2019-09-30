// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}

// Initial position : on +Z
glm::vec3 position = glm::vec3( 0, 0, 60 ); 
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float alpha = 3.14f * 0.5f; // theta
float beta = 0; // fi
float r = 60;

bool handleKeyboard(GLFWwindow* window, SpriteGenerator* sprites, bool u) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE ) == GLFW_PRESS) return true;
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		if (u) {
			sprites->viewer_vertical_pos += 1;
			sprites->recalculatePositions();
		}
		else alpha += 3.14f * 0.05f;
		return false;
	}
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		if (u) {
			sprites->viewer_horizontal_pos -= 1;
			sprites->recalculatePositions();
		}
		else beta -= 3.14f * 0.05f;
		return false;
	}
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		if (u) {
			sprites->viewer_vertical_pos -= 1;
			sprites->recalculatePositions();
		}
		else alpha -= 3.14f * 0.05f;
		return false;
	}
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		if (u) {
			sprites->viewer_horizontal_pos += 1;
			sprites->recalculatePositions();
		}
		else beta += 3.14f * 0.05f;
		return false;
	}
	if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		r -= 1.0f;
		return false;
	}
	if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		r += 1.0f;
		return false;
	}
	if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		sprites->removeSelected();
		return false;
	}
	if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		sprites->cancelSelection();
		return false;
	}
	return false;
}

void computeMatrices() {
	position = glm::vec3(
		r * sin(alpha) * sin(beta),
		r * cos(alpha),
		r * sin(alpha) * cos(beta)
	);

	float FoV = initialFoV;

	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
	ViewMatrix       = glm::lookAt(
								position,
								glm::vec3(0,0,0),
								glm::vec3(0,1,0)
						   );
}