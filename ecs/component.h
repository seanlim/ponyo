#pragma once

#include <vector>

#define ENTITY_NULL_HANDLE nullptr;

typedef void* EntityHook;

struct BaseComponent;
typedef unsigned int (*ComponentCreateFunction)(
    std::vector<unsigned int>& memory, EntityHook entity, BaseComponent* comp);
typedef void (*ComponentFreeFunction)(BaseComponent* comp);

struct BaseComponent {
  static unsigned int nextID();
  EntityHook entity = ENTITY_NULL_HANDLE; // Blind reference to attached entity
};

// Recurring template
template <typename T> struct Component : BaseComponent {
  static const ComponentCreateFunction create;
  static const ComponentFreeFunction free;
  static const unsigned int id;
  static const size_t size;
};

template <typename Component>
unsigned int CreateComponent(std::vector<unsigned int>& memory,
                             EntityHook entity, BaseComponent* comp)
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
const ComponentCreateFunction Component<T>::create(CreateComponent<T>);
template <typename T>
const ComponentFreeFunction Component<T>::free(FreeComponent<T>);

template <typename T>
const unsigned int Component<T>::id(BaseComponent::nextID());

template <typename T> const size_t Component<T>::size(sizeof(T));

// struct ExampleComponent : public Component<ExampleComponent> {
//   float x, y;
// };