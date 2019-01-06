#pragma once

#include "array.h"
#include "component.h"
#include "system.h"
#include <map>

// pair (Component type, Component id)
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

  // Components
  template <class Component>
  void addComponent(EntityHook entity, Component* component);
  template <class Component> void removeComponent(EntityHook entity);
  template <class Component> void getComponent(EntityHook entity);

  // Systems
  inline void addSystem(System& system) { systems.push_back(&system); }
  void updateSystems(float delta);
  void removeSystem(System& system);

private:
  Array<System*> systems;
  // map (Component type, Component id)
  std::map<unsigned int, Array<unsigned int>> components;
  Array<Entity*> entities;

  // Inline methods for entities
  inline Entity* hookEntity(EntityHook hook) { return (Entity*)hook; }
  inline unsigned int hookToEntityIndex(EntityHook hook)
  {
    return hookEntity(hook)->first;
  }
  inline Array<ComponentReference>& hookToEntity(EntityHook hook)
  {
    return hookEntity(hook)->second;
  }
};