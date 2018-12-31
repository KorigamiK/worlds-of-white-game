#include "TerrainEntity.h"

btRigidBody* createTerrainBody(Model* model)
{
  // TODO: move shape creation to model, though... this will almost always be created once anyways...
  auto terrainMesh = new btTriangleIndexVertexArray(model->faceIndexes.size() / 3, (int*)model->faceIndexes.data(), 3 * sizeof(int), model->vertexData.size() / Model::DATA_COUNT_PER_VERTEX, model->vertexData.data(), Model::DATA_COUNT_PER_VERTEX * sizeof(float));
  auto terrainShape = new btBvhTriangleMeshShape(terrainMesh, true);
  terrainShape->setMargin(0.0f);

  auto terrainMotionState = new btDefaultMotionState();
  auto terrainBody = new btRigidBody(0.0, terrainMotionState, terrainShape);
  terrainBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
  terrainBody->setFriction(0.95f);

  return terrainBody;
}

TerrainEntity::TerrainEntity(Model* model, const EntitySpawnInfo& info)
  : PhysicsEntity{ model, info, createTerrainBody(model), Type::SCENERY }
{ }
