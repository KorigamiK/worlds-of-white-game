#include "SpiritEntity.h"

#include <iostream>
#include <random>

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "PhysicsEntity.h"

using namespace std::chrono_literals;

const auto SPIRIT_MIN_SPEED = 0.1f;
const auto SPIRIT_MIN_ROTATION = 0.2f;
const auto SPIRIT_MIN_PLAYER_DISTANCE = 1.0f;
const auto SPIRIT_TAIL_DISTANCE_1 = 0.25f;
const auto SPIRIT_TAIL_DISTANCE_2 = 0.35f;
const auto SPIRIT_TAIL_DISTANCE_3 = 0.45f;
const auto SPIRIT_TAIL_SIZE_1 = 0.50f;
const auto SPIRIT_TAIL_SIZE_2 = 0.30f;
const auto SPIRIT_TAIL_SIZE_3 = 0.20f;
const auto SPIRIT_IDLE_CORRECTION_RATE = 0.04f;
const auto SPIRIT_ATTACK_CORRECTION_RATE = 0.8f;
const auto SPIRIT_ATTACK_TIME = std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(0.15s);
const auto SPIRIT_ATTACK_DISTANCE_THRESHOLD = 0.5f;
const auto SPIRIT_ATTACK_SPEED = 0.4f;
const auto SPIRIT_RETREAT_CORRECTION_RATE = 0.8f;
const auto SPIRIT_RETREAT_SPEED = 0.3f;
const auto SPIRIT_RETREAT_DISTANCE_THRESHOLD = 2.0f;

SpiritEntity::SpiritEntity(Model* model, const EntitySpawnInfo& info)
  : Entity{ model, info }
  , state{ IDLING }
  , playerPosition{ }
  , tailPosition1{ position + glm::rotateZ(glm::vec3(-1, 0, 0), rotation.z) * SPIRIT_TAIL_DISTANCE_1 } // make this use rotation.y
  , tailPosition2{ tailPosition1 + glm::rotateZ(glm::vec3(-1, 0, 0), rotation.z) * SPIRIT_TAIL_DISTANCE_2 } // make this use rotation.y
  , tailPosition3{ tailPosition2 + glm::rotateZ(glm::vec3(-1, 0, 0), rotation.z) * SPIRIT_TAIL_DISTANCE_3 } // make this use rotation.y
{
  static std::random_device rd;
  static std::mt19937 gen(rd());

  std::uniform_real_distribution<> dis1(0, 2 * 3.1415926535);
  std::uniform_real_distribution<> dis2(0.3, 0.7);

  // TODO: make these pull from a fixed set instead of random
  heightMax    = (float)dis2(gen) / 2.0f;
  heightPeriod = (float)dis2(gen);
  heightPoint  = (float)dis1(gen);
  anglePoint   = (float)dis1(gen);
  distance     = (float)dis2(gen) * 2.5f;
}

void SpiritEntity::update(GameState& state, float time)
{
  glm::vec3 desiredPosition;
  float correctionRate;

  // TODO: make idle positioning time-based instead of increments
  heightPoint += 0.01f;
  anglePoint += 0.01f;

  switch (this->state)
  {
  case IDLING:
    {
      auto offset = glm::rotateZ(glm::vec3{ 0, distance, std::sin(heightPoint * heightPeriod) * heightMax + 0.25 }, anglePoint);
      auto offsetRatio = glm::length(playerPosition - position) > 2.0f
        ? 1.5f / glm::length(playerPosition - position) + 0.25f
        : 1.0f;

      desiredPosition = playerPosition + offset * offsetRatio;
      correctionRate = SPIRIT_IDLE_CORRECTION_RATE;
      break;
    }

  case ATTACKING:
    {
      { // check for enemy collisions

        // TODO: consider just making it a physics entity

        struct MyResultCallback : btCollisionWorld::ContactResultCallback
        {
          const btCollisionObject* self;
          const btCollisionObject* target = nullptr;

          MyResultCallback(const btCollisionObject* self) : self{ self } {}

          btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) override
          {
            if (target == nullptr)
            {
              auto targetA = colObj0Wrap->getCollisionObject();
              auto targetB = colObj1Wrap->getCollisionObject();
              if (targetA == self)
                target = targetB;
              else
                target = targetA;
            }

            return 1.0f;
          }
        };

        btSphereShape sphereShape(0.5f * scale); // TODO: use radius constant
        btCollisionObject sphere;
        sphere.setCollisionShape(&sphereShape);
        sphere.setWorldTransform(btTransform(btQuaternion::getIdentity(), btVector3(position.x, position.y, position.z)));

        MyResultCallback callback(&sphere);
        state.world->contactTest(&sphere, callback);

        if (callback.target != nullptr)
        {
          if (((PhysicsEntity*)callback.target->getUserPointer())->getType() == PhysicsEntity::Type::ENEMY)
          {
            auto direction = btVector3(1, 0, 0)
              .rotate({ 0, 1, 0 }, 0.75f) // TODO: move to constant height angle
              .rotate({ 0, 0, 1 }, rotation.z)
              * 500.0f; // TODO: move to contant power

            ((btRigidBody*)callback.target)->activate();
            ((btRigidBody*)callback.target)->applyCentralImpulse(direction);
          }
          this->state = RETREATING;
        }
      }

      desiredPosition = position + glm::normalize(attackTarget - position) * SPIRIT_ATTACK_SPEED;
      correctionRate = SPIRIT_ATTACK_CORRECTION_RATE;

      if (glm::length(desiredPosition - attackTarget) < SPIRIT_ATTACK_DISTANCE_THRESHOLD)
        this->state = RETREATING;

      break;
    }

  case RETREATING:
    {
      desiredPosition = position + glm::normalize(playerPosition - position) * SPIRIT_RETREAT_SPEED;
      correctionRate = SPIRIT_RETREAT_CORRECTION_RATE;

      if (glm::length(desiredPosition - playerPosition) < SPIRIT_RETREAT_DISTANCE_THRESHOLD)
        this->state = IDLING;

      break;
    }
  }

  auto oldPosition = position;

  auto dx = desiredPosition.x - position.x;
  auto dy = desiredPosition.y - position.y;
  auto dz = desiredPosition.z - position.z;
  auto az = std::atan2(dy, dx);
  auto ay = std::atan2(dz, std::hypot(dx, dy));

  auto currentAngle = glm::quat(rotation);
  auto desiredAngle = glm::quat(glm::vec3(0,-ay, az));

  // ok, do this later
  //auto requiredAngle = std::acos(glm::dot(currentAngle, desiredAngle));
  //if (requiredAngle > SPIRIT_MIN_ROTATION)
  //{
  //  // min speed
  //}
  //else
  //{
  //  // just do it
  //}

  auto newPosition = oldPosition * (1 - correctionRate) + desiredPosition * correctionRate;

  if (glm::length(playerPosition - newPosition) < SPIRIT_MIN_PLAYER_DISTANCE)
    newPosition = playerPosition + glm::normalize(newPosition - playerPosition) * SPIRIT_MIN_PLAYER_DISTANCE;

  {
    auto dx = newPosition.x - oldPosition.x;
    auto dy = newPosition.y - oldPosition.y;
    auto dz = newPosition.z - oldPosition.z;
    auto az = std::atan2(dy, dx);
    auto ay = std::atan2(dz, std::hypot(dx, dy));

    rotation.x = 0.0f;
    rotation.y =-ay;
    rotation.z = az;

    position = newPosition;

    tailPosition1 = position + glm::normalize(tailPosition1 - position) * SPIRIT_TAIL_DISTANCE_1;
    tailPosition2 = tailPosition1 + glm::normalize(tailPosition2 - tailPosition1) * SPIRIT_TAIL_DISTANCE_2;
    tailPosition3 = tailPosition2 + glm::normalize(tailPosition3 - tailPosition2) * SPIRIT_TAIL_DISTANCE_3;
  }
}


void SpiritEntity::draw_faces(GameState& state, Program& program, float time)
{
  glm::mat4 jointTransforms[MAX_JOINTS];
  glUniformMatrix4fv(glGetUniformLocation(program.id(), "positions"), MAX_JOINTS, false, glm::value_ptr(jointTransforms[0]));
  program.setFloat("draw_percentage", 1.0f);
  model->draw_faces(program, time, model->makeEntityTransform(position, rotation, scale));
  model->draw_faces(program, time, model->makeEntityTransform(tailPosition1, {}, scale * SPIRIT_TAIL_SIZE_1));
  model->draw_faces(program, time, model->makeEntityTransform(tailPosition2, {}, scale * SPIRIT_TAIL_SIZE_2));
  model->draw_faces(program, time, model->makeEntityTransform(tailPosition3, {}, scale * SPIRIT_TAIL_SIZE_3));
}

void SpiritEntity::draw_lines(GameState& state, Program& program, float time)
{
  glm::mat4 jointTransforms[MAX_JOINTS];
  glUniformMatrix4fv(glGetUniformLocation(program.id(), "positions"), MAX_JOINTS, false, glm::value_ptr(jointTransforms[0]));
  program.setFloat("draw_percentage", 1.0f);
  model->draw_lines(program, time, model->makeEntityTransform(position, rotation, scale));
  model->draw_lines(program, time, model->makeEntityTransform(tailPosition1, {}, scale * SPIRIT_TAIL_SIZE_1));
  model->draw_lines(program, time, model->makeEntityTransform(tailPosition2, {}, scale * SPIRIT_TAIL_SIZE_2));
  model->draw_lines(program, time, model->makeEntityTransform(tailPosition3, {}, scale * SPIRIT_TAIL_SIZE_3));
}

void SpiritEntity::draw_debug(GameState& state, Program& program, float time)
{

}

void SpiritEntity::attack(glm::vec3 target)
{
  if (state != IDLING)
    return;

  state = ATTACKING;
  attackTarget = target;
}
