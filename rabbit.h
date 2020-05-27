#ifndef RABBIT_H
#define RABBIT_H
#include "animal.h"

class Rabbit : public Animal
{
public:
    Rabbit(Vector2 pos, Enviroment * currentEnviroment,float mutateProbability);
    void update() override;
    void Reproduce(const Animal * father) override;
    friend Animal;
private:
};

#endif // RABBIT_H
