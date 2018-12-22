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

public:
  static int KEY_W;
  static int KEY_A;
  static int KEY_S;
  static int KEY_D;
  static int KEY_SPACE;
  static int KEY_UP;
  static int KEY_DOWN;
  static int KEY_LEFT;
  static int KEY_RIGHT;
};

#endif // !WILT_INPUTMANAGER_H
