#ifndef WILT_ENTITYTYPE_H
#define WILT_ENTITYTYPE_H

#include "Model.h"
#include "EntitySpawnInfo.h"
#include "entities/Entity.h"

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
    , model{ nullptr }
  { }

public:
  void read() override
  {
    std::ifstream file(filename);

    std::string type;
    file >> type;

    model = (TModel*)TModel::read(file);
  }

  void load() override
  {
    model->load();
  }

  virtual Model* getModel() const
  {
    return model;
  }

  virtual Entity* spawn(const EntitySpawnInfo& info)
  {
    return new TEntity(model, info);
  }
};

#endif // !WILT_ENTITYTYPE_H
