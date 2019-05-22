#ifndef WILT_ENTITYTYPE_H
#define WILT_ENTITYTYPE_H

class Model;
class Entity;
class EntitySpawnInfo;

class IEntityType
{
public:
  virtual void read() = 0;
  virtual void load() = 0;

  virtual Model* getModel() const = 0;
  virtual Entity* spawn(const EntitySpawnInfo& info) = 0;
};

template <class TEntity, class TModel>
class EntityType : public IEntityType
{
private:
  std::string filename;
  TModel* model;

public:
  EntityType(std::string filename)
    : filename{ filename }
    , model{ new TModel() }
  { }

public:
  void read() override
  {
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
