//opengl includes
#include<GL\glew.h>
#include<GLFW\glfw3.h>
//c++ includes
#include<iostream>
#include<string>
//macros
#define ASSERT(x) if (x) return 0;
#define GLValidate(x) while(glGetError());\
x;\
ASSERT(GLCheckError(#x,relativePath(__FILE__),__LINE__))//true--> there's an error
//function defs
	//opengl functions
unsigned int compileShader(int type, const std::string& src);
unsigned int createProgram(const std::string& vertexShader, const std::string& fragmentShader);
static bool GLCheckError(const char* function, std::string file, int line);
static int Exist(unsigned int program, const char* name, std::string file, int line);	//c++ functions
std::string relativePath(const char* fullPath);
//classes, enums and structs

int main()
{
	GLFWwindow* window;
	if (!glfwInit())
		return 0;
	window = glfwCreateWindow(640, 480, "Hello,Trig!", nullptr, nullptr);
	if (!window)
		return 0;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	//initialize glew library
	if (glewInit()) {
		std::cout << "Failed to Initialize GLEW" << std::endl;
		return 0;
	}
	unsigned int buffers[2];
	//data 
	float positions[] = {
	-0.5f,-0.5f,//0
	0.5f,-0.5f,//1
	0.5f,0.5f,//2
	-0.5f,0.5f//3
	};
	unsigned char indecies[]=
		{
			0,1,2,
			0,2,3
		};
//create vertexbuffer
	GLValidate(glGenBuffers(2,buffers));
	GLValidate(glBindBuffer(GL_ARRAY_BUFFER,buffers[0]));
	GLValidate(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2*4,positions,GL_STATIC_DRAW ));
	//vertex attributes and layout
	GLValidate(glEnableVertexAttribArray(0));
	GLValidate(glVertexAttribPointer(0, 2, GL_FLOAT, 0, sizeof(float) * 2, (const void*)0));
	//create indexbuffer
	GLValidate(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,buffers[1]));
	GLValidate(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned char) * 3 * 2, indecies,GL_STATIC_DRAW));
	//create programs and their source code
	std::string vertexShader =
		"#version 330 core\n"
		"layout(location = 0) in vec4 position;\n"
		"void main()\n"
		"{\n"
		"gl_Position = position;\n"
		"}\n";
	std::string fragmentShader =
		"#version 330 core\n"
		"layout(location = 0) out vec4 color;\n"
		"uniform vec4 u_Color;\n"
		"void main()\n"
		"{\n"
		"color =u_Color;\n"
		"}\n";
	float r = 0.0f;
	float increment = 0.05f;
	unsigned int program = createProgram(vertexShader, fragmentShader);
	GLValidate(glUseProgram(program));
	int location = Exist(program, "u_Color", relativePath(__FILE__), __LINE__);
	ASSERT(location);
	while (!glfwWindowShouldClose(window))
	{
		GLValidate(glClear(GL_COLOR_BUFFER_BIT));
		//render 
		GLValidate(glUniform4f(location, r, 1.0, 0.0, 1.0));
		GLValidate(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr));
		if (r > 1.0f)
			increment -= 0.05f;
		if (r < 0.0f)
			increment += 0.05f;
		glfwSwapBuffers(window);
		glfwPollEvents();
		r += increment;

	}
	glfwTerminate();
	return 0;
}
unsigned int compileShader(int type, const std::string& src)
{
	const char* c_src = src.c_str();
	unsigned int shaderId = glCreateShader(type);
	GLValidate(glShaderSource(shaderId, 1, &c_src, nullptr));
	GLValidate(glCompileShader(shaderId));
	//Error Handling
	int result;
	GLValidate(glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result));
	if (!result)
	{ 
		int length;
		GLValidate(glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length));
		char* infoLog = (char*)alloca(sizeof(char) * length);
		GLValidate(glGetShaderInfoLog(shaderId, length, &length, infoLog));
		std::cout << "Failed to compile " <<
			(type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!"
			<< std::endl;
		std::cout << infoLog << std::endl;
		GLValidate(glDeleteShader(shaderId));
		return 0;
	}
	return shaderId;

}
unsigned int createProgram(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int programId = glCreateProgram();
	unsigned int compiledVsId = compileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int compiledFsId = compileShader(GL_FRAGMENT_SHADER, fragmentShader);
	GLValidate(glAttachShader(programId, compiledVsId));
	GLValidate(glAttachShader(programId, compiledFsId));
	GLValidate(glLinkProgram(programId));
	GLValidate(glValidateProgram(programId));
	GLValidate(glDeleteShader(compiledVsId));
	GLValidate(glDeleteShader(compiledFsId));
	return programId;
}
static bool GLCheckError(const char* function,std::string file, int line)
{
	bool flag = false;
	while (GLenum error = glGetError())
	{
		switch (error)
		{
		case GL_INVALID_ENUM:
			std::cout << "function: " << function << std::endl;
			std::cout << "error: GL_INVALID_ENUM " << ",line: " << line << ", file: " << file << std::endl;
			std::cout << "hint: an unacceptable value is specified for an enumerated argumentd" << std::endl;
			flag = true;
			break;
		case GL_INVALID_VALUE:
			std::cout << "function: " << function << std::endl;
			std::cout << "error: GL_INVALID_VALUE" << ", line: " << line << ", file: " << file << std::endl;
			std::cout << "hint: a numeric argument is out of range" << std::endl;
			flag = true;
			break;
		case GL_INVALID_OPERATION:
			std::cout << "function: " << function << std::endl;
			std::cout << "error: GL_INVALID_OPERATION" << ", line: " << line << ", file: " << file << std::endl;
			std::cout << "hint: the specified operation is not allowed in the current state" << std::endl;
			flag = true;
			break;
		case GL_STACK_OVERFLOW:
			std::cout << "function: " << function << std::endl;
			std::cout << "error: GL_STACK_OVERFLOW" << ", line: " << line << ", file: " << file << std::endl;
			std::cout << "hint : this function would cause a stack overflow" << std::endl;
			flag = true;
			break;
		case GL_STACK_UNDERFLOW:
			std::cout << "function: " << function << std::endl;
			std::cout << "error: GL_STACK_UNDERFLOW" << ", line: " <<line<< ", file: " << file << std::endl;
			std::cout << "hint : this function would cause a stack underflow" << std::endl;
			flag = true;
			break;
		case GL_OUT_OF_MEMORY:
			std::cout << "function: " << function << std::endl;
			std::cout << "error: GL_OUT_OF_MEMORY " << ", line: " <<line << ", file: " << file << std::endl;
			std::cout << "hint: there is not enough memory left to execute the function" << std::endl;
			flag = true;
			break;
		default:
			std::cout << "function: " << function << std::endl;
			std::cout << "error: GL_OUT_OF_MEMORY " << ", line: " << line << ", file: " << file << std::endl;
			std::cout << "hint: invalid error code" << std::endl;
			flag = true;
		}
	}
	if (flag)
	{
		glfwTerminate();
	}
	return flag;
}
std::string relativePath(const char* fullPath) {
	// Assuming __FILE__ macro is used to get the current file path
	std::string filePath(fullPath);
	// Find the position of the solution directory name "Quantum"
	size_t pos = filePath.find("Quantum");
	if (pos != std::string::npos) {
		// Extract the part of the path after "Quantum"
		std::string relativePath = filePath.substr(pos + 8); // +7 to skip "Quantum" and the path separatore
		return relativePath;
	}
	else {
		// If "Quantum" is not found, print the full path
		return filePath;
	}

}

static int Exist(unsigned int program, const char* name, std::string file, int line)
{
GLValidate(int location = glGetUniformLocation(program, name));
if(location ==-1)
{
	std::cout << "Error: Uniform " << name << " wasn't found or not used! " << ", line: " << line << ", file: " << file << std::endl;
	return 1;
}
return location;
}