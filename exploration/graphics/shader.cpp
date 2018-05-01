#include "shader.h"

#include <fstream>
#include <sstream>

#include "../logging/LoggingManager.h"
namespace { auto logger = wilt::logging.createLogger("graphics-shader"); }

Shader::Shader()
  : _id{ 0 }
{ }

Shader::Shader(GLuint id)
  : _id{ id }
{ }

Shader::Shader(Shader&& s)
  : _id{ s._id }
{
  s._id = 0;
}

Shader& Shader::operator= (Shader&& s)
{
  release();

  _id = s._id;
  s._id = 0;

  return *this;
}

Shader::~Shader()
{
  release();
}

GLuint Shader::id() const
{
  return _id;
}

void Shader::release()
{
  if (_id != 0)
  {
    glDeleteShader(_id);
    _id = 0;
  }
}

bool Shader::loaded() const
{
  return _id != 0;
}

GLuint load(const char* source, const char* data, GLenum shaderType)
{
  GLuint id = glCreateShader(shaderType);
  glShaderSource(id, 1, &data, NULL);
  glCompileShader(id);

  GLint success;
  glGetShaderiv(id, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    GLchar error[1024];
    glGetShaderInfoLog(id, sizeof(error), NULL, error);
    logger.error(std::string("compiling shader from \'") + source + "\': " + error);
  }

  return id;
}

Shader Shader::fromMemory(const char* data, GLenum shaderType)
{
  return Shader{ load("<memory>", data, shaderType) };
}

Shader Shader::fromFile(const char* filename, GLenum shaderType)
{
  std::ifstream file(filename);
  std::stringstream ss;
  std::string data;
  ss << file.rdbuf();
  data = ss.str();
  return Shader{ load(filename, data.data(), shaderType) };
}
