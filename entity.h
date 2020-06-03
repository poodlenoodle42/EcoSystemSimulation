#ifndef ENTITY_H
#define ENTITY_H
#include "vector2.h"
#include <memory>
#include <random>
#include <map>
#include <mutex>
struct Enviroment;

enum class EntityType{
    Plant,Water,Rabbit,Fox,Entity,Animal
};
class Entity
{
public:
    //Entity();
    mutable std::mutex accessEntity;
    Entity(const Vector2 &pos, Enviroment * currentEnviroment);
    Vector2 pos;

    EntityType eType = EntityType::Entity;
    virtual void update() = 0;
    uint id;
    static int idCount;
    void setnextEnviroment(Enviroment * nextEnviroment);
    bool dead = false;
    mutable std::mutex accessUpdated;
    bool updated = false;
    bool operator == (const Entity &e)const;
protected:
    Enviroment * currentEnviroment; //former Enviroment
    Enviroment * nextEnviroment;


};

struct Gens{
    float speed = 0.5;
    float energyEfficiency = 1.5;
    float senseRadius = 15;


    //Just used in statistics for average
    const Gens operator + (const Gens &in)const{
        Gens newGens;
        newGens.speed = this->speed + in.speed;
        newGens.senseRadius = this->senseRadius + in.senseRadius;
        newGens.energyEfficiency = this->energyEfficiency + in.energyEfficiency;
        return  newGens;
    }
};

struct EnviromentStatistics{
    std::map<EntityType,unsigned int> typeCounts;
    Gens averageFoxGens;
    Gens averageRabbitGens;
};
class Animal;
struct Enviroment{
    Enviroment() = default;
    Enviroment(const Enviroment &env){
        Entitys = env.Entitys;
        minCoordinates = env.minCoordinates;
        maxCoordinates = env.maxCoordinates;
    }
    Enviroment& operator = (const Enviroment &env){
        Entitys = env.Entitys;
        minCoordinates = env.minCoordinates;
        maxCoordinates = env.maxCoordinates;
        return *this;

    }
    mutable std::mutex protectEntitys;
    std::vector<std::shared_ptr<Entity>> Entitys;
    Vector2 minCoordinates;
    Vector2 maxCoordinates;
    void addEntity(std::shared_ptr<Entity> e){
        Entitys.push_back(e);
    }
    void removeEntity(int pos){
        std::lock_guard<std::mutex> lock(protectEntitys);
        Entitys.erase(Entitys.begin()+pos);
    }
    int getEntityIndexByID(uint entityID)const{
        int index = 0;
        std::lock_guard l(protectEntitys);
        for(const std::shared_ptr<Entity> e : Entitys){
            if(entityID == e->id){
                return index;
            }
            index++;
        }
        return -1;
    }
    void newPlants(const int amountOfPlants);

    EnviromentStatistics calcEnviromentStatistics()const;

    const std::shared_ptr<Animal> getAnimal(const std::shared_ptr<Animal> an)const;

    void resetUpdated(){
        std::lock_guard<std::mutex> lock(protectEntitys);
        for(auto entity : Entitys){
            entity->updated = false;
        }
    }

};





#endif // ENTITY_H
