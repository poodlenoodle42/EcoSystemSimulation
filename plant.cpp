#include "plant.h"

Plant::Plant(Vector2 pos, Enviroment * enviroment):Entity(pos,enviroment)
{
    eType = EntityType::Plant;
}

void Plant::update(){
}
