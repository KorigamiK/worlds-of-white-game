#ifndef WILT_ANIMATEDINSTANCE_H
#define WILT_ANIMATEDINSTANCE_H

#include "Instance.h"
#include "../graphics/IAnimator.h"

class AnimatedInstance : public Instance
{
public:
  IAnimator* animator;

public:
  AnimatedInstance(Model* model, glm::vec3 position, float rotation, IAnimator* animator, float scale = 1.0f);

public:
  // Instance overrides
  void draw_faces(Program& program, float time) override;
  void draw_lines(Program& program, float time) override;
  void draw_debug(Program& program, float time) override;

}; // class AnimatedInstance

#endif // !WILT_ANIMATEDINSTANCE_H
