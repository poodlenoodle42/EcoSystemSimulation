#include "simulationview.h"
#include <QPainter>
#include "rabbit.h"
#include <QDebug>
#include <QMouseEvent>
SimulationView::SimulationView(QWidget * parent) : QOpenGLWidget(parent)
{
    qDebug() << "SimViewCalled";
    lastClickedAnimal = std::make_shared<Rabbit>(Vector2(-10,-10),nullptr,15);
    lastClickedAnimal->dead = true;
}

void SimulationView::initializeGL(){
    glClearColor(1,1,1,1);
}

const std::map<EntityType,QColor> colorTable = {
    {EntityType::Plant , Qt::green},
    {EntityType::Water ,Qt::blue},
    {EntityType::Rabbit ,QColor(139,69,19)}, // Brown
    {EntityType::Fox ,QColor(255,165,0)}, // Orange
    {EntityType::Entity ,Qt::gray},
    {EntityType::Animal ,Qt::black}

};
void SimulationView::paintGL(){
    QPainter p(this);
    const float XScale = this->width() / enviroment->maxCoordinates.x;
    const float YScale = this->height() / enviroment->maxCoordinates.y;
    const float minScale = XScale < YScale ? XScale : YScale;
    const int squareSize = 2 * minScale;
    const int borderSize = 1.3 * squareSize;
    const int borderdistance = (borderSize - squareSize)/2;
    for(const std::shared_ptr<Entity> entity : enviroment->Entitys){
        p.fillRect(entity->pos.x*XScale-(squareSize/2),entity->pos.y*YScale-(squareSize/2),
                   squareSize,squareSize,colorTable.at(entity->eType));
    }
    p.setPen(Qt::red);
    if(lastClickedAnimal->dead){
        lastClickedAnimal = enviroment->getAnimal(lastClickedAnimal);
    }
    p.drawEllipse(lastClickedAnimal->pos.x*XScale-lastClickedAnimal->gens.senseRadius*XScale
                  ,lastClickedAnimal->pos.y*YScale-lastClickedAnimal->gens.senseRadius*YScale,
               lastClickedAnimal->gens.senseRadius*XScale*2,lastClickedAnimal->gens.senseRadius*YScale*2);
}

void SimulationView::resizeGL(int w, int h){
    glViewport(0,0,w,h);
}

void SimulationView::updateEnviroment(const Enviroment *env){
    enviroment = env;
}

void SimulationView::mousePressEvent(QMouseEvent * event){
    Vector2 min(event->x(),event->y());
    const float XScale = this->width() / enviroment->maxCoordinates.x;
    const float YScale = this->height() / enviroment->maxCoordinates.y;
    const float minScale = XScale < YScale ? XScale : YScale;
    const int squareSize = 2 * minScale;
    min.x /= XScale;
    min.y /= YScale;
    Vector2 max(min.x+squareSize/2,min.y+squareSize/2);
    min.x -= squareSize/2;
    min.y -= squareSize/2;
    for(const auto entity : enviroment->Entitys){
        if(entity->pos.x > min.x && entity->pos.y > min.y &&entity->pos.x < max.x && entity->pos.y < max.y &&
                (entity->eType == EntityType::Fox ||
                 entity->eType == EntityType::Rabbit)){
                    lastClickedAnimal = std::dynamic_pointer_cast<Animal>(entity);
                    update();
        }
    }


}
