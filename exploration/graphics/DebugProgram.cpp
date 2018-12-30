#include "DebugProgram.h"

namespace
{
  float boxVertices[] {
    // x-axis lines
     1.0f,  1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,

    // y-axis lines
     1.0f,  1.0f,  1.0f,   1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,   1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,

    // z-axis lines
     1.0f,  1.0f,  1.0f,   1.0f,  1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,   1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,  -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,  -1.0f, -1.0f, -1.0f
  };
}

DebugProgram::DebugProgram(Shader vertexShader, Shader fragmentShader)
  : Program{ std::move(vertexShader), std::move(fragmentShader) }
{
  if (_id == 0)
    return;

  locationProjection = glGetUniformLocation(_id, "projection");
  locationView       = glGetUniformLocation(_id, "view");
  locationModel      = glGetUniformLocation(_id, "model");

  glGenVertexArrays(1, &boxVAO);
  glGenBuffers(1, &boxVBO);
  glBindVertexArray(boxVAO);
  glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), &boxVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
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

void DebugProgram::drawBox(const glm::mat4& transform) const
{
  setModel(transform);
  glBindVertexArray(boxVAO);
  glDrawArrays(GL_LINES, 0, 24);
}
