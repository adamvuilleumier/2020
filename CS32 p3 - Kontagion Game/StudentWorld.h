#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>

class Actor;
class Socrates;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    Socrates* getPlayer();
    void addActor(Actor* actor);
    void increaseBacteria();
    void reduceBacteria();
    bool openSpace(double x, double y);
    int findDirToFood(double x, double y);
    int angleBetweenPoints(double x1, double y1, double x2, double y2);
    Actor* overlap(Actor* actor);
    double distance(double x1, double y1, double x2, double y2) const;

private:
    void randomCoord(double& x, double& y);
    std::list<Actor*> Actors;
    Socrates* player;
    int nBacteriaLeft;
    bool levelOver;
};

#endif // STUDENTWORLD_H_
