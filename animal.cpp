#include "animal.h"
//Just testing
#include <iostream>
//
Animal::Animal(Vector2 pos, Enviroment * enviroment, float mutateProbability) : Entity(pos,enviroment)
{
    std::lock_guard<std::mutex> lock(accessEntity);
    this->mutateProbability = mutateProbability;
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> sexGenerator(0,100);
    int sex = sexGenerator(mt);
    if(sex < 50){
        this->sex = Gender::Male;
    }
    else if(sex >= 50 && sex<=99){
        this->sex = Gender::Female;
    }
    else{
        this->sex = Gender::Whatever;
    }

    eType = EntityType::Animal;
}


Gens Animal::CalcNewGens(const Animal *father)const{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(0,100);
    std::uniform_int_distribution<short> randBool(0,1);
   // float random = dist(mt);
    Gens newGens;
    //Gens from which parent
    {
        std::lock_guard<std::mutex> lockfather(father->accessEntity);
        //std::lock_guard lockmother(accessEntity); already locked in Reproduce
        newGens.speed = randBool(mt) ? father->gens.speed : this->gens.speed;
        newGens.senseRadius = randBool(mt) ? father->gens.senseRadius : this->gens.senseRadius;
        newGens.energyEfficiency = randBool(mt) ? father->gens.energyEfficiency : this->gens.energyEfficiency;
    }
    if(dist(mt) < mutateProbability){
        if(dist(mt)>=50){//50-50 of Positive or negative mutation
            newGens.energyEfficiency += 0.3f;
        }
        else{
            newGens.energyEfficiency -= 0.3f;
        }
    }

    if(dist(mt) < mutateProbability){
        if(dist(mt)>=50){//50-50 of Positive or negative mutation
            newGens.speed += 0.3f;
        }
        else{
            newGens.speed -= 0.3f;
        }
    }

    if(dist(mt) < mutateProbability){
        if(dist(mt)>=50){//50-50 of Positive or negativ mutation
            newGens.senseRadius +=  3;
        }
        else{
            newGens.senseRadius -= 3;
        }
    }
    if(newGens.speed<0.1){
        newGens.speed = 0.1;
    }
    if(newGens.energyEfficiency<0.2){
        newGens.energyEfficiency = 0.2;
    }
    return newGens;
}

float Animal::getMutateProbability() const
{
    return mutateProbability;
}

float Animal::getUrgeToReproduce() const
{
    return urgeToReproduce;
}

float Animal::getHunger() const
{
    return hunger;
}


void Animal::calcMostNeed(){
    //std::lock_guard lock(accessEntity); already locked in update
    if(hunger>thirst && hunger>=urgeToReproduce){
        mostNeed = MostNeed::Foot;
    }
    else if(thirst>=hunger && thirst>= urgeToReproduce){
        mostNeed = MostNeed::Water;
    }
    else{
        mostNeed = MostNeed::Reproduce;
    }
}

void Animal::eat(Entity &eaten){
    //std::lock_guard lockeaten(eaten.accessEntity); should already be locked (in update)
    int entityIndex = nextEnviroment->getEntityIndexByID(eaten.id);
    if(entityIndex == -1 ){
        // Entity with this id was already deleted for some reason
        //std::cout << "Entity with this id was already deleted for some reason" <<std::endl;
    }
    else{
        eaten.dead = true;
        nextEnviroment->removeEntity(entityIndex);
        //std::lock_guard lockthis(accessEntity); should already be locked (in update)
        if(this->eType == EntityType::Fox){
            this->hunger -= 20;
        }
        this->hunger -= this->hunger*0.2;
    }
}
