#pragma once

#include "component.h"
#include "system.h"
#include <map>

class ECS
{
public:
  ECS() {}
  ~ECS();

  // Entity
  EntityHook makeEntity(BaseComponent* component,
                        const unsigned int* componentIDs, size_t numComponents);

  // Component
  template <class Component>
  void addComponent(EntityHook entity, Component* component);
  template <class Component> void removeComponent(EntityHook entity);
  template <class Component> void getComponent(EntityHook entity);

  // System
  inline void addSystem(System& system) { systems.push_back(&system); }
  void updateSystems(float delta);
  void removeSystem(System& system);

private:
  std::vector<System*> systems;
  std::map<unsigned int, std::vector<unsigned int>> components;
  std::vector<std::pair<unsigned int,
                        std::vector<std::pair<unsigned int, unsigned int>*>>>
      entities;

  // Inline methods for entities
  inline std::pair<unsigned int,
                   std::vector<std::pair<unsigned int, unsigned int>>>*
  hookEntity(EntityHook hook)
  {
    return (std::pair<unsigned int,
                      std::vector<std::pair<unsigned int, unsigned int>>>*)hook;
  }
  inline unsigned int hookToEntityIndex(EntityHook hook)
  {
    return hookEntity(hook)->first;
  }
  inline std::vector<std::pair<unsigned int, unsigned int>>&
  hookToEntity(EntityHook hook)
  {
    return hookEntity(hook)->second;
  }
};