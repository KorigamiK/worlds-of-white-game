#include "ScreenProgram.h"

namespace
{
  float quadVertices[] = {
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
  };
}

ScreenProgram::ScreenProgram(Shader vertexShader, Shader fragmentShader)
  : Program{ std::move(vertexShader), std::move(fragmentShader) }
{
  if (_id == 0)
    return;

  locationFaceTexture        = glGetUniformLocation(_id, "face_texture");
  locationDepthTexture       = glGetUniformLocation(_id, "depth_texture");
  locationLineTexture        = glGetUniformLocation(_id, "line_texture");
  locationLineTextureSamples = glGetUniformLocation(_id, "line_texture_samples");
  locationDebugTexture       = glGetUniformLocation(_id, "bkgd_texture");
  locationBackgroundTexture  = glGetUniformLocation(_id, "debg_texture");

  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);
  glBindVertexArray(quadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

ScreenProgram::~ScreenProgram()
{
  glDeleteVertexArrays(1, &quadVAO);
  glDeleteBuffers(1, &quadVBO);
}

void ScreenProgram::setFaceTexture(const Texture& texture) const
{
  glUniform1i(locationFaceTexture, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(texture.target(), texture.id());
}

void ScreenProgram::setDepthTexture(const Texture& texture) const
{
  glUniform1i(locationDepthTexture, 4);
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(texture.target(), texture.id());
}

void ScreenProgram::setLineTexture(const Texture& texture) const
{
  glUniform1i(locationLineTexture, 1);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(texture.target(), texture.id());

  glUniform1i(locationLineTextureSamples, texture.samples());
}

void ScreenProgram::setDebugTexture(const Texture& texture) const
{
  glUniform1i(locationDebugTexture, 3);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(texture.target(), texture.id());
}

void ScreenProgram::setBackgroundTexture(const Texture& texture) const
{
  glUniform1i(locationBackgroundTexture, 2);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(texture.target(), texture.id());
}

void ScreenProgram::drawScreen() const
{
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}
