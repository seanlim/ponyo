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

EntityHook ECS::makeEntity(BaseComponent* component,
                           const unsigned int* componentIDs,
                           size_t numComponents)
{
  Entity* newEntity = new Entity();
  EntityHook hook = (EntityHook)newEntity;

  // Set ID
  newEntity->first = entities.size();
  entities.push_back(newEntity);

  // TODO notify observers on entity construction

  return hook;
}

void ECS::removeEntity(EntityHook hook)
{
  Array<ComponentReference>& entity = componentsFrom(hook);

  for (int i = 0; i < entity.size(); i++)
    deleteComponent(entity[i].first, entity[i].second);

  int d = entityIDFrom(hook);
  int s = entities.size() - 1;
  delete entities[s];
  entities[d] = entities[s];
  entities[d]->first = d;
  entities.pop_back();
}

void ECS::addComponentInternal(EntityHook hook,
                               Array<ComponentReference>& entityComponents,
                               unsigned int componentID,
                               BaseComponent* component)
{
  ComponentCreateFunction createFn =
      BaseComponent::getTypeCreateFunction(componentID);
  ComponentReference newComponent;
  newComponent.first = componentID;
  newComponent.second = createFn(components[componentID], hook, component);
  entityComponents.push_back(newComponent);
}

void ECS::deleteComponent(unsigned int componentID, unsigned int index)
{
  Array<unsigned int>& componentInstances = components[componentID];
  ComponentFreeFunction freeFn =
      BaseComponent::getTypeFreeFunction(componentID);
  size_t typeSize = BaseComponent::getTypeSize(componentID);

  unsigned int srcIndex = componentInstances.size() - typeSize;

  BaseComponent* destComponent = (BaseComponent*)&componentInstances[index];
  BaseComponent* sourceComponent =
      (BaseComponent*)&componentInstances[srcIndex];

  freeFn(destComponent); // Free target component

  // Just resize memory if component is at last index
  if (componentID == srcIndex) {
    componentInstances.resize(srcIndex);
    return;
  }

  // Copy last component to target component and resize memory
  memcpy(destComponent, sourceComponent, typeSize);

  // Get collection of references to components of entity that is bound to our
  // target component
  Array<ComponentReference>& srcComponents =
      componentsFrom(sourceComponent->entity);

  // Update affected components in entity
  for (int i = 0; i < srcComponents.size(); i++) {
    if (componentID == srcComponents[i].first &&
        srcIndex == srcComponents[i].second) {
      srcComponents[i].second = componentID;
      break;
    }
  }

  componentInstances.resize(srcIndex);
}

// Delete component in entity
bool ECS::removeComponentInternal(EntityHook hook, unsigned int index)
{
  Array<ComponentReference>& entityComponents = componentsFrom(hook);
  for (int i = 0; i < entityComponents.size(); i++) {
    if (index == entityComponents[i].first) {
      deleteComponent(entityComponents[i].first, entityComponents[i].second);
      unsigned int srcIndex = entityComponents.size() - 1;
      unsigned int destIndex = i;
      entityComponents[destIndex] = entityComponents[srcIndex];
      entityComponents.pop_back();
      return true;
    }
  }
  return false;
}

BaseComponent*
ECS::getComponentInternal(Array<ComponentReference>& entityComponents,
                          unsigned int componentID)
{
  for (int i = 0; i < entityComponents.size(); i++) {
    if (componentID == entityComponents[i].first) {
      // Access component collection and get collection at index (from component
      // ref)
      return (
          BaseComponent*)&components[componentID][entityComponents[i].second];
    }
  }
  return nullptr;
}
