#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>
#include <iostream> // defines the overloads of the << operator
#include <sstream>  // defines the type std::ostringstream
#include <iomanip>  // defines the manipulator setw

using namespace std;

bool hasCollidedWithActor(Actor* a1, Actor* a2);

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetDir)
: GameWorld(assetDir)
{
    m_actors.clear();
    m_player = nullptr;
    m_shipsDestroyed = 0;
    m_shipsOnScreen = 0;
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

Actor* StudentWorld::hitAlien(Actor* cab)
{
    std::vector<Actor*>::iterator it;
    it = m_actors.begin();
    
    while (it != m_actors.end())
    {
        if ((*it)->isAlien() && hasCollidedWithActor(cab, *it))
                return *it;
        else
            it++;
    }
    return nullptr;
}

bool StudentWorld::completedLevel()
{
    if (m_shipsDestroyed >= (6 + (4 * getLevel())))
        return true;
    else
        return false;
}

int StudentWorld::init()
{
    // Add 30 Stars
    for (int i = 0; i != 30; i++)
    {
        addActor(new Star(this));
    }
    
    
    // Create NachenBlaster
    m_player = new NachenBlaster(this);
    
    // Set Ships Destroyed to 0
    m_shipsDestroyed = 0;
    m_shipsOnScreen = 0;
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    
    m_player->doSomething();    // Let User Do Something
    
    
    std::vector<Actor*>::iterator it;
    it = m_actors.begin();
    
    while (it != m_actors.end())
    {
        if (!(*it)->isDead())
        {
            (*it)->doSomething();
            if (m_player->isDead())
            {
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            if (completedLevel())
            {
                // increase score?
                playSound(SOUND_FINISHED_LEVEL);
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
        it++;
    }
    
    // Add New Stars
    int random = randInt(1, 15);
    if (random == 1)
    {
        addActor(new Star(this));
    }
    
    // Add New Aliens
    if (addNewAlien())
    {
        double S1 = 60;
        double S2 = 20 + getLevel() * 5;
        double S3 =  5 + getLevel() * 10;
        int rand = randInt(0, (S1 + S2 + S3));

        // Probabilies to decide what Alien to create
        if (rand >= 0 && rand < S1)
            addActor(new Smallgon(VIEW_WIDTH-1,randInt(0, VIEW_HEIGHT) -1 ,this));
        else if (rand >= S1 && rand < (S1 +S2))
            addActor(new Smoregon(VIEW_WIDTH-1,randInt(0, VIEW_HEIGHT) -1 ,this));
        else
            addActor(new Snagglegon(VIEW_WIDTH-1,randInt(0, VIEW_HEIGHT) -1 ,this));
        
    }
    
    
    removeDeadGameObjects();
        
    // Update the Game Status Line
    updateDisplayText();
    
    // the player hasn’t completed the current level and hasn’t died, so
    // continue playing the current level
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::removeDeadGameObjects()
{
    std::vector<Actor*>::iterator it;
    it = m_actors.begin();
    
    while (it != m_actors.end()) {
        if ((*it)->isDead())
        {
            delete *it;
            it = m_actors.erase(it);
        }
        else
            it++;
    }
}

void StudentWorld::addActor(Actor* object)
{
    m_actors.push_back(object);
}

void StudentWorld::cleanUp()
{
    std::vector<Actor*>::iterator it;
    
    it = m_actors.begin();
    
    while (it != m_actors.end())
    {
            delete *it;
            it = m_actors.erase(it);
    }
    if (m_player != nullptr)
        delete m_player;
    
    m_player = nullptr;
}

bool StudentWorld::addNewAlien()
{
    int total =  6 + (4 * getLevel());
    int remaining = total - m_shipsDestroyed;
    int max =   4 + (.5 * getLevel());

    if (remaining < max)
    {
        if (m_shipsOnScreen < remaining)
        {
            m_shipsOnScreen++;
            return true;
        }
    }
    else
    {
        if (m_shipsOnScreen < max)
        {
            m_shipsOnScreen++;
            return true;
        }
    }
    return false;
}

string StudentWorld::getHealthOrCabbagePercantage(double points, double max)
{
    double amt = (points / max * 100);
    ostringstream oss;  
    oss.setf(ios::fixed);
    oss.precision(0);
    oss << amt;
    return oss.str();
}

void StudentWorld::updateDisplayText()
{
    string s;
    s += "Lives: " + to_string(getLives()) + " Health: " + getHealthOrCabbagePercantage(m_player->hitPoints(), 50) + "% Score: " + to_string(getScore()) + " Level: " + to_string(getLevel()) + " Cabbages: " + getHealthOrCabbagePercantage(m_player->getCabbageEnergy(), 30) + "% Torpedoes: " + to_string(m_player->getTorpedos());
    setGameStatText(s);
}

