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
{ }

Texture::Texture(GLuint id, GLint format)
  : _id{ id }
  , _format{ format }
{ }

Texture::Texture(Texture&& s)
  : _id{ s._id }
  , _format{ s._format }
{
  s._id = 0;
  s._format = GL_NONE;
}

Texture& Texture::operator= (Texture&& s)
{
  release();

  _id = s._id;
  _format = s._format;
  s._id = 0;
  s._format = GL_NONE;

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

void Texture::release()
{
  if (_id != 0)
  {
    glDeleteTextures(1, &_id);
    _id = 0;
    _format = GL_NONE;
  }
}

bool Texture::loaded() const
{
  return _id != 0;
}

void Texture::resize(const char* data, GLsizei width, GLsizei height)
{
  glBindTexture(GL_TEXTURE_2D, _id);
  glTexImage2D(GL_TEXTURE_2D, 0, _format, width, height, 0, _format, GL_UNSIGNED_BYTE, data);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setMinFilter(GLint value)
{
  glBindTexture(GL_TEXTURE_2D, _id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, value);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setMagFilter(GLint value)
{
  glBindTexture(GL_TEXTURE_2D, _id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, value);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setWrapS(GLint value)
{
  glBindTexture(GL_TEXTURE_2D, _id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, value);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setWrapT(GLint value)
{
  glBindTexture(GL_TEXTURE_2D, _id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, value);
  glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint load(const char* source, const char* data, GLint format, GLsizei width, GLsizei height)
{
  GLuint id;
  glCreateTextures(GL_TEXTURE_2D, 1, &id);

  glBindTexture(GL_TEXTURE_2D, id);
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  glBindTexture(GL_TEXTURE_2D, 0);

  return id;
}

Texture Texture::fromMemory(const char* data, GLint format, GLsizei width, GLsizei height)
{
  return Texture{ load("<memory>", data, format, width, height), format };
}

Texture Texture::fromFile(const char* filename)
{
  cimg_library::CImg<unsigned char> texture;
  texture.load_jpeg(filename);

  wilt::NArray<unsigned char, 3>textureData = { { 3, texture.width(), texture.height() }, texture.data(), wilt::PTR::REF };
  textureData = textureData.t(0, 1).t(1, 2).clone();

  return Texture{ load(filename, (const char*)textureData._basePtr(), GL_RGB, textureData.width(), textureData.height()), GL_RGB };
}
