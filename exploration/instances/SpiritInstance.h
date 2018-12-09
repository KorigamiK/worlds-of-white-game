#ifndef WILT_SPIRITINSTANCE_H
#define WILT_SPIRITINSTANCE_H

#include "Instance.h"

class SpiritInstance : public Instance
{
public:
  using Instance::Instance;

  float height = 1.0f;

public:
  // Instance overrides
  void update(GameState& state, float time) override;
};

#endif // !WILT_SPIRITINSTANCE_H
