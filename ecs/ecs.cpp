#include "ecs.h"

ECS::~ECS()
{
  // Free components of all types
  for (std::map<unsigned int, Array<unsigned int>>::iterator it =
           components.begin();
       it != components.end(); ++it) {
    size_t typeSize = BaseComponent::getTypeSize(it->first);
    ComponentFreeFunction freefn =
        BaseComponent::getTypeFreeFunction(it->first);
    for (int i = 0; i < it->second.size(); i += typeSize)
      freefn((BaseComponent*)&it->second[i]);
  }

  // Delete all entities
  for (int i = 0; i < entities.size(); i++) delete entities[i];

}