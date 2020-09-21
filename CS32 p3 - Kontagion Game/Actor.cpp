#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>

///////////////////Actor//////////////////////////////////////////////

Actor::Actor(int IID, double x, double y, int dir, int depth, double size, StudentWorld* world)
    : GraphObject(IID, x, y, dir, depth, size)
{
    m_depth = depth;
    m_dir = dir;
    m_size = size;
    m_alive = true;
    m_solid = true;
    m_edible = false;
    m_damageable = true;
    m_hasHp = false;
    m_world = world;
}

Actor::~Actor()
{

}

void Actor::takeDamage(int amt)
{
    if (!getDamageable())
        return;

    if (getHasHp())
        affectHp(amt);

    if (!getHasHp())
        setAlive(false);
}

StudentWorld* Actor::getWorld() const
{
    return m_world;
}

bool Actor::getAlive() const
{
    return m_alive;
}

bool Actor::getSolid() const
{
    return m_solid;
}

bool Actor::getDamageable() const
{
    return m_damageable;
}

bool Actor::getHasHp() const
{
    return m_hasHp;
}

bool Actor::getEdible() const
{
    return m_edible;
}

void Actor::setAlive(bool value)
{
    m_alive = value;
}

void Actor::setSolid(bool value)
{
    m_solid = value;
}

void Actor::setDamageable(bool value)
{
    m_damageable = value;
}

void Actor::setHasHp(bool value)
{
    m_hasHp = value;
}

void Actor::setEdible(bool value)
{
    m_edible = value;
}

///////////////Socrates///////////////////////////////////////////////

Socrates::Socrates(StudentWorld* world) : Actor(IID_PLAYER, 0, 128, 0, 0, 1.0, world)
{
    m_positionalAngle = 180;
    m_spray = 20;
    m_flame = 5;
    m_hp = 100;
    setSolid(false);
    setHasHp(true);
}

void Socrates::doSomething()
{
    if (!getAlive())
        return;

    int key;

    if (getWorld()->getKey(key))
    {
        if (key == KEY_PRESS_LEFT)
        {
            int angle = normalizeAngle(getPositionalAngle() + 5); //sets angle 5 degrees counterclockwise
            setPositionalAngle(angle);

            double changedX;  //adjusts (x,y) based on new angle
            double changedY;
            angleToCoord(changedX, changedY, getPositionalAngle());
            moveTo(changedX, changedY);

            setDirection(getPositionalAngle() + 180);  //sets new animation direction
        }

        else if (key == KEY_PRESS_RIGHT)
        {
            int angle = normalizeAngle(getPositionalAngle() - 5); //sets angle 5 degrees clockwise
            setPositionalAngle(angle);

            double changedX;
            double changedY;
            angleToCoord(changedX, changedY, getPositionalAngle());
            moveTo(changedX, changedY);

            setDirection(getPositionalAngle() + 180);
        }

        else if (key == KEY_PRESS_SPACE)
        {
            if (getSpray() > 0)
            {
                double newX; //creates new spray object and adds to ActorList
                double newY;
                getPositionInThisDirection(getDirection(), SPRITE_WIDTH, newX, newY);
                Actor* p = new Spray(newX, newY, getDirection(), getWorld());
                getWorld()->addActor(p);

                setSpray(getSpray() - 1); //decrement spray count
                getWorld()->playSound(SOUND_PLAYER_SPRAY);
            }
        }
        else if (key == KEY_PRESS_ENTER)
        {
            if (getFlame() > 0)
            {
                int dir = getDirection();
                for (int i = 0; i < 16; i++)
                {
                    double newX; //creates new flame object and adds to ActorList
                    double newY;
                    getPositionInThisDirection(dir, SPRITE_WIDTH, newX, newY);
                    Actor* p = new Flame(newX, newY, dir, getWorld());
                    getWorld()->addActor(p);
                    dir += 22;
                }

                setFlame(getFlame() - 1); //decrement flame count
                getWorld()->playSound(SOUND_PLAYER_FIRE);
            }
        }
    }
    else
    {
        if (getSpray() < 20)  //if user does nothing, spray replenishes
            setSpray(getSpray() + 1);
    }
}

void Socrates::activate(Actor* affected)
{

}

void Socrates::affectHp(int amt)
{
    setHp(getHp() - amt);

    if (getHp() <= 0)
    {
        setAlive(false);
        getWorld()->playSound(SOUND_PLAYER_DIE);
        return;
    }

    getWorld()->playSound(SOUND_PLAYER_HURT);
}

int Socrates::getPositionalAngle() const
{
    return m_positionalAngle;
}

int Socrates::getSpray() const
{
    return m_spray;
}

int Socrates::getFlame() const
{
    return m_flame;
}

int Socrates::getHp() const
{
    return m_hp;
}

void Socrates::setPositionalAngle(int angle)
{
    m_positionalAngle = angle;
}

void Socrates::setSpray(int amt)
{
    m_spray = amt;
}

void Socrates::setFlame(int amt)
{
    m_flame = amt;
}

void Socrates::setHp(int amt)
{
    m_hp = amt;
}

int Socrates::normalizeAngle(int angle)
{
    return angle % 360;
}

void Socrates::angleToCoord(double& x, double& y, int angle)
{
    double radAngle = angle * (3.141593 / 180);

    double coordx = VIEW_RADIUS * cos(radAngle); //gives x distance from center of circle
    coordx += VIEW_RADIUS;

    double coordy = VIEW_RADIUS * sin(radAngle);
    coordy += VIEW_RADIUS;

    x = coordx;
    y = coordy;
}

//////////Dirt/////////////////////////////////////////////////////////

Dirt::Dirt(double x, double y, StudentWorld* world) : Actor(IID_DIRT, x, y, 0, 1, 1.0, world)
{

}

void Dirt::doSomething()
{

}

void Dirt::activate(Actor* affected)
{

}

///////////Pit//////////////////////////////////////////////////////////////

Pit::Pit(double x, double y, StudentWorld* world) : Actor(IID_PIT, x, y, 0, 1, 1.0, world)
{
    m_nRegular = 5;
    m_nAggressive = 3;
    m_nEColi = 2;
    setDamageable(false);
    setSolid(false);
}

void Pit::doSomething()
{
    if (empty())
        setAlive(false);

    int spawnDecider = randInt(1, 50);
    if (spawnDecider == 1)
    {
        while (true) //if loop repeats, it means one or more bacteria ran out
        {
            int typeDecider = randInt(1, 3);
            if (typeDecider == 1 && getRegular() > 0)
            {
                Actor* p = new Regular(getX(), getY(), getWorld());
                getWorld()->addActor(p);
                reduceRegular();
                break;
            }

            else if (typeDecider == 2 && getAggressive() > 0)
            {
                Actor* p = new Aggressive(getX(), getY(), getWorld());
                getWorld()->addActor(p);
                reduceAggressive();
                break;
            }

            else if (typeDecider == 3 && getEColi() > 0)
            {
                Actor* p = new EColi(getX(), getY(), getWorld());
                getWorld()->addActor(p);
                reduceEColi();
                break;
            }
        }
        getWorld()->playSound(SOUND_BACTERIUM_BORN);
    }
}

void Pit::activate(Actor* affected)
{

}

void Pit::reduceAggressive()
{
    m_nAggressive--;
}

void Pit::reduceRegular()
{
    m_nRegular--;
}

void Pit::reduceEColi()
{
    m_nEColi--;
}

int Pit::getAggressive()
{
    return m_nAggressive;
}

int Pit::getRegular()
{
    return m_nRegular;
}

int Pit::getEColi()
{
    return m_nEColi;
}

bool Pit::empty()
{
    if (getEColi() == 0 && getAggressive() == 0 && getRegular() == 0)
        return true;
    return false;
}

//////////Food////////////////////////////////////////////////////////////////

Food::Food(double x, double y, StudentWorld* world) : Actor(IID_FOOD, x, y, 90, 1, 1.0, world)
{
    setSolid(false);
    setDamageable(false);
    setEdible(true);
}

void Food::doSomething()
{

}
void Food::activate(Actor* affected)
{

}

//////////Bacteria/////////////////////////////////////////////////////////////

Bacteria::Bacteria(int IID, double x, double y, StudentWorld* world) : Actor(IID, x, y, 90, 0, 1.0, world)
{
    m_hp = 0;
    m_movementPlanDistance = 0;
    m_movementPlanAngle = 0;
    m_foodEaten = 0;
    setSolid(false);
    setHasHp(true);
}

Bacteria::~Bacteria()
{

}

void Bacteria::doSomething()
{
     if (!getAlive())
        return;

    bool alreadyMoved = false;

    specificBehavior(alreadyMoved);

    if (alreadyMoved) //the aggressive bacteria doesn't move again if it alread did in specificBehavior()
        return;

    if (getMovementPlanDist() > 0)
    {
        decMovementPlanDist();

        double possibleX;
        double possibleY;
        getPositionInThisDirection(getDirection(), 3, possibleX, possibleY);

        if (getWorld()->openSpace(possibleX, possibleY)) //checks if there's dirt here or if outside the dish
            moveTo(possibleX, possibleY);
        else
        {
            setMovementPlanAngle(randInt(0, 359)); //set random angle if cannot move with previous plan
            setMovementPlanDist(10);
        }
        return;
    }

    else //if movement plan is 0, set movement plan
    {
        int plannedDir = getWorld()->findDirToFood(getX(), getY()); //returns direction to closest food/ random if not close enough
        setMovementPlanAngle(plannedDir);
        setMovementPlanDist(10);
        return;
    }
}

void Bacteria::activate(Actor* affected)
{
    if (affected->getEdible())
    {
        affected->setAlive(false);
        incrementFoodEaten();
    }

    if (affected == getWorld()->getPlayer())
        damageSocrates();
}

void Bacteria::affectHp(int amt)
{
    setHp(getHp() - amt);

    if (getHp() <= 0)
    {
        setAlive(false);
        deathSound(); //plays sepcific death sound depending on type
        getWorld()->increaseScore(100);

        int random = randInt(1, 2);
        if (random == 1) //50% chance a food object is birthed/added to ActorList
        {
            Actor* p = new Food(getX(), getY(), getWorld());
            getWorld()->addActor(p);
        }
    }
    hurtSound();
}

void Bacteria::divide()
{
    double newX = getX();
    double newY = getY();

    if (getX() < VIEW_WIDTH / 2)  //sets new x and y coordinate of spawned bacteria
        newX += SPRITE_WIDTH / 2;
    if (getX() > VIEW_WIDTH / 2)
        newX -= SPRITE_WIDTH / 2;

    if (getY() < VIEW_HEIGHT / 2)
        newY += SPRITE_WIDTH / 2;
    if (getY() > VIEW_HEIGHT / 2)
        newY -= SPRITE_WIDTH / 2;

    resetFoodEaten();   //resets food eaten in parent bacteria back to 0

    specificDivide(newX, newY);

    getWorld()->increaseBacteria();
}

void Bacteria::setHp(int hp)
{
    m_hp = hp;
}

void Bacteria::setMovementPlanDist(int dist)
{
    m_movementPlanDistance = dist;
}

void Bacteria::decMovementPlanDist()
{
    m_movementPlanDistance--;
}

void Bacteria::setMovementPlanAngle(int angle)
{
    m_movementPlanAngle = angle;
}

void Bacteria::resetFoodEaten()
{
    m_foodEaten = 0;
}

void Bacteria::incrementFoodEaten()
{
    m_foodEaten++;
}

int Bacteria::getHp()
{
    return m_hp;
}

double Bacteria::getMovementPlanDist()
{
    return m_movementPlanDistance;
}

int Bacteria::getMovementPlanAngle()
{
    return m_movementPlanAngle;
}

int Bacteria::getFoodEaten()
{
    return m_foodEaten;
}

/////////Regular Salmonella///////////////////////////////////////////////////

Regular::Regular(double x, double y, StudentWorld* world) : Bacteria(IID_SALMONELLA, x, y, world)
{
    setHp(4);
}

void Regular::hurtSound()
{
    getWorld()->playSound(SOUND_SALMONELLA_HURT);
}

void Regular::deathSound()
{
    getWorld()->playSound(SOUND_SALMONELLA_DIE);
}

void Regular::specificDivide(double x, double y)
{
    Actor* p = new Regular(x, y, getWorld()); //creates bacteria and adds to world's list
    getWorld()->addActor(p);
}

void Regular::specificBehavior(bool& alreadyMoved)
{
    if (getWorld()->overlap(this) != getWorld()->getPlayer()) //if not overlapping with Socrates
        if (getFoodEaten() >= 3)
            divide();
}

void Regular::damageSocrates()
{
    getWorld()->getPlayer()->affectHp(1);
}

/////////Aggressive Salmonella///////////////////////////////////////////////////

Aggressive::Aggressive(double x, double y, StudentWorld* world) : Bacteria(IID_SALMONELLA, x, y, world)
{
    setHp(10);
}

void Aggressive::hurtSound()
{
    getWorld()->playSound(SOUND_SALMONELLA_HURT);
}

void Aggressive::deathSound()
{
    getWorld()->playSound(SOUND_SALMONELLA_DIE);
}

void Aggressive::specificBehavior(bool& alreadyMoved)
{
    double playerX = getWorld()->getPlayer()->getX();
    double playerY = getWorld()->getPlayer()->getY();

    if (getWorld()->distance(getX(), getY(), playerX, playerY) <= 72) //if player is within range, move towards him if possible
    {
        int dirToPlayer = getWorld()->angleBetweenPoints(getX(), getY(), playerX, playerY);
        double possibleX;
        double possibleY;
        getPositionInThisDirection(dirToPlayer, 3, possibleX, possibleY);

        if (getWorld()->openSpace(possibleX, possibleY))
        {
            moveTo(possibleX, possibleY);
            alreadyMoved = true;
        }
    }

    if (getWorld()->overlap(this) != getWorld()->getPlayer()) //if not overlapping with Socrates
        if (getFoodEaten() >= 3)
            divide();
}

void Aggressive::specificDivide(double x, double y)
{
    Actor* p = new Aggressive(x, y, getWorld()); //creates bacteria and adds to world's list
    getWorld()->addActor(p);
}

void Aggressive::damageSocrates()
{
    getWorld()->getPlayer()->affectHp(2);
}

/////////E Coli///////////////////////////////////////////////////

EColi::EColi(double x, double y, StudentWorld* world) : Bacteria(IID_ECOLI, x, y, world)
{
    setHp(5);
}

void EColi::hurtSound()
{
    getWorld()->playSound(SOUND_ECOLI_HURT);
}

void EColi::deathSound()
{
    getWorld()->playSound(SOUND_ECOLI_DIE);
}

void EColi::specificDivide(double x, double y)
{
    Actor* p = new EColi(x, y, getWorld()); //creates bacteria and adds to world's list
    getWorld()->addActor(p);
}

void EColi::specificBehavior(bool& alreadyMoved)
{
    double playerX = getWorld()->getPlayer()->getX();
    double playerY = getWorld()->getPlayer()->getY();

    alreadyMoved = false; //we set to false because EColi will never set a new movement plan

    if (getWorld()->overlap(this) != getWorld()->getPlayer()) //if not overlapping with Socrates
        if (getFoodEaten() >= 3)
            divide();

    if (getWorld()->distance(getX(), getY(), playerX, playerY) <= 256)
    {
        int theta = getWorld()->angleBetweenPoints(getX(), getY(), playerX, playerY);
        double possibleX;
        double possibleY;

        for (int i = 0; i < 10; i++)
        {
            theta = theta % 360;  //makes sure theta isn't over 360

            getPositionInThisDirection(theta, 3, possibleX, possibleY);

            if (getWorld()->openSpace(possibleX, possibleY))
            {
                moveTo(possibleX, possibleY);
                return;
            }
            else
                theta += 10;
        }
    }
}

void EColi::damageSocrates()
{
    getWorld()->getPlayer()->affectHp(4);
}

/////////Goodie///////////////////////////////////////////////////////////////

Goodie::Goodie(int IID, double x, double y, StudentWorld* world) : Actor(IID, x, y, 0, 1, 1.0, world)
{
    setSolid(false);

    int level = getWorld()->getLevel();
    int lifeChance = 300 - 10 * (level - 1);

    int random = randInt(0, lifeChance);
    if (random > 50)
        lifetime = random;
    else
        lifetime = 50;
}

Goodie::~Goodie()
{

}

void Goodie::doSomething()
{
    if (!getAlive())
        return;

    if (lifetime <= 0)
        setAlive(false);

    lifetime--;
}

/////Extra Life///////////////////////////////////////////////////////////////

ExtraLife::ExtraLife(double x, double y, StudentWorld* world) : Goodie(IID_EXTRA_LIFE_GOODIE, x, y, world)
{

}

void ExtraLife::activate(Actor* affected)
{
    getWorld()->incLives();
    getWorld()->increaseScore(500);
    getWorld()->playSound(SOUND_GOT_GOODIE);
    setAlive(false);
}

/////Restore Health///////////////////////////////////////////////////////////

RestoreHealth::RestoreHealth(double x, double y, StudentWorld* world) : Goodie(IID_RESTORE_HEALTH_GOODIE, x, y, world)
{

}

void RestoreHealth::activate(Actor* affected)
{
    getWorld()->getPlayer()->setHp(100);
    getWorld()->increaseScore(250);
    getWorld()->playSound(SOUND_GOT_GOODIE);
    setAlive(false);
}

/////Restore Flame////////////////////////////////////////////////////////////

RestoreFlame::RestoreFlame(double x, double y, StudentWorld* world) : Goodie(IID_FLAME_THROWER_GOODIE, x, y, world)
{

}

void RestoreFlame::activate(Actor* affected)
{
    int nFlames = getWorld()->getPlayer()->getFlame();
    nFlames += 5;
    getWorld()->getPlayer()->setFlame(nFlames);

    getWorld()->increaseScore(300);
    getWorld()->playSound(SOUND_GOT_GOODIE);
    setAlive(false);
}

/////Fungus///////////////////////////////////////////////////////////////////

Fungus::Fungus(double x, double y, StudentWorld* world) : Goodie(IID_FUNGUS, x, y, world)
{

}

void Fungus::doSomething()
{
    if (!getAlive())
        return;
}

void Fungus::activate(Actor* affected)
{
    getWorld()->increaseScore(-50);
    setAlive(false);
    getWorld()->getPlayer()->takeDamage(20);
}

///////Weapon////////////////////////////////////////////////////////////////

Weapon::Weapon(int IID, double x, double y, int dir, StudentWorld* world) : Actor(IID, x, y, dir, 0, 1.0, world)
{
    setDamageable(false);
    travelDist = 0;
}

Weapon::~Weapon()
{

}

void Weapon::doSomething()
{
    if (!getAlive())
        return;

    moveForward(SPRITE_WIDTH);
    incTravelDist(SPRITE_WIDTH);

    checkDead();
}

double Weapon::getTravelDist() const
{
    return travelDist;
}

void Weapon::incTravelDist(double amt)
{
    travelDist += amt;
}

void Weapon::setTravelDist(double amt)
{
    travelDist = amt;
}

///////Flame//////////////////////////////////////////////////////////////////

Flame::Flame(double x, double y, int dir, StudentWorld* world) : Weapon(IID_FLAME, x, y, dir, world)
{
    setTravelDist(0);
}

void Flame::checkDead()
{
    if (getTravelDist() >= 32)
        setAlive(false);
}

void Flame::activate(Actor* affected)
{
    if (affected == getWorld()->getPlayer())
        return;

    affected->takeDamage(5);
    setAlive(false);
}

///////Spray//////////////////////////////////////////////////////////////////

Spray::Spray(double x, double y, int dir, StudentWorld* world) : Weapon(IID_SPRAY, x, y, dir, world)
{
    setTravelDist(0);
}

void Spray::checkDead()
{
    if (getTravelDist() >= 112)
        setAlive(false);
}

void Spray::activate(Actor* affected)
{
    if (affected == getWorld()->getPlayer())
        return;

    affected->takeDamage(2);
    setAlive(false);
}
