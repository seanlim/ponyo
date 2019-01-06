#include "ecs.h"

ECS::~ECS()
{
  for (std::map<unsigned int, std::vector<unsigned int>>::iterator it =
           components.begin();
       it != components.end(); ++it) {
    size_t typeSize = BaseComponent::getTypeSize(it->first);
    ComponentFreeFunction freefn =
        BaseComponent::getTypeFreeFunction(it->first);
    for (int i = 0; i < it->second.size(); i += typeSize)
      freefn((BaseComponent*)&it->second[i]);
  }
  for (int i = 0; i < entities.size(); i++) delete entities[i];
}
