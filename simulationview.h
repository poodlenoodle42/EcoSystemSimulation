#ifndef SIMULATIONVIEW_H
#define SIMULATIONVIEW_H

#include <QOpenGLWidget>
#include <QColor>
#include <memory>
struct Enviroment;
class Animal;
class SimulationView : public QOpenGLWidget
{
//Q_OBJECT
public:
    SimulationView(QWidget * parent = 0);
    void updateEnviroment(const Enviroment * env);
    std::shared_ptr<Animal> lastClickedAnimal;
protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void mousePressEvent(QMouseEvent * event) override;
private:
    const Enviroment * enviroment;


};

#endif // SIMULATIONVIEW_H
