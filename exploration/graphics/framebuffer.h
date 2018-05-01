#ifndef WILT_FRAMEBUFFER_H
#define WILT_FRAMEBUFFER_H

#include <glad/glad.h>

#include "texture.h"

class Framebuffer
{
private:
  GLuint _id;
  Texture _colorTexture;
  Texture _depthTexture;
  Texture _stencilTexture;

public:
  Framebuffer();
  explicit Framebuffer(GLuint id);
  explicit Framebuffer(Texture colorTexture);
  explicit Framebuffer(Texture colorTexture, Texture depthTexture);
  explicit Framebuffer(Texture colorTexture, Texture depthTexture, Texture stencilTexture);
  Framebuffer(const Framebuffer& s) = delete;
  Framebuffer(Framebuffer&& s);

  Framebuffer& operator= (const Framebuffer& s) = delete;
  Framebuffer& operator= (Framebuffer&& s);

  ~Framebuffer();

public:
  GLuint id() const;
  void release();
  bool loaded() const;

public:
  const Texture& colorTexture() const;
  const Texture& depthTexture() const;
  const Texture& stencilTexture() const;

public:
  void resize(GLsizei width, GLsizei height);

}; // class Framebuffer

#endif // !WILT_FRAMEBUFFER_H
