#include "model.h"

#include <fstream>

#include <glad/glad.h>
#include <cimg/cimg.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

void Model::load()
{
  // load vertices
  glGenVertexArrays(1, &vertexDataVAO);
  glGenBuffers(1, &vertexDataVBO);
  glBindVertexArray(vertexDataVAO);
  glBindBuffer(GL_ARRAY_BUFFER, vertexDataVBO);
  glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(9 * sizeof(float)));

  // load faces (again)
  glGenBuffers(1, &faceIndexesID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceIndexesID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceIndexes.size() * sizeof(unsigned int), faceIndexes.data(), GL_STATIC_DRAW);

  // load lines
  glGenBuffers(1, &lineIndexesID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineIndexesID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, lineIndexes.size() * sizeof(unsigned int), lineIndexes.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Model::draw_faces(Program& program, float time, glm::vec3 position, glm::vec3 rotation, float scale)
{
  glm::mat4 modelTransform = glm::mat4()
    * glm::translate(glm::mat4(), position)
    // * glm::yawPitchRoll(rotation.x, rotation.y, rotation.z)
    * glm::rotate(glm::mat4(), rotation.z, { 0, 0, 1 })
    * glm::rotate(glm::mat4(), rotation.y, { 0, 1, 0 })
    * glm::rotate(glm::mat4(), rotation.x, { 1, 0, 0 })
    * glm::scale(glm::mat4(), glm::vec3(scale, scale, scale))
    * transform;

  program.setMat4("model", modelTransform);
  program.setInt("model_texture", 0);

  glBindVertexArray(vertexDataVAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceIndexesID);
  glDrawElements(GL_TRIANGLES, faceIndexes.size(), GL_UNSIGNED_INT, (void*)0);

  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Model::draw_lines(Program& program, float time, glm::vec3 position, glm::vec3 rotation, float scale)
{
  glm::mat4 entityTransform = glm::mat4()
    * glm::translate(glm::mat4(), position)
    // * glm::yawPitchRoll(rotation.x, rotation.y, rotation.z)
    * glm::rotate(glm::mat4(), rotation.z, { 0, 0, 1 })
    * glm::rotate(glm::mat4(), rotation.y, { 0, 1, 0 })
    * glm::rotate(glm::mat4(), rotation.x, { 1, 0, 0 })
    * glm::scale(glm::mat4(), glm::vec3(scale, scale, scale))
    * transform;

  program.setMat4("model", entityTransform);

  glBindVertexArray(vertexDataVAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineIndexesID);
  glPatchParameteri(GL_PATCH_VERTICES, 4);
  glDrawElements(GL_PATCHES, lineIndexes.size(), GL_UNSIGNED_INT, (void*)0);

  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Entity* Model::spawn(const EntitySpawnInfo& info)
{
  return new Entity(this, info);
}

Model* Model::read(std::ifstream& file)
{
  auto model = new Model();

  // get rotations
  model->transform = glm::scale(glm::mat4(), { 1.0f, 1.0f, 1.0f });

  // read version
  int version;
  file >> version;

  switch (version)
  {
  case 1: readVersion1(*model, file); break;
  case 2: readVersion2(*model, file); break;
  case 3: readVersion3(*model, file); break;
  }

  return model;
}

void Model::readVersion1(Model& model, std::ifstream& file)
{
  // read vertices
  int vertexCount;
  file >> vertexCount;
  model.vertexData.resize(vertexCount * 10); // 10 floats per vertex (x, y, z, g1, g2, g3, w1, w2, w3, o)
  for (std::size_t i = 0; i < model.vertexData.size(); i += 10)
  {
    file >> model.vertexData[i + 0];
    file >> model.vertexData[i + 1];
    file >> model.vertexData[i + 2];
    file >> model.vertexData[i + 3];
    file >> model.vertexData[i + 4];
    file >> model.vertexData[i + 5];
    file >> model.vertexData[i + 6];
    file >> model.vertexData[i + 7];
    file >> model.vertexData[i + 8];
    model.vertexData[i + 9] = 1.0f;
  }

  // read faces
  int faceCount;
  file >> faceCount;
  model.faceIndexes.resize(faceCount * 3); // 3 ints per line (vId1, vId2, vId3)
  for (std::size_t i = 0; i < model.faceIndexes.size(); ++i)
    file >> model.faceIndexes[i];

  // read lines
  int lineCount;
  file >> lineCount;
  model.lineIndexes.resize(lineCount * 4); // 4 ints per line (vId1, vId2, vId3, vId4)
  for (std::size_t i = 0; i < model.lineIndexes.size(); ++i)
    file >> model.lineIndexes[i];

  // read bones
  int jointCount;
  file >> jointCount;
  model.joints.resize(jointCount);
  for (int i = 0; i < jointCount; ++i)
  {
    int parentIndex;
    glm::vec3 location;
    glm::quat rotation;

    file >> parentIndex;

    file >> location[0];
    file >> location[1];
    file >> location[2];

    file >> rotation[3]; // w
    file >> rotation[0]; // x
    file >> rotation[1]; // y
    file >> rotation[2]; // z

    model.joints[i] = Joint(parentIndex, location, rotation);
  }
}

void Model::readVersion2(Model& model, std::ifstream& file)
{
  // read vertices
  int vertexCount;
  file >> vertexCount;
  model.vertexData.resize(vertexCount * 10); // 10 floats per vertex (x, y, z, g1, g2, g3, w1, w2, w3, o)
  for (std::size_t i = 0; i < model.vertexData.size(); i += 10)
  {
    file >> model.vertexData[i + 0];
    file >> model.vertexData[i + 1];
    file >> model.vertexData[i + 2];
    file >> model.vertexData[i + 3];
    file >> model.vertexData[i + 4];
    file >> model.vertexData[i + 5];
    file >> model.vertexData[i + 6];
    file >> model.vertexData[i + 7];
    file >> model.vertexData[i + 8];
    file >> model.vertexData[i + 9];
  }

  // read faces
  int faceCount;
  file >> faceCount;
  model.faceIndexes.resize(faceCount * 3); // 3 ints per line (vId1, vId2, vId3)
  for (std::size_t i = 0; i < model.faceIndexes.size(); ++i)
    file >> model.faceIndexes[i];

  // read lines
  int lineCount;
  file >> lineCount;
  model.lineIndexes.resize(lineCount * 4); // 4 ints per line (vId1, vId2, vId3, vId4)
  for (std::size_t i = 0; i < model.lineIndexes.size(); ++i)
    file >> model.lineIndexes[i];

  // read bones
  int jointCount;
  file >> jointCount;
  model.joints.resize(jointCount);
  for (int i = 0; i < jointCount; ++i)
  {
    int parentIndex;
    glm::vec3 location;
    glm::quat rotation;

    file >> parentIndex;

    file >> location[0];
    file >> location[1];
    file >> location[2];

    file >> rotation[3]; // w
    file >> rotation[0]; // x
    file >> rotation[1]; // y
    file >> rotation[2]; // z

    model.joints[i] = Joint(parentIndex, location, rotation);
  }
}

void Model::readVersion3(Model& model, std::ifstream& file)
{
  // read bounding box
  file >> model.boundingA.x;
  file >> model.boundingA.y;
  file >> model.boundingA.z;
  file >> model.boundingB.x;
  file >> model.boundingB.y;
  file >> model.boundingB.z;

  // read vertices
  int vertexCount;
  file >> vertexCount;
  model.vertexData.resize(vertexCount * 10); // 10 floats per vertex (x, y, z, g1, g2, g3, w1, w2, w3, o)
  for (std::size_t i = 0; i < model.vertexData.size(); i += 10)
  {
    file >> model.vertexData[i + 0];
    file >> model.vertexData[i + 1];
    file >> model.vertexData[i + 2];
    file >> model.vertexData[i + 3];
    file >> model.vertexData[i + 4];
    file >> model.vertexData[i + 5];
    file >> model.vertexData[i + 6];
    file >> model.vertexData[i + 7];
    file >> model.vertexData[i + 8];
    file >> model.vertexData[i + 9];
  }

  // read faces
  int faceCount;
  file >> faceCount;
  model.faceIndexes.resize(faceCount * 3); // 3 ints per line (vId1, vId2, vId3)
  for (std::size_t i = 0; i < model.faceIndexes.size(); ++i)
    file >> model.faceIndexes[i];

  // read lines
  int lineCount;
  file >> lineCount;
  model.lineIndexes.resize(lineCount * 4); // 4 ints per line (vId1, vId2, vId3, vId4)
  for (std::size_t i = 0; i < model.lineIndexes.size(); ++i)
    file >> model.lineIndexes[i];

  // read bones
  int jointCount;
  file >> jointCount;
  model.joints.resize(jointCount);
  for (int i = 0; i < jointCount; ++i)
  {
    int parentIndex;
    glm::vec3 location;
    glm::quat rotation;

    file >> parentIndex;

    file >> location[0];
    file >> location[1];
    file >> location[2];

    file >> rotation[3]; // w
    file >> rotation[0]; // x
    file >> rotation[1]; // y
    file >> rotation[2]; // z

    model.joints[i] = Joint(parentIndex, location, rotation);
  }
}

