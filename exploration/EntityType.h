#ifndef WILT_ENTITYTYPE_H
#define WILT_ENTITYTYPE_H

#include <filesystem>
#include <iostream>
#include <fstream>

class Model;
class Entity;
class EntitySpawnInfo;

class IEntityType
{
public:
  virtual void read() = 0;
  virtual void load() = 0;
  virtual void unload() = 0;
  virtual void reload() = 0;

  virtual Model* getModel() const = 0;
  virtual Entity* spawn(const EntitySpawnInfo& info) = 0;
};

template <class TEntity, class TModel>
class EntityType : public IEntityType
{
private:
  std::string filename;
  std::filesystem::file_time_type fileLoadTime;
  TModel* model;

public:
  EntityType(std::string filename)
    : filename{ filename }
    , model{ new TModel() }
  { }

public:
  void read() override
  {
    fileLoadTime = std::filesystem::last_write_time(filename);
    std::ifstream file(filename);
    if (!file)
      return;

    std::string type;
    file >> type;

    model->read(file);
  }

  void load() override
  {
    model->load();
  }

  void unload() override
  {
    model->unload();
  }

  void reload() override
  {
    auto time = std::filesystem::last_write_time(filename);
    if (time == fileLoadTime)
      return;

    std::cout << "reloading: " << filename;
    unload();
    read();
    load();
    fileLoadTime = time;
  }

  Model* getModel() const override
  {
    return model;
  }

  Entity* spawn(const EntitySpawnInfo& info) override
  {
    return new TEntity(model, info);
  }
};

#include "Model.h"
#include "EntitySpawnInfo.h"
#include "entities/Entity.h"

#endif // !WILT_ENTITYTYPE_H
