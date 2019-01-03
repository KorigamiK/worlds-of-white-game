#ifndef WILT_SCREENPROGRAM_H
#define WILT_SCREENPROGRAM_H

#include <array>

#include "../program.h"
#include "../texture.h"

class ScreenProgram : public Program
{
private:
  GLint locationFaceTexture;
  GLint locationDepthTexture;
  GLint locationLineTexture;
  GLint locationLineTextureSamples;
  GLint locationDebugTexture;
  GLint locationBackgroundTexture;

  GLuint quadVAO;
  GLuint quadVBO;

public:
  ScreenProgram(Shader vertexShader, Shader fragmentShader);

  ~ScreenProgram();

public:
  void setFaceTexture(const Texture& texture) const;
  void setDepthTexture(const Texture& texture) const;
  void setLineTexture(const Texture& texture) const;
  void setDebugTexture(const Texture& texture) const;
  void setBackgroundTexture(const Texture& texture) const;

public:
  void drawScreen() const;
};

#endif // !WILT_SCREENPROGRAM_H
