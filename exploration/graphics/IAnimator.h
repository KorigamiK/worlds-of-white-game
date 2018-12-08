#ifndef WILT_IANIMATOR_H
#define WILT_IANIMATOR_H

#include <vector>

#include "program.h"
#include "joint.h"

class IAnimator
{
public:
  virtual void applyAnimation(Program& program, float time, std::vector<Joint>& joints) = 0;

}; // class IAnimator

#endif // !WILT_IANIMATOR_H
