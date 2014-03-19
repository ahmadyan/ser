#include <iostream>
#include <fstream>
#include <cstdlib>
#include "liberty.h"

using namespace std;

liberty::Library __y_library;

extern int __line_no;
string __file_name;
int __err_counter;
extern FILE *yyin;

extern "C"
{
	int yyparse();
	int yywrap(void) {
		return 1;
	}
	int yyerror(char *str) {
		if(__err_counter==0) {
			__err_counter++;
			cout << __file_name << ":" << __line_no << ": " <<  str << endl;
		}
		return 0;
	}
}


namespace liberty {


	Library parse(string filename)
	{
		__y_library.clear();
		__line_no = 1;
		__err_counter = 0;
		__file_name = filename;
		yyin = fopen(__file_name.c_str(), "r");
		if (yyin == NULL) {
			cerr << "Error: Could not open file\n";
			exit(1);
		}
		if (yyparse()!=0 || __err_counter>0) {
			cerr << "Error: Could not parse file\n";
			exit(1);
		}
		fclose(yyin);

		Library l = __y_library;
		return l;
	}


	bool Library::addCell(const Cell &c)
	{
		std::cout << "CELL " << c.getName() << " Added." << endl ;
		if (!c.check()) {
			return false;
		}
		string cellName = c.getName();
		if (_cells.find(cellName) != _cells.end()) {
			cerr << "Error: Cell " << cellName << " already read.\n";
			return false;
		}
		_cells[cellName] = c;
		return true;
	}

	void Library::clear()
	{
		_cells.clear();
	}

	Cell::Cell()
	{
		clear();
	}

	void Cell::clear()
	{
		_nameExists = false;
		_footprintExists = false;
		_areaExists = false;
		_pins.clear();
	}


	bool Cell::check() const
	{
		return true;
	}

	bool Cell::setName(const std::string &s)
	{
		if (_nameExists) {
			cerr << "Error: Name for cell " << s << " alredy set.\n";
			return false;
		}
		_name = s;
		_nameExists = true;
		return true;
	}

	bool Cell::setFootprint(const std::string &s)
	{
		if (_footprintExists) {
			cerr << "Error: Footprint already set.\n";
			return false;
		}
		_footprint = s;
		_footprintExists = true;
		return true;
	}

	bool Cell::setArea(const float &f)
	{
		if (_areaExists) {
			cerr << "Error: Area already set.\n";
			return false;
		}
		_area = f;
		_areaExists = true;
		return true;
	}

	bool Cell::addPin(const Pin &p)
	{
		if (!p.check()) {
			return false;
		}
		for (vector<Pin>::const_iterator i = _pins.begin(); i != _pins.end(); i++) {
			if (i->getName() == p.getName()) {
				return false;
			}
		}
		_pins.push_back(p);
		return true;
	}

	Pin Cell::getPin(const std::string &pinName) const
		throw (not_found)
	{
		for (vector<Pin>::const_iterator i = _pins.begin(); i!=_pins.end(); i++) {
			if (i->getName() == pinName) {
				return *i;
			}
		}
		string s = "Pin " + pinName + " not found";
		throw not_found(s);
	}

	int	Cell::getInputPinCount()
	{
		int total=0;
		std::vector<liberty::Pin> pins = getPins();
		for(int j=0;j<pins.size();j++){
			if (pins[j].getDirection()==input)
			{
				total++;				
			}
		}
		return total;
	}


	std::vector<Cell> Library::getCells() const
	{
		vector<Cell> cells;
		for (map<string, Cell>::const_iterator i = _cells.begin(); i != _cells.end();
			i++) {
				cells.push_back(i->second);
		}
		return cells;
	}


	Cell Library::getCell(const string &cellName) const
		throw (not_found)
	{
		if (_cells.find(cellName) == _cells.end()) {
			string s = "Cell " + cellName + " not found.";
			throw not_found(s);
		}
		return _cells.find(cellName)->second;
	}


	Pin::Pin()
	{
		clear();
	}

	void Pin::clear()
	{
		_nameExists = false;
		_capacitanceExists = false;
		_directionExists = false;
		_timing.clear();
	}

	bool Pin::check() const
	{
		return true;
	}

	void Pin::setDirection(const string &s)
	{
		if (s=="input")
		{
			_direction = input;
		} 
		else if (s=="output")
		{
			_direction = output;
		}
		//_direction inout
		
	}
	bool Pin::setName(const string &s)
	{
		if (_nameExists) {
			cerr << "Error: Pin name already exists.\n";
			return false;
		}
		_name = s;
		_nameExists = true;
		return true;
	}

	bool Pin::setCapacitance(const float &f)
	{
		if (_capacitanceExists) {
			cerr << "Error: Capacitance already exists.\n";
			return false;
		}
		_capacitance = f;
		_capacitanceExists = true;
		return true;
	}

	bool Pin::addTiming(const Timing &t)
	{
		_timing.push_back(t);
		return true;
	}

	float Pin::getCapacitance() const
		throw (not_found)
	{
		if (!_capacitanceExists) {
			throw not_found("");
		}
		return _capacitance;
	}

	vector<Timing> Pin::getTiming() const
		throw (not_found)
	{
		if (_timing.size() == 0) {
			throw not_found("No timing arc found");
		}
		return _timing;
	}

	Timing::Timing()
	{
		clear();
	}

	void Timing::clear()
	{
		_cellRise.clear();
		_riseTransition.clear();
		_cellFall.clear();
		_fallTransition.clear();
		_cellRiseExists = false;
		_riseTransitionExists = false;
		_cellFallExists = false;
		_fallTransitionExists = false;
	}

	bool Timing::setCellRise(const utils::Table &t)
	{
		if (_cellRiseExists) {
			cerr << "Error: cell_rise already exists.\n";
			return false;
		}
		_cellRise = t;
		_cellRiseExists = true;
		return true;
	}  

	bool Timing::setRiseTransition(const utils::Table &t)
	{
		if (_riseTransitionExists) {
			cerr << "Error: rise_transition already exists.\n";
			return false;
		}
		_riseTransition = t;
		_riseTransitionExists = true;
		return true;
	}

	bool Timing::setCellFall(const utils::Table &t)
	{
		if (_cellFallExists) {
			cerr << "Error: cell_fall already exists.\n";
			return false;
		}
		_cellFall = t;
		_cellFallExists = true;
		return true;
	}  

	bool Timing::setFallTransition(const utils::Table &t)
	{
		if (_fallTransitionExists) {
			cerr << "Error: fall_transition already exists.\n";
			return false;
		}
		_fallTransition = t;
		_fallTransitionExists = true;
		return true;
	}

	utils::Table Timing::getCellRiseTable() const
		throw (not_found)
	{
		if (!_cellRiseExists) {
			throw not_found("");
		}
		return _cellRise;
	}

	utils::Table Timing::getRiseTransitionTable() const
		throw (not_found)
	{
		if (!_riseTransitionExists) {
			throw not_found("");
		}
		return _riseTransition;
	}

	utils::Table Timing::getCellFallTable() const
		throw (not_found)
	{
		if (!_cellFallExists) {
			throw not_found("");
		}
		return _cellFall;
	}

	utils::Table Timing::getFallTransitionTable() const
		throw (not_found)
	{
		if (!_fallTransitionExists) {
			throw not_found("");
		}
		return _fallTransition;
	}

} // namespace liberty

