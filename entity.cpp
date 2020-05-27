#include "entity.h"
#include "plant.h"
#include "animal.h"
int Entity::idCount = 0;
/*
Entity::Entity(){
    pos = Vector2(0,0);
    id = this->idCount;
    this->idCount++;
}
*/
Entity::Entity(const Vector2 &actPos, Enviroment * enviroment)
{
    pos = actPos;
    id = this->idCount;
    this->idCount++;
    this->currentEnviroment = enviroment;
}

void Entity::setnextEnviroment(Enviroment *nextEnviroment){
    this->nextEnviroment = nextEnviroment;
}


void Enviroment::newPlants(const int amountOfNewPlants){
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(0,100);
    for(int i = 0;i<amountOfNewPlants;i++){
        addEntity(std::make_shared<Plant>(Vector2::randomVec(minCoordinates,maxCoordinates),this));
    }
}

EnviromentStatistics Enviroment::calcEnviromentStatistics()const{
    EnviromentStatistics statistics;
    statistics.averageFoxGens.speed = 0;
    statistics.averageFoxGens.senseRadius = 0;
    statistics.averageFoxGens.energyEfficiency = 0;
    statistics.averageRabbitGens.speed = 0;
    statistics.averageRabbitGens.senseRadius = 0;
    statistics.averageRabbitGens.energyEfficiency = 0;
    for(const auto entity : Entitys){
        statistics.typeCounts[entity->eType]++;
        if(entity->eType == EntityType::Rabbit){
            statistics.averageRabbitGens = statistics.averageRabbitGens +
                    dynamic_cast<Animal*>(entity.get())->gens;
        }
        else if(entity->eType == EntityType::Fox){
            statistics.averageFoxGens = statistics.averageFoxGens +
                    dynamic_cast<Animal*>(entity.get())->gens;
        }
    }
    statistics.averageFoxGens.speed /= statistics.typeCounts[EntityType::Fox];
    statistics.averageFoxGens.senseRadius /= statistics.typeCounts[EntityType::Fox];
    statistics.averageFoxGens.energyEfficiency /= statistics.typeCounts[EntityType::Fox];
    statistics.averageRabbitGens.speed /= statistics.typeCounts[EntityType::Rabbit];
    statistics.averageRabbitGens.senseRadius /= statistics.typeCounts[EntityType::Rabbit];
    statistics.averageRabbitGens.energyEfficiency /= statistics.typeCounts[EntityType::Rabbit];
    return statistics;
}
const std::shared_ptr<Animal> Enviroment::getAnimal(const std::shared_ptr<Animal> an)const
{
    for(const auto entity : Entitys){
        if(entity->eType == an->eType){
            return std::dynamic_pointer_cast<Animal>(entity);
        }
    }
    return an;
}
