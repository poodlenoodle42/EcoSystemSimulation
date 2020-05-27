#ifndef PLANT_H
#define PLANT_H
#include "entity.h"

class Plant : public Entity
{
public:
    Plant(Vector2 pos, Enviroment * currentEnviroment);
    void update() override;
};

#endif // PLANT_H
