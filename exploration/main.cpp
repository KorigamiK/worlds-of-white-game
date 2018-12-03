#define _CRT_SECURE_NO_WARNINGS

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

#include <btBulletDynamicsCommon.h>

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <numeric>
#include <cmath>

#include "logging/LoggingManager.h"
#include "logging/loggers/StreamLogger.h"
#include "graphics/program.h"
#include "graphics/texture.h"
#include "graphics/framebuffer.h"
#include "graphics/joint.h"
#include "graphics/jointPose.h"
#include "graphics/model.h"
#include "graphics/modelInstance.h"
#include "utilities/narray/narray.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 720;
bool paused = false;

float character_scale = 1.0f;

Framebuffer faceFramebuffer;
Framebuffer lineFramebuffer;

constexpr int MAX_JOINTS = 24;

struct AnimationFrame
{
  std::vector<JointPose> _poses;
};

struct Animation
{
  std::vector<AnimationFrame> _frames;
};

class StaticAnimator : public IAnimator
{
public:
  void applyAnimation(Program& program, float time, ModelInstance& instance) override
  {
    glm::mat4 joints[MAX_JOINTS];
    glUniformMatrix4fv(glGetUniformLocation(program.id(), "positions"), MAX_JOINTS, false, glm::value_ptr(joints[0]));
  }
};

class LoopAnimator : public IAnimator
{
public:
  Animation animation;
  float framesPerSecond;

public:
  LoopAnimator(Animation animation, float fps = 24.0f)
    : animation{ animation }
    , framesPerSecond{ fps }
  { }

public:
  void applyAnimation(Program& program, float time, ModelInstance& instance) override
  {
    auto& joints = instance.model->joints;

    float frame_pos = std::fmod(time * framesPerSecond, animation._frames.size());
    int frame1 = (int)frame_pos;
    int frame2 = (frame1 + 1) % animation._frames.size();
    float interlop = frame_pos - frame1;

    std::vector<glm::mat4> animatedTransforms(animation._frames[0]._poses.size());
    std::vector<glm::mat4> forwardTransforms(animation._frames[0]._poses.size());
    std::vector<glm::mat4> backwardTransforms(animation._frames[0]._poses.size());

    for (int i = 0; i < animation._frames[0]._poses.size(); ++i)
    {
      auto p = joints[i].parentIndex();
      auto forward = (p != -1) ? forwardTransforms[p] : glm::mat4();
      auto backward = (p != -1) ? backwardTransforms[p] : glm::mat4();

      JointPose interpolatedJointPose = JointPose::interpolate(animation._frames[frame1]._poses[i], animation._frames[frame2]._poses[i], interlop);
      glm::mat4 interpolatedTransform = interpolatedJointPose.calcLocalTransform();
      forwardTransforms[i] = forward * joints[i].transform() * interpolatedTransform;
      backwardTransforms[i] = glm::inverse(joints[i].transform()) * backward;
      animatedTransforms[i] = forwardTransforms[i] * backwardTransforms[i];
    }

    glUniformMatrix4fv(glGetUniformLocation(program.id(), "positions"), animatedTransforms.size(), false, glm::value_ptr(animatedTransforms[0]));
  }
};

//IAnimator* walk_animator;
//IAnimator* trudge_animator;
//IAnimator* stand_animator;

class CharacterInstance : public ModelInstance
{
public:
  using ModelInstance::ModelInstance;

  float speed = 0.0;
  glm::vec3 velocity{ 0, 1, 0 };

public:
  void update(GLFWwindow *window, float time) override
  {
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
      velocity = glm::vec3(glm::rotate(glm::mat4(), glm::radians(1.0f), { 0, 0, 1 }) * glm::vec4(velocity, 0.0));
      rotation += glm::radians(1.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
      velocity = glm::vec3(glm::rotate(glm::mat4(), glm::radians(-1.0f), { 0, 0, 1 }) * glm::vec4(velocity, 0.0));
      rotation -= glm::radians(1.0f);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
      if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
      {
        speed = 0.003f;
        //animator = trudge_animator;
      }
      else
      {
        speed = 0.0005f;
        //animator = walk_animator;
      }
    }
    else
    {
      speed = 0.0f;
      //animator = stand_animator;
    }

    position += velocity * speed;
  }
};

class SpiritInstance : public ModelInstance
{
public:
  using ModelInstance::ModelInstance;

  float height = 1.0f;

public:
  void update(GLFWwindow *window, float time) override
  {
    position += (glm::mat3)glm::rotate(glm::mat4(), rotation, { 0, 0, 1 }) * glm::vec3(0.005f, 0, 0);
    rotation += 0.005f;
    position.z = 1.0f + 0.5f * std::sin(time + rotation);
  }
};

Animation read_animation(std::string path)
{
  std::ifstream file(path);
  unsigned int frameCount;
  unsigned int boneCount;

  file >> frameCount >> boneCount;
  Animation ret;
  ret._frames = std::vector<AnimationFrame>{ frameCount, AnimationFrame{ std::vector<JointPose>{ boneCount } } };

  for (int i = 0; i < frameCount; ++i)
  {
    for (int j = 0; j < boneCount; ++j)
    {
      file >> ret._frames[i]._poses[j].location[0];
      file >> ret._frames[i]._poses[j].location[1];
      file >> ret._frames[i]._poses[j].location[2];

      file >> ret._frames[i]._poses[j].rotation[3]; // w
      file >> ret._frames[i]._poses[j].rotation[0]; // x
      file >> ret._frames[i]._poses[j].rotation[1]; // y
      file >> ret._frames[i]._poses[j].rotation[2]; // z
    }
  }

  return ret;
}

class ICamera
{
public:
  virtual void update(GLFWwindow *window, float time) = 0;
  virtual glm::mat4 transform() const = 0;
  virtual glm::vec3 position() const = 0;
};

class FollowCamera : public ICamera
{
public:
  ModelInstance** _instance;
  float distance = 3.0f;
  float offsetAngle = 0.0f;

public:
  FollowCamera(ModelInstance** instance)
    : _instance{ instance }
  { }

public:
  virtual void update(GLFWwindow *window, float time)
  {
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
      distance -= 0.01f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
      distance += 0.01f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
      offsetAngle += 0.01f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
      offsetAngle -= 0.01f;
  }

  virtual glm::mat4 transform() const
  {
    return glm::lookAt(position(), (*_instance)->position + glm::vec3(0, 0, 1), { 0, 0, 1 });
  }

  virtual glm::vec3 position() const
  {
    return (*_instance)->position + glm::vec3(glm::rotate(glm::mat4(), (*_instance)->rotation + offsetAngle, { 0, 0, 1 }) * glm::vec4(0, 4, 2, 1)) * distance;
  }
};

class TrackCamera : public ICamera
{
public:
  ModelInstance** _instance;

public:
  TrackCamera(ModelInstance** instance)
    : _instance{ instance }
  { }

public:
  virtual void update(GLFWwindow *window, float time)
  {

  }

  virtual glm::mat4 transform() const
  {
    return glm::lookAt({ 0, 0, 1 }, (*_instance)->position, { 0, 0, 1 });
  }

  virtual glm::vec3 position() const
  {
    return { 0, 0, 1 };
  }
};

class FreeCamera : public ICamera
{
private:
  glm::vec3 _location;
  glm::vec3 _direction;

  float CAMERA_SPEED = 0.05f;

public:
  FreeCamera()
    : _location { 0, -0.5, 1 }
    , _direction { 0, 1, 0 }
  { }

public:
  virtual void update(GLFWwindow *window, float time)
  {
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
      _location += _direction * CAMERA_SPEED;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
      _location -= _direction * CAMERA_SPEED;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
      _direction = glm::vec3(glm::rotate(glm::mat4(), glm::radians(30.0f) / 144.0f, { 0, 0, 1 }) * glm::vec4(_direction, 1.0f));
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
      _direction = glm::vec3(glm::rotate(glm::mat4(),-glm::radians(30.0f) / 144.0f, { 0, 0, 1 }) * glm::vec4(_direction, 1.0f));
  }

  virtual glm::mat4 transform() const
  {
    return glm::lookAt(_location, _location + _direction, { 0, 0, 1 });
  }

  virtual glm::vec3 position() const
  {
    return _location;
  }
};

void doCharacterDeformation(ModelInstance* character, btCollisionWorld* world)
{
  auto& characterModel = *character->model;
  auto characterRotation = character->rotation;
  auto characterScale = character->scale;
  auto characterPosition = btVector3(
    character->position.x,
    character->position.y,
    character->position.z);

  const auto DATA_COUNT_PER_VERTEX = 9;

  const auto POINT_COUNT = characterModel.vertexData.size() / 9;
  const auto POINT_DISTANCE_MAX = 2.0f;
  const auto POINT_WORLD_MARGIN = 0.0625f;

  auto pointAmounts = std::vector<float>(POINT_COUNT, 1.0f);
  auto pointBounds = std::vector<float>(POINT_COUNT, POINT_DISTANCE_MAX);

  auto probeAmounts = std::vector<float>(12, 1.0f);
  auto probeVectors = std::vector<btVector3>
  {
    {  0.0000f,  0.0000f,  1.0000f },
    {  0.8944f,  0.0000f,  0.4472f },
    {  0.2764f, -0.8507f,  0.4472f },
    { -0.7236f, -0.5257f,  0.4472f },
    { -0.7236f,  0.5257f,  0.4472f },
    {  0.2764f,  0.8507f,  0.4472f },
    { -0.2764f, -0.8507f, -0.4472f },
    {  0.7236f, -0.5257f, -0.4472f },
    {  0.7236f,  0.5257f, -0.4472f },
    { -0.2764f,  0.8507f, -0.4472f },
    { -0.8944f,  0.0000f, -0.4472f },
    {  0.0000f,  0.0000f, -1.0000f }
  };

  auto probeWeightFunction = [](float a)
  {
    static auto weights = std::vector<float>{ -0.025f, -0.020f, -0.015f, 0.00f, 0.005f, 0.01f, 0.01f, 0.005f, 0.00f, 0.00f, 0.00f, 0.00f };

    const auto WEIGHT_SCALING = 1.0f;

    auto index = int(a * 10);
    auto frac = a * 10 - index;
    return (weights[index] * (1 - frac) + weights[index + 1] * (frac)) * WEIGHT_SCALING;
  };

  auto newVertexData = characterModel.vertexData;
  for (int i = 0; i < newVertexData.size(); i += DATA_COUNT_PER_VERTEX)
  {
    // should be a unit vector
    auto point = btVector3(
      newVertexData[i + 0],
      newVertexData[i + 1],
      newVertexData[i + 2]);

    // these are in world space
    auto rayStart = characterPosition;
    auto rayEnd = characterPosition + point.rotate({ 0, 0, 1 }, characterRotation) * characterScale * (POINT_DISTANCE_MAX + POINT_WORLD_MARGIN);

    btCollisionWorld::AllHitsRayResultCallback result(rayStart, rayEnd);
    world->rayTest(rayStart, rayEnd, result);

    auto closestFraction = 1.0f;
    for (int j = 0; j < result.m_hitFractions.size(); ++j)
      if (result.m_hitFractions.at(j) < closestFraction)
        closestFraction = result.m_hitFractions.at(j);

    pointBounds[i / DATA_COUNT_PER_VERTEX] = closestFraction * (POINT_DISTANCE_MAX + POINT_WORLD_MARGIN) - POINT_WORLD_MARGIN;

    auto closestAmount = closestFraction * (POINT_DISTANCE_MAX + POINT_WORLD_MARGIN);
    if (closestAmount < 1.0f)
    {
      for (int j = 0; j < probeVectors.size(); ++j)
      {
        auto& ang = probeVectors[j];
        float PI = 3.14159265358979f;
        probeAmounts[j] += probeWeightFunction(point.angle(ang) / PI) * closestAmount;
      }
    }
  }

  for (int i = 0; i < newVertexData.size(); i += DATA_COUNT_PER_VERTEX)
  {
    auto pointBound = pointBounds[i / DATA_COUNT_PER_VERTEX];

    auto x = newVertexData[i + 0];
    auto y = newVertexData[i + 1];
    auto z = newVertexData[i + 2];
    auto g1 = newVertexData[i + 3];
    auto g2 = newVertexData[i + 4];
    auto g3 = newVertexData[i + 5];
    auto w1 = newVertexData[i + 6];
    auto w2 = newVertexData[i + 7];
    auto w3 = newVertexData[i + 8];

    auto amount =
      probeAmounts[g1] * w1 +
      probeAmounts[g2] * w2 +
      probeAmounts[g3] * w3;
    amount = amount < pointBound ? amount : pointBound;

    newVertexData[i + 0] *= amount;
    newVertexData[i + 1] *= amount;
    newVertexData[i + 2] *= amount;
  }

  glBindVertexArray(characterModel.vertexDataVAO);
  glBindBuffer(GL_ARRAY_BUFFER, characterModel.vertexDataVBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, newVertexData.size() * sizeof(float), newVertexData.data());
}

int main()
{
  wilt::logging.setLogger<wilt::StreamLogger>(std::cout);

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSwapInterval(1);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glEnable(GL_DEPTH_TEST);

  Program lineProgram{ 
    VertexShader::fromFile("shaders/line.vert.glsl"),
    TessellationControlShader::fromFile("shaders/line.tesc.glsl"),
    TessellationEvaluationShader::fromFile("shaders/line.tess.glsl"),
    GeometryShader::fromFile("shaders/line.geom.glsl"),
    FragmentShader::fromFile("shaders/line.frag.glsl")
  };
  Program depthProgram{
    VertexShader::fromFile("shaders/depth.vert.glsl"),
    FragmentShader::fromFile("shaders/depth.frag.glsl")
  };
  Program screenProgram{
    VertexShader::fromFile("shaders/screen.vert.glsl"),
    FragmentShader::fromFile("shaders/screen.frag.glsl")
  };

  auto testlandModel = Model::read("models/testland_model.txt", "models/testland_texture.jpg", 1.0f);
  auto ballModel = Model::read("models/spirit_model.txt", "models/spirit_texture.jpg", 1.0f);

  std::vector<ModelInstance*> instances =
  {
    new CharacterInstance(&ballModel,{ 0, 0, 0.5f + 0.001f },  glm::radians(90.0f), new StaticAnimator{}, 0.5f),
    new ModelInstance(&testlandModel, { 0, 0, 0 }, 0.0, new StaticAnimator{}, 1.0f)
  };

  // load models
  std::set<Model*> models;
  models.insert(&ballModel);
  for (auto instance : instances)
    models.insert(instance->model);
  for (auto model : models)
    model->load();

  // load quad
  float quadVertices[] = {
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
  };

  unsigned int quadVAO, quadVBO;
  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);
  glBindVertexArray(quadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

  faceFramebuffer = Framebuffer(
    Texture::fromMemory(NULL, GL_RGB, SCR_WIDTH, SCR_HEIGHT),
    Texture::fromMemory(NULL, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT)
  );

  lineFramebuffer = Framebuffer(
    Texture::fromMemory(NULL, GL_RGB, SCR_WIDTH, SCR_HEIGHT),
    Texture::fromMemory(NULL, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT)
  );

  Texture paperTexture = Texture::fromFile("models/paper_texture.jpg");
  paperTexture.setMinFilter(GL_LINEAR);
  paperTexture.setMagFilter(GL_LINEAR);

  int i = 0;
  float iterationsPerSecond = 144.0f;
  int selected_perlin = 0;
  glm::vec3 view_reference;

  int instanceIndex = 0;
  ModelInstance* currentInstance = instances[instanceIndex];
  ICamera* freeCam = new FreeCamera();
  ICamera* trackCam = new TrackCamera(&currentInstance);
  ICamera* followCam = new FollowCamera(&currentInstance);
  ICamera* cam = followCam;

  // create the physics world
  auto collisionConfiguration = new btDefaultCollisionConfiguration(); // I don't
  auto dispatcher = new	btCollisionDispatcher(collisionConfiguration); // know what
  auto broadphase = new btDbvtBroadphase();                            // these things
  auto solver = new btSequentialImpulseConstraintSolver;               // fucking do

  auto dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
  dynamicsWorld->setGravity(btVector3(0, 0, -10));

  bool canJump = false;
  dynamicsWorld->setInternalTickCallback(+[](btDynamicsWorld* world, float timeStep) -> void
  {
    int numManifolds = world->getDispatcher()->getNumManifolds();
    auto& canJump = *(bool*)world->getWorldUserInfo();
    canJump = false;

    for (int i = 0; i < numManifolds; ++i)
    {
      btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
      const btCollisionObject* obA = contactManifold->getBody0();
      const btCollisionObject* obB = contactManifold->getBody1();

      int numContacts = contactManifold->getNumContacts();
      if (numContacts > 0)
        canJump = true;

      for (int j = 0; j < numContacts; j++)
      {
        btManifoldPoint& pt = contactManifold->getContactPoint(j);
        if (pt.getDistance() < 0.f)
        {
          const btVector3& ptA = pt.getPositionWorldOnA();
          const btVector3& ptB = pt.getPositionWorldOnB();
          const btVector3& normalOnB = pt.m_normalWorldOnB;
        }
      }
    }

  }, &canJump);

  // create terrain
  auto terrainIndices = testlandModel.faceIndexes;
  auto terrainVertices = testlandModel.vertexData;
  auto terrainMesh = new btTriangleIndexVertexArray(terrainIndices.size() / 3, (int*)terrainIndices.data(), 3 * sizeof(int), terrainVertices.size() / 9, terrainVertices.data(), 9 * sizeof(float));
  auto terrainShape = new btBvhTriangleMeshShape(terrainMesh, false);
  auto terrainShapeScaled = new btScaledBvhTriangleMeshShape(terrainShape, btVector3(1, 1, 1));
  auto terrainMotionState = new btDefaultMotionState();
  auto terrainBody = new btRigidBody(0.0, terrainMotionState, terrainShapeScaled);
  terrainBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
  terrainBody->setFriction(0.95f);
  dynamicsWorld->addRigidBody(terrainBody);

  struct InstanceBinding {
    btRigidBody* body = nullptr;
    glm::vec3 offset = glm::vec3();
  };

  std::map<ModelInstance*, InstanceBinding> instancesToBodies;

  // create character
  auto characterShape = new btSphereShape(0.25);
  auto characterMotionState = new btDefaultMotionState(btTransform(btMatrix3x3::getIdentity(), btVector3(0, 0, currentInstance->position.z + 1.0f)));
  auto characterBody = new btRigidBody(1.0, characterMotionState, characterShape);
  characterBody->setAngularFactor(0);
  characterBody->setFriction(0.95f);
  characterBody->setDamping(0.5f, 0.0f);
  characterBody->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
  dynamicsWorld->addRigidBody(characterBody);
  instancesToBodies[instances[0]] = { characterBody, glm::vec3(0, 0, 0) };

  auto character = instances[0];
  while (!glfwWindowShouldClose(window))
  {
    float time = i / 144.0f;

    // input
    processInput(window);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
      cam = followCam;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
      cam = trackCam;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
      cam = freeCam;
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
    {
      instanceIndex = (instanceIndex != instances.size() - 1) ? instanceIndex + 1 : 0;
      currentInstance = instances[instanceIndex];
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
    {
      instanceIndex = (instanceIndex != 0) ? instanceIndex - 1 : instances.size() - 1;
      currentInstance = instances[instanceIndex];
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
      auto rotation = character->rotation;
      auto velocity = characterBody->getLinearVelocity();
      auto new_velocity = btMatrix3x3(btQuaternion(0, 0, rotation)) * btVector3(0, -3.0f, 0);
      new_velocity.setZ(velocity.z());
      characterBody->setLinearVelocity(new_velocity);
      characterBody->activate();
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
      auto rotation = character->rotation;
      auto velocity = characterBody->getLinearVelocity();
      auto new_velocity = btMatrix3x3(btQuaternion(0, 0, rotation)) * btVector3(0, 3.0f, 0);
      new_velocity.setZ(velocity.z());
      characterBody->setLinearVelocity(new_velocity);
      characterBody->activate();
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && canJump)
    {
      auto velocity = characterBody->getLinearVelocity();
      velocity.setZ(5.0f);
      characterBody->setLinearVelocity(velocity);
      characterBody->activate();
    }

    glm::mat4 view = cam->transform();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    if (!paused)
    {
      cam->update(window, time);
      for (auto& instance : instances)
      {
        instance->update(window, time);
        if (instancesToBodies[instance].body != nullptr)
        {
          btTransform bodyTransform;
          instancesToBodies[instance].body->getMotionState()->getWorldTransform(bodyTransform);
          btVector3 bodyPosition = bodyTransform.getOrigin();
          instance->position = glm::vec3(bodyPosition.x(), bodyPosition.y(), bodyPosition.z()) + instancesToBodies[instance].offset;
        }
      }

      dynamicsWorld->stepSimulation(1.0f / 144.0f);
    }

    doCharacterDeformation(character, dynamicsWorld);

    // render faces and depth
    depthProgram.use();
    depthProgram.setMat4("projection", projection);
    depthProgram.setMat4("view", view);
    depthProgram.setFloat("frame", i / 24);
    depthProgram.setVec3("view_reference", view_reference);

    glBindFramebuffer(GL_FRAMEBUFFER, faceFramebuffer.id());
    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto& instance : instances)
      instance->model->draw_faces(*instance, depthProgram, time);

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // render lines
    lineProgram.use();
    lineProgram.setMat4("projection", projection);
    lineProgram.setMat4("view", view);
    lineProgram.setFloat("frame", i / 24);
    lineProgram.setFloat("ratio", (float)SCR_WIDTH / (float)SCR_HEIGHT);
    lineProgram.setInt("depth_texture", 0);
    lineProgram.setVec3("view_reference", view_reference);

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, faceFramebuffer.depthTexture().id());

    glBindFramebuffer(GL_FRAMEBUFFER, lineFramebuffer.id());
    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto& instance : instances)
      instance->model->draw_lines(*instance, lineProgram, time);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // render depth
    screenProgram.use();
    screenProgram.setInt("face_texture", 0);
    screenProgram.setInt("line_texture", 1);
    screenProgram.setInt("bkgd_texture", 2);

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, faceFramebuffer.colorTexture().id());

    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, lineFramebuffer.colorTexture().id());

    glActiveTexture(GL_TEXTURE2);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, paperTexture.id());

    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();

    if (!paused)
    {
      i++;
      if (i % 24 == 0)
      {
        selected_perlin = (selected_perlin + (rand() % 7) + 1) % 8;
        view_reference = cam->position();
      }
    }
  }

  // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------
  glDeleteVertexArrays(1, &quadVAO);
  glDeleteBuffers(1, &quadVBO);

  // glfw: terminate, clearing all previously allocated GLFW resources.
  // ------------------------------------------------------------------
  glfwTerminate();
  return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
    paused = !paused;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  // make sure the viewport matches the new window dimensions; note that width and 
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);

  SCR_WIDTH = width;
  SCR_HEIGHT = height;

  // resize framebuffer textures
  faceFramebuffer.resize(width, height);
  lineFramebuffer.resize(width, height);
}