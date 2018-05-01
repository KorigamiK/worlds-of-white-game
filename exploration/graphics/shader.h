#ifndef WILT_SHADER_H
#define WILT_SHADER_H

#include <glad/glad.h>

class Shader
{
private:
  GLuint _id;

public:
  Shader();
  explicit Shader(GLuint id);
  Shader(const Shader& s) = delete;
  Shader(Shader&& s);

  Shader& operator= (const Shader& s) = delete;
  Shader& operator= (Shader&& s);

  ~Shader();

public:
  GLuint id() const;
  void release();
  bool loaded() const;

public:
  static Shader fromMemory(const char* data, GLenum shaderType);
  static Shader fromFile(const char* filename, GLenum shaderType);

}; // class Shader

class VertexShader
{
public:
  static Shader fromMemory(const char* data) { return Shader::fromMemory(data, GL_VERTEX_SHADER); }
  static Shader fromFile(const char* filename) { return Shader::fromFile(filename, GL_VERTEX_SHADER); }
};

class GeometryShader
{
public:
  static Shader fromMemory(const char* data) { return Shader::fromMemory(data, GL_GEOMETRY_SHADER); }
  static Shader fromFile(const char* filename) { return Shader::fromFile(filename, GL_GEOMETRY_SHADER); }
};

class FragmentShader
{
public:
  static Shader fromMemory(const char* data) { return Shader::fromMemory(data, GL_FRAGMENT_SHADER); }
  static Shader fromFile(const char* filename) { return Shader::fromFile(filename, GL_FRAGMENT_SHADER); }
};

#endif // !WILT_SHADER_H
