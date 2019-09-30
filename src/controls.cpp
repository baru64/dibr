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

float alpha = 0; // theta
float beta = 0; // fi
float r = 60;

bool handleKeyboard(GLFWwindow* window, SpriteGenerator* sprites) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE ) == GLFW_PRESS) return true;
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		beta += 3.14f * 0.05f;
		return false;
	}
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		alpha -= 3.14f * 0.05f;
		return false;
	}
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		beta -= 3.14f * 0.05f;
		return false;
	}
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		alpha += 3.14f * 0.05f;
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
		r * sin(alpha) * cos(beta),
		r * sin(alpha) * sin(beta),
		r * cos(alpha)
	);

	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle), 
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f), 
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);
	glm::vec3 up = glm::cross( right, direction );
	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
	ViewMatrix       = glm::lookAt(
								position,
								glm::vec3(0,0,0),
								glm::vec3(0,1,0)
						   );
}