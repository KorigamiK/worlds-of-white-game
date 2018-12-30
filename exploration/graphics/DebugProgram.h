#ifndef WILT_DEBUGPROGRAM_H
#define WILT_DEBUGPROGRAM_H

#include <array>

#include "program.h"

class DebugProgram : public Program
{
private:
  GLint locationProjection;
  GLint locationView;
  GLint locationModel;

  GLuint boxVAO;
  GLuint boxVBO;

public:
  DebugProgram(Shader vertexShader, Shader fragmentShader);

public:
  void setProjection(const glm::mat4 &mat) const;
  void setView(const glm::mat4 &mat) const;
  void setModel(const glm::mat4 &mat) const;

public:
  void drawBox(const glm::mat4& transform) const;
};

#endif // !WILT_DEBUGPROGRAM_H
