#include "cube.h" // Position
#include "Arduino.h"

class Movable {
public:
  virtual void move() = 0;
  virtual Position & pos() = 0;
};

class Movement : public Movable {
protected:
  Position _pos;
  Direction _dir;
  
  Direction oposedDir(Direction dir) {
    switch(dir) {
      case XMinus: return XPlus;
      case XPlus:  return XMinus;
      case YMinus: return YPlus;
      case YPlus:  return YMinus;
      case ZMinus: return ZPlus;
      case ZPlus:  return ZMinus;
    }
  }
  
public:
  Movement(): _pos(0,0,0) {}
  Movement(int x, int y, int z, Direction dir) :
    _pos(x,y,z),
    _dir(dir) {}
  
  void setDir(Direction dir) {
    if(dir >= 0 && dir <= 5)
     _dir = dir;
  }
  
  Direction setDir() { return _dir; }
  
  void moveAhead() {
    Position testPos = _pos;
    testPos.moveOnDir(_dir);
    if(!testPos.outsideCube()) {
      _pos = testPos;
    }
  }
  
  void moveBackwards() {
    Position testPos = _pos;
    testPos.moveOnDir(oposedDir(_dir));
    if(!testPos.outsideCube())
      _pos = testPos;
  }
  
  void set(int x, int y, int z, Direction dir) {
    Position testPos;
    testPos.x = x; testPos.y = y; testPos.z = z;
    if(!testPos.outsideCube())
      _pos = testPos;
    setDir(dir);
  }
  
  void set(Position & pos, Direction dir) {
    if(!pos.outsideCube())
      _pos = pos;
    setDir(dir);
  }
  
  Position & pos() {
    return _pos;
  }
  
  void pos(Position & pos) {
    if(!pos.outsideCube())
      _pos = pos;
  }
  
  void pos(int x, int y, int z) {
    Position newPos(x,y,z);
    pos(newPos);
  }
};

class AheadMovement : public Movement {
public:
  AheadMovement(int x, int y, int z, Direction dir):
    Movement(x,y,z,dir) {}
  
  void move() {
    moveAhead();
  }
};

class CollectionMovement : public Movable {
  Collection<Position> *_positions;
  Iterator<Position> * _it;
  Position *_currentPos;
public:
  CollectionMovement(Collection<Position> * positions):
    _positions(positions),
    _it(_positions->ciclicalIterator()),
    _currentPos(&_it->next()) {}
  
  void move() {
    _currentPos = &_it->next();
  }
  
  Position & pos() {
    return *_currentPos;
  }
};

class XYMovement : public CollectionMovement {
  Collection<Position> _positions;
  int _z;
  
public:
  XYMovement(int index, int z=0):
    _positions(8),
    CollectionMovement(&_positions),
    _z(z)
  {
    // init the positions collection
    Position auxPos(0,0,_z);
    
    for(int x = 0; x < 3; x++) {
      auxPos.x = x;
      _positions.add(auxPos);
    }
    
    for(int y = 1; y < 3; y++) {
      auxPos.y = y;
      _positions.add(auxPos);
    }
    
    for(int x = 1; x >= 0; x--) {
      auxPos.x = x;
      _positions.add(auxPos);
    }
    
    auxPos.y = 1;
    _positions.add(auxPos);
    
    
    for(int i = 0; i < index; i++)
      move();
  }
};

class FirstToLastMove : public CollectionMovement {
  const int _n_leds_in_cube;
  Collection<Position> _positions;
public:
  FirstToLastMove():
    _n_leds_in_cube(3*3*3),
    _positions(_n_leds_in_cube),
    CollectionMovement(&_positions)
  {
    int i, j, x, y, z; // for indices
    
    // init the collection with (0,0,0) positions
    Position auxPos(0,0,0);
    for(i = 0; i < _n_leds_in_cube; i++)
      _positions.add(auxPos);
    
    // init the x coordenates
    for(x = 0, i = 0; x < 3; x++, i++)
      _positions[i].x = x;
    
    for(j = 0; j < 4; j++) {
      for(x = 2; x >= 0; x--, i++)
       _positions[i].x = x;
      
      for(x = 0; x < 3; x++, i++)
        _positions[i].x = x;
    }
    
    // init the y coordinates
    for(y = 0, i = 0; y < 3; y++)
      for(j = 0; j < 3; j++, i++)
        _positions[i].y = y;
    
    for(y = 2; y >= 0; y--)
      for(j = 0; j < 3; j++, i++)
        _positions[i].y = y;
    
    for(y = 0; y < 3; y++)
      for(j = 0; j < 3; j++, i++)
        _positions[i].y = y;
    
    // init the z coordinates
    for(z = 0, i = 0; z < 3; z++)
      for(j = 0; j < 9; j++, i++)
        _positions[i].z = z;
  }
};

class RandomMovement : public Movement {
protected:
  Direction randomDir() {
    return (Direction)random(0,6);
  }
  
  int randomPos() {
    return random(0,3);
  }
  
  Direction randomNewDir() {
    Direction newDir = oposedDir(_dir);
    while(newDir == oposedDir(_dir)) {
      newDir = randomDir();
    }
    return newDir;
  }
  
public:
  RandomMovement(int seed) {
    randomSeed(seed);
    _dir = randomDir();
    _pos.x = randomPos();
    _pos.y = randomPos();
    _pos.z = randomPos();
  }
  
  virtual void moveRandom() = 0;
  
  //Override
  void move() {
    moveRandom();
  }
};

class RandomAllDirMovement : public RandomMovement {
public:
  RandomAllDirMovement(int seed):
    RandomMovement(seed) {}
  
  // Override
  void moveRandom() {
    Position newPos;
    Direction newDir;
    do {
      newDir = randomNewDir();
      newPos = _pos;
      newPos.moveOnDir(newDir);
    } while(newPos.outsideCube());
    _pos = newPos;
    _dir = newDir;
  }
};

// Moves on a random direction inside the cube.
class RandomArestaMovement : public RandomMovement {
public:
  RandomArestaMovement(int seed):
    RandomMovement(seed) {}
  
  // Override
  void moveRandom() {
    Position newPos(_pos);
    Direction newDir = _dir;
     
    newPos.moveOnDir(newDir);
    while(newPos.outsideCube()) {
      newDir = randomNewDir();
      newPos = _pos;
      newPos.moveOnDir(newDir);
    }
    _pos = newPos;
    _dir = newDir;
  }
};
