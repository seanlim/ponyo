#include "component.h"

static unsigned int componentID = 0;

unsigned int BaseComponent::nextID() { return componentID++; }