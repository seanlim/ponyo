#pragma once

#include "array.h"
#include "component.h"

class System
{
public:
  enum {
    FLAG_OPTIONAL = 1,
  };
  System() {}
  const Array<unsigned int>& getComponentTypes() { return componentTypes; }
  const Array<unsigned int>& getComponentFlags() { return componentFlags; }
  virtual void updateComponents(float delta, BaseComponent** components) {}

  bool isValid()
  {
    for (int i = 0; i < componentFlags.size(); i++) {
      if ((componentFlags[i] & System::FLAG_OPTIONAL) == 0) return true;
    }
    return false;
  }

private:
  Array<unsigned int> componentTypes;
  Array<unsigned int> componentFlags;
};

class SystemList
{
public:
  inline bool addSystem(System& system)
  {
    if (!system.isValid()) return false;

    systems.push_back(&system);
    return true;
  }

  inline size_t size() { return systems.size(); }

  inline System* operator[](unsigned int index) { return systems[index]; }

  bool removeSystem(System& system)
  {
    for (int i = 0; i < systems.size(); i++) {
      if (&system == systems[i]) {
        systems.erase(systems.begin() + i);
        return true;
      }
    }

    return false;
  }

private:
  Array<System*> systems;
}