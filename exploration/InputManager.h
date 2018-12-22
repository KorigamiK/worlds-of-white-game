#ifndef WILT_INPUTMANAGER_H
#define WILT_INPUTMANAGER_H

#include <vector>

class InputManager
{
private:
  std::vector<unsigned char> keysCurrent;
  std::vector<unsigned char> keysPrevious;
  std::vector<unsigned char> buttonsCurrent;
  std::vector<unsigned char> buttonsPrevious;
  std::vector<float> axes;

  int joystickId;

public:
  InputManager(int joystickId);

public:
  void update();

public:
  bool isKeyTriggered(int key);
  bool isKeyHeld(int key);
  bool isKeyReleased(int key);

  bool isButtonTriggered(int button);
  bool isButtonHeld(int button);
  bool isButtonReleased(int button);

  float getAxis(int axis);

public:
  void setKeyState(int key, int state);
};

#endif // !WILT_INPUTMANAGER_H
