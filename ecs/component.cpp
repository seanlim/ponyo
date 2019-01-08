#include "component.h"

Array<std::tuple<ComponentCreateFunction, ComponentFreeFunction, size_t>>*
    BaseComponent::componentTypes;

unsigned int
BaseComponent::registerComponentType(ComponentCreateFunction createFn,
                                     ComponentFreeFunction freeFn, size_t size)
{
  if (componentTypes == nullptr)
    componentTypes = new Array<
        std::tuple<ComponentCreateFunction, ComponentFreeFunction, size_t>>();

  unsigned int componentID = componentTypes->size();
  componentTypes->push_back(
      std::tuple<ComponentCreateFunction, ComponentFreeFunction, size_t>(
          createFn, freeFn, size));
  return componentID;
}