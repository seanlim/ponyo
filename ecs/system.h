#pragma once

#include "component.h"
#include "vector"

class System
{
public:
  System(const std::vector<unsigned int>& _componentTypes)
      : componentTypes(_componentTypes)
  {
  }

  virtual void updateComponents(float delta, BaseComponent** components) {}

  const std::vector<unsigned int> getComponentTypes()
  {
    return this->componentTypes;
  }

private:
  std::vector<unsigned int> componentTypes;
};