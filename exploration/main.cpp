#define _CRT_SECURE_NO_WARNINGS

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtx/intersect.hpp>

#include <btBulletDynamicsCommon.h>

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <numeric>
#include <cmath>
#include <iomanip>

#include "Model.h"
#include "DecorationModel.h"
#include "GameState.h"
#include "EntitySpawnInfo.h"
#include "EntityType.h"
#include "logging/LoggingManager.h"
#include "logging/loggers/StreamLogger.h"
#include "graphics/program.h"
#include "graphics/texture.h"
#include "graphics/framebuffer.h"
#include "graphics/joint.h"
#include "graphics/jointPose.h"
#include "utilities/narray/narray.hpp"
#include "cameras/FollowCamera.h"
#include "cameras/TrackCamera.h"
#include "cameras/FreeCamera.h"
#include "entities/Entity.h"
#include "entities/PlayerEntity.h"
#include "entities/DecorationEntity.h"
#include "entities/SmashEffectEntity.h"

namespace { auto logger = wilt::logging.createLogger("main"); }

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 720;
bool paused = false;

Framebuffer faceFramebuffer;
Framebuffer lineFramebuffer;
Framebuffer debgFramebuffer;

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
  void applyAnimation(Program& program, float time, std::vector<Joint>& joints) override
  {
    glm::mat4 jointTransforms[MAX_JOINTS];
    glUniformMatrix4fv(glGetUniformLocation(program.id(), "positions"), MAX_JOINTS, false, glm::value_ptr(jointTransforms[0]));
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
  void applyAnimation(Program& program, float time, std::vector<Joint>& joints) override
  {
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

class Level
{
public:
  std::vector<EntitySpawnInfo> entities;

  static Level read(const std::string& filename)
  {
    Level level;

    std::ifstream file(filename);
    if (!file)
      return level;

    // read type
    std::string type;
    file >> type;
    if (type != "level")
      return level;

    // read version
    int version;
    file >> version;
    if (version != 1)
      return level;

    // read data
    int entityCount;
    file >> entityCount;
    level.entities.resize(entityCount);
    for (std::size_t i = 0; i < level.entities.size(); ++i)
    {
      file >> level.entities[i].name;
      file >> level.entities[i].location[0];
      file >> level.entities[i].location[1];
      file >> level.entities[i].location[2];
      file >> level.entities[i].rotation[0];
      file >> level.entities[i].rotation[1];
      file >> level.entities[i].rotation[2];
      file >> level.entities[i].scale[0];
      file >> level.entities[i].scale[1];
      file >> level.entities[i].scale[2];
    }

    return level;
  }
};

void logError(const std::string &name)
{
  auto error = glGetError();
  if (error == GL_NO_ERROR)
    return;

  switch (error)
  {
  case GL_INVALID_ENUM:
    logger.error("GL_INVALID_ENUM");
    logger.error(name);
    break;
  case GL_INVALID_VALUE:
    logger.error("GL_INVALID_VALUE");
    logger.error(name);
    break;
  case GL_INVALID_OPERATION:
    logger.error("GL_INVALID_OPERATION");
    logger.error(name);
    break;
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    logger.error("GL_INVALID_FRAMEBUFFER_OPERATION");
    logger.error(name);
    break;
  case GL_OUT_OF_MEMORY:
    logger.error("GL_OUT_OF_MEMORY");
    logger.error(name);
    break;
  case GL_STACK_UNDERFLOW:
    logger.error("GL_STACK_UNDERFLOW");
    logger.error(name);
    break;
  case GL_STACK_OVERFLOW:
    logger.error("GL_STACK_OVERFLOW");
    logger.error(name);
    break;
  default:
    logger.error("UNKNOWN");
    logger.error(name);
    break;
  }
}

int main()
{
  wilt::logging.setLogger<wilt::StreamLogger>(std::cout);
  wilt::logging.setLevel(wilt::LoggingLevel::DEBUG);

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
  Program debugProgram{
    VertexShader::fromFile("shaders/debug.vert.glsl"),
    FragmentShader::fromFile("shaders/debug.frag.glsl")
  };
  Program screenProgram{
    VertexShader::fromFile("shaders/screen.vert.glsl"),
    FragmentShader::fromFile("shaders/screen.frag.glsl")
  };

  // read in levels
  auto testLevel = Level::read("levels/testing_level.txt");
  auto floatingLevel = Level::read("levels/floating_level.txt");
  auto tempLevel = Level::read("levels/temp_level.txt");

  // read in entityTypes
  std::map<std::string, IEntityType*> entityTypes;
  entityTypes["_spawn"]         = new EntityType<PlayerEntity, Model>{ "models/player_model.txt" };
  entityTypes["tallgrass"]      = new EntityType<DecorationEntity, DecorationModel>{ "models/tallgrass_model.txt" };
  entityTypes["shortgrass"]     = new EntityType<DecorationEntity, DecorationModel>{ "models/shortgrass_model.txt" };
  entityTypes["tree"]           = new EntityType<DecorationEntity, DecorationModel>{ "models/tree_model.txt" };
  entityTypes["flower"]         = new EntityType<DecorationEntity, DecorationModel>{ "models/flower_model.txt" };
  entityTypes["ring"]           = new EntityType<SmashEffectEntity, Model>{ "models/ring_model.txt" };
  entityTypes["testland"]       = new EntityType<Entity, Model>{ "models/testland_model.txt" };
  entityTypes["floatingisland"] = new EntityType<Entity, Model>{ "models/floatingisland_model.txt" };
  entityTypes["temp"]           = new EntityType<Entity, Model>{ "models/temp_model.txt" };
  for (auto& [name, type] : entityTypes)
    type->read();

  // load level
  //auto& level = testLevel;
  //auto terrainModel = entityTypes["testland"]->getModel();
  auto& level = floatingLevel;
  auto terrainModel = entityTypes["floatingisland"]->getModel();
  //auto& level = tempLevel;
  //auto terrainModel = (Model*)nullptr;

  auto entities = std::vector<Entity*>();
  entities.reserve(100);
  for (auto& info : level.entities)
    entities.push_back(entityTypes[info.name]->spawn(info));

  // load entityTypes
  for (auto& [name, type] : entityTypes)
    type->load();

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

  // load box
  float boxVertices[]{
    // x-axis lines
     1.0f,  1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,

    // y-axis lines
     1.0f,  1.0f,  1.0f,   1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,   1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,

    // z-axis lines
     1.0f,  1.0f,  1.0f,   1.0f,  1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,   1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,  -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,  -1.0f, -1.0f, -1.0f
  };

  unsigned int boxVAO, boxVBO;
  glGenVertexArrays(1, &boxVAO);
  glGenBuffers(1, &boxVBO);
  glBindVertexArray(boxVAO);
  glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), &boxVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

  faceFramebuffer = Framebuffer(
    Texture::fromMemory(NULL, GL_RGB, SCR_WIDTH, SCR_HEIGHT),
    Texture::fromMemory(NULL, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT)
  );

  lineFramebuffer = Framebuffer(
    Texture::fromMemory(NULL, GL_RGBA8, SCR_WIDTH, SCR_HEIGHT, GL_TEXTURE_2D_MULTISAMPLE, 8)
  );

  debgFramebuffer = Framebuffer(
    Texture::fromMemory(NULL, GL_RGB, SCR_WIDTH, SCR_HEIGHT)
  );

  Texture paperTexture = Texture::fromFile("models/paper_texture.jpg");
  paperTexture.setMinFilter(GL_LINEAR);
  paperTexture.setMagFilter(GL_LINEAR);

  int i = 0;
  float iterationsPerSecond = 144.0f;
  int selected_perlin = 0;
  glm::vec3 view_reference;

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
  btBvhTriangleMeshShape* terrainShape;
  std::vector<unsigned int> terrainIndices;
  std::vector<float> terrainVertices;
  if (terrainModel)
  {
    terrainIndices = terrainModel->faceIndexes;
    terrainVertices = terrainModel->vertexData;
    auto terrainMesh = new btTriangleIndexVertexArray(terrainIndices.size() / 3, (int*)terrainIndices.data(), 3 * sizeof(int), terrainVertices.size() / Model::DATA_COUNT_PER_VERTEX, terrainVertices.data(), Model::DATA_COUNT_PER_VERTEX * sizeof(float));
    terrainShape = new btBvhTriangleMeshShape(terrainMesh, true);

    auto terrainMotionState = new btDefaultMotionState();
    auto terrainBody = new btRigidBody(0.0, terrainMotionState, terrainShape);
    terrainBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
    terrainBody->setFriction(0.95f);
    terrainShape->setMargin(0.0f);
    dynamicsWorld->addRigidBody(terrainBody);
  }
  else
  {
    terrainIndices = std::vector<unsigned int>{ 0, 1, 2 };
    terrainVertices = { 0, 0, 0, 2, 0, 0, 1, 1, 0 };
    auto terrainMesh = new btTriangleIndexVertexArray(1, (int*)terrainIndices.data(), 3 * sizeof(int), 3, terrainVertices.data(), 3 * sizeof(float));
    terrainShape = new btBvhTriangleMeshShape(terrainMesh, true);
  }

  // load player
  Entity* player = nullptr;
  for (auto entity : entities)
  {
    auto playerEntity = dynamic_cast<PlayerEntity*>(entity);
    if (playerEntity)
      player = playerEntity;
  }
  if (player == nullptr && entities.size() > 0)
    player = entities[0];
  if (player == nullptr)
    return -1;

  // load physics objects
  for (auto entity : entities)
  {
    auto physicsEntity = dynamic_cast<PhysicsEntity*>(entity);
    if (physicsEntity)
      dynamicsWorld->addRigidBody(physicsEntity->getBody());
  }

  // load camera
  int entityIndex = 0;
  Entity* currentEntity = entities[entityIndex];
  FreeCamera* freeCam = new FreeCamera();
  TrackCamera* trackCam = new TrackCamera(&player);
  FollowCamera* followCam = new FollowCamera(&player);
  ICamera* cam = followCam;

  auto printJoystickInfo = [&](int joystickId)
  {
    const char* name = glfwJoystickPresent(joystickId) ? glfwGetJoystickName(joystickId) : "<none>";
    std::cout << joystickId << ' ' << name << std::endl;

    auto buttonsCount = 0;
    auto buttons = glfwGetJoystickButtons(joystickId, &buttonsCount);
    std::cout << "  buttons:";
    for (int i = 0; i < buttonsCount; ++i)
      std::cout << ' ' << (int)buttons[i];
    std::cout << std::endl;

    auto axesCount = 0;
    auto axes = glfwGetJoystickAxes(joystickId, &axesCount);
    std::cout << "  axis   :";
    for (int i = 0; i < axesCount; ++i)
      std::cout << ' ' << axes[i];
    std::cout << std::endl;
  };

  //printJoystickInfo(GLFW_JOYSTICK_1);
  //printJoystickInfo(GLFW_JOYSTICK_2);
  //printJoystickInfo(GLFW_JOYSTICK_3);
  //printJoystickInfo(GLFW_JOYSTICK_4);

  auto selectedJoystickId = []() -> int
  {
    auto selectedJoystickId = -1;
    for (auto joystickId : { GLFW_JOYSTICK_1, GLFW_JOYSTICK_2, GLFW_JOYSTICK_3, GLFW_JOYSTICK_4 })
    {
      if (selectedJoystickId == -1 && glfwJoystickPresent(joystickId))
        selectedJoystickId = joystickId;

      const char* name = glfwJoystickPresent(joystickId) ? glfwGetJoystickName(joystickId) : "<none>";
      logger.debug(std::to_string(joystickId) + " " + name);
    }

    if (selectedJoystickId == -1)
      logger.info("no controller connected");
    else
      logger.info("using controller " + std::to_string(selectedJoystickId));

    return selectedJoystickId;
  } ();

  if (selectedJoystickId == -1)
    return -1;

  auto gameState = GameState{ window, selectedJoystickId, dynamicsWorld, terrainShape, &canJump, followCam, player->position, entityTypes, entities };

  auto maxFPS = 0.0f;
  auto minFPS = 1000.0f;
  auto totFPS = 0.0f;

  auto lastFrameTime = std::chrono::high_resolution_clock::now();
  auto currFrameTime = std::chrono::high_resolution_clock::now();

  while (!glfwWindowShouldClose(window))
  {
    float time = i / 144.0f;
    lastFrameTime = currFrameTime;
    currFrameTime = std::chrono::high_resolution_clock::now();

    // input
    processInput(window);

    //if (i % 15 == 0)
    //  printJoystickInfo(selectedJoystickId);

    auto fps = 1000000000.0f / (currFrameTime - lastFrameTime).count();
    totFPS += fps;
    if (fps > maxFPS)
      maxFPS = fps;
    if (fps < minFPS)
      minFPS = fps;

    if (i % 144 == 143)
    {
      std::cout << " avg: " << std::setw(7) << std::left << totFPS / 144;
      std::cout << " min: " << std::setw(7) << std::left << minFPS << std::endl;

      maxFPS = 0.0f;
      minFPS = 1000.0f;
      totFPS = 0.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
      cam = followCam;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
      cam = trackCam;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
      cam = freeCam;
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
    {
      entityIndex = (entityIndex != entities.size() - 1) ? entityIndex + 1 : 0;
      currentEntity = entities[entityIndex];
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
    {
      entityIndex = (entityIndex != 0) ? entityIndex - 1 : entities.size() - 1;
      currentEntity = entities[entityIndex];
    }

    if (!paused)
    {
      dynamicsWorld->stepSimulation(1.0f / 144.0f, 2, 1.0f / 120.0f);

      for (auto& entity : entities)
        entity->update(gameState, time);

      cam->update(gameState, time);

      for (auto& entity : gameState.removeList)
        entities.erase(std::find(entities.begin(), entities.end(), entity));
      gameState.removeList.clear();

      for (auto& entity : gameState.addList)
        entities.push_back(entity);
      gameState.addList.clear();
    }

    glm::mat4 view = cam->getTransform();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    // render depth
    depthProgram.use();
    depthProgram.setMat4("projection", projection);
    depthProgram.setMat4("view", view);
    depthProgram.setFloat("frame", i / 24);
    depthProgram.setVec3("view_reference", view_reference);

    glBindFramebuffer(GL_FRAMEBUFFER, faceFramebuffer.id());
    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto& entity : entities)
      entity->draw_faces(gameState, depthProgram, time);

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
    glBindTexture(GL_TEXTURE_2D, faceFramebuffer.depthTexture().id());

    glBindFramebuffer(GL_FRAMEBUFFER, lineFramebuffer.id());
    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto& entity : entities)
      entity->draw_lines(gameState, lineProgram, time);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // render debug
    debugProgram.use();
    debugProgram.setMat4("projection", projection);
    debugProgram.setMat4("view", view);

    glBindFramebuffer(GL_FRAMEBUFFER, debgFramebuffer.id());
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (auto& entity : entities)
      entity->draw_debug(gameState, debugProgram, time);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // render to screen
    screenProgram.use();
    screenProgram.setInt("face_texture", 0);
    screenProgram.setInt("line_texture", 1);
    screenProgram.setInt("line_texture_samples", lineFramebuffer.colorTexture().samples());
    screenProgram.setInt("bkgd_texture", 2);
    screenProgram.setInt("debg_texture", 3);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(faceFramebuffer.colorTexture().target(), faceFramebuffer.colorTexture().id());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(lineFramebuffer.colorTexture().target(), lineFramebuffer.colorTexture().id());

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, paperTexture.id());

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(debgFramebuffer.colorTexture().target(), debgFramebuffer.colorTexture().id());

    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glfwSwapBuffers(window);
    glfwPollEvents();
    logError("any");

    if (!paused)
    {
      i++;
      if (i % 24 == 0)
      {
        selected_perlin = (selected_perlin + (rand() % 7) + 1) % 8;
        view_reference = cam->getPosition();
      }
    }
  }

  glDeleteVertexArrays(1, &quadVAO);
  glDeleteBuffers(1, &quadVBO);

  glfwTerminate();
  return 0;
}

void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
    paused = !paused;
}

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
  debgFramebuffer.resize(width, height);
}