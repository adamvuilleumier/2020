#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <iostream>
#include <sstream>
#include <iomanip>  
#include <string>
#include <list>
#include <cmath>
#include <algorithm>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
    : GameWorld(assetPath)
{
    player = nullptr;

    int nPits = getLevel();
    nBacteriaLeft = nPits * 10; //10 bacteria per pit
    levelOver = false;
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    player = new Socrates(this); //initializes a new Socrates

    int nDirt;
    if (180 - (20 * getLevel()) < 20) //populates map with dirt
        nDirt = 20;
    else
        nDirt = 180 - (20 * getLevel());

    for (int i = 0; i < nDirt; i++)
    {
        double x = 0;
        double y = 0;
        randomCoord(x, y);
        Actor* p = new Dirt(x, y, this);
        addActor(p);
    }

    int nPits = getLevel(); //populates map with pits
    for (int j = 0; j < nPits; j++)
    {
        double x = 0;
        double y = 0;
        randomCoord(x, y);
        Actor* p = new Pit(x, y, this);

        if (overlap(p) != nullptr) //if accidentally creates overlapping pit, will delete and retry
        {
            delete p;
            j--;
        }
        else
            addActor(p);
    }

    int nFood = 5 * getLevel();
    if (nFood > 25)
        nFood = 25;

    for (int k = 0; k < nFood; k++)
    {
        double x = 0;
        double y = 0;
        randomCoord(x, y);
        Actor* p = new Food(x, y, this);

        if (overlap(p) != nullptr) //if accidentally creates overlapping pit, will delete and retry
        {
            delete p;
            k--;
        }
        else
            addActor(p);
    }

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    player->doSomething();

    for (list<Actor*>::iterator it = Actors.begin(); //give all actors a chance to move
        it != Actors.end(); it++)
    {
        if ((*it)->getAlive())
            (*it)->doSomething();

        if (!player->getAlive())
            return GWSTATUS_PLAYER_DIED;

        if (nBacteriaLeft == 0)
        {
            playSound(SOUND_FINISHED_LEVEL);
            return GWSTATUS_FINISHED_LEVEL;
        }
    }

    for (list<Actor*>::iterator it2 = Actors.begin(); //call activate if there's overlap
        it2 != Actors.end(); it2++)
    {
        if (overlap(*it2) != nullptr && overlap(*it2)->getAlive())
            (*it2)->activate(overlap(*it2));
    }

    list<Actor*>::iterator it3 = Actors.begin();
    while (it3 != Actors.end())
    {
        if (!(*it3)->getAlive()) //if dead, delete from list and call destructor
        {
            list<Actor*>::iterator killMe = it3;
            it3++;
            delete (*killMe);
            Actors.erase(killMe);
        }
        else
            it3++;   //just increment iterator if actor is still alive
    }

    int chanceFungus = 510 - 10 * getLevel(); //generates fungus randomly
    if (chanceFungus < 200)
        chanceFungus = 200;

    int random = randInt(0, chanceFungus);
    if (random == 0)
    {
        double x;
        double y;
        randomCoord(x, y);

        while (distance(x, y, VIEW_WIDTH / 2, VIEW_HEIGHT / 2) != VIEW_RADIUS)
        {
            randomCoord(x, y);
        }

        Actor* f = new Fungus(x, y, this);
        addActor(f);
    }

    int chanceGoodie = 510 - 10 * getLevel(); //generates goodies of various tyhpes randomly
    if (chanceGoodie < 250)
        chanceGoodie = 250;

    int random2 = randInt(0, chanceGoodie);
    if (random2 == 0)
    {
        double x;
        double y;
        randomCoord(x, y);

        while (distance(x, y, VIEW_WIDTH / 2, VIEW_HEIGHT / 2) != VIEW_RADIUS)
        {
            randomCoord(x, y);
        }

        int random3 = randInt(0, 100);
        Actor* g;
        if (random3 < 60)
            g = new RestoreHealth(x, y, this);
        if (random3 >= 60 && random3 < 90)
            g = new RestoreFlame(x, y, this);
        if (random3 >= 90)
            g = new ExtraLife(x, y, this);
    }

    ostringstream oss;
    oss.fill('0');
    oss << "Score: " << setw(6) << getScore() << "  ";
    oss << "Level: " << getLevel() << "  ";
    oss << "Lives: " << getLives() << "  ";
    oss << "health: " << player->getHp() << "  ";
    oss << "Sprays: " << player->getSpray() << "  ";
    oss << "Flames: " << player->getFlame() << "  ";
    string s = oss.str();
    setGameStatText(s);

        return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    list<Actor*>::iterator it = Actors.begin();
    while (it != Actors.end())
    {
        list<Actor*>::iterator killMe = it;
        it++;
        delete (*killMe);
        killMe = Actors.erase(killMe);
    }
    delete player;
}

Socrates* StudentWorld::getPlayer()
{
    return player;
}

void StudentWorld::addActor(Actor* actor) //adds the new actor to end of list
{
    if (actor != nullptr)
        Actors.push_back(actor);
}

void StudentWorld::increaseBacteria()
{
    nBacteriaLeft++;
}

void StudentWorld::reduceBacteria()
{
    nBacteriaLeft--;
}

bool StudentWorld::openSpace(double x, double y)
{
    for (list<Actor*>::iterator it = Actors.begin();
        it != Actors.end(); it++)
    {
        if (distance(x, y, (*it)->getX(), (*it)->getY()) <= SPRITE_WIDTH && (*it)->getSolid())
            return false;
    }

    if (distance(x, y, VIEW_WIDTH / 2, VIEW_HEIGHT / 2) >= VIEW_RADIUS)
        return false;

    return true;
}

int StudentWorld::findDirToFood(double x, double y)
{
    double smallestDist = 128;
    int bestDir;
    for (list<Actor*>::iterator it = Actors.begin();
        it != Actors.end(); it++)
    {
        if ((*it)->getEdible())
        {
            if (distance(x, y, (*it)->getX(), (*it)->getY()) < smallestDist)
            {
                smallestDist = distance(x, y, (*it)->getX(), (*it)->getY());
                bestDir = angleBetweenPoints(x, y, (*it)->getX(), (*it)->getY());
            }
        }
    }

    if (smallestDist >= 128)  //if did not find a food close enough, return random
        return randInt(0, 359);
    return bestDir;
}

int StudentWorld::angleBetweenPoints(double x1, double y1, double x2, double y2)
{
    double changeX = x2 - x1; //use trig to find angle
    double changeY = y2 - y1;

    return atan2(changeY, changeX) * 180 / 3.1416;
}

Actor* StudentWorld::overlap(Actor* actor) //returns pointer to first overlapping object or nullptr if none
{
    double myX = actor->getX();
    double myY = actor->getY();

    if (distance(myX, myY, getPlayer()->getX(), getPlayer()->getY()) <= SPRITE_WIDTH) //checks for overlap with player first since that's most important
        return player;

    for (list<Actor*>::iterator it = Actors.begin();
        it != Actors.end(); it++)
    {
        if (distance(myX, myY, (*it)->getX(), (*it)->getY()) <= SPRITE_WIDTH)
            return *it;
    }
    return nullptr;
}

void StudentWorld::randomCoord(double& x, double& y)
{
    x = randInt(0, VIEW_WIDTH + 1);
    y = randInt(0, VIEW_HEIGHT + 1);

    while (distance(VIEW_WIDTH / 2, VIEW_HEIGHT / 2, x, y) > 120)
    {
        x = randInt(0, VIEW_WIDTH + 1);
        y = randInt(0, VIEW_HEIGHT + 1);
    }
}

double StudentWorld::distance(double x1, double y1, double x2, double y2) const
{
    double d = (x2 - x1) * (x2 - x1); //change in x squared
    d += (y2 - y1) * (y2 - y1);       //plus change in y squared
    return sqrt(d);                   //distance is square root of quantity
}

