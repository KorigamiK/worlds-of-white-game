#ifndef WILT_LINEPROGRAM_H
#define WILT_LINEPROGRAM_H

#include <array>
#include <vector>

#include "../program.h"
#include "../texture.h"

class LineProgram : public Program
{
private:
  GLint locationProjection;
  GLint locationView;
  GLint locationViewReference;
  GLint locationFrame;
  GLint locationPositions;
  GLint locationDrawPercentage;
  GLint locationModel;
  GLint locationRatio;
  GLint locationDepthTexture;
  GLint locationBurstLocations;
  GLint locationBurstRanges;
  GLint locationBurstCount;
  GLint locationCameraPosition;

  std::vector<glm::vec3> burstLocations;
  std::vector<float> burstRanges;

public:
  LineProgram(Shader vertexShader, Shader tessellationControlShader, Shader tessellationEvaluationShader, Shader geometryShader, Shader fragmentShader);

public:
  void use();

public:
  void setProjection(const glm::mat4 &mat) const;
  void setView(const glm::mat4 &mat) const;
  void setViewReference(const glm::vec3 &vec) const;
  void setFrame(float val) const;
  void setPositions(const std::array<glm::mat4, 24>& positions) const;
  void setDrawPercentage(float val) const;
  void setModel(const glm::mat4 &mat) const;
  void setRatio(float val) const;
  void setDepthTexture(const Texture& texture) const;
  void setCameraPosition(const glm::vec3 &vec) const;

  void addBurst(glm::vec3 location, float range);
  void reset();
};

#endif // !WILT_LINEPROGRAM_H
