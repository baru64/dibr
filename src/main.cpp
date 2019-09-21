#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <algorithm>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
GLFWwindow* window;

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
using namespace glm;


#include "shader.hpp"
#include "controls.hpp"

struct Sprite{
	glm::vec3 pos;
	unsigned char r,g,b,a; // Color
	float size;
	float cameradistance; 

	bool operator<(const Sprite& that) const {
		// Sort in reverse order : far sprites drawn first.
		return this->cameradistance > that.cameradistance;
	}
};
const int SpriteCount = 2;
Sprite SpritesContainer[SpriteCount];
void SortSprites(){
	std::sort(&SpritesContainer[0], &SpritesContainer[SpriteCount]);
}

int main( void )
{
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
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Tutorial 18 - Billboards", NULL, NULL);
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
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

	// Dark blue background
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
	// GLuint BillboardPosID = glGetUniformLocation(programID, "BillboardPos");
	// GLuint BillboardSizeID = glGetUniformLocation(programID, "BillboardSize");

	// TODO NA RAZIE NA SZTYWNO 2 PARTICLE
	static GLfloat* g_sprite_position_size_data = new GLfloat[SpriteCount * 4];
	static GLubyte* g_sprite_color_data         = new GLubyte[SpriteCount * 4];

	// TODO POMIJAM PRZYPISYWANIE CAMERADISTANCE BO I TAK STATYCZNIE ZROBIM

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
	glBufferData(GL_ARRAY_BUFFER, SpriteCount * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	// The VBO containing the colors of the sprites
	GLuint sprites_color_buffer;
	glGenBuffers(1, &sprites_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, sprites_color_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, SpriteCount * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

	SpritesContainer[0].pos = glm::vec3(0,0,0);
	SpritesContainer[0].r = 255;
	SpritesContainer[0].g = 0;
	SpritesContainer[0].b = 0;
	SpritesContainer[0].a = 255;
	SpritesContainer[0].size = 1.0f;

	SpritesContainer[1].pos = glm::vec3(1.0f,0,0);
	SpritesContainer[1].r = 255;
	SpritesContainer[1].g = 0;
	SpritesContainer[1].b = 0;
	SpritesContainer[1].a = 255;
	SpritesContainer[1].size = 1.0f;

	double lastTime = glfwGetTime();
	do
	{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		double currentTime = glfwGetTime();
		double delta = currentTime - lastTime;
		lastTime = currentTime;

		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();

		glm::vec3 CameraPosition(glm::inverse(ViewMatrix)[3]);

		glm::mat4 ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

		// particle stuff
		SpritesContainer[0].cameradistance =
			glm::length2( SpritesContainer[0].pos - CameraPosition );
		SpritesContainer[1].cameradistance =
			glm::length2( SpritesContainer[1].pos - CameraPosition );
		g_sprite_position_size_data[0] = SpritesContainer[0].pos.x;
		g_sprite_position_size_data[1] = SpritesContainer[0].pos.y;
		g_sprite_position_size_data[2] = SpritesContainer[0].pos.z;
		g_sprite_position_size_data[3] = SpritesContainer[0].size;
		g_sprite_position_size_data[4] = SpritesContainer[1].pos.x;
		g_sprite_position_size_data[5] = SpritesContainer[1].pos.y;
		g_sprite_position_size_data[6] = SpritesContainer[1].pos.z;
		g_sprite_position_size_data[7] = SpritesContainer[1].size;
		
		g_sprite_color_data[0] = SpritesContainer[0].r;
		g_sprite_color_data[1] = SpritesContainer[0].g;
		g_sprite_color_data[2] = SpritesContainer[0].b;
		g_sprite_color_data[3] = SpritesContainer[0].a;
		g_sprite_color_data[4] = SpritesContainer[1].r;
		g_sprite_color_data[5] = SpritesContainer[1].g;
		g_sprite_color_data[6] = SpritesContainer[1].b;
		g_sprite_color_data[7] = SpritesContainer[1].a;

		SortSprites();

		glBindBuffer(GL_ARRAY_BUFFER, sprites_position_buffer);
		glBufferData(GL_ARRAY_BUFFER, SpriteCount * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
		glBufferSubData(GL_ARRAY_BUFFER, 0, SpriteCount * sizeof(GLfloat) * 4, g_sprite_position_size_data);

		glBindBuffer(GL_ARRAY_BUFFER, sprites_color_buffer);
		glBufferData(GL_ARRAY_BUFFER, SpriteCount * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
		glBufferSubData(GL_ARRAY_BUFFER, 0, SpriteCount * sizeof(GLubyte) * 4, g_sprite_color_data);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Use our shader
		glUseProgram(programID);

		glUniform3f(CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
		glUniform3f(CameraUp_worldspace_ID   , ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
		

		glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : positions of sprites' centers
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, sprites_position_buffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size : x + y + z + size => 4
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);
		
		// 3rd attribute buffer : sprites' colors
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, sprites_color_buffer);
		glVertexAttribPointer(
			2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size : r + g + b + a => 4
			GL_UNSIGNED_BYTE,                 // type
			GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		glVertexAttribDivisor(0, 0); // sprites vertices : always reuse the same 4 vertices -> 0
		glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
		glVertexAttribDivisor(2, 1); // color : one per quad

		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 2);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );


	delete[] g_sprite_position_size_data;

	// Cleanup VBO and shader
	glDeleteBuffers(1, &sprites_color_buffer);
	glDeleteBuffers(1, &sprites_position_buffer);
	glDeleteBuffers(1, &billboard_vertex_buffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}