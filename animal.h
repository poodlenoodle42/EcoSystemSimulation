#ifndef ANIMAL_H
#define ANIMAL_H
#include "entity.h"
#include <string>

class Animal : public Entity
{
public:
    Animal(Vector2 pos, Enviroment * currentEnviroment,float mutateProbability);
    virtual void Reproduce(const Animal * father) = 0;
    Gens gens = Gens();

    float getHunger() const;

    float getUrgeToReproduce() const;

    float getMutateProbability() const;

    enum class Activity{
        SearchingForFood,SearchingForMate,Fleeing,Hunting
    };
    Activity activity = Activity::SearchingForFood;
    enum class Gender{
        Male,Female,Whatever
    };
    Gender sex;
    enum class MostNeed{
        Foot,Water,Reproduce
    };
    MostNeed mostNeed = MostNeed::Foot;
    unsigned int generation = 1;
protected:
    Gens CalcNewGens(const Animal * father) const;
    float hunger = 10;
    float thirst = 0;
    float urgeToReproduce = 0;
    float mutateProbability;
    int maxChildren = 5;
    int minChildren = 3;
    void calcMostNeed();
    bool fertility = false;
    int infertilityDuration = 10;
    void eat(Entity &eaten);
};


#endif // ANIMAL_H
