#include <cstdio>
#include <cstdlib>

#include <vector>
#include <algorithm>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
using namespace glm;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shader.hpp"
#include "controls.hpp"
#include "sprites.hpp"
#include "overlay.hpp"

struct DIBRContext {
	double last_mouse_x;
	double last_mouse_y;
	bool is_lmb_pressed;
	SpriteGenerator* sprites;
	GLfloat* depth_store;
	int depth_store_size;
};

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	DIBRContext* context =
			reinterpret_cast<DIBRContext *>(glfwGetWindowUserPointer(window));

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		glfwGetCursorPos(window, &(context->last_mouse_x), &(context->last_mouse_y));
        printf("LMB pressed. last xy: %f %f \n", context->last_mouse_x,
				context->last_mouse_y);
		context->is_lmb_pressed = true;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        printf("LMB released.\n");
		double mouse_x, mouse_y;
		glfwGetCursorPos(window, &mouse_x, &mouse_y);

		GLfloat depth;
		unsigned width, height;
		if (mouse_x > context->last_mouse_x) {
			width = mouse_x - context->last_mouse_x + 2;
			mouse_x = context->last_mouse_x; // to make correct selection area
		} else {
			width = context->last_mouse_x - mouse_x + 2;
		}
		if (mouse_y > context->last_mouse_y) {
			height = mouse_y - context->last_mouse_y + 2;
			mouse_y = context->last_mouse_y; // to make correct selection area
		} else {
			height = context->last_mouse_y - mouse_y + 2;
		}

		glReadnPixels(context->last_mouse_x, 768 - context->last_mouse_y - 1,
					  width, height,
					  GL_DEPTH_COMPONENT, GL_FLOAT,
					  context->depth_store_size, context->depth_store);

		glm::vec4 viewport = glm::vec4(0, 0, 1024, 768);
		glm::mat4 view_matrix = getViewMatrix();
		glm::mat4 projection_matrix = getProjectionMatrix();
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
  				glm::vec3 win_pos = glm::vec3(
					mouse_x+j,
					768 - (mouse_y+i) - 1,
				  	context->depth_store[i*width + j]
				);

  				glm::vec3 obj_pos = glm::unProject(
					win_pos,
					view_matrix,
					projection_matrix, 
					viewport
				);

				context->sprites->select(obj_pos);
			}
		}

		context->is_lmb_pressed = false;
	}

}

int main( int argc, char** argv )
{
	// Parse arguments
	float depth_scale = 0.1f;
	float background_filter = 0.0f;
	char src_image[128] = "sample_img.jpg";
	char src_depth[128] = "sample_depth.jpg";
	bool unproject = true;
	int colorkey[3];
	bool use_colorkey = false;
	if (argc >= 2) {
		if (strncmp(argv[1],"u",1) == 0) unproject = true;
		if (strncmp(argv[1],"n",1) == 0) unproject = false;
		if (strncmp(argv[1],"h",1) == 0) {
			printf(
				"usage: %s [u|n] [DEPTH_SCALE] [BG_FILTER] [IMAGE] [DEPTH_MAP] [KEY_COLOR_RGB]\n",
				argv[0]
			);
			return 0;
		}
	}
	if (argc >= 3) depth_scale = atof(argv[2]);
	if (argc >= 4) background_filter = atof(argv[3]);
	if (argc >= 6) {
		strcpy(src_image, argv[4]);
		strcpy(src_depth, argv[5]);
	}
	if (argc == 9) {
		colorkey[0] = atoi(argv[6]);
		colorkey[1] = atoi(argv[7]);
		colorkey[2] = atoi(argv[8]);
		use_colorkey = true;
	}

	// Print controls
	printf(	"<> CONTROLS:\n\t" \
			"WASD\t - camera movement\n\t" \
			"QE\t - zoom\n\t" \
			"X/C\t - delete selected / cancel selection \n\n");

	// Load images and create sprite generator
	int rgb_width, rgb_height, d_width, d_height, bpp;
	unsigned char* rgb_image = stbi_load(src_image, &rgb_width, &rgb_height, &bpp, 3);
	unsigned char* depth_image = stbi_load(src_depth, &d_width, &d_height, &bpp, 1);

	if (rgb_width != d_width || rgb_height != d_height) {
		printf("ERROR: Image and depth map have different size!\n");
		return 1;
	}

	// create SpriteGenerator
	SpriteGenerator sprites(rgb_image, depth_image,
							rgb_width, rgb_height,
							depth_scale, background_filter,
							unproject, colorkey, use_colorkey);
	printf("Number of sprites: %d\n", sprites.sprite_count);

	// Set DIBR context
	DIBRContext context;
	context.last_mouse_x = 0.0f;
	context.last_mouse_y = 0.0f;
	context.is_lmb_pressed = false;
	context.sprites = &sprites;
	context.depth_store = new GLfloat[rgb_width*rgb_height];
	context.depth_store_size = rgb_width*rgb_height;

	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	GLFWwindow* window = glfwCreateWindow( 1024, 768, "DIBR", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

	// Dark magenta background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	
	
	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "sprites.vertexshader", "sprites.fragmentshader" );

	// Vertex shader
	GLuint CameraRight_worldspace_ID  = glGetUniformLocation(programID, "CameraRight_worldspace");
	GLuint CameraUp_worldspace_ID  = glGetUniformLocation(programID, "CameraUp_worldspace");
	GLuint ViewProjMatrixID = glGetUniformLocation(programID, "VP");

	// Set window pointer to sprites
	glfwSetWindowUserPointer(window, reinterpret_cast<void *>(&context));

	// Buffers for rendering
	static GLfloat* g_sprite_position_size_data = new GLfloat[sprites.sprite_count * 4];
	static GLubyte* g_sprite_color_data         = new GLubyte[sprites.sprite_count * 4];

	// The VBO containing the 4 vertices of the sprites.
	static const GLfloat g_vertex_buffer_data[] = { 
		 -0.5f, -0.5f, 0.0f,
		  0.5f, -0.5f, 0.0f,
		 -0.5f,  0.5f, 0.0f,
		  0.5f,  0.5f, 0.0f,
	};
	GLuint billboard_vertex_buffer;
	glGenBuffers(1, &billboard_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_DYNAMIC_DRAW);

	// The VBO containing the positions and sizes of the sprites
	GLuint sprites_position_buffer;
	glGenBuffers(1, &sprites_position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, sprites_position_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, sprites.sprite_count * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	// The VBO containing the colors of the sprites
	GLuint sprites_color_buffer;
	glGenBuffers(1, &sprites_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, sprites_color_buffer);
	glBufferData(GL_ARRAY_BUFFER, sprites.sprite_count * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

	// set mouse calback
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// Initialize overlay 
	initOverlay2D();

	do
	{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		computeMatrices();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();

		glm::vec3 CameraPosition(glm::inverse(ViewMatrix)[3]);

		glm::mat4 ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

		// sprite stuff
		sprites.updateCameraDistance(CameraPosition);
		sprites.fillPositionSizeBuffer(g_sprite_position_size_data);
		sprites.fillColorBuffer(g_sprite_color_data);
		sprites.sortSprites();

		glBindBuffer(GL_ARRAY_BUFFER, sprites_position_buffer);
		glBufferData(GL_ARRAY_BUFFER, sprites.sprite_count * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
		glBufferSubData(GL_ARRAY_BUFFER, 0, sprites.sprite_count * sizeof(GLfloat) * 4, g_sprite_position_size_data);

		glBindBuffer(GL_ARRAY_BUFFER, sprites_color_buffer);
		glBufferData(GL_ARRAY_BUFFER, sprites.sprite_count * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
		glBufferSubData(GL_ARRAY_BUFFER, 0, sprites.sprite_count * sizeof(GLubyte) * 4, g_sprite_color_data);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Use our shader
		glUseProgram(programID);

		glUniform3f(CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
		glUniform3f(CameraUp_worldspace_ID   , ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
		

		glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
		glVertexAttribPointer(
			0,                  // attribute location
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, sprites_position_buffer);
		glVertexAttribPointer(
			1,
			4,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0 
		);
		
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, sprites_color_buffer);
		glVertexAttribPointer(
			2,
			4,
			GL_UNSIGNED_BYTE,
			GL_TRUE, 
			0, 
			(void*)0 
		);

		glVertexAttribDivisor(0, 0); // sprites vertices
		glVertexAttribDivisor(1, 1); // positions : one per quad
		glVertexAttribDivisor(2, 1); // color : one per quad

		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, sprites.sprite_count);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisable(GL_BLEND);

		if (context.is_lmb_pressed) {
			double mouse_x, mouse_y;
			glfwGetCursorPos(window, &mouse_x, &mouse_y);
			int width, height;
				width = mouse_x - context.last_mouse_x + 1;
				height = mouse_y - context.last_mouse_y + 1;
			drawRectangle2D((int)context.last_mouse_x, (int)context.last_mouse_y,
							width, height);
		}

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	while( !handleKeyboard(window, &sprites, unproject) &&
		   glfwWindowShouldClose(window) == 0 );


	delete[] g_sprite_position_size_data;

	// Cleanup VBO and shader
	glDeleteBuffers(1, &sprites_color_buffer);
	glDeleteBuffers(1, &sprites_position_buffer);
	glDeleteBuffers(1, &billboard_vertex_buffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);
	// Overlay cleanup
	cleanupOverlay2D();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}