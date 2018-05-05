#ifndef WILT_PROGRAM_H
#define WILT_PROGRAM_H

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.h"

class Program
{
private:
  GLuint _id;

public:
  Program();
  explicit Program(GLuint id);
  explicit Program(Shader fragmentShader);
  Program(Shader vertexShader, Shader fragmentShader);
  Program(Shader vertexShader, Shader geometryShader, Shader fragmentShader);
  Program(Shader vertexShader, Shader tessellationControlShader, Shader tessellationEvaluationShader, Shader geometryShader, Shader fragmentShader);
  Program(const Program& s) = delete;
  Program(Program&& s);

  Program& operator= (const Program& s) = delete;
  Program& operator= (Program&& s);

  ~Program();

public:
  GLuint id();
  void use();
  void release();

public:
  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const;
  void setVec2(const std::string &name, const glm::vec2 &value) const;
  void setVec3(const std::string &name, const glm::vec3 &value) const;
  void setVec4(const std::string &name, const glm::vec4 &value) const;
  void setMat2(const std::string &name, const glm::mat2 &mat) const;
  void setMat3(const std::string &name, const glm::mat3 &mat) const;
  void setMat4(const std::string &name, const glm::mat4 &mat) const;

}; // class Program

#endif // !WILT_PROGRAM_H
