#include <iostream>
#include <string>
#include <random>
#include <utility>
#include <cstdlib>
#include <cassert>
using namespace std;

///////////////////////////////////////////////////////////////////////////
// Manifest constants
///////////////////////////////////////////////////////////////////////////

const int MAXROWS = 20;             // max number of rows in the arena
const int MAXCOLS = 30;             // max number of columns in the arena
const int MAXROBOTS = 100;          // max number of robots allowed

const int UP    = 0;
const int DOWN  = 1;
const int LEFT  = 2;
const int RIGHT = 3;

///////////////////////////////////////////////////////////////////////////
//  Auxiliary function declarations
///////////////////////////////////////////////////////////////////////////

int decodeDirection(char dir);
int randInt(int min, int max);
void clearScreen();

///////////////////////////////////////////////////////////////////////////
// Type definitions
///////////////////////////////////////////////////////////////////////////

class Arena;  // This is needed to let the compiler know that Arena is a
              // type name, since it's mentioned in the Robot declaration.

class Robot
{
  public:
        // Constructor
    Robot(Arena* ap, int r, int c);

        // Accessors
    int  row() const;
    int  col() const;

        // Mutators
    void move();
    bool getAttacked(int dir);

  private:
    Arena* m_arena;
    int    m_row;
    int    m_col;
    int    m_health;
};

class Player
{
  public:
        // Constructor
    Player(Arena *ap, int r, int c);

        // Accessors
    int  row() const;
    int  col() const;
    int  age() const;
    bool isDead() const;

        // Mutators
    void   stand();
    void   moveOrAttack(int dir);
    void   setDead();

  private:
    Arena* m_arena;
    int    m_row;
    int    m_col;
    int    m_age;
    bool   m_dead;
};

class Arena
{
  public:
        // Constructor/destructor
    Arena(int nRows, int nCols);
    ~Arena();

        // Accessors
    int     rows() const;
    int     cols() const;
    Player* player() const;
    int     robotCount() const;
    int     nRobotsAt(int r, int c) const;
    bool    determineNewPosition(int& r, int& c, int dir) const;
    void    display() const;

        // Mutators
    bool   addRobot(int r, int c);
    bool   addPlayer(int r, int c);
    bool   attackRobotAt(int r, int c, int dir);
    bool   moveRobots();

  private:
    int     m_rows;
    int     m_cols;
    Player* m_player;
    Robot*  m_robots[MAXROBOTS];
    int     m_nRobots;
};

class Game
{
  public:
        // Constructor/destructor
    Game(int rows, int cols, int nRobots);
    ~Game();

        // Mutators
    void play();

  private:
    Arena* m_arena;
};

///////////////////////////////////////////////////////////////////////////
//  Robot implementation
///////////////////////////////////////////////////////////////////////////

Robot::Robot(Arena* ap, int r, int c)
{
    if (ap == nullptr)
    {
        cout << "***** A robot must be in some Arena!" << endl;
        exit(1);
    }
    if (r < 1  ||  r > ap->rows()  ||  c < 1  ||  c > ap->cols())
    {
        cout << "***** Robot created with invalid coordinates (" << r << ","
             << c << ")!" << endl;
        exit(1);
    }
    m_arena = ap;
    m_row = r;
    m_col = c;
    m_health = 2;
}

int Robot::row() const
{
    return m_row;
}

int Robot::col() const
{
    return m_col;
}

void Robot::move()
{
    int dir = randInt(0, 3);  // dir is now UP, DOWN, LEFT, or RIGHT
    m_arena->determineNewPosition(m_row, m_col, dir);
}

bool Robot::getAttacked(int dir)  // return true if robot dies
{
  m_health--;

  if (m_health == 0)  //Robot is destroyed
  {
    return true;
  }
  
    switch(dir)
    {
      case UP:
	if (m_row - 1 < 1) //Robot dies if it lands on an invalid position
	  return true;
	else //Robot gets pushed back
	  m_arena->determineNewPosition(m_row, m_col, dir);
	break;
	
    case DOWN: //Same template as UP
	if (m_row + 1 > m_arena->rows())
	  return true;
	else
	  m_arena->determineNewPosition(m_row, m_col, dir);
	break;

      case LEFT:
	if (m_col - 1 < 1)
	  return true;
	else
	  m_arena->determineNewPosition(m_row, m_col, dir);
	break;

      case RIGHT:
        if (m_col + 1 > m_arena->cols())
	  return true;
	else
	  m_arena->determineNewPosition(m_row, m_col, dir);
	break;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////
//  Player implementations
///////////////////////////////////////////////////////////////////////////

Player::Player(Arena* ap, int r, int c)
{
    if (ap == nullptr)
    {
        cout << "***** The player must be in some Arena!" << endl;
        exit(1);
    }
    if (r < 1  ||  r > ap->rows()  ||  c < 1  ||  c > ap->cols())
    {
        cout << "**** Player created with invalid coordinates (" << r
             << "," << c << ")!" << endl;
        exit(1);
    }
    m_arena = ap;
    m_row = r;
    m_col = c;
    m_age = 0;
    m_dead = false;
}

int Player::row() const
{
  return m_row; 
}

int Player::col() const
{
  return m_col;
}

int Player::age() const
{
  return m_age;
}

void Player::stand()
{
  m_age++;
}

void Player::moveOrAttack(int dir)
{
    m_age++;
    
    switch (dir)
    {
      case UP:
	if (m_row - 1 > 0 && m_arena->nRobotsAt(m_row - 1, m_col) > 0) //If there is a robot UP
	  m_arena->attackRobotAt(m_row - 1, m_col, dir); //attack one robot
	else if (m_row - 1 > 0) //If there is no robot UP
	  m_row--; //move
	else //If UP is an invalid position
	  break;
	break;

      case DOWN: //Same template as UP
	if (m_row + 1 <= m_arena->rows() && m_arena->nRobotsAt(m_row + 1, m_col) > 0)
          m_arena->attackRobotAt(m_row + 1, m_col, dir);
        else if (m_row + 1 <= m_arena->rows())
	  m_row++;
        else
	  break;
	break;
	
      case LEFT:
	if (m_col - 1 > 0 && m_arena->nRobotsAt(m_row, m_col - 1) > 0)
	  m_arena->attackRobotAt(m_row, m_col - 1, dir);
	else if (m_col - 1 > 0)
          m_col--;
	else
	  break;
	break;

      case RIGHT:
	if (m_col + 1 <= m_arena->cols() && m_arena->nRobotsAt(m_row, m_col + 1) > 0)
	  m_arena->attackRobotAt(m_row, m_col + 1, dir);
	else if (m_col + 1 <= m_arena->cols())
	  m_col++;
	else
	  break;
	break;

      default:
	break;
    }
}

bool Player::isDead() const
{
  if (m_dead == true)
    return true;
  else
    return false;
}

void Player::setDead()
{
    m_dead = true;
}

///////////////////////////////////////////////////////////////////////////
//  Arena implementations
///////////////////////////////////////////////////////////////////////////

Arena::Arena(int nRows, int nCols)
{
    if (nRows <= 0  ||  nCols <= 0  ||  nRows > MAXROWS  ||  nCols > MAXCOLS)
    {
        cout << "***** Arena created with invalid size " << nRows << " by "
             << nCols << "!" << endl;
        exit(1);
    }
    m_rows = nRows;
    m_cols = nCols;
    m_player = nullptr;
    m_nRobots = 0;
}

Arena::~Arena() //This is clean-up function
{
  delete m_player;

  for (int i = 0; i != m_nRobots; i++)
    delete m_robots[i];
}

int Arena::rows() const
{
  return m_rows;
}

int Arena::cols() const
{
  return m_cols;
}

Player* Arena::player() const
{
  return m_player;
}

int Arena::robotCount() const
{
  return m_nRobots;
}

int Arena::nRobotsAt(int r, int c) const
{
  int robotCounter = 0;
  
  //Check each robot in m_robots[] until all robots at (r,c) are accounted for
  for (int i = 0; i < m_nRobots; i++)
    if (r == m_robots[i]->row() && c == m_robots[i]->col())
      robotCounter++;
  
  return robotCounter;
}

bool Arena::determineNewPosition(int& r, int& c, int dir) const
{
    switch (dir)
    {
      case UP:
	{
	  if (r - 1 >= 1) //Move UP by one
	    r--;
	  else
	    return false; //If invalid position
	  break;
	}
      case DOWN: //Same template as UP
	{
	  if (r + 1 <= rows())
	    r++;
	  else
	    return false;
	  break;
	}
      case LEFT:
	{
	  if (c - 1 >= 1)
	    c--;
	  else
	    return false;
	  break;
	}
      case RIGHT:
        {
	  if (c + 1 <= cols())
	    c++;
	  else
	    return false;
	  break;
	}
      default:
        return false;
    }
    return true;
}

void Arena::display() const
{
      // Position (row,col) in the arena coordinate system is represented in
      // the array element grid[row-1][col-1]
    char grid[MAXROWS][MAXCOLS];
    int r, c;
    
        // Fill the grid with dots
    for (r = 0; r < rows(); r++)
        for (c = 0; c < cols(); c++)
            grid[r][c] = '.';
    
    for (r = 0; r < rows(); r++)
      for (c = 0; c < cols(); c++)
	{
	  if (nRobotsAt(r+1,c+1) == 1) //If there is 1 robot
	    grid[r][c] = 'R';
	  else if (nRobotsAt(r+1,c+1) > 1 && nRobotsAt(r+1,c+1) < 9) //If multiple robots
	    grid[r][c] = nRobotsAt(r+1,c+1) + '0';
	  else if (nRobotsAt(r+1,c+1) > 9) //If more than 9 robots in a spot
	    grid[r][c] = '9';
	  else  //If there are no robots in (r,c), then leave (r,c) as it is
	    continue;
	}
    
        // Indicate player's position
    if (m_player != nullptr)
    {
          // Set the char to '@', unless there's also a robot there,
          // in which case set it to '*'.
        char& gridChar = grid[m_player->row()-1][m_player->col()-1];
        if (gridChar == '.')
            gridChar = '@';
        else
            gridChar = '*';
    }

        // Draw the grid
    clearScreen();
    for (r = 0; r < rows(); r++)
    {
        for (c = 0; c < cols(); c++)
            cout << grid[r][c];
        cout << endl;
    }
    cout << endl;

        // Write message, robot, and player info
    cout << endl;
    cout << "There are " << robotCount() << " robots remaining." << endl;
    if (m_player == nullptr)
        cout << "There is no player." << endl;
    else
    {
        if (m_player->age() > 0)
            cout << "The player has lasted " << m_player->age() << " steps." << endl;
        if (m_player->isDead())
            cout << "The player is dead." << endl;
    }
}

bool Arena::addRobot(int r, int c)
{
  if (m_nRobots == MAXROBOTS) //If max limit of robots is reached
    return false;
  m_robots[m_nRobots] = new Robot(this, r, c);
    m_nRobots++;
    return true;
}

bool Arena::addPlayer(int r, int c)
{
      // Don't add a player if one already exists
    if (m_player != nullptr)
        return false;

      // Dynamically allocate a new Player and add it to the arena
    m_player = new Player(this, r, c);
    return true;
}

bool Arena::attackRobotAt(int r, int c, int dir)
{
  if (nRobotsAt(r,c) >= 1)
    {
      for (int i = 0; i < m_nRobots; i++)
        if (r == m_robots[i]->row() && c == m_robots[i]->col() && m_robots[i]->getAttacked(dir) == 1) //If the robot has 0 health
	  {
	    delete m_robots[i];
	    m_nRobots--;

	    for (int x = i; x < m_nRobots; x++)
	      m_robots[x] = m_robots[x+1];

	    m_robots[m_nRobots] = nullptr;
	    return true; //Attack one robot only
	                 //Terminate function immediately
	  }
	else
	  continue;
    }
 
  //There are no robots at (r,c) or robot didn't die
  return false;
}

bool Arena::moveRobots()
{
    for (int k = 0; k < m_nRobots; k++)
    {
      m_robots[k]->move();
      if (m_robots[k]->row() == m_player->row() && m_robots[k]->col() == m_player->col())
	  m_player->setDead(); //Player dies if robot lands on
	                       //same position as the player
    }

      // return true if the player is still alive, false otherwise
    return ! m_player->isDead();
}

///////////////////////////////////////////////////////////////////////////
//  Game implementations
///////////////////////////////////////////////////////////////////////////

Game::Game(int rows, int cols, int nRobots)
{
    if (nRobots < 0)
    {
        cout << "***** Cannot create Game with negative number of robots!" << endl;
        exit(1);
    }
    if (nRobots > MAXROBOTS)
    {
        cout << "***** Trying to create Game with " << nRobots
             << " robots; only " << MAXROBOTS << " are allowed!" << endl;
        exit(1);
    }
    if (rows == 1  &&  cols == 1  &&  nRobots > 0)
    {
        cout << "***** Cannot create Game with nowhere to place the robots!" << endl;
        exit(1);
    }

        // Create arena
    m_arena = new Arena(rows, cols);

        // Add player
    int rPlayer = randInt(1, rows);
    int cPlayer = randInt(1, cols);
    m_arena->addPlayer(rPlayer, cPlayer);

      // Populate with robots
    while (nRobots > 0)
    {
        int r = randInt(1, rows);
        int c = randInt(1, cols);
          // Don't put a robot where the player is
        if (r == rPlayer  &&  c == cPlayer)
            continue;
        m_arena->addRobot(r, c);
        nRobots--;
    }
}

Game::~Game()
{
    delete m_arena;
}

void Game::play()
{
    Player* p = m_arena->player();
    if (p == nullptr)
    {
        m_arena->display();
        return;
    }
    do
    {
        m_arena->display();
        cout << endl;
        cout << "Move (u/d/l/r//q): ";
        string action;
        getline(cin,action);
        if (action.size() == 0)  // player stands
            p->stand();
        else
        {
            switch (action[0])
            {
              default:   // if bad move, nobody moves
                cout << '\a' << endl;  // beep
                continue;
              case 'q':
                return;
              case 'u':
              case 'd':
              case 'l':
              case 'r':
                p->moveOrAttack(decodeDirection(action[0]));
                break;
            }
        }
        m_arena->moveRobots();
    } while ( ! m_arena->player()->isDead()  &&  m_arena->robotCount() > 0);
    m_arena->display();
}

///////////////////////////////////////////////////////////////////////////
//  Auxiliary function implementations
///////////////////////////////////////////////////////////////////////////

int decodeDirection(char dir)
{
    switch (dir)
    {
      case 'u':  return UP;
      case 'd':  return DOWN;
      case 'l':  return LEFT;
      case 'r':  return RIGHT;
    }
    return -1;  // bad argument passed in!
}

  // Return a random int from min to max, inclusive
int randInt(int min, int max)
{
    if (max < min)
        swap(max, min);
    static random_device rd;
    static mt19937 generator(rd());
    uniform_int_distribution<> distro(min, max);
    return distro(generator);
}

///////////////////////////////////////////////////////////////////////////
//  main()
///////////////////////////////////////////////////////////////////////////

int main()
{
      // Create a game
      // Use this instead to create a mini-game:   Game g(3, 4, 2);
  Game g(7, 8, 25);
      // Play the game
    g.play();
}

///////////////////////////////////////////////////////////////////////////
//  clearScreen implementation
///////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER  //  Microsoft Visual C++

#include <windows.h>

void clearScreen()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    DWORD dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    COORD upperLeft = { 0, 0 };
    DWORD dwCharsWritten;
    FillConsoleOutputCharacter(hConsole, TCHAR(' '), dwConSize, upperLeft,
                                                        &dwCharsWritten);
    SetConsoleCursorPosition(hConsole, upperLeft);
}

#else  // not Microsoft Visual C++, so assume UNIX interface

#include <iostream>
#include <cstring>
#include <cstdlib>

void clearScreen()
{
    static const char* term = getenv("TERM");
    if (term == nullptr  ||  strcmp(term, "dumb") == 0)
        cout << endl;
    else
    {
        static const char* ESC_SEQ = "\x1B[";  // ANSI Terminal esc seq:  ESC [
        cout << ESC_SEQ << "2J" << ESC_SEQ << "H" << flush;
    }
}

#endif


