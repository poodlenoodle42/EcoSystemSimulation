#include "rabbit.h"
#include "plant.h"
#include "fox.h"
Rabbit::Rabbit(Vector2 pos, Enviroment * enviroment,float mutateProbability):Animal(pos,enviroment,mutateProbability)
{
    eType = EntityType::Rabbit;
    //Gender
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

void Rabbit::update() {
    std::lock_guard thisLock(accessEntity);
    if(this->hunger > 200){
        //Starving to death
        this->dead = true;
        int index = nextEnviroment->getEntityIndexByID(this->id);
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
    std::vector<Plant*> nearbyPlants;
    std::vector<Fox*> nearbyFoxes;
    std::vector<Rabbit*> nearbyRabbit;
    calcMostNeed();
    EntityType entityTypeToInteract = EntityType::Plant;
    for(const std::shared_ptr<Entity> entity : currentEnviroment->Entitys){
        if(!(*entity == *this )){
            //std::lock_guard lockE(entity->accessEntity);
            float dis = Vector2::distance(this->pos, entity->pos);
            if(dis <= gens.senseRadius){
                if(entity->eType == EntityType::Fox){
                    nearbyFoxes.push_back(dynamic_cast<Fox*>(entity.get()));
                    entityTypeToInteract = EntityType::Fox;

                }
                else if(entity->eType == EntityType::Plant
                        && entityTypeToInteract == EntityType::Plant){
                    nearbyPlants.push_back(dynamic_cast<Plant*>(entity.get()));
                    entityTypeToInteract = EntityType::Plant;
                }
                else if(entity->eType == EntityType::Rabbit && mostNeed == MostNeed::Reproduce
                        && entityTypeToInteract != EntityType::Fox){
                    nearbyRabbit.push_back(dynamic_cast<Rabbit*>(entity.get()));
                    if(nearbyRabbit.back()->sex != this->sex && this->fertility == true){
                        entityTypeToInteract = EntityType::Rabbit;
                    }
                }
                //Water needed
            }
        }
    }
    if(entityTypeToInteract == EntityType::Fox){
        Fox * nearestFox;
        float minDistance = gens.senseRadius*2;
        for(Fox * fox : nearbyFoxes){
            //std::lock_guard lockFox(fox->accessEntity);
            float distance = Vector2::distance(this->pos,fox->pos);
            if(distance<minDistance){
                minDistance = distance;
                nearestFox = fox;
            }
        }
        //std::lock_guard l(nearestFox->accessEntity);
        direction = this->pos - nearestFox->pos;
        direction.normalize();
    }
    else if(entityTypeToInteract == EntityType::Plant && this->hunger > 10){
        Plant * nearestPlant;
        float minDistance = gens.senseRadius*2;
        bool nearestPlantNotEaten = false;
        for(Plant * plant : nearbyPlants){
            //std::lock_guard lockPlant(plant->accessEntity);
            float distance = Vector2::distance(this->pos,plant->pos);
            if(distance < 1){
                this->eat(*plant);
                break;
            }
            else if(distance<minDistance){
                minDistance = distance;
                nearestPlantNotEaten = true;
                nearestPlant = plant;
            }
        }
        if(nearestPlantNotEaten){
            std::lock_guard l(nearestPlant->accessEntity);
            direction = nearestPlant->pos - this->pos;
            direction.normalize();
        }
    }
    else if(entityTypeToInteract == EntityType::Rabbit){
        Rabbit * nearestRabbit;
        float minDistance = gens.senseRadius*2;
        bool mateFound = false;
        for(Rabbit * rabbit : nearbyRabbit){
            //std::lock_guard lockRabbit(rabbit->accessEntity);
            if(rabbit->sex != this->sex && rabbit->fertility == true && this->fertility == true){
                mateFound = true;
                float distance = Vector2::distance(this->pos,rabbit->pos);
                if(distance < 2
                        && this->sex == Gender::Female){
                    Reproduce(rabbit);

                    mateFound = false;
                    break;
                }
                else if(distance<2){
                    mateFound = false;
                    this->urgeToReproduce -= 50;
                    this->fertility = false;
                    this->infertilityDuration = 200;
                    this->hunger += 20;
                }
                else if(distance<minDistance){
                    minDistance = distance;
                    nearestRabbit = rabbit;
                }
            }
        }
        if(mateFound){
            //std::lock_guard l(nearestRabbit->accessEntity);
            direction =  nearestRabbit->pos - this->pos;
            direction.normalize();
        }

    }
    hunger += this->gens.speed * this->gens.energyEfficiency;
    urgeToReproduce += 0.1f;
    this->pos = this->pos + (direction*gens.speed);
    //Update of Water
    if(entityTypeToInteract == EntityType::Rabbit){
        activity = Activity::SearchingForMate;
    }
    else if(entityTypeToInteract == EntityType::Plant){
        activity = Activity::SearchingForFood;
    }
    else if(entityTypeToInteract == EntityType::Fox){
        activity = Activity::Fleeing;
    }
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
}


void Rabbit::Reproduce(const Animal * father){
    //std::lock_guard thisLock(accessEntity);
    this->fertility = false;
    this->infertilityDuration = 200;
    this->urgeToReproduce -= 50;
    this->hunger += 20;
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(minChildren,maxChildren);
    int numChildren = dist(mt);
    std::vector<std::shared_ptr<Rabbit>> ans(numChildren,std::make_shared<Rabbit>(Vector2(0,0),
                                                                                  currentEnviroment,this->mutateProbability));
    for (auto an:ans){
        an->generation += this->generation;
        an->pos = this->pos;

        an->eType = this->eType;
        an->currentEnviroment = this->currentEnviroment;

        an->hunger = this->hunger; //Needs to find food first
        an->maxChildren = this->maxChildren;
        an->minChildren = this->minChildren;
        an->gens = CalcNewGens(father);
        nextEnviroment->addEntity(an);

    }

}
