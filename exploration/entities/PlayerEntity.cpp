#include "PlayerEntity.h"

#include <random>

#include <glm/gtx/intersect.hpp>
#include <glm/gtx/rotate_vector.hpp>

using namespace std::chrono_literals;

const auto PLAYER_SPEED = 4.0f;
const auto PLAYER_JUMP_SPEED = 4.0f;
const auto PLAYER_JUMP_RISE_SPEED = 0.04f;
const auto PLAYER_DASH_SPEED = 20.0f;
const auto PLAYER_DASH_DURATION = std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(0.125s);
const auto PLAYER_DEADZONE = 0.12f;

const auto BUTTON_JUMP = 0;
const auto BUTTON_DASH = 2;

btRigidBody* createPlayerBody(glm::vec3 position, glm::vec3 rotation);
void doPlayerDeformation(Entity* player, btCollisionWorld* world, btBvhTriangleMeshShape* mesh);

PlayerEntity::PlayerEntity(Model* model, const EntitySpawnInfo& info)
  : PhysicsEntity{ model, info, createPlayerBody(info.location, info.rotation) }
  , velocity{ 0, 1, 0 }
  , dashing{ false }
  , dashUsed{ false }
  , dashTime{ }
  , dashDirection{ }
{ }

void PlayerEntity::update(GameState& state, float time)
{
  static std::random_device rd;
  static std::mt19937 gen(rd());

  if (dashUsed && *state.canJump)
  {
    dashUsed = false;
  }

  if (dashing)
  {
    body->setLinearVelocity(btVector3(dashDirection.x, dashDirection.y, dashDirection.z));
    if (dashTime <= std::chrono::high_resolution_clock::now())
      dashing = false;

    if (*state.canJump)
    {
      static std::uniform_real_distribution<> dis(0.0, 3.1415926535);

      auto ring = state.types["ring"]->spawn({ "", position + glm::vec3(0, 0, -0.25), { 0, 0, dis(gen) }, { 0.5, 0.5, 0.5 } });
      state.addList.push_back(ring);
      dashing = false;
    }
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

      if (magnitude > PLAYER_DEADZONE)
      {
        angle += state.camera->getAngle() + glm::radians(90.0f);
        rotation = angle;

        auto old_velocity = body->getLinearVelocity();
        auto new_velocity = btMatrix3x3(btQuaternion(0, 0, angle)) * btVector3(0, magnitude * PLAYER_SPEED, 0);
        new_velocity.setZ(old_velocity.z());
        body->setLinearVelocity(new_velocity);
        body->activate();
      }
      if (buttons[BUTTON_JUMP] == GLFW_PRESS && *state.canJump)
      {
        auto velocity = body->getLinearVelocity();
        velocity.setZ(PLAYER_JUMP_SPEED);
        body->setLinearVelocity(velocity);
        body->activate();
      }
      if (buttons[BUTTON_JUMP] == GLFW_PRESS && !*state.canJump && body->getLinearVelocity().z() > 0.0f)
      {
        auto velocity = body->getLinearVelocity();
        velocity.setZ(velocity.getZ() + PLAYER_JUMP_RISE_SPEED);
        body->setLinearVelocity(velocity);
        body->activate();
      }
      if (!dashUsed && buttons[BUTTON_DASH] == GLFW_PRESS && magnitude <= PLAYER_DEADZONE && !*state.canJump)
      {
        dashing = true;
        dashUsed = true;
        dashDirection = glm::vec3(0, 0, -1) * PLAYER_DASH_SPEED;
        dashTime = std::chrono::high_resolution_clock::now() + PLAYER_DASH_DURATION;
      }
      if (!dashUsed && buttons[BUTTON_DASH] == GLFW_PRESS && magnitude > PLAYER_DEADZONE && !*state.canJump)
      {
        auto direction = btVector3(0, 1, 0).rotate({ 0, 0, 1 }, rotation);

        dashing = true;
        dashUsed = true;
        dashDirection = glm::vec3(direction.x(), direction.y(), direction.z()) * PLAYER_DASH_SPEED;
        dashTime = std::chrono::high_resolution_clock::now() + PLAYER_DASH_DURATION;
      }
    }
  }

  PhysicsEntity::update(state, time);

  state.playerPosition = position;

  if (spirits.size() < 3)
  {
    static std::uniform_real_distribution<> dis1(0, 2 * 3.1415926535);
    static std::uniform_real_distribution<> dis2(0.3, 0.7);

    auto spirit1 = state.types["spirit"]->spawn({ "", position, { 0, 0, 0 }, { 0.05, 0.05, 0.05 } });
    auto spirit2 = state.types["spirit"]->spawn({ "", position, { 0, 0, 0 }, { 0.05, 0.05, 0.05 } });
    auto spirit3 = state.types["spirit"]->spawn({ "", position, { 0, 0, 0 }, { 0.05, 0.05, 0.05 } });
    state.addList.push_back(spirit1);
    state.addList.push_back(spirit2);
    state.addList.push_back(spirit3);
    spirits.push_back(
      { 
        (SpiritEntity*)spirit1,
        (float)dis2(gen) / 2.0f,
        (float)dis2(gen),
        (float)dis1(gen),
        (float)dis1(gen),
        (float)dis2(gen) * 2.5f
      });
    spirits.push_back(
      {
        (SpiritEntity*)spirit2,
        (float)dis2(gen) / 2.0f,
        (float)dis2(gen),
        (float)dis1(gen),
        (float)dis1(gen),
        (float)dis2(gen) * 2.5f
      });
    spirits.push_back(
      {
        (SpiritEntity*)spirit3,
        (float)dis2(gen) / 2.0f,
        (float)dis2(gen),
        (float)dis1(gen),
        (float)dis1(gen),
        (float)dis2(gen) * 2.5f
      });
  }

  for (auto& spirit : spirits)
  {
    auto offset = glm::rotateZ(glm::vec3{ 0, spirit.distance, std::sin(spirit.heightPoint * spirit.heightPeriod) * spirit.heightMax }, spirit.anglePoint);

    spirit.spirit->desiredPosition = position + offset;
    spirit.heightPoint += 0.01f;
    spirit.anglePoint += 0.01f;
  }
}

void PlayerEntity::draw_faces(GameState& state, Program& program, float time)
{
  doPlayerDeformation(this, state.world, state.terrain);
  Entity::draw_faces(state, program, time);
}

void PlayerEntity::draw_lines(GameState& state, Program& program, float time)
{
  Entity::draw_lines(state, program, time);
}

void PlayerEntity::draw_debug(GameState& state, Program& program, float time)
{

}

btRigidBody* createPlayerBody(glm::vec3 position, glm::vec3 rotation)
{
  const auto PLAYER_BODY_MASS = 1.0f;
  const auto PLAYER_BODY_RADIUS = 0.25f;

  auto playerTransform = btTransform();
  playerTransform.setRotation({ rotation.x, rotation.y, rotation.z }); // might need to by YXZ, i dunno
  playerTransform.setOrigin({ position.x, position.y, position.z });

  auto playerShape = new btSphereShape(PLAYER_BODY_RADIUS);
  auto playerMotionState = new btDefaultMotionState(playerTransform);
  auto playerBody = new btRigidBody(PLAYER_BODY_MASS, playerMotionState, playerShape);
  playerBody->setAngularFactor(0);
  playerBody->setFriction(0.95f);
  playerBody->setDamping(0.5f, 0.0f);
  playerBody->setRestitution(0.0f);
  playerBody->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
  playerBody->setCcdSweptSphereRadius(PLAYER_BODY_RADIUS);
  playerBody->setCcdMotionThreshold(0.00000001f);

  return playerBody;
}

void doPlayerDeformation(Entity* player, btCollisionWorld* world, btBvhTriangleMeshShape* mesh)
{
  auto& playerModel = *player->model;
  auto playerRotation = player->rotation;
  auto playerScale = player->scale;
  auto playerPosition = btVector3(
    player->position.x,
    player->position.y,
    player->position.z);

  const auto POINT_COUNT = playerModel.vertexData.size() / Model::DATA_COUNT_PER_VERTEX;
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

    auto boundingBoxMin = playerPosition - btVector3(POINT_DISTANCE_MAX, POINT_DISTANCE_MAX, POINT_DISTANCE_MAX) * playerScale;
    auto boundingBoxMax = playerPosition + btVector3(POINT_DISTANCE_MAX, POINT_DISTANCE_MAX, POINT_DISTANCE_MAX) * playerScale;
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

  auto newVertexData = playerModel.vertexData;
  for (std::size_t i = 0; i < newVertexData.size(); i += Model::DATA_COUNT_PER_VERTEX)
  {
    // should be a unit vector
    auto point = btVector3(
      newVertexData[i + 0],
      newVertexData[i + 1],
      newVertexData[i + 2]);

    // these are in world space
    auto rayStart = playerPosition;
    auto rayEnd = playerPosition + point.rotate({ 0, 0, 1 }, playerRotation) * playerScale * (POINT_DISTANCE_MAX + POINT_WORLD_MARGIN);
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

  glBindVertexArray(playerModel.vertexDataVAO);
  glBindBuffer(GL_ARRAY_BUFFER, playerModel.vertexDataVBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, newVertexData.size() * sizeof(float), newVertexData.data());
}
