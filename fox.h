#ifndef FOX_H
#define FOX_H
#include "animal.h"

class Fox : public Animal
{
public:
    Fox(Vector2 pos, Enviroment * currentEnviroment,float mutateProbability);
    void Reproduce(const Animal * father) override;
    void update() override;
    friend Animal;
};

#endif // FOX_H
