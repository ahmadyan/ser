// ****************************************************************************
// ****************************************************************************
// liberty.h
//
// API header for Liberty parser.
//
// *****************************************************************************

#ifndef __LIBERTY_H__
#define __LIBERTY_H__

#include <vector>
#include <string>
#include <map>
#include <stdexcept>

#include "utils.h"
#include "parserFunctions.h"

enum PinDirection {input, output};

/// namespace that contains are parsing functions and datastructures
namespace liberty {

// ****************************************************************************
// not_found
/// Exception thrown when some data is not available
// ****************************************************************************
class not_found : public std::logic_error {
public:
  not_found(const std::string &msg = "") : logic_error(msg) {}
};


// ****************************************************************************
// Leakage
/// Storage class for leakage information
// ****************************************************************************
class Leakage {

};



// ****************************************************************************
// Timing
/// Storage class for timing information of a timing arc
// ****************************************************************************
class Timing {
  bool                    _cellRiseExists, _riseTransitionExists;
  bool                    _cellFallExists, _fallTransitionExists;
  utils::Table            _cellRise;
  utils::Table            _riseTransition;
  utils::Table            _cellFall;
  utils::Table            _fallTransition;
public:
                          Timing();
  void                    clear();

  bool                    setCellRise(const utils::Table &t);
  bool                    setRiseTransition(const utils::Table &t);
  bool                    setCellFall(const utils::Table &t);
  bool                    setFallTransition(const utils::Table &t);

/// Returns the table for the rise time (i.e., cell_rise)
/** Exception not_found is thrown if data is not available.
*/
  utils::Table            getCellRiseTable() const
    throw (not_found);

/// Returns the table for the rise slew (i.e., rise_tran)
/** Exception not_found is thrown if data is not available.
*/
  utils::Table            getRiseTransitionTable() const
    throw (not_found);

/// Returns the table for the fall time (i.e., cell_fall)
/** Exception not_found is thrown if data is not available.
*/
  utils::Table            getCellFallTable() const
    throw (not_found);

/// Returns the table for the fall slew (i.e., fall_tran)
/** Exception not_found is thrown if data is not available.
*/
  utils::Table            getFallTransitionTable() const
    throw (not_found);
};


// ****************************************************************************
// Pin
/// Storage class for data of a pin
// ****************************************************************************
class Pin {
  bool                    _nameExists, _capacitanceExists, _directionExists;
  std::string             _name;
  PinDirection            _direction;
  float                   _capacitance;
  std::vector<Timing>     _timing;
public:
                          Pin();
  void                    clear();
  bool                    check() const;
  bool                    setName(const std::string &s);
  bool                    setCapacitance(const float &c);
  bool                    addTiming(const Timing &t);
  void					  setDirection(const std::string &s);
/// Returns the name of the pin
  std::string             getName() const { return _name; }

/// Returns the direction of the pin
  PinDirection            getDirection() const { return _direction; }

/// Returns the input capacitance of the pin
/** Exception not_found is thrown if data is not available.
*/
  float                   getCapacitance() const
    throw (not_found);

/// \brief Returns a vector of Timing objects that store the timing information
/// for all arcs associated with this pin.
/** Exception not_found is thrown if data is not available.
*/
  std::vector<Timing>     getTiming() const
    throw (not_found);
};


// ****************************************************************************
// Cell
/// Storage class for data of a cell
// ****************************************************************************
class Cell {
  bool                    _nameExists, _footprintExists, _areaExists;
  std::string             _name;
  std::string             _footprint;
  float                   _area;
  std::vector<Pin>        _pins;

public:
                          Cell();
  bool                    setName(const std::string &s);
  bool                    setFootprint(const std::string &s);
  bool                    setArea(const float &f);
  bool                    addPin(const Pin &p);
  void                    clear();
  int					  getInputPinCount();
/// Returns the name of the cell
  std::string             getName() const { return _name; }

/// Returns the footprint of the cell
  std::string             getFootprint() const { return _footprint; }

/// Returns the area of the cell (in library units)
  float                   getArea() const { return _area; }

/// Returns a vector of pins of the cell
/** Also see Cell::getPin(const std::string &pinName) const
*/
  std::vector<Pin>        getPins() const { return _pins; }

/// Searches and returns a pin
/** Exception not_found is thrown if data is not available.
* Also see Cell::getPins() const
*/
  Pin                     getPin(const std::string &pinName) const
    throw (not_found);

  bool                    check() const;
};

// ****************************************************************************
// Library
/// Top level library class that stores all data.
// ****************************************************************************
class Library {
  std::map<std::string, Cell> _cells;
public:
  bool                    addCell(const Cell &c);
  void                    clear();

/// Searches and returns a cell
/** Exception not_found is thrown if data is not available.
* Also see Library::getCells() const
*/
  Cell                    getCell(const std::string &cellName) const
    throw (not_found);

/// Returns all cells in a vector
/** Also see Library::getCell(const std::string &cellName) const
*/
  std::vector<Cell>       getCells() const;

/// Returns the names of all cells
  std::vector<std::string> getCellNames() const;
};

/// Function to parse a Liberty file
Library parse(std::string filename);

} // namespace liberty

#endif
