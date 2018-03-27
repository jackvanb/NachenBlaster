#include "Actor.h"
#include "StudentWorld.h"
#include <cmath>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

// STAR CONSTANTS
const double STAR_MIN_SIZE = 5;
const double STAR_MAX_SIZE = 50;
const int STAR_DEPTH = 3;
// NACHENBLASTER CONSTANS
const double NACHENBLASTER_SIZE = 1.0;
const int NACHENBLASTER_DEPTH = 0;
const int CABBAGE_ENERGY_MAX = 30;
const int PLAYER_HP_MAX = 50;

// FLIGHT PLANS
const int DUE_LEFT = 1;
const int UP_AND_LEFT = 2;
const int DOWN_AND_LEFT = 3;

//////////// Helper Functions ////////////////////
bool hasCollidedWithActor(Actor* a1, Actor* a2)
{
    double x = a1->getX() - a2->getX();
    double y = a1->getY() - a2->getY();
    double dist;
    
    dist = pow(x, 2) + pow(y, 2);
    dist = sqrt(dist);
    
    // Eucledian Distance Between Two Actors
    if (dist < (.75 * (a1->getRadius() + a2->getRadius())))
        return true;
    else
        return false;
}


/////////////////// ACTOR   /////////////////////

Actor::Actor(int ID, double x, double y, int dir, double size, int depth, StudentWorld* world)
: GraphObject(ID, x, y, dir, size, depth)
{
    m_ID = ID;
    m_x = x;
    m_y = y;
    m_dir = dir;
    m_size = size;
    m_depth = depth;
    m_dead = false;
    m_world = world;
    
}

void Actor::moveToIfAble(double x, double y)
{
  if (x < 0 || x >= VIEW_WIDTH || y < 0 || y >= VIEW_HEIGHT)    // Out of View
      return;
  else
      moveTo(x, y);
}

/////////////////// DAMAGEABLE OBJECT /////////////////////
DamageableObject::DamageableObject(int ID, double x, double y, int dir, double size, int depth, StudentWorld* world, double hitPoints)
: Actor(ID, x, y, dir, size, depth, world)
{
    m_hitPoints = hitPoints;
}

/////////////////// NACHENBLASTER  /////////////////////

NachenBlaster::NachenBlaster(StudentWorld* world)
: DamageableObject(IID_NACHENBLASTER, 0, 128, 0, NACHENBLASTER_SIZE, NACHENBLASTER_DEPTH, world, PLAYER_HP_MAX)
{
    m_cabbageEnergy = CABBAGE_ENERGY_MAX;
    m_torpedos = 0;
}

void NachenBlaster::increaseHitPoints(double amt)
{
    if (( hitPoints() + amt ) > PLAYER_HP_MAX)
        setHitPoints(PLAYER_HP_MAX);
    else
        DamageableObject::increaseHitPoints(amt);
}


void NachenBlaster::sufferDamage(double amt, int cause)
{
    if (cause == HIT_BY_SHIP)
    {
        decreaseHitPoints(amt);
    }
    
    if (cause == HIT_BY_PROJECTILE)
    {
        decreaseHitPoints(amt);
        getWorld()->playSound(SOUND_BLAST);
    }
    
    if (hitPoints() <= 0)
        die();
}

void NachenBlaster::doSomething()
{
    if (isDead())
        return;
    
    int ch;
    
    if (getWorld()->getKey(ch))
    {
        switch (ch) {
            case KEY_PRESS_LEFT:
            {
                moveToIfAble(getX() - 6, getY());
                break;
            }
            case KEY_PRESS_RIGHT:
            {
                moveToIfAble(getX() + 6, getY());
                break;
            }
            case KEY_PRESS_UP:
            {
                moveToIfAble(getX(), getY() + 6);
                break;
            }
            case KEY_PRESS_DOWN:
            {
                moveToIfAble(getX(), getY() - 6);
                break;
            }
                
            case KEY_PRESS_SPACE:
            {
                if (m_cabbageEnergy > 5)
                {
                    getWorld()->addActor(new Cabbage(getX() + 12, getY(), getWorld()));
                    getWorld()->playSound(SOUND_PLAYER_SHOOT);
                    m_cabbageEnergy -= 5;
                }
                break;
            }
            case KEY_PRESS_TAB:
            {
                if (m_torpedos > 0)
                {
                    
                    getWorld()->addActor(new PlayerFiredTorpedo(getX() + 12, getY(), getWorld()));
                    m_torpedos--;
                    getWorld()->playSound(SOUND_TORPEDO);
                    
                }
                break;
            }
                
            default:
                break;
        }
        
    }
    
    if (m_cabbageEnergy < 30)
        m_cabbageEnergy++;
}

/////////////////// ALIEN /////////////////////
Alien::Alien(int ID, double x, double y, StudentWorld* world,
      double hitPoints, double damageAmt, double deltaX,
      double deltaY, double speed, unsigned int scoreValue)
: DamageableObject(ID, x, y, 0, 1.5, 1, world, hitPoints)
{
    m_damageAmt = damageAmt;
    m_deltaX = deltaX;
    m_deltaY = deltaY;
    m_speed = speed;
    m_scoreValue = scoreValue;
}

void Alien::die()
{
    Actor::die();
    getWorld()->shipOffScreen();
    
}
void Alien::doSomething()
{
    // CHECK TO SEE IF DEAD OR OUT OF VIEW
    
    if (isDead())
        return;
    
    if (outOfView())
    {
        die();
        return;
    }
    
}

void Alien::sufferDamage(double amt, int cause)
{
    decreaseHitPoints(amt);
    if (hitPoints() <= 0 || cause == HIT_BY_SHIP)
    {
        getWorld()->increaseScore(getScoreValue());
        die();
        getWorld()->shipDestroyed();
        getWorld()->playSound(SOUND_DEATH);
        getWorld()->addActor(new Explosion(getX(), getY(), getWorld()));
        possiblyDropGoodie();
    }
    else
        getWorld()->playSound(SOUND_BLAST);
    
}

void Alien::collidesWithPlayer()
{
    getWorld()->getPlayer()->sufferDamage(getDamageAmt(), HIT_BY_SHIP);
    sufferDamage(hitPoints(), HIT_BY_SHIP);
}

bool Alien::playerNear()
{
    double playerX;
    double playerY;
    getWorld()->getPlayerLocation(playerX, playerY);
    
    if (playerX < getX()) {
        if (playerY - getY() <= 4 && playerY - getY() >= -4) {
            return true;
        }
    }
    return false;
}

void Alien::newFlightPlan()
{
    if (getY() >= VIEW_HEIGHT - 1)
    {        
        // Up and Left
        setDeltaX(getSpeed() * -1);
        setDeltaY(getSpeed() * -1);
    }
    else if (getY() <= 0)
    {
        // Down and Left
        setDeltaX(getSpeed() * -1);
        setDeltaY(getSpeed());
    }
    else
    {
        int x = randInt(1, 3);
        switch(x)
        {
            case DUE_LEFT:
            {
                setDeltaX(getSpeed() * -1);
                setDeltaY(0);
                break;
            }
            case DOWN_AND_LEFT:
            {
                setDeltaX(getSpeed() * -1);
                setDeltaY(getSpeed());
                break;
            }
            case UP_AND_LEFT:
            {
                setDeltaX(getSpeed() * -1);
                setDeltaY(getSpeed() * -1);
                break;
            }
        }
    }
}

/////////////////// SMALLGON /////////////////////
Smallgon::Smallgon(double x, double y, StudentWorld* world)
: Alien(IID_SMALLGON, x, y, world, 5, 5, 0, 0, 2.0, 250)
{
    m_flightPlanLength = 0;
    setHitPoints(5 * (1 + (getWorld()->getLevel() - 1) * 0.1));
}

void Smallgon::possiblyDropGoodie()
{
    // Smallgon's can't drop goodies
    return;
}

void Smallgon::doSomething()
{
    Alien::doSomething();
    
    // CHECK TO SEE IF COLLIDED WITH NACHENBLASTER
    if (hasCollidedWithActor(this, getWorld()->getPlayer()))
        collidesWithPlayer();
    
    // FLIGHT PLAN
    if (m_flightPlanLength == 0 || getY() >= VIEW_HEIGHT - 1 || getY() <= 0)
    {
        newFlightPlan();
        m_flightPlanLength = randInt(1, 32);
    }
    
    // Check to see NachenBlaster Location
    if (playerNear())
    {
            int rand = randInt(1, 20/(getWorld()->getLevel())+5);
            if (rand == 1)
            {
                getWorld()->addActor(new Turnip(getX() + 14, getY(), getWorld()));
                getWorld()->playSound(SOUND_ALIEN_SHOOT);
                return;
            }
        }
    
    // CHOOSE NEW FLIGTH PLAN
    move();
    m_flightPlanLength--;
    
    // CHECK AGAIN
    if (hasCollidedWithActor(this, getWorld()->getPlayer()))
        collidesWithPlayer();
    
    
}

/////////////////// SMOREGON /////////////////////
Smoregon::Smoregon(double x, double y, StudentWorld* world)
: Alien(IID_SMOREGON, x, y, world, 5, 5, 0, 0, 2.0, 250)
{
    m_flightPlanLength = 0;
    setHitPoints(5 * (1 + (getWorld()->getLevel() - 1) * 0.1));
}


void Smoregon::possiblyDropGoodie()
{
    int rand = randInt(1, 3);
    
    if (rand == 1)
    {
        int rand1 = randInt(1, 2);
        if (rand1 == 1)
            getWorld()->addActor(new RepairGoodie(getX(), getY(), getWorld()));
        else
            getWorld()->addActor(new TorpedoGoodie(getX(), getY(), getWorld()));
    }
}

void Smoregon::doSomething()
{
    Alien::doSomething();
    
    // CHECK TO SEE IF COLLIDED WITH NACHENBLASTER
    if (hasCollidedWithActor(this, getWorld()->getPlayer()))
        collidesWithPlayer();
    
    
    //FLIGHT PLAN
    if (m_flightPlanLength == 0 || getY() >= VIEW_HEIGHT - 1 || getY() <= 0)
    {
        newFlightPlan();
        m_flightPlanLength = randInt(1, 32);
    }
    
    // Check to see NachenBlaster Location
    if (playerNear())
    {
            int rand = randInt(1, 20/(getWorld()->getLevel())+5);
            int rand1 = randInt(1, 20/(getWorld()->getLevel())+5);
            if (rand == 1)
            {
                getWorld()->addActor(new Turnip(getX() + 14, getY(), getWorld()));
                getWorld()->playSound(SOUND_ALIEN_SHOOT);
                return;
            }
            if (rand1 == 1)
            {
                // Set location to due left
                setDeltaX(getSpeed() * -1);
                setDeltaY(0);
                m_flightPlanLength = VIEW_WIDTH;
                setSpeed(5.0);
            }
    }
    
    //CHOSE NEW FLIGHT PLAN
    move();
    m_flightPlanLength--;
    
    // CHECK AGAIN
    if (hasCollidedWithActor(this, getWorld()->getPlayer()))
        collidesWithPlayer();
    
    
}

/////////////////// SNAGGLEGON /////////////////////
Snagglegon::Snagglegon(double x, double y, StudentWorld* world)
: Alien(IID_SNAGGLEGON, x, y, world, 10, 15, -1.75, 1.75, 1.75, 1000)
{
    setHitPoints(10 * (1 + (getWorld()->getLevel() - 1) * 0.1));
}


void Snagglegon::possiblyDropGoodie()
{
    int rand = randInt(1, 6);
    
    if (rand == 1)
    {
        getWorld()->addActor(new ExtraLifeGoodie(getX(), getY(), getWorld()));
        
    }
}

void Snagglegon::doSomething()
{
    Alien::doSomething();
    if (getY() >= VIEW_HEIGHT - 1 || getY() <= 0)
    {
        newFlightPlan();
    }
    
    // CHECK TO SEE IF COLLIDED WITH NACHENBLASTER
    if (hasCollidedWithActor(this, getWorld()->getPlayer()))
        collidesWithPlayer();
    
    
    // Check to see NachenBlaster Location
    if (playerNear())
    {
            int rand = randInt(1, 15 /(getWorld()->getLevel())+10);
            if (rand == 1)
            {
                getWorld()->addActor(new AlienFiredTorpedo(getX() + 14, getY(), getWorld()));
                getWorld()->playSound(SOUND_TORPEDO);
                return;
            }
        }
    
    move();
        
    // CHECK AGAIN
    if (hasCollidedWithActor(this, getWorld()->getPlayer()))
        collidesWithPlayer();
}



/////////////////// STAR   /////////////////////

Star::Star(StudentWorld* world)
: Actor(IID_STAR, randInt(0, VIEW_WIDTH-1), randInt(0, VIEW_HEIGHT-1), 0, randInt(STAR_MIN_SIZE, STAR_MAX_SIZE) / 100.0, STAR_DEPTH, world)
{
}

void Star::doSomething()
{
    moveTo(getX() - 1, getY());
    
    if (getX() <= -1)
        die();
}

/////////////////// EXPLOSION /////////////////////

Explosion::Explosion(double x_cord, double y_cord, StudentWorld* world)
: Actor(IID_EXPLOSION, x_cord, y_cord, 0, 1, 0, world)
{
    m_tickCount = 0;
}

void Explosion::doSomething()
{
    setSize(getSize() * 1.5);
    
    m_tickCount++;
    if (m_tickCount >= 4)
    {
        m_tickCount = 0;
        die();
    }
}

/////////////////// PROJECTILE /////////////////////

Projectile::Projectile(int ID, double x_cord, double y_cord, StudentWorld* world)
: Actor(ID, x_cord, y_cord, 0, 0.5, 1, world)
{

}



/////////////////// CABBAGE /////////////////////
Cabbage::Cabbage(double x_cord, double y_cord, StudentWorld* world)
: Projectile(IID_CABBAGE, x_cord, y_cord, world)
{
}


void Cabbage::doSomething()
{
    if (isDead())
        return;
    
    if (getX() >= VIEW_WIDTH)
    {
        die();
        return;
    }
    
    // Check Collison
    Actor* alien = getWorld()->hitAlien(this);
    if (alien != nullptr)
    {
        alien->sufferDamage(2, HIT_BY_PROJECTILE);
        die();
        return;
    }
    
    moveTo(getX() + 8 , getY());
    setDirection(getDirection() + 20);
    
    // Check Collison
    Actor* alien1 = getWorld()->hitAlien(this);
    if (alien1 != nullptr)
    {
        alien1->sufferDamage(2, HIT_BY_PROJECTILE);
        die();
        return;
    }
}

/////////////////// TURNIP /////////////////////
Turnip::Turnip(double x_cord, double y_cord, StudentWorld* world)
: Projectile(IID_TURNIP, x_cord, y_cord, world)
{
}


void Turnip::doSomething()
{
    if (isDead())
        return;
    
    if (getX() < 0)
    {
        die();
        return;
    }
    
    
    // CHECK TO SEE IF COLLIDED WITH NACHENBLASTER
    if (hasCollidedWithActor(this, getWorld()->getPlayer()))
    {
        getWorld()->getPlayer()->sufferDamage(2, HIT_BY_PROJECTILE);
        die();
        return;
    }
    
    moveTo(getX() - 6 , getY());
    setDirection(getDirection() + 20);
    
    // Check Again
    if (hasCollidedWithActor(this, getWorld()->getPlayer()))
    {
        getWorld()->getPlayer()->sufferDamage(2, HIT_BY_PROJECTILE);
        die();
        return;
    }
    
}

/////////////////// TORPEDO /////////////////////

Torpedo::Torpedo(double x_cord, double y_cord, StudentWorld* world)
: Projectile(IID_TORPEDO, x_cord, y_cord, world)
{
}

void Torpedo::doSomething()
{
    if (isDead())
        return;
    
    if (getX() < 0 || getX() >= VIEW_WIDTH)
    {
        die();
        return;
    }
    // Rest implemented in specifc torpedos
}

/////////////////// PLAYER FIRED TORPEDO /////////////////////

PlayerFiredTorpedo::PlayerFiredTorpedo(double x_cord, double y_cord, StudentWorld* world)
: Torpedo(x_cord, y_cord, world)
{
}


void PlayerFiredTorpedo::doSomething()
{
    Torpedo::doSomething();
    
    // Check Collison
    Actor* alien = getWorld()->hitAlien(this);
    if (alien != nullptr)
    {
        alien->sufferDamage(8, HIT_BY_PROJECTILE);
        die();
        return;
    }
    
    moveTo(getX() + 8 , getY());
    
    // Check Collison AGAIN
    Actor* alien1 = getWorld()->hitAlien(this);
    if (alien1 != nullptr)
    {
        alien1->sufferDamage(8, HIT_BY_PROJECTILE);
        die();
        return;
    }
    
}


/////////////////// ALIEN FIRED TORPEDO /////////////////////

AlienFiredTorpedo::AlienFiredTorpedo(double x_cord, double y_cord, StudentWorld* world)
: Torpedo(x_cord, y_cord, world)
{
}


void AlienFiredTorpedo::doSomething()
{
    Torpedo::doSomething();
    
    //CHECK IF HIT PLAYER
    if (hasCollidedWithActor(this, getWorld()->getPlayer()))
    {
        getWorld()->getPlayer()->sufferDamage(8, HIT_BY_PROJECTILE);
        die();
        return;
    }
    moveTo(getX() - 8 , getY());
    
//AGAIN
    if (hasCollidedWithActor(this, getWorld()->getPlayer()))
    {
        getWorld()->getPlayer()->sufferDamage(8, HIT_BY_PROJECTILE);
        die();
        return;
    }
    
}

/////////////////// GOODIE /////////////////////

Goodie::Goodie(int ID, double x_cord, double y_cord, StudentWorld* world)
: Actor(ID, x_cord, y_cord, 0, 0.5, 1, world)
{
    
}

void Goodie::collidedWithPlayer()
{
    getWorld()->increaseScore(100);
    die();
    getWorld()->playSound(SOUND_GOODIE);
    specificGoodieAction();
}

void Goodie::doSomething()
{
    if (isDead())
        return;
    
    if (getX() < 0 || getX() >= VIEW_WIDTH)
    {
        die();
        return;
    }
    
    // CHECK
    if (hasCollidedWithActor(this, getWorld()->getPlayer()))
    {
        collidedWithPlayer();
        return;
    }
    
    goodieMove();
    
    // CHECK AGAIN
    if (hasCollidedWithActor(this, getWorld()->getPlayer()))
    {
        collidedWithPlayer();
        return;
    }
}

/////////////////// EXTRA LIFE GOODIE /////////////////////

ExtraLifeGoodie::ExtraLifeGoodie(double x_cord, double y_cord, StudentWorld* world)
: Goodie(IID_LIFE_GOODIE, x_cord, y_cord, world)
{
    
}
void ExtraLifeGoodie::specificGoodieAction()
{
    getWorld()->incLives();
}

/////////////////// REPAIR GOODIE /////////////////////

RepairGoodie::RepairGoodie(double x_cord, double y_cord, StudentWorld* world)
: Goodie(IID_REPAIR_GOODIE, x_cord, y_cord, world)
{
    
}

void RepairGoodie::specificGoodieAction()
{
    getWorld()->getPlayer()->increaseHitPoints(10);
}


/////////////////// TORPEDO GOODIE /////////////////////

TorpedoGoodie::TorpedoGoodie(double x_cord, double y_cord, StudentWorld* world)
: Goodie(IID_TORPEDO_GOODIE, x_cord, y_cord, world)
{
    
}

void TorpedoGoodie::specificGoodieAction()
{
    getWorld()->getPlayer()->addTorpedos();
}

