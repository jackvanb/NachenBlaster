#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>
#include <vector>

class Actor;
class NachenBlaster;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    void removeDeadGameObjects();
    void updateDisplayText();
    void addActor(Actor* object);
    bool addNewAlien();
    bool completedLevel();
    std::string getHealthOrCabbagePercantage(double points, double max);
    Actor* hitAlien(Actor* cab);
    
    // INLINE
    void getPlayerLocation(double &x, double &y)
    {
        x = m_player->getX();
        y = m_player->getY();
    }
    void shipDestroyed() { m_shipsDestroyed++; }
    void shipOffScreen() { m_shipsOnScreen--; }
    NachenBlaster* getPlayer() { return m_player; }
    std::vector<Actor*> getActors() { return m_actors; }

private:
    std::vector<Actor*> m_actors;
    NachenBlaster* m_player;
    int m_shipsDestroyed;
    int m_shipsOnScreen;
    
};

#endif // STUDENTWORLD_H_
