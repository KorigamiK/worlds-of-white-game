#include "CharacterInstance.h"

#include <glm/gtx/intersect.hpp>

using namespace std::chrono_literals;

const auto CHARACTER_SPEED = 4.0f;
const auto CHARACTER_JUMP_SPEED = 4.0f;
const auto CHARACTER_JUMP_RISE_SPEED = 0.04f;
const auto CHARACTER_DASH_SPEED = 20.0f;
const auto CHARACTER_DASH_DURATION = std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(0.125s);
const auto CHARACTER_DEADZONE = 0.12f;

const auto BUTTON_JUMP = 0;
const auto BUTTON_DASH = 2;

btRigidBody* createCharacterBody(glm::vec3 position);
void doCharacterDeformation(Instance* character, btCollisionWorld* world, btBvhTriangleMeshShape* mesh);

CharacterInstance::CharacterInstance(Model* model, glm::vec3 position, float rotation, float scale)
  : PhysicsInstance{ model, position, rotation, createCharacterBody(position), scale }
  , velocity{ 0, 1, 0 }
  , dashing{ false }
  , dashUsed{ false }
  , dashTime{ }
  , dashDirection{ }
{ }

void CharacterInstance::update(GameState& state, float time)
{
  if (dashUsed && *state.canJump)
  {
    dashUsed = false;
  }

  if (dashing)
  {
    body->setLinearVelocity(btVector3(dashDirection.x, dashDirection.y, dashDirection.z));
    if (dashTime <= std::chrono::high_resolution_clock::now())
      dashing = false;
  }
  else
  {
    { // keyboard
      if (glfwGetKey(state.window, GLFW_KEY_A) == GLFW_PRESS) {
        velocity = glm::vec3(glm::rotate(glm::mat4(), glm::radians(1.0f), { 0, 0, 1 }) * glm::vec4(velocity, 0.0));
        rotation += glm::radians(1.0f);
      }
      if (glfwGetKey(state.window, GLFW_KEY_D) == GLFW_PRESS) {
        velocity = glm::vec3(glm::rotate(glm::mat4(), glm::radians(-1.0f), { 0, 0, 1 }) * glm::vec4(velocity, 0.0));
        rotation -= glm::radians(1.0f);
      }
      if (glfwGetKey(state.window, GLFW_KEY_W) == GLFW_PRESS) {
        auto velocity = body->getLinearVelocity();
        auto new_velocity = btMatrix3x3(btQuaternion(0, 0, rotation)) * btVector3(0, -3.0f, 0);
        new_velocity.setZ(velocity.z());
        body->setLinearVelocity(new_velocity);
        body->activate();
      }
      if (glfwGetKey(state.window, GLFW_KEY_S) == GLFW_PRESS) {
        auto velocity = body->getLinearVelocity();
        auto new_velocity = btMatrix3x3(btQuaternion(0, 0, rotation)) * btVector3(0, 3.0f, 0);
        new_velocity.setZ(velocity.z());
        body->setLinearVelocity(new_velocity);
        body->activate();
      }
      if (glfwGetKey(state.window, GLFW_KEY_SPACE) == GLFW_PRESS && *state.canJump)
      {
        auto velocity = body->getLinearVelocity();
        velocity.setZ(5.0f);
        body->setLinearVelocity(velocity);
        body->activate();
      }
    }

    { // controller
      auto buttonsCount = 0;
      auto buttons = glfwGetJoystickButtons(state.selectedJoystickId, &buttonsCount);
      auto axesCount = 0;
      auto axes = glfwGetJoystickAxes(state.selectedJoystickId, &axesCount);

      auto xAxis = axes[0];
      auto yAxis = -axes[1];

      auto angle = std::atan2(yAxis, xAxis);
      auto magnitude = std::hypot(xAxis, yAxis);

      if (magnitude > CHARACTER_DEADZONE)
      {
        angle += state.camera->getAngle() + glm::radians(90.0f);
        rotation = angle;

        auto old_velocity = body->getLinearVelocity();
        auto new_velocity = btMatrix3x3(btQuaternion(0, 0, angle)) * btVector3(0, magnitude * CHARACTER_SPEED, 0);
        new_velocity.setZ(old_velocity.z());
        body->setLinearVelocity(new_velocity);
        body->activate();
      }
      if (buttons[BUTTON_JUMP] == GLFW_PRESS && *state.canJump)
      {
        auto velocity = body->getLinearVelocity();
        velocity.setZ(CHARACTER_JUMP_SPEED);
        body->setLinearVelocity(velocity);
        body->activate();
      }
      if (buttons[BUTTON_JUMP] == GLFW_PRESS && !*state.canJump && body->getLinearVelocity().z() > 0.0f)
      {
        auto velocity = body->getLinearVelocity();
        velocity.setZ(velocity.getZ() + CHARACTER_JUMP_RISE_SPEED);
        body->setLinearVelocity(velocity);
        body->activate();
      }
      if (!dashUsed && buttons[BUTTON_DASH] == GLFW_PRESS && magnitude <= CHARACTER_DEADZONE)
      {
        dashing = true;
        dashUsed = true;
        dashDirection = glm::vec3(0, 0, -1) * CHARACTER_DASH_SPEED;
        dashTime = std::chrono::high_resolution_clock::now() + CHARACTER_DASH_DURATION;
      }
      if (!dashUsed && buttons[BUTTON_DASH] == GLFW_PRESS && magnitude > CHARACTER_DEADZONE)
      {
        auto direction = btVector3(0, 1, 0).rotate({ 0, 0, 1 }, rotation);

        dashing = true;
        dashUsed = true;
        dashDirection = glm::vec3(direction.x(), direction.y(), direction.z()) * CHARACTER_DASH_SPEED;
        dashTime = std::chrono::high_resolution_clock::now() + CHARACTER_DASH_DURATION;
      }
    }
  }

  PhysicsInstance::update(state, time);
}

void CharacterInstance::draw_faces(GameState& state, Program& program, float time)
{
  doCharacterDeformation(this, state.world, state.terrain);
  Instance::draw_faces(state, program, time);
}

void CharacterInstance::draw_lines(GameState& state, Program& program, float time)
{
  Instance::draw_lines(state, program, time);
}

void CharacterInstance::draw_debug(GameState& state, Program& program, float time)
{

}

btRigidBody* createCharacterBody(glm::vec3 position)
{
  auto characterShape = new btSphereShape(0.25);
  auto characterMotionState = new btDefaultMotionState(btTransform(btMatrix3x3::getIdentity(), btVector3(0, 0, position.z + 1.0f)));
  auto characterBody = new btRigidBody(1.0, characterMotionState, characterShape);
  characterBody->setAngularFactor(0);
  characterBody->setFriction(0.95f);
  characterBody->setDamping(0.5f, 0.0f);
  characterBody->setRestitution(0.0f);
  characterBody->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
  characterBody->setCcdSweptSphereRadius(0.25f);
  characterBody->setCcdMotionThreshold(0.00000001f);
  return characterBody;
}

void doCharacterDeformation(Instance* character, btCollisionWorld* world, btBvhTriangleMeshShape* mesh)
{
  auto& characterModel = *character->model;
  auto characterRotation = character->rotation;
  auto characterScale = character->scale;
  auto characterPosition = btVector3(
    character->position.x,
    character->position.y,
    character->position.z);

  const auto POINT_COUNT = characterModel.vertexData.size() / Model::DATA_COUNT_PER_VERTEX;
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

  auto getRayIntersectionWorld = [&](btVector3& start, btVector3 end)
  {
    btCollisionWorld::AllHitsRayResultCallback result(start, end);
    world->rayTest(start, end, result);

    auto closestFraction = 1.0f;
    for (int j = 0; j < result.m_hitFractions.size(); ++j)
      if (result.m_hitFractions.at(j) < closestFraction)
        closestFraction = result.m_hitFractions.at(j);

    return closestFraction;
  };

  auto getMeshTriangles = [&]()
  {
    struct CustomTriangleCallback : btTriangleCallback
    {
      std::vector<glm::vec3> trianglePoints;
      void processTriangle(btVector3* triangle, int partId, int index) override
      {
        trianglePoints.push_back(glm::vec3(triangle[0].x(), triangle[0].y(), triangle[0].z()));
        trianglePoints.push_back(glm::vec3(triangle[1].x(), triangle[1].y(), triangle[1].z()));
        trianglePoints.push_back(glm::vec3(triangle[2].x(), triangle[2].y(), triangle[2].z()));
      }
    };

    auto boundingBoxMin = characterPosition - btVector3(POINT_DISTANCE_MAX, POINT_DISTANCE_MAX, POINT_DISTANCE_MAX) * characterScale;
    auto boundingBoxMax = characterPosition + btVector3(POINT_DISTANCE_MAX, POINT_DISTANCE_MAX, POINT_DISTANCE_MAX) * characterScale;
    auto triangleCallback = CustomTriangleCallback();

    mesh->processAllTriangles(&triangleCallback, boundingBoxMin, boundingBoxMax);

    return triangleCallback.trianglePoints;
  };

  auto meshTriangles = getMeshTriangles();
  auto getRayIntersectionMesh = [&](btVector3& start, btVector3 end)
  {
    auto rayStart = glm::vec3(start.x(), start.y(), start.z());
    auto rayEnd = glm::vec3(end.x(), end.y(), end.z());
    auto rayLength = (rayStart - rayEnd).length();
    auto closestFraction = 1.0f;

    for (std::size_t i = 0; i < meshTriangles.size(); i += 3)
    {
      glm::vec3 data;
      if (glm::intersectRayTriangle(rayStart, rayEnd - rayStart, meshTriangles[i], meshTriangles[i + 1], meshTriangles[i + 2], data))
      {
        auto intersectFraction = data.z;
        if (intersectFraction < closestFraction)
          closestFraction = intersectFraction;
      }
    }

    return closestFraction;
  };

  auto newVertexData = characterModel.vertexData;
  for (std::size_t i = 0; i < newVertexData.size(); i += Model::DATA_COUNT_PER_VERTEX)
  {
    // should be a unit vector
    auto point = btVector3(
      newVertexData[i + 0],
      newVertexData[i + 1],
      newVertexData[i + 2]);

    // these are in world space
    auto rayStart = characterPosition;
    auto rayEnd = characterPosition + point.rotate({ 0, 0, 1 }, characterRotation) * characterScale * (POINT_DISTANCE_MAX + POINT_WORLD_MARGIN);
    auto closestFraction = getRayIntersectionMesh(rayStart, rayEnd);
    auto closestAmount = closestFraction * (POINT_DISTANCE_MAX + POINT_WORLD_MARGIN);

    pointBounds[i / Model::DATA_COUNT_PER_VERTEX] = closestAmount - POINT_WORLD_MARGIN;

    if (closestAmount < 1.0f)
    {
      for (std::size_t j = 0; j < probeVectors.size(); ++j)
      {
        auto& ang = probeVectors[j];
        float PI = 3.14159265358979f;
        probeAmounts[j] += probeWeightFunction(point.angle(ang) / PI) * closestAmount;
      }
    }
  }

  for (std::size_t i = 0; i < newVertexData.size(); i += Model::DATA_COUNT_PER_VERTEX)
  {
    auto pointBound = pointBounds[i / Model::DATA_COUNT_PER_VERTEX];

    auto x = newVertexData[i + 0];
    auto y = newVertexData[i + 1];
    auto z = newVertexData[i + 2];
    auto g1 = (std::size_t)newVertexData[i + 3];
    auto g2 = (std::size_t)newVertexData[i + 4];
    auto g3 = (std::size_t)newVertexData[i + 5];
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
