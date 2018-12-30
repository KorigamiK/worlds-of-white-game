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
  : Program{ Shader{}, Shader{}, Shader{}, Shader{}, std::move(fragmentShader) }
{ }

Program::Program(Shader vertexShader, Shader fragmentShader)
  : Program{ std::move(vertexShader), Shader{}, Shader{}, Shader{}, std::move(fragmentShader) }
{ }

Program::Program(Shader vertexShader, Shader geometryShader, Shader fragmentShader)
  : Program{ std::move(vertexShader), Shader{}, Shader{}, std::move(geometryShader), std::move(fragmentShader) }
{ }

Program::Program(Shader vertexShader, Shader tessellationControlShader, Shader tessellationEvaluationShader, Shader geometryShader, Shader fragmentShader)
  : _id{ 0 }
{
  _id = glCreateProgram();

  if (vertexShader.loaded())
    glAttachShader(_id, vertexShader.id());
  if (geometryShader.loaded())
    glAttachShader(_id, geometryShader.id());
  if (fragmentShader.loaded())
    glAttachShader(_id, fragmentShader.id());
  if (tessellationControlShader.loaded())
    glAttachShader(_id, tessellationControlShader.id());
  if (tessellationEvaluationShader.loaded())
    glAttachShader(_id, tessellationEvaluationShader.id());

  glLinkProgram(_id);

  GLint success;
  glGetProgramiv(_id, GL_LINK_STATUS, &success);
  if (!success)
  {
    GLchar error[1024];
    glGetProgramInfoLog(_id, 1024, NULL, error);
    logger.error(std::string("linking program: ") + error);
    glDeleteProgram(_id);
    _id = 0;
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

namespace {
  // TODO: move this to a utility function or something

  void logError(const std::string &name)
  {
    auto error = glGetError();
    if (error == GL_NO_ERROR)
      return;

    switch (error)
    {
    case GL_INVALID_ENUM:
      logger.error("GL_INVALID_ENUM");
      logger.error(name);
      break;
    case GL_INVALID_VALUE:
      logger.error("GL_INVALID_VALUE");
      logger.error(name);
      break;
    case GL_INVALID_OPERATION:
      logger.error("GL_INVALID_OPERATION");
      logger.error(name);
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      logger.error("GL_INVALID_FRAMEBUFFER_OPERATION");
      logger.error(name);
      break;
    case GL_OUT_OF_MEMORY:
      logger.error("GL_OUT_OF_MEMORY");
      logger.error(name);
      break;
    case GL_STACK_UNDERFLOW:
      logger.error("GL_STACK_UNDERFLOW");
      logger.error(name);
      break;
    case GL_STACK_OVERFLOW:
      logger.error("GL_STACK_OVERFLOW");
      logger.error(name);
      break;
    default:
      logger.error("UNKNOWN");
      logger.error(name);
      break;
    }
  }
}

void Program::setBool(const std::string &name, bool value) const
{
  glUniform1i(glGetUniformLocation(_id, name.c_str()), (int)value);
  logError(name);
}

void Program::setInt(const std::string &name, int value) const
{
  glUniform1i(glGetUniformLocation(_id, name.c_str()), value);
  logError(name);
}

void Program::setFloat(const std::string &name, float value) const
{
  glUniform1f(glGetUniformLocation(_id, name.c_str()), value);
  logError(name);
}

void Program::setVec2(const std::string &name, const glm::vec2 &value) const
{
  glUniform2fv(glGetUniformLocation(_id, name.c_str()), 1, &value[0]);
  logError(name);
}

void Program::setVec3(const std::string &name, const glm::vec3 &value) const
{
  glUniform3fv(glGetUniformLocation(_id, name.c_str()), 1, &value[0]);
  logError(name);
}

void Program::setVec4(const std::string &name, const glm::vec4 &value) const
{
  glUniform4fv(glGetUniformLocation(_id, name.c_str()), 1, &value[0]);
  logError(name);
}

void Program::setMat2(const std::string &name, const glm::mat2 &mat) const
{
  glUniformMatrix2fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
  logError(name);
}

void Program::setMat3(const std::string &name, const glm::mat3 &mat) const
{
  glUniformMatrix3fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
  logError(name);
}

void Program::setMat4(const std::string &name, const glm::mat4 &mat) const
{
  glUniformMatrix4fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
  logError(name);
}
