#pragma once

#include "array.h"
#include <tuple>

#define ENTITY_NULL_HANDLE nullptr;

typedef void* EntityHook;

struct BaseComponent; // Forward definition of BaseComponent

// Create and free function pointers
typedef unsigned int (*ComponentCreateFunction)(Array<unsigned int>& memory,
                                                EntityHook entity,
                                                BaseComponent* comp);
typedef void (*ComponentFreeFunction)(BaseComponent* comp);

struct BaseComponent {
public:
  // Add component type
  static unsigned int registerComponentType(ComponentCreateFunction createFn,
                                            ComponentFreeFunction freeFn,
                                            size_t size);

  EntityHook entity = ENTITY_NULL_HANDLE; // Blind reference to attached entity

  // Sugar for getters
  inline static ComponentCreateFunction getTypeCreateFunction(unsigned int id)
  {
    return std::get<0>((*componentTypes)[id]);
  }

  inline static ComponentFreeFunction getTypeFreeFunction(unsigned int id)
  {
    return std::get<1>((*componentTypes)[id]);
  }
  inline static size_t getTypeSize(unsigned int id)
  {
    return std::get<2>((*componentTypes)[id]);
  }

private:
  static Array<std::tuple<ComponentCreateFunction, ComponentFreeFunction,
                          size_t>>* componentTypes;
};

template <typename T> struct Component : BaseComponent {
  static const ComponentCreateFunction createFunction;
  static const ComponentFreeFunction freeFunction;
  static const unsigned int id;
  static const size_t size;
};

template <typename Component>
unsigned int CreateComponent(Array<unsigned int>& memory, EntityHook entity,
                             BaseComponent* comp)
{
  unsigned int index = memory.size();
  memory.resize(index + Component::size);
  Component* component = new (&memory[index]) Component(*(Component*)comp);
  component->entity = entity;
  return index;
}

template <typename Component> void FreeComponent(BaseComponent* comp)
{
  Component* component = (Component*)comp;
  component->~Component();
}

template <typename T>
const ComponentCreateFunction Component<T>::createFunction(CreateComponent<T>);
template <typename T>
const ComponentFreeFunction Component<T>::freeFunction(FreeComponent<T>);

template <typename T> const size_t Component<T>::size(sizeof(T));

template <typename T>
const unsigned int Component<T>::id(BaseComponent::registerComponentType(
    CreateComponent<T>, FreeComponent<T>, sizeof(T)));

// struct ExampleComponent : public Component<ExampleComponent> {
//   float x, y;
// };