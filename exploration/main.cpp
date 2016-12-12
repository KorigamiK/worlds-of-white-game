#include <iostream>
#include <fstream>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include "utilities\logger.h"

bool initLogger()
{
  static std::ofstream file("log.txt");
  static wilt::Logger logger(&file, wilt::Logger::Level::DEBUG);

  wilt::Logger::instance = &logger;

  return true;
}

int main()
{
  initLogger();
  wilt::Logger::info("core", "initialized logger");

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
  if (window == nullptr)
  {
    wilt::Logger::fatal("graphics", "failed to create GLFW window");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK)
  {
    wilt::Logger::fatal("graphics", "Failed to initialize GLEW");
    return -1;
  }

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);

  glViewport(0, 0, width, height);

  // vertices
  GLfloat vertices[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f,  0.5f, 0.0f
  };

  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // vertex shader
  const char* vertexShaderSource = 
    "#version 330 core\n"
    "layout(location = 0) in vec3 position;\n"
    "void main()\n"
    "{\n"
    "  gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
    "}";

  GLuint vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  // fragment shader
  const char* fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 color;\n"
    "void main()\n"
    "{\n"
    "    color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "} \n";

  GLuint fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  // program
  GLuint shaderProgram;
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);

  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  while (!glfwWindowShouldClose(window))
  {
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  glfwTerminate();
	return 0;
}