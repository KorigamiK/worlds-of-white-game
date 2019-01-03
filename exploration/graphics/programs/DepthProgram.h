#ifndef WILT_DEPTHPROGRAM_H
#define WILT_DEPTHPROGRAM_H

#include <array>

#include "../program.h"

class DepthProgram : public Program
{
private:
  GLint locationProjection;
  GLint locationView;
  GLint locationViewReference;
  GLint locationFrame;
  GLint locationPositions;
  GLint locationDrawPercentage;
  GLint locationModel;

public:
  DepthProgram(Shader vertexShader, Shader geometryShader, Shader fragmentShader);

public:
  void setProjection(const glm::mat4 &mat) const;
  void setView(const glm::mat4 &mat) const;
  void setViewReference(const glm::vec3 &vec) const;
  void setFrame(float val) const;
  void setPositions(const std::array<glm::mat4, 24>& positions) const;
  void setDrawPercentage(float val) const;
  void setModel(const glm::mat4 &mat) const;
};

#endif // !WILT_DEPTHPROGRAM_H
