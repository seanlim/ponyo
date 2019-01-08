#pragma once

#include "array.h"
#include "component.h"
#include "map.h"
#include "system.h"
#include <algorithm>

// pair (Component id, Component index)
typedef std::pair<unsigned int, unsigned int> ComponentReference;
// pair (ECSEntity id, components)
typedef std::pair<unsigned int, Array<ComponentReference>> ECSEntity;

class ECSListener
{
public:
  virtual void onMakeEntity(EntityHook hook) {}
  virtual void onRemoveEntity(EntityHook hook) {}
  virtual void onAddComponent(EntityHook hook, unsigned int id) {}
  virtual void onRemoveComponent(EntityHook hook, unsigned int id) {}

  const Array<unsigned int>& getComponentIDs() { return componentIDs; }
  inline bool shouldNotifyOnAllComponentOperations()
  {
    return notifyOnAllComponentOperations;
  }
  inline bool shouldNotifyOnAllEntityOperations()
  {
    return notifyOnAllEntityOperations;
  }

protected:
  void setNotificationSettings(bool shouldNotifyOnAllComponentOperations,
                               bool shouldNotifyOnAllEntityOperations)
  {
    notifyOnAllComponentOperations = shouldNotifyOnAllComponentOperations;
    notifyOnAllEntityOperations = shouldNotifyOnAllEntityOperations;
  }

  void addComponentID(unsigned int id) { componentIDs.push_back(id); }

private:
  Array<unsigned int> componentIDs;
  bool notifyOnAllComponentOperations = false;
  bool notifyOnAllEntityOperations = false;
};
class ECS
{
public:
  ECS() {}
  ~ECS();

  // Entities
  EntityHook makeEntity(BaseComponent** entityComponents,
                        const unsigned int* componentIDs, size_t numComponents);

  template <class Component> EntityHook makeEntity(Component& c1)
  {
    BaseComponent* components[] = {(BaseComponent*)&c1};
    unsigned int componentIDs[] = {Component::id};
    return makeEntity(components, componentIDs, 1);
  }

  void removeEntity(EntityHook hook);

  // Components
  template <class Component>
  void addComponent(EntityHook hook, Component* component)
  {
    addComponentInternal(hook, componentsFrom(hook), Component::id, component);

    // TODO: Notify listeners
  }

  template <class Component> void removeComponent(EntityHook hook)
  {
    // TODO: Notify listeners
    return removeComponentInternal(hook, Component::id);
  }

  template <class Component> Component* getComponent(EntityHook hook)
  {
    return (Component*)getComponentInternal(componentsFrom(hook),
                                            Component::id);
  }

  BaseComponent* getComponentByType(EntityHook hook, unsigned int componentID)
  {
    return getComponentInternal(componentsFrom(hook), componentID);
  }
  // Systems
  void updateSystems(SystemList& systems, float delta);

private:
  Array<System*> systems;
  // (Component id, concrete components)
  Map<unsigned int, Array<unsigned int>> components;
  Array<ECSEntity*> entities;

  // Inline entity hook helpers
  inline ECSEntity* entityFrom(EntityHook hook) { return (ECSEntity*)hook; }
  inline unsigned int entityIDFrom(EntityHook hook)
  {
    return entityFrom(hook)->first;
  }
  inline Array<ComponentReference>& componentsFrom(EntityHook hook)
  {
    return entityFrom(hook)->second;
  }

  void deleteComponent(unsigned int componentID, unsigned int index);
  bool removeComponentInternal(EntityHook hook, unsigned int index);
  void addComponentInternal(EntityHook hook,
                            Array<ComponentReference>& entityComponents,
                            unsigned int componentID, BaseComponent* component);
  BaseComponent*
  getComponentInternal(Array<ComponentReference>& entityComponents,
                       unsigned int componentID);

  void updateComplexSystem(unsigned int index, SystemList& system, float delta,
                           const Array<unsigned int>& componentTypes,
                           Array<BaseComponent*>& componentParam,
                           Array<Array<unsigned int>*>& componentArrays);
  unsigned int

  getLeastCommonComponentID(const Array<unsigned int>& componentTypes,
                            const Array<unsigned int>& componentFlags);
};