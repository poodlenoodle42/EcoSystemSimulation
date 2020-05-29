#include "fox.h"
#include "rabbit.h"

Fox::Fox(Vector2 pos,  Enviroment * enviroment,float mutateProbability) : Animal(pos,enviroment,mutateProbability)
{
    eType = EntityType::Fox;
    this->gens.speed *= 2;
    this->gens.senseRadius *= 1.5;
    this->gens.energyEfficiency /= 1.5;
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution randBool(0,1);
    if(randBool(mt)){
        this->sex = Gender::Male;
    }
    else{
        this->sex = Gender::Female;
    }
}


void Fox::update(){
    std::lock_guard thisLock(accessEntity);
    if(this->hunger > 200){
        //Starving to death
        int index = nextEnviroment->getEntityIndexByID(this->id);
        this->dead = true;
        if(index != -1){
            nextEnviroment->removeEntity(index);
        }
    }
    if(this->infertilityDuration > 0){
        infertilityDuration--;
    }
    else if(this->infertilityDuration == 0){
        fertility = true;
    }
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> dist(-100,100);
    Vector2 direction(dist(mt),dist(mt));
    direction.normalize();
    std::vector<Fox*> nearbyFoxes;
    std::vector<Rabbit*> nearbyRabbits;
    calcMostNeed();
    EntityType entityTypeToInteract = EntityType::Rabbit;
    for(const std::shared_ptr<Entity> entity : currentEnviroment->Entitys){
        if(!(*entity == *this )){
            std::lock_guard lockE(entity->accessEntity);
            float dis = Vector2::distance(this->pos, entity->pos);
            if(dis <= gens.senseRadius){
                if (entity->eType == EntityType::Rabbit && entityTypeToInteract == EntityType::Rabbit ) {
                    nearbyRabbits.push_back(dynamic_cast<Rabbit*>(entity.get()));
                    entityTypeToInteract = EntityType::Rabbit;
                }
                else if(entity->eType == EntityType::Fox && mostNeed == MostNeed::Reproduce){
                    nearbyFoxes.push_back(dynamic_cast<Fox*>(entity.get()));
                    if(nearbyFoxes.back()->sex != this->sex && this->fertility == true){
                        entityTypeToInteract = EntityType::Fox;
                    }
                }
                //Water needed
            }
        }
    }
    if(entityTypeToInteract == EntityType::Fox){
        Fox * nearestFox;
        float minDistance = gens.senseRadius;
        bool mateFound = false;
        for(Fox * fox : nearbyFoxes){
            std::lock_guard lockFox(fox->accessEntity);
            if(fox->sex != this->sex && fox->fertility == true && this->fertility == true){
                mateFound = true;
                float distance = Vector2::distance(this->pos,fox->pos);
                if(distance < 2 && this->sex == Gender::Female){
                    Reproduce(fox);
                    mateFound = false;
                    break;
                }
                else if(distance < 2){
                    mateFound = false;
                    this->urgeToReproduce -= 50;
                    this->fertility = false;
                    this->infertilityDuration = 500;
                    this->hunger += 20;
                }
                else if(distance<minDistance){
                    minDistance = distance;
                    nearestFox = fox;
                }
            }
        }
        if(mateFound){
            std::lock_guard l(nearestFox->accessEntity);
            direction = nearestFox->pos - this->pos;
            direction.normalize();
        }
    }
    else if(entityTypeToInteract == EntityType::Rabbit && this->hunger > 20){
        Rabbit * nearestRabbit;
        float minDistance = gens.senseRadius;
        bool nearRabbitNotEaten = false;
        for(Rabbit * rabbit : nearbyRabbits){
            std::lock_guard lockRabbit(rabbit->accessEntity);
            float distance = Vector2::distance(this->pos,rabbit->pos);
            if(distance<3){
                this->eat(*rabbit);
                break;
            }
            else if(distance<minDistance){
                nearRabbitNotEaten = true;
                minDistance = distance;
                nearestRabbit = rabbit;
            }
        }
        if(nearRabbitNotEaten){
            std::lock_guard lockRabbit(nearestRabbit->accessEntity);
            direction = nearestRabbit->pos - this->pos;
            direction.normalize();
        }

    }
    hunger += this->gens.speed * this->gens.energyEfficiency;
    urgeToReproduce += 0.1f;
    if(entityTypeToInteract == EntityType::Fox){
        activity = Activity::SearchingForMate;
    }
    else if(entityTypeToInteract == EntityType::Rabbit){
        activity = Activity::Hunting;
    }
    this->pos = this->pos + (direction*gens.speed);
    //Until better solution
    if(this->pos.x>currentEnviroment->maxCoordinates.x){
        this->pos.x = currentEnviroment->maxCoordinates.x;
    }
    else if(this->pos.x<currentEnviroment->minCoordinates.x){
        this->pos.x = currentEnviroment->minCoordinates.x;
    }
    if(this->pos.y>currentEnviroment->maxCoordinates.y){
        this->pos.y = currentEnviroment->maxCoordinates.y;
    }

    else if(this->pos.y<currentEnviroment->minCoordinates.y){
        this->pos.y = currentEnviroment->minCoordinates.y;
    }
    //Update of Water
}

void Fox::Reproduce(const Animal * father){
    std::lock_guard thisLock(accessEntity);
    this->fertility = false;
    this->infertilityDuration = 500;
    this->urgeToReproduce -= 50;
    this->hunger+=20;
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(minChildren,maxChildren);
    int numChildren = dist(mt);
    std::vector<std::shared_ptr<Fox>> ans(numChildren,std::make_shared<Fox>(Vector2(0,0),
                                                                            currentEnviroment,this->mutateProbability));
    for (auto an:ans){
        an->generation += this->generation;
        an->pos = this->pos;
        an->eType = this->eType;
        an->infertilityDuration = 300;
        an->hunger = this->hunger;
        an->maxChildren = this->maxChildren;
        an->minChildren = this->minChildren;
        an->gens = CalcNewGens(father);
        nextEnviroment->addEntity(an);

    }

}
