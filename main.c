#ifdef WIN32
void* __chk_fail=0; //janky fix
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>

int windowWidth = 600;
int windowHeight = 600;

#include "library.h"
#include "terrainGen.h"

uint32_t VBO;
uint32_t VAO;
uint32_t EBO;
uint32_t shaderProgram;

const char* vertexShaderSrc = "#version 330 core\n"
	"uniform mat4 projection;"
	"uniform vec2 camPos;"
	"layout (location=0) in vec2 pos;"
	"void main(){"
	"vec2 npos=pos - camPos;"
	"gl_Position=projection * vec4(npos,0,1);}\0";

const char* fragmentShaderSrc = "#version 330 core\n"
	"out vec4 outColor;"
	"uniform vec4 color;"
	"void main(){outColor=color;}\0";


float camx;
float camy;
float camSpeed = 4.0f;

int main(int argc, char** argv) {
	srand (time(NULL));
	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(windowWidth,windowHeight,"test",NULL,NULL);
	if(window==NULL){
		printf("no window :(");
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		printf("Failed to initialize GLAD\n");
		return -1;
	}   
	
	//capture mouse
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
	glfwSetCursorPosCallback(window, mouse_callback);  
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	
	glViewport(0,0,windowWidth,windowHeight);
	
	
	//uint32_t vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSrc);
	//uint32_t fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);
				

	
	//uint32_t shaderProgram = combineShaders(vertexShader, fragmentShader);
	
	uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader,1,&vertexShaderSrc,NULL);
	glCompileShader(vertexShader);
	uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader,1,&fragmentShaderSrc,NULL);
	glCompileShader(fragmentShader);
	
	shaderProgram=glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	
	glLinkProgram(shaderProgram); 
	
	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glUseProgram(shaderProgram);
	
	
	
	glGenVertexArrays(1,&VAO);
	glGenBuffers(1,&VBO);
	glGenBuffers(1,&EBO);
	
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);
	const int camPosLocation = glGetUniformLocation(shaderProgram, "camPos");
	const int colorLocation = glGetUniformLocation(shaderProgram, "color");
	const int projectionLocation = glGetUniformLocation(shaderProgram, "projection");
		
		
	
	float* mapDrawPoints = malloc(4*2*(mapWidth+1)*(mapHeight+1));
	int* mapDrawIndices = malloc(4*6*mapSize);
	
	for(int i=0;i<mapWidth+1;i++) {
		for(int j=0;j<mapHeight+1;j++) {
			int pos = (i+j*(mapWidth+1))*2;
			mapDrawPoints[pos]=1.0f*i;
			mapDrawPoints[pos+1]=1.0f*j;
		}
	}
	
	for(int i=0;i<mapWidth;i++) {
		for(int j=0;j<mapHeight;j++) {
			int pos = (i+j*mapWidth)*6;
			int n = i+j*(mapWidth+1);
			mapDrawIndices[pos]=n;
			mapDrawIndices[pos+1]=n+1;
			mapDrawIndices[pos+2]=n+mapWidth+1;
			mapDrawIndices[pos+3]=n+1;
			mapDrawIndices[pos+4]=n+mapWidth+1;
			mapDrawIndices[pos+5]=n+mapWidth+2;
		}
	}
	
	
	
	//terrain gen
	World world = generateWorldGrids();
	
	
	
	
	
	
	
	
	
	
	
	
	//projection matrix for rendering
	float projection[16] = {2.0f / mapWidth,0,0,0, 0,2.0f / mapHeight,0,0, 0,0,1,0, -1.0f,-1.0f,0,1};
	
	glUniformMatrix4fv(projectionLocation, 1, 0, projection);
	glUniform4f(colorLocation, 1,0,0,1.0f);
	glUniform2f(camPosLocation,0.0f,0.0f);
	
	
	GLenum e = glGetError();
	if (e) {
			printf(getGLErrorStr(e));
			fflush(stdout);
	}
	printf("Main loop start\n");
	
	//main loop
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			camy+=camSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camx-=camSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camy-=camSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camx+=camSpeed;
		}
		
		
		
		
		//draw
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);	
		
		
		glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
		glUniform2f(camPosLocation, camx,camy);
		
		
		glUniform4f(colorLocation,0.0f,0.5f,0.5f,1.0f);

		//draw map
		glBufferData(GL_ARRAY_BUFFER, 8*(mapWidth+1)*(mapHeight+1), mapDrawPoints, GL_DYNAMIC_DRAW);
		//glDrawArrays(GL_LINE_STRIP, 0, (mapWidth+1)*(mapHeight+1));
		
		
		
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24*mapSize, mapDrawIndices, GL_DYNAMIC_DRAW);
		//glDrawElements(GL_TRIANGLES, 6*mapSize, GL_UNSIGNED_INT, 0);
		
		
		
		
		for (int i = 0; i < mapWidth; i++) {
			for (int j = 0; j < mapHeight; j++) {
				int pos=i+j*mapWidth;
				
				glUniform4f(colorLocation,world.colorGrid[pos][0],world.colorGrid[pos][1],world.colorGrid[pos][2],1.0f);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0 + pos*24);
			}
		}
		
		
		
		
		
		//draw lines
		//glUniform4f(colorLocation, 1,0,0,1.0f);
		//glBufferData(GL_ARRAY_BUFFER, totalLines*8*4, lineDrawPoints, GL_DYNAMIC_DRAW);
		//glDrawArrays(GL_LINES, 0, totalLines*2);

		
		
		
		glfwSwapBuffers(window);
		
		usleep(1000000/30); //1000000 / x fps
		
		//error test
		GLenum e = glGetError();
		if (e) {
				printf(getGLErrorStr(e));
				fflush(stdout);
		}
	}
}