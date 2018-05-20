#include "model.h"

#include <fstream>
#include <cimg/cimg.h>

void Model::load()
{
  // load vertices
  glGenVertexArrays(1, &vertexDataVAO);
  glGenBuffers(1, &vertexDataVBO);
  glBindVertexArray(vertexDataVAO);
  glBindBuffer(GL_ARRAY_BUFFER, vertexDataVBO);
  glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));

  // load faces
  glGenVertexArrays(1, &faceDataVAO);
  glGenBuffers(1, &faceDataVBO);
  glBindVertexArray(faceDataVAO);
  glBindBuffer(GL_ARRAY_BUFFER, faceDataVBO);
  glBufferData(GL_ARRAY_BUFFER, faceData.size() * sizeof(float), faceData.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));

  // load lines
  glGenBuffers(1, &lineIndexesID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineIndexesID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, lineIndexes.size() * sizeof(unsigned int), lineIndexes.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // load texture
  texture = Texture::fromMemory((const char*)textureData._basePtr(), GL_RGB, textureData.width(), textureData.height());
  texture.setMinFilter(GL_LINEAR);
  texture.setMagFilter(GL_LINEAR);
}

Model Model::read(const std::string& modelPath, const std::string& texturePath, float scale)
{
  std::ifstream file(modelPath);
  Model model;
  int vertexCount;
  int faceCount;
  int lineCount;

  // read vertices
  file >> vertexCount;
  model.vertexData.resize(vertexCount * 9); // 9 floats per vertex (x, y, z, g1, g2, g3, w1, w2, w3)
  for (std::size_t i = 0; i < model.vertexData.size(); ++i)
    file >> model.vertexData[i];

  // read faces
  file >> faceCount;
  model.faceData.resize(faceCount * 3 * 11); // 3 vertices per face (vId1, vId2, vId3), 11 floats per vertex (x, y, z, g1, g2, g3, w1, w2, w3, u, v)
  for (std::size_t i = 0; i < model.faceData.size(); ++i)
    file >> model.faceData[i];

  // read lines
  file >> lineCount;
  model.lineIndexes.resize(lineCount * 4); // 4 ints per line (vId1, vId2, vId3, vId4)
  for (std::size_t i = 0; i < model.lineIndexes.size(); ++i)
    file >> model.lineIndexes[i];

  // read texture
  cimg_library::CImg<unsigned char> texture;
  texture.load_jpeg(texturePath.c_str());
  model.textureData = wilt::NArray<unsigned char, 3>{ { 3, texture.width(), texture.height() }, texture.data(), wilt::PTR::REF };
  model.textureData = model.textureData.t(0, 1).t(1, 2).clone();

  // get rotations
  model.transform = glm::scale(glm::mat4(), { scale, scale, scale });

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

  return model;
}
