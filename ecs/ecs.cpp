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

void ECS::updateSystems(SystemList& systems, float delta)
{
  Array<BaseComponent*> componentParam;
  Array<Array<unsigned int>*> componentArrays;

  for (int i = 0; i < systems.size(); i++) {
    const Array<unsigned int>& componentTypes = systems[i]->getComponentTypes();
    if (componentTypes.size() == 1) {
      size_t typeSize = BaseComponent::getTypeSize(componentTypes[0]);
      Array<unsigned int>& concreteComponents = components[componentTypes[0]];
      for (int j = 0; j < concreteComponents.size(); j += typeSize) {
        BaseComponent* component = (BaseComponent*)&concreteComponents[j];
        systems[i]->updateComponents(delta, &component);
      }
    } else if (componentTypes.size() > 1) {
      // More than one component type
      updateComplexSystem(i, systems, delta, componentTypes, componentParam,
                          componentArrays);
    }
  }
}

// Returns index of component type with the least instances
unsigned int
ECS::getLeastCommonComponentID(const Array<unsigned int>& componentTypes,
                               const Array<unsigned int>& componentFlags)
{
  unsigned int minSize = (unsigned int)-1;
  unsigned int minIndex = (unsigned int)-1;

  for (int i = 0; i < componentTypes.size(); i++) {

    if ((componentFlags[i] & System::FLAG_OPTIONAL) != 0) continue;

    size_t typeSize = BaseComponent::getTypeSize(componentTypes[i]);
    unsigned int s = components[componentTypes[i]].size() / typeSize;
    if (s <= minSize) {
      minSize = s;
      minIndex = i;
    }
  }

  return minIndex;
}

void ECS::updateComplexSystem(unsigned int index, SystemList& system,
                              float delta,
                              const Array<unsigned int>& componentTypes,
                              Array<BaseComponent*>& componentParam,
                              Array<Array<unsigned int>*>& componentArrays)
{

  const Array<unsigned int>& componentFlags =
      systems[index]->getComponentFlags();

  componentParam.resize(std::max(componentParam.size(), componentTypes.size()));
  componentArrays.resize(
      std::max(componentArrays.size(), componentTypes.size()));

  for (int i = 0; i < componentTypes.size(); i++)
    componentArrays[i] = &components[componentTypes[i]];

  // Get index of component type that has the smallest colleciton of components
  unsigned int minSizeIndex =
      getLeastCommonComponentID(componentTypes, componentFlags);

  size_t typeSize = BaseComponent::getTypeSize(componentTypes[minSizeIndex]);
  Array<unsigned int>& smallestComponentCollection =
      *componentArrays[minSizeIndex];

  for (int i = 0; i < smallestComponentCollection.size(); i += typeSize) {
    componentParam[minSizeIndex] =
        (BaseComponent*)&smallestComponentCollection[i];

    Array<ComponentReference>& entityComponents =
        componentsFrom(componentParam[minSizeIndex]->entity);

    bool isValid = true;

    for (int j = 0; j < componentTypes.size(); j++) {
      if (j == minSizeIndex) continue;

      // TODO: getComponentInternal may not return the correct component in this
      // case
      componentParam[j] =
          getComponentInternal(entityComponents, componentTypes[j]);

      if (componentParam[j] == nullptr &&
          (componentFlags[j] & System::FLAG_OPTIONAL) == 0) {
        isValid = false;
        break;
      }
    }

    if (isValid) systems[index]->updateComponents(delta, &componentParam[0]);
  }
}