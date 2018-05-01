#include "program.h"

#include "../logging/LoggingManager.h"
namespace { auto logger = wilt::logging.createLogger("graphics-program"); }

Program::Program()
  : _id{ 0 }
{ }

Program::Program(GLuint id)
  : _id{ id }
{ }

Program::Program(Shader fragmentShader)
  : Program{ Shader{}, Shader{}, std::move(fragmentShader) }
{ }

Program::Program(Shader vertexShader, Shader fragmentShader)
  : Program{ std::move(vertexShader), Shader{}, std::move(fragmentShader) }
{ }

Program::Program(Shader vertexShader, Shader geometryShader, Shader fragmentShader)
  : _id{ 0 }
{
  _id = glCreateProgram();

  if (vertexShader.loaded())
    glAttachShader(_id, vertexShader.id());
  if (geometryShader.loaded())
    glAttachShader(_id, geometryShader.id());
  if (fragmentShader.loaded())
    glAttachShader(_id, fragmentShader.id());

  glLinkProgram(_id);

  GLint success;
  glGetProgramiv(_id, GL_LINK_STATUS, &success);
  if (!success)
  {
    GLchar error[1024];
    glGetProgramInfoLog(_id, 1024, NULL, error);
    logger.error(std::string("linking program: ") + error);
  }
}

Program::Program(Program&& s)
  : _id{ s._id }
{
  s._id = 0;
}

Program& Program::operator= (Program&& s)
{
  release();

  _id = s._id;
  s._id = 0;

  return *this;
}

Program::~Program()
{
  release();
}

GLuint Program::id()
{
  return _id;
}

void Program::use()
{
  glUseProgram(_id);
}

void Program::release()
{
  if (_id != 0)
  {
    glDeleteProgram(_id);
    _id = 0;
  }
}

void Program::setBool(const std::string &name, bool value) const
{
  glUniform1i(glGetUniformLocation(_id, name.c_str()), (int)value);
}

void Program::setInt(const std::string &name, int value) const
{
  glUniform1i(glGetUniformLocation(_id, name.c_str()), value);
}

void Program::setFloat(const std::string &name, float value) const
{
  glUniform1f(glGetUniformLocation(_id, name.c_str()), value);
}

void Program::setVec2(const std::string &name, const glm::vec2 &value) const
{
  glUniform2fv(glGetUniformLocation(_id, name.c_str()), 1, &value[0]);
}

void Program::setVec3(const std::string &name, const glm::vec3 &value) const
{
  glUniform3fv(glGetUniformLocation(_id, name.c_str()), 1, &value[0]);
}

void Program::setVec4(const std::string &name, const glm::vec4 &value) const
{
  glUniform4fv(glGetUniformLocation(_id, name.c_str()), 1, &value[0]);
}

void Program::setMat2(const std::string &name, const glm::mat2 &mat) const
{
  glUniformMatrix2fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Program::setMat3(const std::string &name, const glm::mat3 &mat) const
{
  glUniformMatrix3fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Program::setMat4(const std::string &name, const glm::mat4 &mat) const
{
  glUniformMatrix4fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
