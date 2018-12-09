#include "model.h"

#include <fstream>

#include <glad/glad.h>
#include <cimg/cimg.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

  // load texture
  texture = Texture::fromMemory((const char*)textureData._basePtr(), GL_RGB, textureData.width(), textureData.height());
  texture.setMinFilter(GL_LINEAR);
  texture.setMagFilter(GL_LINEAR);
}

Model Model::read(const std::string& modelPath, const std::string& texturePath, float scale)
{
  Model model;

  // get rotations
  model.transform = glm::scale(glm::mat4(), { scale, scale, scale });

  {
    std::ifstream file(modelPath);

    // read vertices
    int vertexCount;
    file >> vertexCount;
    model.vertexData.resize(vertexCount * 9); // 9 floats per vertex (x, y, z, g1, g2, g3, w1, w2, w3)
    for (std::size_t i = 0; i < model.vertexData.size(); ++i)
      file >> model.vertexData[i];

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

  // read texture
  cimg_library::CImg<unsigned char> texture;
  texture.load_jpeg(texturePath.c_str());
  model.textureData = wilt::NArray<unsigned char, 3>{ { 3, texture.width(), texture.height() }, texture.data(), wilt::PTR::REF };
  model.textureData = model.textureData.t(0, 1).t(1, 2).clone();

  return model;
}

void Model::draw_faces(Program& program, float time, glm::vec3 position, float rotation, float scale)
{
  program.setMat4("model", glm::scale(glm::translate(glm::mat4(), position) * glm::rotate(transform, rotation, { 0,0,1 }), glm::vec3(scale, scale, scale)));
  program.setInt("model_texture", 0);

  glBindVertexArray(vertexDataVAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceIndexesID);
  glDrawElements(GL_TRIANGLES, faceIndexes.size(), GL_UNSIGNED_INT, (void*)0);

  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Model::draw_lines(Program& program, float time, glm::vec3 position, float rotation, float scale)
{
  program.setMat4("model", glm::scale(glm::translate(glm::mat4(), position) * glm::rotate(transform, rotation, { 0,0,1 }), glm::vec3(scale, scale, scale)));

  glBindVertexArray(vertexDataVAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineIndexesID);
  glPatchParameteri(GL_PATCH_VERTICES, 4);
  glDrawElements(GL_PATCHES, lineIndexes.size(), GL_UNSIGNED_INT, (void*)0);

  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
