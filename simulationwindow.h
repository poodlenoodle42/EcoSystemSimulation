#ifndef SIMULATIONWINDOW_H
#define SIMULATIONWINDOW_H

#include <QMainWindow>
#include "animal.h" //Includes Vector2
#include <QtCharts>
#include <thread>
namespace Ui {
class SimulationWindow;
}

class SimulationWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit SimulationWindow(QWidget *parent = 0);
    ~SimulationWindow();
public slots:
    void updateEnviroment();
private slots:
    void on_addFox_clicked();

    void on_addRabbit_clicked();

    void on_addAverageFox_clicked();

    void on_addAverageRabbit_clicked();

    void on_StartButton_clicked();

    void on_PauseButton_clicked();

    void on_ResetButton_clicked();



    void on_SimulationSpeed_valueChanged(int value);

private:
    Ui::SimulationWindow *ui;
    Enviroment enviroment;
    void initilizeEnviroment();
    QTimer * timer;
    //Charts
    QChart * foxPopulation;
    QChart * foxSpeed;
    QChart * foxSenseRadius;
    QChart * foxEnergyEficiency;

    QLineSeries * foxPopulationS;
    QLineSeries * foxSpeedS;
    QLineSeries * foxSenseRadiusS;
    QLineSeries * foxEnergyEficiencyS;

    unsigned int foxPopulationMax = 0;
    unsigned int foxSpeedMax = 0;
    unsigned int foxSenseRadiusMax = 0;
    unsigned int foxEnergyEficiencyMax = 0;

    QChart * rabbitPopulation;
    QChart * rabbitSpeed;
    QChart * rabbitSenseRadius;
    QChart * rabbitEnergyEfficiency;

    QLineSeries * rabbitPopulationS;
    QLineSeries * rabbitSpeedS;
    QLineSeries * rabbitSenseRadiusS;
    QLineSeries * rabbitEnergyEfficiencyS;

    unsigned int rabbitPopulationMax = 0;
    unsigned int rabbitSpeedMax = 0;
    unsigned int rabbitSenseRadiusMax = 0;
    unsigned int rabbitEnergyEfficiencyMax = 0;

    QChart * totalPlants;
    QChart * newPlantsPerSteep;

    QLineSeries * totalPlantsS;
    QLineSeries * newPlantsPerSteepS;

    unsigned int totalPlantsMax = 0;
    unsigned int newPlantsPerSteepMax = 0;

    bool isPause = true;
    void updateStatistics(const EnviromentStatistics &stats,const unsigned int timeStep);

    Gens averageFoxGens;
    Gens averageRabbitGens;

    unsigned int timeStepCount = 0;

    const uint threadCount = std::thread::hardware_concurrency();

    std::vector<std::thread> threadPool;
    std::mutex accessBoolVecs;
    std::vector<bool> threadPoolFinished;
    std::vector<bool> startExec;
    void initThreadPool();

    bool allThreadsFinished()const;


};

#endif // SIMULATIONWINDOW_H
