#include "simulationwindow.h"
#include "ui_simulationwindow.h"
#include <iostream>
#include "fox.h"
#include "rabbit.h"
#include "plant.h"
#include <QTimer>
#include <QDebug>

SimulationWindow::SimulationWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SimulationWindow)
{
    ui->setupUi(this);
    this->initilizeEnviroment();
    ui->simulationView->updateEnviroment(&enviroment);
    timer = new QTimer(this);
    QObject::connect(timer,SIGNAL(timeout()),this,SLOT(updateEnviroment()));

    connect(ui->StartButton_2,SIGNAL(clicked()),this,SLOT(on_StartButton_clicked()));
    connect(ui->PauseButton_2,SIGNAL(clicked()),this,SLOT(on_PauseButton_clicked()));
    connect(ui->ResetButton_2,SIGNAL(clicked()),this,SLOT(on_ResetButton_clicked()));
    //Init Charts
    foxPopulation = new QChart();
    foxPopulationS = new QLineSeries(foxPopulation);
    foxPopulation->legend()->hide();
    foxPopulation->addSeries(foxPopulationS);
    foxPopulation->setTitle("Fox Population over Time");
    foxPopulation->createDefaultAxes();
    foxPopulation->axisX()->setTitleText("Time");
    foxPopulation->axisY()->setTitleText("Number of Foxes");
    ui->FoxPopulation->setChart(foxPopulation);

    foxSpeed = new QChart();
    foxSpeedS = new QLineSeries(foxSpeed);
    foxSpeed->legend()->hide();
    foxSpeed->addSeries(foxSpeedS);
    foxSpeed->setTitle("Fox Average Speed over Time");
    foxSpeed->createDefaultAxes();
    foxSpeed->axisX()->setTitleText("Time");
    foxSpeed->axisY()->setTitleText("Average Speed");
    ui->FoxSpeed->setChart(foxSpeed);

    foxSenseRadius = new QChart();
    foxSenseRadiusS = new QLineSeries(foxSenseRadius);
    foxSenseRadius->legend()->hide();
    foxSenseRadius->addSeries(foxSenseRadiusS);
    foxSenseRadius->setTitle("Fox Average Sense Radius over Time");
    foxSenseRadius->createDefaultAxes();
    foxSenseRadius->axisX()->setTitleText("Time");
    foxSenseRadius->axisY()->setTitleText("Average Sense Radius");
    ui->FoxSenseRadius->setChart(foxSenseRadius);

    foxEnergyEficiency = new QChart();
    foxEnergyEficiencyS = new QLineSeries(foxEnergyEficiency);
    foxEnergyEficiency->legend()->hide();
    foxEnergyEficiency->addSeries(foxEnergyEficiencyS);
    foxEnergyEficiency->setTitle("Fox Average Energy Efficiency over Time");
    foxEnergyEficiency->createDefaultAxes();
    foxEnergyEficiency->axisX()->setTitleText("Time");
    foxEnergyEficiency->axisY()->setTitleText("Average Energy Efficiency");
    ui->FoxEnergyEfficiency->setChart(foxEnergyEficiency);

    rabbitPopulation = new QChart();
    rabbitPopulationS = new QLineSeries(rabbitPopulation);
    rabbitPopulation->legend()->hide();
    rabbitPopulation->addSeries(rabbitPopulationS);
    rabbitPopulation->setTitle("Rabbit Population over Time");
    rabbitPopulation->createDefaultAxes();
    rabbitPopulation->axisX()->setTitleText("Time");
    rabbitPopulation->axisY()->setTitleText("Number of Rabbits");
    ui->RabbitPopulation->setChart(rabbitPopulation);

    rabbitSpeed = new QChart();
    rabbitSpeedS = new QLineSeries(rabbitSpeed);
    rabbitSpeed->legend()->hide();
    rabbitSpeed->addSeries(rabbitSpeedS);
    rabbitSpeed->setTitle("Rabbit Average Speed over Time");
    rabbitSpeed->createDefaultAxes();
    rabbitSpeed->axisX()->setTitleText("Time");
    rabbitSpeed->axisY()->setTitleText("Average Speed");
    ui->RabbitSpeed->setChart(rabbitSpeed);

    rabbitSenseRadius = new QChart();
    rabbitSenseRadiusS = new QLineSeries(rabbitSenseRadius);
    rabbitSenseRadius->legend()->hide();
    rabbitSenseRadius->addSeries(rabbitSenseRadiusS);
    rabbitSenseRadius->setTitle("Rabbit Average Sense Radius over Time");
    rabbitSenseRadius->createDefaultAxes();
    rabbitSenseRadius->axisX()->setTitleText("Time");
    rabbitSenseRadius->axisY()->setTitleText("Average Sense Radius");
    ui->RabbitSenseRadius->setChart(rabbitSenseRadius);

    rabbitEnergyEfficiency = new QChart();
    rabbitEnergyEfficiencyS = new QLineSeries(rabbitEnergyEfficiency);
    rabbitEnergyEfficiency->legend()->hide();
    rabbitEnergyEfficiency->addSeries(rabbitEnergyEfficiencyS);
    rabbitEnergyEfficiency->setTitle("Rabbit Average Energy Efficiency over Time");
    rabbitEnergyEfficiency->createDefaultAxes();
    rabbitEnergyEfficiency->axisX()->setTitleText("Time");
    rabbitEnergyEfficiency->axisY()->setTitleText("Average Energy Efficiency");
    ui->RabbitEnergyEfficiency->setChart(rabbitEnergyEfficiency);

    totalPlants = new QChart();
    totalPlantsS = new QLineSeries();
    totalPlants->legend()->hide();
    totalPlants->addSeries(totalPlantsS);
    totalPlants->setTitle("Number of Plants over Time");
    totalPlants->createDefaultAxes();
    totalPlants->axisX()->setTitleText("Time");
    totalPlants->axisY()->setTitleText("Number of Plants");
    ui->TotalPlants->setChart(totalPlants);

    newPlantsPerSteep = new QChart();
    newPlantsPerSteepS = new QLineSeries();
    newPlantsPerSteep->legend()->hide();
    newPlantsPerSteep->addSeries(newPlantsPerSteepS);
    newPlantsPerSteep->setTitle("Number of new Plants per Steep over Time");
    newPlantsPerSteep->createDefaultAxes();
    newPlantsPerSteep->axisX()->setTitleText("Time");
    newPlantsPerSteep->axisY()->setTitleText("Number of new Plants");
    ui->NewPlantsPerSteep->setChart(newPlantsPerSteep);
    for(uint i = 0 ; i<threadCount;i++){
        threadPoolFinished.push_back(false);
        startExec.push_back(false);
    }
    //initThreadPool();
    threadFun = [&](const uint id){
        while(!stopExec){
            while(!startExec[id] && !stopExec){}
            for(const auto entity : enviroment.Entitys){
                entity->accessUpdated.lock();
                if(!entity->updated){
                    entity->updated = true;
                    entity->accessUpdated.unlock();
                    entity->update();
                }
                else{
                    entity->accessUpdated.unlock();
                }
            }
            std::lock_guard<std::mutex> l(accessBoolVecs);
            threadPoolFinished[id] = true;
            startExec[id] = false;
        }
    };


}

SimulationWindow::~SimulationWindow()
{
    delete foxPopulation;
    delete foxSpeed;
    delete foxSenseRadius;
    delete foxEnergyEficiency;

    delete foxPopulationS;
    delete foxSpeedS;
    delete foxSenseRadiusS;
    delete foxEnergyEficiencyS;

    delete rabbitPopulation;
    delete rabbitSpeed;
    delete rabbitSenseRadius;
    delete rabbitEnergyEfficiency;

    delete rabbitPopulationS;
    delete rabbitSpeedS;
    delete rabbitSenseRadiusS;
    delete rabbitEnergyEfficiencyS;

    delete totalPlants;
    delete totalPlantsS;
    delete timer;
    delete ui;
    stopExec = true;
    for(std::thread &t : threadPool){
        t.join();
    }
}

void SimulationWindow::initilizeEnviroment(){
    //Start Gens
    //Amount of Start Fox, Rabbits, Plants
    //Mutate PRobability
    enviroment.Entitys.clear();
    enviroment.minCoordinates = Vector2(0,0);
    enviroment.maxCoordinates = Vector2(ui->MaxX->value(),ui->MaxY->value());
    int plants = ui->NumberPlants->value();
    enviroment.newPlants(plants);
    int foxes = ui->NumberFoxes->value();
    float foxMutate = ui->FoxGensMutateProbability->value();
    float rabbitMutate = ui->RabbitGensMutatePropability->value();
    int rabbits = ui->NumberRabbits->value();
    Gens foxGens;
    foxGens.speed = ui->FoxGensSpeed->value();
    foxGens.senseRadius = ui->FoxGensSenseRadius->value();
    foxGens.energyEfficiency = ui->FoxGensEnergyEfficiency->value();
    Gens rabbitGens;
    rabbitGens.speed = ui->RabbitGensSpeed->value();
    rabbitGens.senseRadius = ui->RabbitGensSenseRadius->value();
    rabbitGens.energyEfficiency = ui->RabbitGensEnergyEfficiency->value();
    for(int i = 0;i<foxes;i++){
        Fox * fox = new Fox(Vector2::randomVec(enviroment.minCoordinates,
                                   enviroment.maxCoordinates),&enviroment,foxMutate);
        fox->gens = foxGens;
        enviroment.Entitys.push_back(std::shared_ptr<Fox>(fox));
    }
    for(int i = 0;i<rabbits;i++){
        Rabbit * rabbit = new Rabbit(Vector2::randomVec(enviroment.minCoordinates,
                                   enviroment.maxCoordinates),&enviroment,rabbitMutate);
        rabbit->gens = rabbitGens;
        enviroment.Entitys.push_back(std::shared_ptr<Rabbit>(rabbit));
    }

}
const std::map<EntityType,QString> typeNames = {
    {EntityType::Rabbit, QString("Rabbit")},
    {EntityType::Fox , QString("Fox")}
};
const std::map<Animal::Gender, QString> genderNames = {
    {Animal::Gender::Male,QString("Male")},
    {Animal::Gender::Female, QString("Female")},
    {Animal::Gender::Whatever, QString("Whatever")}
};
const std::map<Animal::Activity,QString> activityNames = {
    {Animal::Activity::Fleeing, QString("Fleeing")},
    {Animal::Activity::Hunting,QString("Hunting")},
    {Animal::Activity::SearchingForFood,QString("Searching for Food")},
    {Animal::Activity::SearchingForMate,QString("Searching for Mate")}
};
const std::map<Animal::MostNeed,QString> mostNeedNames = {
    {Animal::MostNeed::Foot,QString("Food")},
    {Animal::MostNeed::Water, QString("Water")},
    {Animal::MostNeed::Reproduce, QString("Reproduce")}
};

bool SimulationWindow::allThreadsFinished()const{
    for(const bool b: threadPoolFinished){
        if(!b){
            return false;
        }
    }
    return true;
}



void SimulationWindow::updateEnviroment(){
    static char count = 10;
    if(count == 10){
        count = 0;
        updateStatistics(enviroment.calcEnviromentStatistics(),timeStepCount);
    }
    count++;
    timeStepCount++;
    //if(enviroment.Entitys.size()<1000){
        int spawn = (ui->PlantsDecreaseBase->value()-
                     (timeStepCount/ui->PlantsRateOfDecrease->value()));
        enviroment.newPlants(spawn);
        enviroment.newPlants(ui->PlantsSpawnedPerSteep->value());
    //}
    Enviroment newEnviroment;
    for(const auto entity : enviroment.Entitys){
        entity->setnextEnviroment(&newEnviroment);
    }

    for(auto entity : enviroment.Entitys){
        entity->updated = false;
    }
    newEnviroment = enviroment;
    for(uint i = 0 ; i<threadCount;i++){
        threadPoolFinished[i] = false;
        startExec[i] = true;
    }
    while (!allThreadsFinished()) {}
    enviroment = newEnviroment;

    std::shared_ptr<Animal> cAnimal = ui->simulationView->lastClickedAnimal;
    ui->CAnimalType->setText(typeNames.at(cAnimal->eType));

    ui->CAnimalSpeed->setText(QString("Speed: %1").arg(cAnimal->gens.speed));
    ui->CAnimalSenseRadius->setText(QString("Sense Radius: %1").arg(cAnimal->gens.senseRadius));
    ui->CAnimalEnergyEfficiency->setText(QString("Energy Efficiency: %1").arg(cAnimal->gens.energyEfficiency));
    ui->CAnimalMutatePropability->setText(QString("Mutate Propability: %1").arg(cAnimal->getMutateProbability()));
    ui->CAnimalID->setText(QString("ID: %1").arg(cAnimal->id));
    ui->CAnimalGender->setText(QString("Gender: %1").arg(genderNames.at(cAnimal->sex)));

    ui->CAnimalHunger->setText(QString("Hunger: %1").arg(cAnimal->getHunger()));
    ui->CAnimalPosition->setText(QString("X: %1  Y: %2").arg(cAnimal->pos.x).arg(cAnimal->pos.y));
    ui->CAnimalActivity->setText(QString("Activity: %1").arg(activityNames.at(cAnimal->activity)));
    ui->CAnimalMostNeed->setText(QString("Most Need: %1").arg(mostNeedNames.at(cAnimal->mostNeed)));
    ui->CAnimalUrgeToReproduce->setText(QString("Urge to Reproduce: %1").arg(cAnimal->getUrgeToReproduce()));

    ui->simulationView->update();
    //qDebug() << "Called ones";
}
#define SET_MAX_X(chart) chart->axisX()->setMax(timeStep)
#define SET_MAX_Y(chart,cmax,value) if(value > cmax){cmax = value; chart->axisY()->setMax(value+1);}
void SimulationWindow::updateStatistics(const EnviromentStatistics &stats,const unsigned int timeStep){
    if(!std::isnan(stats.averageFoxGens.speed)){
        averageFoxGens = stats.averageFoxGens;
        foxPopulationS->append(timeStep,stats.typeCounts.at(EntityType::Fox));
        SET_MAX_X(foxPopulation);
        SET_MAX_Y(foxPopulation,foxPopulationMax,stats.typeCounts.at(EntityType::Fox));
        foxSpeedS->append(timeStep,stats.averageFoxGens.speed);
        SET_MAX_X(foxSpeed);
        SET_MAX_Y(foxSpeed,foxSpeedMax,stats.averageFoxGens.speed);
        foxSenseRadiusS->append(timeStep,stats.averageFoxGens.senseRadius);
        SET_MAX_X(foxSenseRadius);
        SET_MAX_Y(foxSenseRadius,foxSenseRadiusMax,stats.averageFoxGens.senseRadius);
        foxEnergyEficiencyS->append(timeStep,stats.averageFoxGens.energyEfficiency);
        SET_MAX_X(foxEnergyEficiency);
        SET_MAX_Y(foxEnergyEficiency,foxEnergyEficiencyMax,stats.averageFoxGens.energyEfficiency);
    }

    if(!std::isnan(stats.averageRabbitGens.speed)){
        averageRabbitGens = stats.averageRabbitGens;
        rabbitPopulationS->append(timeStep,stats.typeCounts.at(EntityType::Rabbit));
        SET_MAX_X(rabbitPopulation);
        SET_MAX_Y(rabbitPopulation,rabbitPopulationMax,stats.typeCounts.at(EntityType::Rabbit));
        rabbitSpeedS->append(timeStep,stats.averageRabbitGens.speed);
        SET_MAX_X(rabbitSpeed);
        SET_MAX_Y(rabbitSpeed,rabbitSpeedMax,stats.averageRabbitGens.speed);
        rabbitSenseRadiusS->append(timeStep,stats.averageRabbitGens.senseRadius);
        SET_MAX_X(rabbitSenseRadius);
        SET_MAX_Y(rabbitSenseRadius,rabbitSenseRadiusMax,stats.averageRabbitGens.senseRadius);
        rabbitEnergyEfficiencyS->append(timeStep,stats.averageRabbitGens.energyEfficiency);
        SET_MAX_X(rabbitEnergyEfficiency);
        SET_MAX_Y(rabbitEnergyEfficiency,rabbitEnergyEfficiencyMax,stats.averageRabbitGens.energyEfficiency);
    }

    totalPlantsS->append(timeStep,stats.typeCounts.at(EntityType::Plant));
    SET_MAX_X(totalPlants);
    SET_MAX_Y(totalPlants,totalPlantsMax,stats.typeCounts.at(EntityType::Plant));
    int plantSpawnBase = ui->PlantsSpawnedPerSteep->value();
    int spawnedPlants = (ui->PlantsDecreaseBase->value()-
                         (timeStep/ui->PlantsRateOfDecrease->value()))+plantSpawnBase;
    if(spawnedPlants < plantSpawnBase){
        spawnedPlants = plantSpawnBase;
    }
    newPlantsPerSteepS->append(timeStep,spawnedPlants);
    SET_MAX_X(newPlantsPerSteep);
    SET_MAX_Y(newPlantsPerSteep,newPlantsPerSteepMax,spawnedPlants);

}



void SimulationWindow::on_addFox_clicked()
{
    std::shared_ptr<Fox> newFox(new Fox(
                                    Vector2::randomVec(enviroment.minCoordinates,enviroment.maxCoordinates),
                                    &enviroment,ui->mutatePropability->value()));
    newFox->gens.speed = ui->Speed->value();
    newFox->gens.senseRadius = ui->SenseRadius->value();
    newFox->gens.energyEfficiency = ui->EnergyEfficeincy->value();
    enviroment.Entitys.push_back(newFox);
}

void SimulationWindow::on_addRabbit_clicked()
{
    std::shared_ptr<Rabbit> newRabbit(new Rabbit(
                                    Vector2::randomVec(enviroment.minCoordinates,enviroment.maxCoordinates),
                                    &enviroment,ui->mutatePropability->value()));
    newRabbit->gens.speed = ui->Speed->value();
    newRabbit->gens.senseRadius = ui->SenseRadius->value();
    newRabbit->gens.energyEfficiency = ui->EnergyEfficeincy->value();
    enviroment.Entitys.push_back(newRabbit);
}

void SimulationWindow::on_addAverageFox_clicked()
{

    std::shared_ptr<Fox> newFox(new Fox(
                                    Vector2::randomVec(enviroment.minCoordinates,enviroment.maxCoordinates),
                                    &enviroment,ui->mutatePropability->value()));
    newFox->gens = averageFoxGens;
    enviroment.Entitys.push_back(newFox);
}

void SimulationWindow::on_addAverageRabbit_clicked()
{
    std::shared_ptr<Rabbit> newRabbit(new Rabbit(
                                    Vector2::randomVec(enviroment.minCoordinates,enviroment.maxCoordinates),
                                    &enviroment,ui->mutatePropability->value()));
    newRabbit->gens = averageRabbitGens;
    enviroment.Entitys.push_back(newRabbit);
}

void SimulationWindow::on_StartButton_clicked()
{
    stopExec = false;
    if(!isPause){
        initilizeEnviroment();
    }

    for(uint i = 0; i <threadCount;i++){
        threadPool.push_back(std::thread(threadFun,i));
    }

    timer->start(ui->SimulationSpeed->value());
}

void SimulationWindow::on_PauseButton_clicked()
{
    isPause = true;
    stopExec = true;
    for(std::thread &t : threadPool){
        t.join();
    }
    threadPool.clear();
    timer->stop();
}

void SimulationWindow::on_ResetButton_clicked()
{
    initilizeEnviroment();

    foxPopulationS->clear();
    foxSpeedS->clear();
    foxSenseRadiusS->clear();
    foxEnergyEficiencyS->clear();

    foxPopulationMax = 0;
    foxSpeedMax = 0;
    foxSenseRadiusMax = 0;
    foxEnergyEficiencyMax = 0;

    rabbitPopulationS->clear();
    rabbitSpeedS->clear();
    rabbitSenseRadiusS->clear();
    rabbitEnergyEfficiencyS->clear();

    rabbitPopulationMax = 0;
    rabbitSpeedMax = 0;
    rabbitSenseRadiusMax = 0;
    rabbitEnergyEfficiencyMax = 0;

    totalPlantsS->clear();
    newPlantsPerSteepS->clear();

    totalPlantsMax = 0;
    newPlantsPerSteepMax = 0;

    timeStepCount = 0;
}


void SimulationWindow::on_SimulationSpeed_valueChanged(int value)
{
    timer->setInterval(value);
}
