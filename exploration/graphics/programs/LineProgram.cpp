#include "LineProgram.h"

LineProgram::LineProgram(Shader vertexShader, Shader tessellationControlShader, Shader tessellationEvaluationShader, Shader geometryShader, Shader fragmentShader)
  : Program{ std::move(vertexShader), std::move(tessellationControlShader), std::move(tessellationEvaluationShader), std::move(geometryShader), std::move(fragmentShader) }
{
  if (_id == 0)
    return;

  locationProjection     = glGetUniformLocation(_id, "projection");
  locationView           = glGetUniformLocation(_id, "view");
  locationViewReference  = glGetUniformLocation(_id, "view_reference");
  locationFrame          = glGetUniformLocation(_id, "frame");
  locationPositions      = glGetUniformLocation(_id, "positions");
  locationDrawPercentage = glGetUniformLocation(_id, "draw_percentage");
  locationModel          = glGetUniformLocation(_id, "model");
  locationRatio          = glGetUniformLocation(_id, "ratio");
  locationDepthTexture   = glGetUniformLocation(_id, "depth_texture");
  locationBurstLocations = glGetUniformLocation(_id, "burst_locations");
  locationBurstRanges    = glGetUniformLocation(_id, "burst_ranges");
  locationBurstCount     = glGetUniformLocation(_id, "burst_count");
}

void LineProgram::use()
{
  Program::use();

  auto getBurstAmount = [](float f)
  {
    static float weights[12] = { 0.0f, 1.0f, 0.60f, 0.35f, 0.20f, 0.15f, 0.10f, 0.05f, 0.0f, 0.0f, 0.0f, 0.0f };

    auto index = int(f * 10);
    auto frac = f * 10 - index;
    return (weights[index] * (1 - frac) + weights[index + 1] * (frac));
  };

  static int i = 0;
  i = (i + 1) % 72;

  std::array<glm::vec3, 8> burstLocations = { glm::vec3{ 0, 0, 1 } };
  std::array<float, 8> burstRanges = { 5.0f * getBurstAmount(i / 72.0f) };
  
  glUniform3fv(locationBurstLocations, 8, &burstLocations[0][0]);
  glUniform1fv(locationBurstRanges, 8, &burstRanges[0]);
  glUniform1f(locationBurstCount, 1);
}

void LineProgram::setProjection(const glm::mat4& mat) const
{
  glUniformMatrix4fv(locationProjection, 1, GL_FALSE, &mat[0][0]);
}

void LineProgram::setView(const glm::mat4& mat) const
{
  glUniformMatrix4fv(locationView, 1, GL_FALSE, &mat[0][0]);
}

void LineProgram::setViewReference(const glm::vec3& vec) const
{
  glUniform3fv(locationViewReference, 1, &vec[0]);
}

void LineProgram::setFrame(float val) const
{
  glUniform1f(locationFrame, val);
}

void LineProgram::setPositions(const std::array<glm::mat4, 24>& positions) const
{
  glUniformMatrix4fv(locationPositions, 24, GL_FALSE, &positions[0][0][0]);
}

void LineProgram::setDrawPercentage(float val) const
{
  glUniform1f(locationDrawPercentage, val);
}

void LineProgram::setModel(const glm::mat4& mat) const
{
  glUniformMatrix4fv(locationModel, 1, GL_FALSE, &mat[0][0]);
}

void LineProgram::setRatio(float val) const
{
  glUniform1f(locationRatio, val);
}

void LineProgram::setDepthTexture(const Texture& texture) const
{
  glUniform1i(locationDepthTexture, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture.id());
}
