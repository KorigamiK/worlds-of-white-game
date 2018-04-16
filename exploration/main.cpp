#define _CRT_SECURE_NO_WARNINGS

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cimg/cimg.h>

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

class Shader
{
public:
  unsigned int ID;
  // constructor generates the shader on the fly
  // ------------------------------------------------------------------------
  Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr)
  {
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
      // open files
      vShaderFile.open(vertexPath);
      fShaderFile.open(fragmentPath);
      std::stringstream vShaderStream, fShaderStream;
      // read file's buffer contents into streams
      vShaderStream << vShaderFile.rdbuf();
      fShaderStream << fShaderFile.rdbuf();
      // close file handlers
      vShaderFile.close();
      fShaderFile.close();
      // convert stream into string
      vertexCode = vShaderStream.str();
      fragmentCode = fShaderStream.str();
      // if geometry shader path is present, also load a geometry shader
      if (geometryPath != nullptr)
      {
        gShaderFile.open(geometryPath);
        std::stringstream gShaderStream;
        gShaderStream << gShaderFile.rdbuf();
        gShaderFile.close();
        geometryCode = gShaderStream.str();
      }
    }
    catch (std::ifstream::failure e)
    {
      std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char * fShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // if geometry shader is given, compile geometry shader
    unsigned int geometry;
    if (geometryPath != nullptr)
    {
      const char * gShaderCode = geometryCode.c_str();
      geometry = glCreateShader(GL_GEOMETRY_SHADER);
      glShaderSource(geometry, 1, &gShaderCode, NULL);
      glCompileShader(geometry);
      checkCompileErrors(geometry, "GEOMETRY");
    }
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    if (geometryPath != nullptr)
      glAttachShader(ID, geometry);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometryPath != nullptr)
      glDeleteShader(geometry);

  }
  // activate the shader
  // ------------------------------------------------------------------------
  void use()
  {
    glUseProgram(ID);
  }
  // utility uniform functions
  // ------------------------------------------------------------------------
  void setBool(const std::string &name, bool value) const
  {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
  }
  // ------------------------------------------------------------------------
  void setInt(const std::string &name, int value) const
  {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
  }
  // ------------------------------------------------------------------------
  void setFloat(const std::string &name, float value) const
  {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
  }
  // ------------------------------------------------------------------------
  void setVec2(const std::string &name, const glm::vec2 &value) const
  {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
  }
  void setVec2(const std::string &name, float x, float y) const
  {
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
  }
  // ------------------------------------------------------------------------
  void setVec3(const std::string &name, const glm::vec3 &value) const
  {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
  }
  void setVec3(const std::string &name, float x, float y, float z) const
  {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
  }
  // ------------------------------------------------------------------------
  void setVec4(const std::string &name, const glm::vec4 &value) const
  {
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
  }
  void setVec4(const std::string &name, float x, float y, float z, float w)
  {
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
  }
  // ------------------------------------------------------------------------
  void setMat2(const std::string &name, const glm::mat2 &mat) const
  {
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
  }
  // ------------------------------------------------------------------------
  void setMat3(const std::string &name, const glm::mat3 &mat) const
  {
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
  }
  // ------------------------------------------------------------------------
  void setMat4(const std::string &name, const glm::mat4 &mat) const
  {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
  }

private:
  // utility function for checking shader compilation/linking errors.
  // ------------------------------------------------------------------------
  void checkCompileErrors(GLuint shader, std::string type)
  {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
      glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
      if (!success)
      {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
      }
    }
    else
    {
      glGetProgramiv(shader, GL_LINK_STATUS, &success);
      if (!success)
      {
        glGetProgramInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
      }
    }
  }
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 720;
bool paused = false;
float camera_distance = 2.5f;
float camera_rotation = 0.0f;

unsigned int color_texture;
unsigned int depth_texture;

std::vector<float> read_line_model(std::string path)
{
  std::ifstream file(path);

  int count = 0;
  file >> count;

  // two vertices of position and normal
  std::vector<float> ret(count * 2 * 6);
  for (int i = 0; i < ret.size(); ++i)
    file >> ret[i];

  return ret;
}

std::vector<float> read_face_model(std::string path)
{
  std::ifstream file(path);

  int count = 0;
  file >> count;

  // three vertices of position
  std::vector<float> ret(count * 3 * 3);
  for (int i = 0; i < ret.size(); ++i)
    file >> ret[i];

  return ret;
}

struct Model
{
  std::vector<float> vertexData;
  std::vector<unsigned int> faceIndexes;
  std::vector<unsigned int> lineIndexes;
  unsigned int vertexDataVBO;
  unsigned int vertexDataVAO;
  unsigned int faceIndexesID;
  unsigned int lineIndexesID;
};

Model read_model(std::string path)
{
  std::ifstream file(path);
  Model model;
  int vertexCount;
  int faceCount;
  int lineCount;

  // read vertices
  file >> vertexCount;
  model.vertexData.resize(vertexCount * 3); // 3 floats per vertex (x, y, z)
  for (int i = 0; i < model.vertexData.size(); ++i)
    file >> model.vertexData[i];

  // read faces
  file >> faceCount;
  model.faceIndexes.resize(faceCount * 3); // 3 ints per face (vId1, vId2, vId3)
  for (int i = 0; i < model.faceIndexes.size(); ++i)
    file >> model.faceIndexes[i];

  // read lines
  file >> lineCount;
  model.lineIndexes.resize(lineCount * 4); // 4 ints per line (vId1, vId2, vId3, vId4)
  for (int i = 0; i < model.lineIndexes.size(); ++i)
    file >> model.lineIndexes[i];

  return model;
}

void load_model(Model& model)
{
  // load vertices
  glGenVertexArrays(1, &model.vertexDataVAO);
  glGenBuffers(1, &model.vertexDataVBO);
  glBindVertexArray(model.vertexDataVAO);
  glBindBuffer(GL_ARRAY_BUFFER, model.vertexDataVBO);
  glBufferData(GL_ARRAY_BUFFER, model.vertexData.size() * sizeof(float), model.vertexData.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

  //glBindVertexArray(0);

  // load faces
  glGenBuffers(1, &model.faceIndexesID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.faceIndexesID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.faceIndexes.size() * sizeof(unsigned int), model.faceIndexes.data(), GL_STATIC_DRAW);

  // load lines
  glGenBuffers(1, &model.lineIndexesID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.lineIndexesID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.lineIndexes.size() * sizeof(unsigned int), model.lineIndexes.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void draw_faces(Model& model)
{
  glBindVertexArray(model.vertexDataVAO);
  //glBindBuffer(GL_ARRAY_BUFFER, model.vertexDataVBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.faceIndexesID);
  glDrawElements(GL_TRIANGLES, model.faceIndexes.size(), GL_UNSIGNED_INT, (void*)0);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void draw_lines(Model& model)
{
  glBindVertexArray(model.vertexDataVAO);
  //glBindBuffer(GL_ARRAY_BUFFER, model.vertexDataVBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.lineIndexesID);
  glDrawElements(GL_LINES_ADJACENCY, model.lineIndexes.size(), GL_UNSIGNED_INT, (void*)0);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

int main()
{
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

                                                       // glfw window creation
                                                       // --------------------
  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSwapInterval(1);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);

  // build and compile our shader programs
  // ------------------------------------
  Shader lineShader("shaders/line.vert.glsl", "shaders/line.frag.glsl", "shaders/line.geom.glsl");
  Shader depthShader("shaders/line.vert.glsl", "shaders/depth.frag.glsl");
  Shader screenShader("shaders/screen.vert.glsl", "shaders/screen.frag.glsl");

  auto spikeLines = read_line_model("models/temp_lines.txt");
  auto spikeFaces = read_face_model("models/temp_faces.txt");
  auto file_model = read_model("models/temp_model.txt");

  cimg_library::CImg<unsigned char> f;
  f.load_jpeg("models/perlin.jpg");

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  float cubeLines[] = {
    // points            // face-dir

    // z-lines
    -0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, // x+
    -0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f, // y+

    -0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, // x+
    -0.5f,  0.5f,  0.5f,  0.0f, -1.0f,  0.0f, // y-

    0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, // x-
    0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f, // y+

    0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, // x-
    0.5f,  0.5f,  0.5f,  0.0f, -1.0f,  0.0f, // y-

                                             // y-lines
                                             -0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f, // z+
                                             -0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, // x+

                                             0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f, // z+
                                             0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, // x-

                                             -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, -1.0f, // z-
                                             -0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, // x+

                                             0.5f, -0.5f,  0.5f,  0.0f,  0.0f, -1.0f, // z-
                                             0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, // x-

                                                                                      // x-lines
                                                                                      -0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f, // y+
                                                                                      0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  1.0f, // z+

                                                                                      -0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f, // y+
                                                                                      0.5f, -0.5f,  0.5f,  0.0f,  0.0f, -1.0f, // z-

                                                                                      -0.5f,  0.5f, -0.5f,  0.0f, -1.0f,  0.0f, // y-
                                                                                      0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  1.0f, // z+

                                                                                      -0.5f,  0.5f,  0.5f,  0.0f, -1.0f,  0.0f, // y-
                                                                                      0.5f,  0.5f,  0.5f,  0.0f,  0.0f, -1.0f  // z-
  };

  float cubeFaces[] = {
    // back
    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    // front
    -0.5f, -0.5f,  0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    // left
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

    // right
    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,

    // bottom
    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    // top
    -0.5f,  0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f
  };

  float quadVertices[] = {
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
    1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
    1.0f, -1.0f,  1.0f, 0.0f,
    1.0f,  1.0f,  1.0f, 1.0f
  };

  load_model(file_model);

  // load lines
  unsigned int linesVBO, linesVAO;
  glGenVertexArrays(1, &linesVAO);
  glGenBuffers(1, &linesVBO);
  glBindVertexArray(linesVAO);
  glBindBuffer(GL_ARRAY_BUFFER, linesVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeLines), cubeLines, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

  // load spike lines
  unsigned int spikeLinesVBO, spikeLinesVAO;
  glGenVertexArrays(1, &spikeLinesVAO);
  glGenBuffers(1, &spikeLinesVBO);
  glBindVertexArray(spikeLinesVAO);
  glBindBuffer(GL_ARRAY_BUFFER, spikeLinesVBO);
  glBufferData(GL_ARRAY_BUFFER, spikeLines.size() * sizeof(float), spikeLines.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

  // load faces
  unsigned int facesVAO, facesVBO;
  glGenVertexArrays(1, &facesVAO);
  glGenBuffers(1, &facesVBO);
  glBindVertexArray(facesVAO);
  glBindBuffer(GL_ARRAY_BUFFER, facesVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeFaces), &cubeFaces, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

  // load spike faces
  unsigned int spikeFacesVAO, spikeFacesVBO;
  glGenVertexArrays(1, &spikeFacesVAO);
  glGenBuffers(1, &spikeFacesVBO);
  glBindVertexArray(spikeFacesVAO);
  glBindBuffer(GL_ARRAY_BUFFER, spikeFacesVBO);
  glBufferData(GL_ARRAY_BUFFER, spikeFaces.size() * sizeof(float), spikeFaces.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

  // load quad
  unsigned int quadVAO, quadVBO;
  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);
  glBindVertexArray(quadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

  // create framebuffer for the depth shader to write to
  // -----------

  // create a color texture
  glGenTextures(1, &color_texture);
  glBindTexture(GL_TEXTURE_2D, color_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);

  // create a depth texture 
  glGenTextures(1, &depth_texture);
  glBindTexture(GL_TEXTURE_2D, depth_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

  // create the framebuffer
  unsigned int framebuffer;
  glGenFramebuffers(1, &framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  GLint maxGeometryOutputVertices;
  glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &maxGeometryOutputVertices);
  std::cout << maxGeometryOutputVertices << std::endl;

  // render loop
  // -----------
  int i = 0;
  while (!glfwWindowShouldClose(window))
  {
    // input
    processInput(window);

    float radius = camera_distance;
    float camX = sin(camera_rotation) * radius;
    float camZ = cos(camera_rotation) * radius;
    glm::mat4 view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 model = glm::rotate(glm::scale(glm::mat4(), { 0.5f, 0.5f, 0.5f }), glm::radians(-90.0f), { 1.0f, 0.0f, 0.0f });

    // render depth
    depthShader.use();
    depthShader.setMat4("projection", projection);
    depthShader.setMat4("view", view);
    depthShader.setFloat("frame", i / 24);
    //depthShader.setMat4("model", model);

    glEnable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glBindVertexArray(facesVAO);
    //depthShader.setMat4("model", glm::translate(model, { 0.0f, 0.0f, 0.0f }));
    //glDrawArrays(GL_TRIANGLES, 0, 36);
    //depthShader.setMat4("model", glm::translate(model, { 0.5f, 0.25f, 0.5f }));
    //glDrawArrays(GL_TRIANGLES, 0, 36);

    //glBindVertexArray(spikeFacesVAO);
    //depthShader.setMat4("model", glm::rotate(glm::scale(model, { 0.5f, 0.5f, 0.5f }), 0.35f, { 0.1, 0.4, -0.2 }));
    depthShader.setMat4("model", model);
    //glDrawArrays(GL_TRIANGLES, 0, spikeFaces.size() / 3); // 3 is the data per vertex

    draw_faces(file_model);

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // render lines
    lineShader.use();
    lineShader.setMat4("projection", projection);
    lineShader.setMat4("view", view);
    //lineShader.setMat4("model", model);
    lineShader.setFloat("frame", i / 24);
    lineShader.setFloat("ratio", 720.0f / 1280.0f);
    lineShader.setInt("depth_texture", 0);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, depth_texture);

    //glBindVertexArray(linesVAO);
    //lineShader.setMat4("model", glm::translate(model, { 0.0f, 0.0f, 0.0f }));
    //glDrawArrays(GL_LINES, 0, 24);
    //lineShader.setMat4("model", glm::translate(model, { 0.5f, 0.25f, 0.5f }));
    //glDrawArrays(GL_LINES, 0, 24);

    //glBindVertexArray(spikeLinesVAO);
    //lineShader.setMat4("model", glm::rotate(glm::scale(model, { 0.5f, 0.5f, 0.5f }), 0.35f, { 0.1, 0.4, -0.2 }));
    lineShader.setMat4("model", model);
    //glDrawArrays(GL_LINES, 0, spikeLines.size() / 6); // 6 is the data per vertex

    draw_lines(file_model);

    // render depth
    //screenShader.use();

    //glDisable(GL_DEPTH_TEST);
    //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT);
    //glBindVertexArray(quadVAO);
    //glBindTexture(GL_TEXTURE_2D, depth_texture);
    //glDrawArrays(GL_TRIANGLES, 0, 6);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();

    if (!paused)
      i++;
  }

  // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------
  glDeleteVertexArrays(1, &linesVAO);
  glDeleteVertexArrays(1, &facesVAO);
  glDeleteVertexArrays(1, &spikeLinesVAO);
  glDeleteVertexArrays(1, &spikeFacesVAO);
  glDeleteVertexArrays(1, &quadVAO);
  glDeleteBuffers(1, &linesVBO);
  glDeleteBuffers(1, &facesVBO);
  glDeleteBuffers(1, &spikeLinesVBO);
  glDeleteBuffers(1, &spikeFacesVBO);
  glDeleteBuffers(1, &quadVBO);

  // glfw: terminate, clearing all previously allocated GLFW resources.
  // ------------------------------------------------------------------
  glfwTerminate();
  return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    paused = !paused;
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    camera_distance -= 0.05;
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    camera_distance += 0.05;
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    camera_rotation -= glm::radians(30.0f) / 144.0f;
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    camera_rotation += glm::radians(30.0f) / 144.0f;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  // make sure the viewport matches the new window dimensions; note that width and 
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);

  // resize framebuffer textures
  glBindTexture(GL_TEXTURE_2D, color_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

  glBindTexture(GL_TEXTURE_2D, depth_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

  glBindTexture(GL_TEXTURE_2D, 0);
}