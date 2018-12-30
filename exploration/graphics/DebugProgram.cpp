#include "DebugProgram.h"

DebugProgram::DebugProgram(Shader vertexShader, Shader fragmentShader)
  : Program{ std::move(vertexShader), std::move(fragmentShader) }
{
  if (_id == 0)
    return;

  locationProjection = glGetUniformLocation(_id, "projection");
  locationView       = glGetUniformLocation(_id, "view");
  locationModel      = glGetUniformLocation(_id, "model");
}

void DebugProgram::setProjection(const glm::mat4& mat) const
{
  glUniformMatrix4fv(locationProjection, 1, GL_FALSE, &mat[0][0]);
}

void DebugProgram::setView(const glm::mat4& mat) const
{
  glUniformMatrix4fv(locationView, 1, GL_FALSE, &mat[0][0]);
}

void DebugProgram::setModel(const glm::mat4& mat) const
{
  glUniformMatrix4fv(locationModel, 1, GL_FALSE, &mat[0][0]);
}
