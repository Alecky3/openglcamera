#include<glm/glm.hpp>
#include<iostream>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
void resizeCallback(GLFWwindow* window, int width, int height);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int modifier);
void createProgram();
void drawtriangle();
std::ostream& operator <<(std::ostream&, const glm::vec3&);
void init();
GLuint VBO[2];
GLuint VAO[2];
GLuint EBO[1];
GLuint programs[1];
float angle = 45;
glm::vec3 camPosition(0.0f, 0.0f, 3.0f);
glm::vec3 targetPosition(0.0f, 0.0f, -1.0f);
glm::vec3 camUp(0.0, 1.0, 0.0);
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main(int argc, char** argv) {

	if (!glfwInit()) {
		std::cout << "Could not initialize glfw" << std::endl;
		exit(1);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "GLFW", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (!gladLoadGL()) {
		std::cout << "Could not initialize GLAD" << std::endl;
		exit(EXIT_FAILURE);
	}
	glfwSetFramebufferSizeCallback(window, resizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	init();
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {


		glClearColor(0.5, 0.3, 0.2, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawtriangle();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;

}

void resizeCallback(GLFWwindow* window, int width, int height)
{
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int modifier)
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	float camspeed = 2.5*deltaTime;
	if (key == GLFW_KEY_EQUAL && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		angle += 1;
	}
	 if (key == GLFW_KEY_MINUS && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		angle -= 1;
	}
	if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		camPosition += camspeed * targetPosition;
	}
	if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		camPosition -= camspeed * targetPosition;
	}
	if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		camPosition -= glm::normalize(glm::cross(targetPosition, camUp))* camspeed;
	}
	if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		camPosition += glm::normalize(glm::cross(targetPosition, camUp)) * camspeed;
	}
	if (key == GLFW_KEY_Q && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		camPosition -= glm::normalize(glm::cross(targetPosition, glm::vec3(1.0, 0.0, 0.0))) * camspeed;
	}
	if (key == GLFW_KEY_E && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		camPosition += glm::normalize(glm::cross(targetPosition, glm::vec3(1.0, 0.0, 0.0))) * camspeed;
	}
	glm::mat4 view;
	view = glm::lookAt(camPosition, camPosition + targetPosition, glm::vec3(0.0, 1.0, 0.0));
	GLuint viewLocation = glGetUniformLocation(programs[0], "view");

	std::cout << "position: " << camPosition;
	std::cout << "target: " << camPosition + targetPosition;
	std::cout << "normalize: " <<glm::normalize(glm::cross(targetPosition, glm::vec3(0.0, 1.0, 0.0)))*camspeed;
	std::cout << "speed: " << camspeed<<std::endl;
	std::cout << "current frame" << currentFrame << std::endl;
	std::cout << "\n" << std::endl;

	
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(angle), 800.0f / 600.0f, 1.0f, 100.0f);

	GLuint projectionLocation = glGetUniformLocation(programs[0], "projection");
	glUseProgram(programs[0]);
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

	std::cout << "angle: " << angle << std::endl;
}

void createProgram()
{
	const char* vertexSource = "#version 330 core\n"
		"in vec3 aPos;\n"
		"in vec3 aColor;"
		"out vec3 outColor;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"void main(){\n"
		"gl_Position= projection * view * model * vec4(aPos,1.0);"
		"outColor=aColor;"
		"\n}\0";
	const char* fragmentSource = "#version 330 core\n"
		"in vec3 outColor;"
		"out vec4 fragColor;"
		"void main()\n{"
		"fragColor=vec4(outColor,1.0);"
		"\n}\0";

	//create vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	//check for errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "Error compiling vertex shader : " << infoLog << std::endl;
	}
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	//check for errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
		std::cout << "Error compiling fragment shader: " << infoLog << std::endl;
	}

	//create program
	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	programs[0] = program;

	//check for errors
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "Error linking program: " << infoLog << std::endl;
	}
}

void drawtriangle()
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUseProgram(programs[0]);
	glBindVertexArray(VAO[0]);

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0,0.0,0.0),
		glm::vec3(2.0,5.0,-10.0),
		glm::vec3(-3.0,3.0,-8.0),
		glm::vec3(-3.0,-3.0,-5.0),
		glm::vec3(-3.0,-2.3,-7.0),
		glm::vec3(5.0,-2.3,-10.0),
		glm::vec3(-3.5,0.35,-3.0),
		glm::vec3(2.5,2.5,-10.0),
		glm::vec3(-2.1,2.5,-11.0),
		glm::vec3(2.2,2.9,-12.0)

	};
	for (int i = 0; i < 10; i++) {
		glm::mat4 model = glm::mat4(1.0);
		model = glm::translate(model, cubePositions[i]);
		float angle = 30.0 * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0, 0.0, 0.0));

		unsigned int modelLocation = glGetUniformLocation(programs[0], "model");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}



}

std::ostream& operator<<(std::ostream& os, const glm::vec3& vec)
{
	os << "{" << vec.x << "," << vec.y << "," << vec.z << "}" << std::endl;

	return os;
}

void init()
{
	float vertices[] = {
-0.5f, -0.5f, -0.5f, 0.5f, 0.3f, 0.6f,
0.5f, -0.5f, -0.5f, 0.5f,0.3f ,0.6f,
0.5f, 0.5f, -0.5f, 0.5f,0.3f ,0.6f,
0.5f, 0.5f, -0.5f,0.5f,0.3f,0.6f,
-0.5f, 0.5f, -0.5f,0.5f,0.3f,0.6f,
-0.5f, -0.5f, -0.5f,0.5f,0.3f,0.6f,

-0.5f, -0.5f, 0.5f,0.2f,0.6f,0.2f,
0.5f, -0.5f, 0.5f,0.2f,0.6f,0.2f,
0.5f, 0.5f, 0.5f,0.2f,0.6f,0.2f,
0.5f, 0.5f, 0.5f,0.2f,0.6f,0.2f,
-0.5f, 0.5f, 0.5f,0.2f,0.6f,0.2f,
-0.5f, -0.5f, 0.5f,0.2f,0.6f,0.2f,

-0.5f, 0.5f, 0.5f,0.3f,0.4f,0.2f,
-0.5f, 0.5f, -0.5f,0.3f,0.4f,0.2f,
-0.5f, -0.5f, -0.5f,0.3f,0.4f,0.2f,
-0.5f, -0.5f, -0.5f,0.3f,0.4f,0.2f,
-0.5f, -0.5f, 0.5f,0.3f,0.4f,0.2f,
-0.5f, 0.5f, 0.5f,0.3f,0.4f,0.2f,

0.5f, 0.5f, 0.5f,0.1f,0.9f,0.7f,
0.5f, 0.5f, -0.5f,0.1f,0.9f,0.7f,
0.5f, -0.5f, -0.5f,0.1f,0.9f,0.7f,
0.5f, -0.5f, -0.5f,0.1f,0.9f,0.7f,
0.5f, -0.5f, 0.5f,0.1f,0.9f,0.7f,
0.5f, 0.5f, 0.5f,0.1f,0.9f,0.7f,

-0.5f, -0.5f, -0.5f,0.7f,0.1f,0.3f,
0.5f, -0.5f, -0.5f,0.7f,0.1f,0.3f,
0.5f, -0.5f, 0.5f,0.7f,0.1f,0.3f,
0.5f, -0.5f, 0.5f,0.7f,0.1f,0.3f,
-0.5f, -0.5f, 0.5f,0.7f,0.1f,0.3f,
-0.5f, -0.5f, -0.5f,0.7f,0.1f,0.3f,

-0.5f, 0.5f, -0.5f,0.6f,0.4f,0.1f,
0.5f, 0.5f, -0.5f,0.6f,0.4f,0.1f,
0.5f, 0.5f, 0.5f,0.6f,0.4f,0.1f,
0.5f, 0.5f, 0.5f, 0.6f,0.4f,0.1f,
-0.5f, 0.5f, 0.5f,0.6f,0.4f,0.1f,
-0.5f, 0.5f, -0.5f,0.6f,0.4f,0.1f
	};


	glGenBuffers(2, VBO);

	//vao
	glGenVertexArrays(2, VAO);
	glBindVertexArray(VAO[0]);

	//vbo
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	//get location of attribute
	createProgram();


	glm::mat4 view = glm::mat4(1.0);
	view = glm::translate(view, glm::vec3(0.0, 0.0, -3.0f));

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 1.0f, 100.0f);


	unsigned int viewLocation = glGetUniformLocation(programs[0], "view");
	unsigned int projectionLocation = glGetUniformLocation(programs[0], "projection");

	glUseProgram(programs[0]);

	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
	GLint aPos = glGetAttribLocation(programs[0], "aPos");
	GLuint aColor = glGetAttribLocation(programs[0], "aColor");
	//configure vertex data
	glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(aColor, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(aPos);
	glEnableVertexAttribArray(aColor);

}
