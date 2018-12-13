#ifndef WILT_TEXTURE_H
#define WILT_TEXTURE_H

#include <glad/glad.h>

class Texture
{
private:
  GLuint _id;
  GLint _format;
  GLenum _target;
  GLint _samples;

public:
  Texture();
  explicit Texture(GLuint id, GLint format, GLenum target = GL_TEXTURE_2D, GLint samples = 1);
  Texture(const Texture& s) = delete;
  Texture(Texture&& s);

  Texture& operator= (const Texture& s) = delete;
  Texture& operator= (Texture&& s);

  ~Texture();

public:
  GLuint id() const;
  GLint format() const;
  GLenum target() const;
  GLint samples() const;

  void release();
  bool loaded() const;

public:
  void resize(const char* data, GLsizei width, GLsizei height);

  void setMinFilter(GLint param);
  void setMagFilter(GLint param);
  void setWrapS(GLint param);
  void setWrapT(GLint param);

public:
  static Texture fromMemory(const char* data, GLint format, GLsizei width, GLsizei height, GLenum target = GL_TEXTURE_2D, GLint samples = 1);
  static Texture fromFile(const char* filename);

}; // class Texture

#endif // !WILT_TEXTURE_H
