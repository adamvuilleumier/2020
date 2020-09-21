#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

class Actor : public GraphObject
{
public:
    Actor(int IID, double x, double y, int dir, int depth, double size, StudentWorld* world);
    virtual ~Actor();
    virtual void doSomething() = 0;  //is called when actors turn to move each tick
    virtual void activate(Actor* affected) = 0;  //is called when overlap occurs
    virtual void affectHp(int amt) = 0;
    void takeDamage(int amt);
    StudentWorld* getWorld() const; //returns pointer to the StudentWorld an actor is in
    bool getAlive() const;
    bool getSolid() const;
    bool getEdible() const;
    void setAlive(bool value);
protected:
    bool getHasHp() const;
    bool getDamageable() const;
    void setSolid(bool value);
    void setDamageable(bool value);
    void setHasHp(bool value);
    void setEdible(bool value);
private:
    int m_depth;
    int m_dir;
    double m_size;
    bool m_alive;
    bool m_edible;
    bool m_solid;
    bool m_damageable;
    bool m_hasHp;
    StudentWorld* m_world;
};

////////Socrates//////////////////////////////////////////////////////////////

class Socrates : public Actor
{
public:
    Socrates(StudentWorld* world);
    virtual void doSomething();
    virtual void activate(Actor* affected);
    virtual void affectHp(int amt);
    int getSpray() const;
    int getFlame() const;
    int getHp() const;
    void setFlame(int amt);
    void setHp(int amt);
private:
    int getPositionalAngle() const;
    void setPositionalAngle(int angle);
    void setSpray(int amt);
    int normalizeAngle(int angle); //makes sure angle is between 0 and 359
    void angleToCoord(double& x, double& y, int angle);  //converts positionalAngle to coordinate
    int m_positionalAngle;
    int m_spray;
    int m_flame;
    int m_hp;
};

/////////////Dirt///////////////////////////////////////////////////////

class Dirt : public Actor
{
public:
    Dirt(double x, double y, StudentWorld* world);
    virtual void doSomething();
    virtual void activate(Actor* affected);
    virtual void affectHp(int amt) { return; };
};

////////////Pit////////////////////////////////////////////////////////////////

class Pit : public Actor
{
public:
    Pit(double x, double y, StudentWorld* world);
    virtual void doSomething();
    virtual void activate(Actor* affected);
    virtual void affectHp(int amt) { return; };
private:
    void reduceAggressive();
    void reduceRegular();
    void reduceEColi();
    int getAggressive();
    int getRegular();
    int getEColi();
    bool empty();
    int m_nAggressive;
    int m_nRegular;
    int m_nEColi;
};

/////////Food//////////////////////////////////////////////////////////////////

class Food : public Actor
{
public:
    Food(double x, double y, StudentWorld* world);
    virtual void doSomething();
    virtual void activate(Actor* affected);
    virtual void affectHp(int amt) { return; };
};

//////Bacteria/////////////////////////////////////////////////////////////////

class Bacteria : public Actor
{
public:
    Bacteria(int IID, double x, double y, StudentWorld* world);
    virtual ~Bacteria();
    virtual void doSomething();
    virtual void activate(Actor* affected);
    virtual void affectHp(int amt);
protected:
    virtual void deathSound() = 0;
    virtual void hurtSound() = 0;
    virtual void specificBehavior(bool& alreadyMoved) = 0;
    virtual void specificDivide(double x, double y) = 0;
    virtual void damageSocrates() = 0;
    void divide();
    void setHp(int hp);
    int getHp();
    void resetFoodEaten();
    void incrementFoodEaten();
    int getFoodEaten();
private:
    void setMovementPlanDist(int dist);
    void decMovementPlanDist();
    void setMovementPlanAngle(int angle);
    double getMovementPlanDist();
    int getMovementPlanAngle();
    int m_hp;
    double m_movementPlanDistance;
    int m_movementPlanAngle;
    int m_foodEaten;
};

/////////Regular//////////////////////////////////////////////////////////////

class Regular : public Bacteria
{
public:
    Regular(double x, double y, StudentWorld* world);
private:
    virtual void specificBehavior(bool& alreadyMoved);
    virtual void specificDivide(double x, double y);
    virtual void damageSocrates();
    virtual void deathSound();
    virtual void hurtSound();
};

/////////Aggressive//////////////////////////////////////////////////////////////

class Aggressive : public Bacteria
{
public:
    Aggressive(double x, double y, StudentWorld* world);
private:
    virtual void specificBehavior(bool& alreadyMoved);
    virtual void specificDivide(double x, double y);
    virtual void damageSocrates();
    virtual void deathSound();
    virtual void hurtSound();
};

/////////E Coli//////////////////////////////////////////////////////////////

class EColi : public Bacteria
{
public:
    EColi(double x, double y, StudentWorld* world);
private:
    virtual void specificDivide(double x, double y);
    virtual void specificBehavior(bool& alreadyMoved);
    virtual void damageSocrates();
    virtual void deathSound();
    virtual void hurtSound();
};

////////Goodie///////////////////////////////////////////////////////////////

class Goodie : public Actor
{
public:
    Goodie(int IID, double x, double y, StudentWorld* world);
    virtual ~Goodie();
    virtual void doSomething();
    virtual void activate(Actor* affected) = 0;
    virtual void affectHp(int amt) { return; };
private:
    int lifetime;
};

///////Extra Life////////////////////////////////////////////////////////////

class ExtraLife : public Goodie
{
public:
    ExtraLife(double x, double y, StudentWorld* world);
    virtual void activate(Actor* affected);
};

//////Restore Health///////////////////////////////////////////////////////////

class RestoreHealth : public Goodie
{
public:
    RestoreHealth(double x, double y, StudentWorld* world);
    virtual void activate(Actor* affected);
};

//////Restore Flame///////////////////////////////////////////////////////////

class RestoreFlame : public Goodie
{
public:
    RestoreFlame(double x, double y, StudentWorld* world);
    virtual void activate(Actor* affected);
};

/////////Fungus///////////////////////////////////////////////////////////////

class Fungus : public Goodie
{
public:
    Fungus(double x, double y, StudentWorld* world);
    virtual void doSomething();
    virtual void activate(Actor* affected);
};

////////Weapon///////////////////////////////////////////////////////////////

class Weapon : public Actor
{
public:
    Weapon(int IID, double x, double y, int dir, StudentWorld* world);
    virtual ~Weapon();
    virtual void doSomething();
    virtual void activate(Actor* affected) = 0;
    virtual void affectHp(int amt) { return; };
protected:
    virtual void checkDead() = 0;
    void setTravelDist(double amt);
    double getTravelDist() const;
    void incTravelDist(double amt);
private:
    double travelDist;
};

/////Flame///////////////////////////////////////////////////////////////////

class Flame : public Weapon
{
public:
    Flame(double x, double y, int dir, StudentWorld* world);
    virtual void checkDead();
    virtual void activate(Actor* affected);
};

////Spray/////////////////////////////////////////////////////////////////////

class Spray : public Weapon
{
public:
    Spray(double x, double y, int dir, StudentWorld* world);
    virtual void checkDead();
    virtual void activate(Actor* affected);
};

#endif // ACTOR_H_
