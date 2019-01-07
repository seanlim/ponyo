#pragma once

#include "array.h"
#include "component.h"
#include "map.h"
#include "system.h"
#include <algorithm>

// pair (Component id, Component index)
typedef std::pair<unsigned int, unsigned int> ComponentReference;
// pair (Entity id, components)
typedef std::pair<unsigned int, Array<ComponentReference>> Entity;

class ECS
{
public:
  ECS() {}
  ~ECS();

  // Entities
  EntityHook makeEntity(BaseComponent* component,
                        const unsigned int* componentIDs, size_t numComponents);
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
  // Map component id to concrete components
  Map<unsigned int, Array<unsigned int>> components;
  Array<Entity*> entities;

  // Inline helpers
  inline Entity* entityFrom(EntityHook hook) { return (Entity*)hook; }
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

  // Complex systems are systems that have more than one component
  void updateComplexSystem(unsigned int index, SystemList& system, float delta,
                           const Array<unsigned int>& componentTypes,
                           Array<BaseComponent*>& componentParam,
                           Array<Array<unsigned int>*>& componentArrays);
  unsigned int
  getLeastCommonComponentID(const Array<unsigned int>& componentTypes,
                            const Array<unsigned int>& componentFlags);
};