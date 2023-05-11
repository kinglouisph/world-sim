typedef unsigned char uchar;
typedef float vec2[2];
typedef float vec3[3];

float mx, my;
char mdown;

float randf() {
	return (float)rand() / (float)RAND_MAX;
}

void mouse_callback(GLFWwindow* window, double dmposx, double dmposy) {
	mx = (float)(dmposx/windowWidth*2) - 1.0f;
	my = -1.0f*(float)(dmposy/windowHeight*2) + 1.0f;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		mdown = 1;
	}
	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		mdown = 0;
	}
}

//takes row major array and blurs it. Creates new array, must free old one
void* diffuse2d(float* inputGrid, int width, int height, float diffuseConst) {
	float* out = malloc(4*width*height);
	for (int i = 0;i<width;i++) {
		for (int j = 0;j<height;j++) {
			out[i+j*width]=inputGrid[i+j*width];
		}
	}
	
	for (int i = 0;i<width;i++) {
		for (int j = 0;j<height;j++) {
			int pos=i+j*width;
			float n = inputGrid[pos]*diffuseConst;
			if(i>0){out[pos]-=n;out[pos-1]+=n;}
			if(i<width-1){out[pos]-=n;out[pos+1]+=n;}
			if(j>0){out[pos]-=n;out[pos-width]+=n;}
			if(j<height-1){out[pos]-=n;out[pos+width]+=n;}
		}
	}
	
	return out;
}

//big clumps gett bigger, small get  smaller. Not really useful for anything.
void* polarize2d(float* inputGrid, int width, int height, float axis, float diffuseConst) {
	float* out = malloc(4*width*height);
	float ot=0;
	float nt=0;
	
	for (int i = 0;i<width;i++) {
		for (int j = 0;j<height;j++) {
			out[i+j*width]=inputGrid[i+j*width] - axis;
			ot+=inputGrid[i+j*width];
		}
	}
	
	for (int i = 0;i<width;i++) {
		for (int j = 0;j<height;j++) {
			int pos=i+j*width;
			float n = (inputGrid[pos]-axis)*diffuseConst;
			out[pos]+=n;
		}
	}
	
	for (int i = 0;i<width;i++) {
		for (int j = 0;j<height;j++) {
			out[i+j*width]+=axis;
			nt+=out[i+j*width];
		}
	}
	nt=ot/nt;
	for (int i = 0;i<width;i++) {
		for (int j = 0;j<height;j++) {
			out[i+j*width]*=nt;
		}
	}
	
	return out;
}



/*doesnt work for some reason
uint32_t compileShader(int type,const char* src) {
	int success;
	char infoLog[512];
	uint32_t shader = glCreateShader(type);
	glShaderSource(shader,1,&src,NULL);
	glCompileShader(shader);

	glGetShaderiv(shader,GL_COMPILE_STATUS,&success);
	if(!success){
		printf("Shader failed to compile. Exiting.\n");
		glGetShaderInfoLog(shader,512,NULL,infoLog);
		printf(infoLog);
		exit(1);
	}
}

uint32_t combineShaders(uint32_t shader1, uint32_t shader2) {
	uint32_t shaderProgram=glCreateProgram();
	int success;
	char infoLog[512];
	
	glAttachShader(shaderProgram, shader1);
	glAttachShader(shaderProgram, shader2);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram,GL_LINK_STATUS,&success);
	
	
	if(!success){
		printf("Shader link fail.\n");
		glGetProgramInfoLog(shaderProgram,512,NULL,infoLog);
		printf(infoLog);
		return 1;
	}

	return shaderProgram;
}*/

const char* getGLErrorStr(GLenum err)
{
	switch (err)
	{
	case GL_NO_ERROR:          return "No error";
	case GL_INVALID_ENUM:      return "Invalid enum";
	case GL_INVALID_VALUE:     return "Invalid value";
	case GL_INVALID_OPERATION: return "Invalid operation";
	//case GL_STACK_OVERFLOW:    return "Stack overflow";
	//case GL_STACK_UNDERFLOW:   return "Stack underflow";
	case GL_OUT_OF_MEMORY:     return "Out of memory";
    default:                   return "Unknown error";
    }
}

//x and y of point, x and ys of line segment
float pointLineDist(float px, float py, float p1x, float p1y, float p2x, float p2y) {
	//I have a desmos repo with the actual math somewhere
	//dot product tests
	if ((px-p1x)*(p2x-p1x)+(py-p1y)*(p2y-p1y) < 0.0f) {
		float a=p1x-px;
		float b=p1y-py;
		return sqrt(a*a+b*b);
	}
	if ((px-p2x)*(p1x-p2x)+(py-p2y)*(p1y-p2y) < 0.0f) {
		float a=p2x-px;
		float b=p2y-py;
		return sqrt(a*a+b*b);
	}
	if (p1x-p2x == 0) {p1x+=0.000001f;}
	float m = (p1y-p2y)/(p1x-p2x);
	float b = p1y - m*p1x;
	float c = -1.0f*(m*b-m*py-px)/(m*m+1.0f);
	float t1 = c-px;
	float t2 = c*m+b-py;
	return sqrt(t1*t1+t2*t2);
}