#define _CRT_SECURE_NO_WARNINGS

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "logging/LoggingManager.h"
#include "logging/loggers/StreamLogger.h"
#include "graphics/program.h"
#include "graphics/texture.h"
#include "graphics/framebuffer.h"
#include "graphics/joint.h"
#include "graphics/jointPose.h"
#include "graphics/model.h"
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

class IAnimator;

struct ModelInstance
{
  Model* model;
  glm::vec3 position;
  float rotation;
  IAnimator* animator;

  ModelInstance(Model* model, glm::vec3 position, float rotation, IAnimator* animator)
    : model{ model }
    , position{ position }
    , rotation{ rotation }
    , animator{ animator }
  { }

  virtual void update(GLFWwindow *window, float time) { }
};

class IAnimator
{
public:
  virtual void applyAnimation(Program& program, float time, ModelInstance& instance) = 0;
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

IAnimator* walk_animator;
IAnimator* trudge_animator;
IAnimator* stand_animator;

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
        animator = trudge_animator;
      }
      else
      {
        speed = 0.01f;
        animator = walk_animator;
      }
    }
    else
    {
      speed = 0.0f;
      animator = stand_animator;
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

void draw_faces(ModelInstance& instance, Program& program, float time)
{
  program.setMat4("model", glm::translate(glm::mat4(), instance.position) * glm::rotate(instance.model->transform, instance.rotation, { 0,0,1 }));
  program.setInt("model_texture", 0);

  instance.animator->applyAnimation(program, time, instance);

  glActiveTexture(GL_TEXTURE0);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, instance.model->texture.id());

  glBindVertexArray(instance.model->faceDataVAO);
  glBindBuffer(GL_ARRAY_BUFFER, instance.model->faceDataVBO);
  glDrawArrays(GL_TRIANGLES, 0, instance.model->faceData.size() / 7 * 3);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_lines(ModelInstance& instance, Program& program, float time)
{
  program.setMat4("model", glm::translate(glm::mat4(), instance.position) * glm::rotate(instance.model->transform, instance.rotation, { 0,0,1 }));

  instance.animator->applyAnimation(program, time, instance);

  glBindVertexArray(instance.model->vertexDataVAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, instance.model->lineIndexesID);
  glPatchParameteri(GL_PATCH_VERTICES, 4);
  glDrawElements(GL_PATCHES, instance.model->lineIndexes.size(), GL_UNSIGNED_INT, (void*)0);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
  float distance = 1.0f;

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
  }

  virtual glm::mat4 transform() const
  {
    return glm::lookAt(position(), (*_instance)->position + glm::vec3(0, 0, 1), { 0, 0, 1 });
  }

  virtual glm::vec3 position() const
  {
    return (*_instance)->position + glm::vec3(glm::rotate(glm::mat4(), (*_instance)->rotation, { 0, 0, 1 }) * glm::vec4(0, 4, 2, 1)) * distance;
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

int main()
{
  wilt::logging.setLogger<wilt::StreamLogger>(std::cout);

  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

                                                       // glfw window creation
                                                       // --------------------
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

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);

  // build and compile our shader programs
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

  //auto treeModel = Model::read("models/tree_model.txt", "models/tree_texture.jpg");
  //auto tree2Model = Model::read("models/tree_2_model.txt", "models/tree_2_texture.jpg");
  //auto grassModel = Model::read("models/grass_model.txt", "models/grass_texture.jpg");
  //auto blockModel = Model::read("models/block_model.txt", "models/block_texture.jpg", 0.05f * character_scale);
  //auto spiritModel = Model::read("models/spirit_model.txt", "models/spirit_texture.jpg", 0.1f);
  //auto birdModel = Model::read("models/bird_final_model_3.txt", "models/bird_final_texture.jpg", 0.2f);
  //auto birdModel = Model::read("models/block_model_2.txt", "models/block_texture.jpg", 0.2f);
  auto birdModel = Model::read("C://Users//kmdre//Downloads//temp_model.txt", "models/bird_final_texture.jpg", 0.2f);

  auto walk_animation = read_animation("models/walk_animation.txt");
  auto trudge_animation = read_animation("models/trudge_animation.txt");
  //auto idle_animation = read_animation("models/bird_final_idle_animation.txt");
  //auto idle_animation = read_animation("models/bird_final_test_animation.txt");
  //auto idle_animation = read_animation("models/block_animation_2.txt");
  auto idle_animation = read_animation("C:\\Users\\kmdre\\Downloads\\animation.txt");


  walk_animator = new LoopAnimator{ walk_animation, 96 };
  trudge_animator = new LoopAnimator{ trudge_animation, 24 };
  stand_animator = new StaticAnimator{};

  ModelInstance* instances[] = 
  {
    //new CharacterInstance(&blockModel,{ 0, 0, 0 },  glm::radians(180.0f), walk_animator),

    //new ModelInstance(&treeModel, { 5,  4, 0 }, -0.2, new StaticAnimator{}),
    //new ModelInstance(&treeModel, { 3,  6, 0 },  0.6, new StaticAnimator{}),
    //new ModelInstance(&treeModel, {-2,  3, 0 }, -1.2, new StaticAnimator{}),
    //new ModelInstance(&treeModel, {-3,  6, 0 },  0.8, new StaticAnimator{}),
    //new ModelInstance(&treeModel, { 3, -5, 0 },  2.3, new StaticAnimator{}),
    //new ModelInstance(&treeModel, {-4, -5, 0 }, -3.0, new StaticAnimator{}),
    //new ModelInstance(&treeModel, {-5,  0, 0 },  1.4, new StaticAnimator{}),

    //// new ModelInstance(&tree2Model, {-20, -20, 0 },  0.0, new StaticAnimator{}),

    //new ModelInstance(&grassModel, {-4, -3, 0 }, -0.2, new StaticAnimator{}),
    //new ModelInstance(&grassModel, {-2, -4, 0 },  0.6, new StaticAnimator{}),
    //new ModelInstance(&grassModel, {-1,  1, 0 }, -1.2, new StaticAnimator{}),
    //new ModelInstance(&grassModel, { 2, -4, 0 },  0.8, new StaticAnimator{}),
    //new ModelInstance(&grassModel, {-2,  4, 0 },  2.3, new StaticAnimator{}),
    //new ModelInstance(&grassModel, { 3,  4, 0 }, -3.0, new StaticAnimator{}),
    //new ModelInstance(&grassModel, { 4,  0, 0 },  1.4, new StaticAnimator{}),

    //new SpiritInstance(&spiritModel, { 3, 5 ,0 }, 0.0, new StaticAnimator{}),
    //new SpiritInstance(&spiritModel, {-5, 4 ,0 }, 1.5, new StaticAnimator{}),
    //new SpiritInstance(&spiritModel, { 4,-3 ,0 }, 4.5, new StaticAnimator{}),
    //new SpiritInstance(&spiritModel,{ -4,-4 ,0 }, 3.0, new StaticAnimator{}),

    new ModelInstance(&birdModel, { 2, 2 ,0 }, 0.0, new LoopAnimator{ idle_animation, 72.0f }),

    //new ModelInstance(&spiritModel, {0, 0, 1}, 0.0, new StaticAnimator{})
  };
  int instanceCount = sizeof(instances) / sizeof(ModelInstance*);

  auto& character = instances[0];

  for (auto model : { /*&treeModel, &tree2Model, &grassModel, &blockModel, &spiritModel,*/ &birdModel })
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

  GLint maxGeometryOutputVertices;
  glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &maxGeometryOutputVertices);
  std::cout << maxGeometryOutputVertices << std::endl;

  // render loop
  // -----------
  int i = 0;
  float iterationsPerSecond = 144.0f;
  int selected_perlin = 0;
  glm::vec3 view_reference;

  int instanceIndex = 0;
  ModelInstance* currentInstance = instances[instanceIndex];
  ICamera* freeCam = new FreeCamera();
  ICamera* trackCam = new TrackCamera(&currentInstance);
  ICamera* followCam = new FollowCamera(&currentInstance);
  ICamera* cam = freeCam;

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
      instanceIndex = (instanceIndex != instanceCount - 1) ? instanceIndex + 1 : 0;
      currentInstance = instances[instanceIndex];
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
    {
      instanceIndex = (instanceIndex != 0) ? instanceIndex - 1 : instanceCount - 1;
      currentInstance = instances[instanceIndex];
    }

    glm::mat4 view = cam->transform();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    if (!paused)
    {
      cam->update(window, time);
      for (auto& instance : instances)
        instance->update(window, time);
    }

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
      draw_faces(*instance, depthProgram, time);

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
      draw_lines(*instance, lineProgram, time);

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
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
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