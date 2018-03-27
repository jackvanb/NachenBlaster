#ifndef GAMECONSTANTS_H_
#define GAMECONSTANTS_H_

#include <random>
#include <utility>

// IDs for the game objects

const int IID_NACHENBLASTER  = 0;
const int IID_SMALLGON       = 1;
const int IID_SMOREGON       = 2;
const int IID_SNAGGLEGON     = 3;
const int IID_REPAIR_GOODIE  = 4;
const int IID_LIFE_GOODIE    = 5;
const int IID_TORPEDO_GOODIE = 6;
const int IID_TORPEDO        = 7;
const int IID_TURNIP         = 8;
const int IID_CABBAGE        = 9;
const int IID_STAR           = 10;
const int IID_EXPLOSION      = 11;

// sounds

const int SOUND_THEME          = 0;
const int SOUND_GOODIE         = 1;
const int SOUND_BLAST          = 2;
const int SOUND_PLAYER_SHOOT   = 3;
const int SOUND_ALIEN_SHOOT    = 4;
const int SOUND_DEATH          = 5;
const int SOUND_FINISHED_LEVEL = 6;
const int SOUND_TORPEDO	       = 7;

const int SOUND_NONE           = -1;

// keys the user can hit

const int KEY_PRESS_LEFT   = 1000;
const int KEY_PRESS_RIGHT  = 1001;
const int KEY_PRESS_UP	   = 1002;
const int KEY_PRESS_DOWN   = 1003;
const int KEY_PRESS_SPACE  = ' ';
const int KEY_PRESS_ESCAPE = '\x1b';
const int KEY_PRESS_TAB	   = '\t';

// board and sprite dimensions

const int VIEW_WIDTH = 256;
const int VIEW_HEIGHT = 256;

const int SPRITE_WIDTH = VIEW_WIDTH / 16;
const int SPRITE_HEIGHT = VIEW_HEIGHT / 16;

const double SPRITE_WIDTH_GL = .6; // note - this is tied implicitly to SPRITE_WIDTH due to carey's sloppy openGL programming
const double SPRITE_HEIGHT_GL = .6; // note - this is tied implicitly to SPRITE_HEIGHT due to carey's sloppy openGL programming

// status of each tick (did the player die?)

const int GWSTATUS_PLAYER_DIED	 = 0;
const int GWSTATUS_CONTINUE_GAME = 1;
const int GWSTATUS_PLAYER_WON	 = 2;
const int GWSTATUS_FINISHED_LEVEL= 3;
const int GWSTATUS_LEVEL_ERROR	 = 4;

// test parameter constants

const int NUM_TEST_PARAMS = 1;

  // Return a uniformly distributed random int from min to max, inclusive

inline
int randInt(int min, int max)
{
	if (max < min)
		std::swap(max, min);
	static std::random_device rd;
	static std::mt19937 generator(rd());
	std::uniform_int_distribution<> distro(min, max);
	return distro(generator);
}

#endif // GAMECONSTANTS_H_
