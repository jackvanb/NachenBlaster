#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"

const int HIT_BY_SHIP = 0;
const int HIT_BY_PROJECTILE = 1;

class StudentWorld;

class Actor : public GraphObject
{
public:
    Actor(int ID, double x, double y, int dir, double size, int depth, StudentWorld* world);
    virtual void doSomething() = 0;
    virtual void sufferDamage(double amt, int cause) = 0;
    
    void moveToIfAble(double x, double y);
    
    // INLINE FUNCTIONS
    
    StudentWorld* getWorld() const { return m_world; }
    int getID() const { return m_ID; }
    virtual void die() { m_dead = true; }
    bool isDead() const { return m_dead; }
    virtual bool isAlien() const { return false; }
    bool outOfView() const
    {
        if (getX() < 0 || getX() >= VIEW_WIDTH)
            return true;
        return false;
    }
private:
    int m_ID;
    double m_x;
    double m_y;
    int m_dir;
    double m_size;
    int m_depth;
    bool m_dead;
    StudentWorld* m_world;
    
};

class DamageableObject : public Actor
{
public:
    DamageableObject(int ID, double x, double y, int dir, double size, int depth, StudentWorld* world, double hitPoints);
    
    virtual void doSomething() = 0;
    
    // This actor suffers an amount of damage caused by being hit by either
    // a ship or a projectile (see constants above).
    virtual void sufferDamage(double amt, int cause) = 0;
    
    // INLINE
    virtual void increaseHitPoints(double amt) { m_hitPoints += amt; }
    void decreaseHitPoints(double amt) { m_hitPoints -= amt;   }
    double hitPoints() const { return m_hitPoints; }
    void setHitPoints(double hp)  { m_hitPoints = hp;  }
    
private:
    int m_hitPoints;
    
};

class NachenBlaster : public DamageableObject
{
public:
    NachenBlaster(StudentWorld* world);
    virtual void doSomething();
    virtual void sufferDamage(double amt, int cause);
    virtual void increaseHitPoints(double amt);
    
    // INLINE
    void addTorpedos() { m_torpedos += 5; }
    int getTorpedos() { return m_torpedos; }
    int getCabbageEnergy() { return m_cabbageEnergy; }
    
private:
    int m_cabbageEnergy;
    int m_torpedos;
    
};

//////////////////////////

class Alien : public DamageableObject
{
public:
    Alien(int ID, double x, double y, StudentWorld* world,
          double hitPoints, double damageAmt, double deltaX,
          double deltaY, double speed, unsigned int scoreValue);
    
    virtual void possiblyDropGoodie() = 0;
    
    // IMPLEMENTED IN ACTOR.CPP
    virtual void sufferDamage(double amt, int cause);
    virtual void doSomething();
    virtual void newFlightPlan();
    void collidesWithPlayer();
    bool playerNear();
    
    
    // INLINE
    virtual void die();
    virtual bool isAlien() const { return true; }
    void move() { moveTo(getX() + m_deltaX, getY() + m_deltaY); }
    
    void setDeltaX(double dy) { m_deltaX = dy; }
    void setDeltaY(double dy) { m_deltaY = dy; }
    void setSpeed(double speed) { m_speed = speed; }
    
    unsigned int getScoreValue() { return m_scoreValue; }
    double getSpeed() { return m_speed; }
    double getDamageAmt() { return m_damageAmt; }
    
private:
    double m_damageAmt;
    double m_deltaX;
    double m_deltaY;
    double m_speed;
    unsigned int m_scoreValue;
};

class Smallgon : public Alien
{
public:
    Smallgon(double x, double y, StudentWorld* world);
    virtual void doSomething();
    virtual void possiblyDropGoodie();
    
private:
    int m_flightPlanLength;
};

class Smoregon : public Alien
{
public:
    Smoregon(double x, double y, StudentWorld* world);
    virtual void doSomething();
    virtual void possiblyDropGoodie();
    
private:
    int m_flightPlanLength;
};

class Snagglegon : public Alien
{
public:
    Snagglegon(double x, double y, StudentWorld* world);
    virtual void doSomething();
    virtual void possiblyDropGoodie();
    
private:
};

////////////////////////////////


class Star : public Actor
{
public:
    Star(StudentWorld* world);
    virtual void doSomething();
    virtual void sufferDamage(double amt, int cause) {}
private:
    
};

class Explosion : public Actor
{
public:
    Explosion(double x_cord, double y_cord, StudentWorld* world);
    virtual void doSomething();
    virtual void sufferDamage(double amt, int cause) {}
private:
    int m_tickCount;
};

/////////////// Projectile Base Class ///////////

class Projectile : public Actor
{
public:
    Projectile (int ID, double x_cord, double y_cord, StudentWorld* world);
    virtual void doSomething() = 0;
    virtual void sufferDamage(double amt, int cause) { }

private:
    
};

class Cabbage : public Projectile
{
public:
    Cabbage(double x_cord, double y_cord, StudentWorld* world);
    virtual void doSomething();
    
    
//    Actor* hitAlien();

private:
    
};

class Turnip : public Projectile
{
public:
    
    Turnip(double x_cord, double y_cord, StudentWorld* world);
    virtual void doSomething();
    
    
private:
    
    
};

class Torpedo : public Projectile
{
public:
    
    Torpedo(double x_cord, double y_cord, StudentWorld* world);
    virtual void doSomething();
    
    
private:
    
};

class PlayerFiredTorpedo : public Torpedo
{
public:
    
    PlayerFiredTorpedo(double x_cord, double y_cord, StudentWorld* world);
    virtual void doSomething();
    
};

class AlienFiredTorpedo : public Torpedo
{
public:
    AlienFiredTorpedo(double x_cord, double y_cord, StudentWorld* world);
    virtual void doSomething();
    
};

////////// GOODIES ///////////

class Goodie : public Actor
{
public:
    Goodie(int ID, double x_cord, double y_cord, StudentWorld* world);
    
    virtual void doSomething();
    virtual void collidedWithPlayer();
    virtual void specificGoodieAction() = 0;
    
    virtual void sufferDamage(double amt, int cause) {}
    
    // INLINE
    void goodieMove() { moveTo(getX() - 0.75, getY() - 0.75); }
};

class ExtraLifeGoodie : public Goodie
{
public:
    ExtraLifeGoodie(double x_cord, double y_cord, StudentWorld* world);
    virtual void specificGoodieAction();
    
};

class RepairGoodie : public Goodie
{
public:
    RepairGoodie(double x_cord, double y_cord, StudentWorld* world);
    virtual void specificGoodieAction();
    
};

class TorpedoGoodie : public Goodie
{
public:
    TorpedoGoodie(double x_cord, double y_cord, StudentWorld* world);
    virtual void specificGoodieAction();
    
};

#endif // ACTOR_H_
