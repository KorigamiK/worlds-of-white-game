#include "DepthProgram.h"

DepthProgram::DepthProgram(Shader vertexShader, Shader fragmentShader)
  : Program{ std::move(vertexShader), std::move(fragmentShader) }
{
  if (_id == 0)
    return;

  locationProjection     = glGetUniformLocation(_id, "projection");
  locationView           = glGetUniformLocation(_id, "view");
  locationViewReference  = glGetUniformLocation(_id, "viewReference");
  locationFrame          = glGetUniformLocation(_id, "frame");
  locationPositions      = glGetUniformLocation(_id, "positions");
  locationDrawPercentage = glGetUniformLocation(_id, "draw_percentage");
  locationModel          = glGetUniformLocation(_id, "model");
}

void DepthProgram::setProjection(const glm::mat4& mat) const
{
  glUniformMatrix4fv(locationProjection, 1, GL_FALSE, &mat[0][0]);
}

void DepthProgram::setView(const glm::mat4& mat) const
{
  glUniformMatrix4fv(locationView, 1, GL_FALSE, &mat[0][0]);
}

void DepthProgram::setViewReference(const glm::vec3& vec) const
{
  glUniform3fv(locationViewReference, 1, &vec[0]);
}

void DepthProgram::setFrame(float val) const
{
  glUniform1f(locationFrame, val);
}

void DepthProgram::setPositions(const std::array<glm::mat4, 24>& positions) const
{
  glUniformMatrix4fv(locationPositions, 24, GL_FALSE, &positions[0][0][0]);
}

void DepthProgram::setDrawPercentage(float val) const
{
  glUniform1f(locationDrawPercentage, val);
}

void DepthProgram::setModel(const glm::mat4& mat) const
{
  glUniformMatrix4fv(locationModel, 1, GL_FALSE, &mat[0][0]);
}
