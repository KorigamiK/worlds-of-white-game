#include "texture.h"

#include <fstream>
#include <sstream>

#include <cimg/cimg.h>

#include "../logging/LoggingManager.h"
#include "../utilities/narray/narray.hpp"
namespace { auto logger = wilt::logging.createLogger("graphics-shader"); }

Texture::Texture()
  : _id{ 0 }
  , _format{ GL_NONE }
  , _target{ GL_NONE }
{ }

Texture::Texture(GLuint id, GLint format, GLenum target)
  : _id{ id }
  , _format{ format }
  , _target{ target }
{ }

Texture::Texture(Texture&& s)
  : _id{ s._id }
  , _format{ s._format }
  , _target{ s._target }
{
  s._id = 0;
  s._format = GL_NONE;
  s._target = GL_NONE;
}

Texture& Texture::operator= (Texture&& s)
{
  release();

  _id = s._id;
  _format = s._format;
  _target = s._target;

  s._id = 0;
  s._format = GL_NONE;
  s._target = GL_NONE;

  return *this;
}

Texture::~Texture()
{
  release();
}

GLuint Texture::id() const
{
  return _id;
}

GLint Texture::format() const
{
  return _format;
}

GLenum Texture::target() const
{
  return _target;
}

void Texture::release()
{
  if (_id != 0)
  {
    glDeleteTextures(1, &_id);
    _id = 0;
    _format = GL_NONE;
    _target = GL_NONE;
  }
}

bool Texture::loaded() const
{
  return _id != 0;
}

void Texture::resize(const char* data, GLsizei width, GLsizei height)
{
  glBindTexture(_target, _id);
  if (_target == GL_TEXTURE_2D_MULTISAMPLE)
    glTexImage2DMultisample(_target, 4, _format, width, height, false);
  else
    glTexImage2D(_target, 0, _format, width, height, 0, _format, GL_UNSIGNED_BYTE, data);
  glBindTexture(_target, 0);
}

void Texture::setMinFilter(GLint value)
{
  if (_target == GL_TEXTURE_2D_MULTISAMPLE)
    return;

  glBindTexture(_target, _id);
  glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, value);
  glBindTexture(_target, 0);
}

void Texture::setMagFilter(GLint value)
{
  if (_target == GL_TEXTURE_2D_MULTISAMPLE)
    return;

  glBindTexture(_target, _id);
  glTexParameteri(_target, GL_TEXTURE_MAG_FILTER, value);
  glBindTexture(_target, 0);
}

void Texture::setWrapS(GLint value)
{
  if (_target == GL_TEXTURE_2D_MULTISAMPLE)
    return;

  glBindTexture(_target, _id);
  glTexParameteri(_target, GL_TEXTURE_WRAP_S, value);
  glBindTexture(_target, 0);
}

void Texture::setWrapT(GLint value)
{
  if (_target == GL_TEXTURE_2D_MULTISAMPLE)
    return;

  glBindTexture(_target, _id);
  glTexParameteri(_target, GL_TEXTURE_WRAP_T, value);
  glBindTexture(_target, 0);
}

GLuint load(const char* source, const char* data, GLint format, GLsizei width, GLsizei height, GLenum target)
{
  GLuint id;
  glCreateTextures(target, 1, &id);

  glBindTexture(target, id);
  if (target == GL_TEXTURE_2D_MULTISAMPLE)
    glTexImage2DMultisample(target, 4, format, width, height, false);
  else
    glTexImage2D(target, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  glBindTexture(target, 0);

  return id;
}

Texture Texture::fromMemory(const char* data, GLint format, GLsizei width, GLsizei height, GLenum target)
{
  return Texture{ load("<memory>", data, format, width, height, target), format, target };
}

Texture Texture::fromFile(const char* filename)
{
  cimg_library::CImg<unsigned char> texture;
  texture.load_jpeg(filename);

  wilt::NArray<unsigned char, 3>textureData = { { 3, texture.width(), texture.height() }, texture.data(), wilt::PTR::REF };
  textureData = textureData.t(0, 1).t(1, 2).clone();

  return Texture{ load(filename, (const char*)textureData._basePtr(), GL_RGB, textureData.width(), textureData.height(), GL_TEXTURE_2D), GL_RGB };
}
