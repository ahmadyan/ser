#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include "utils.h"

using namespace std;

namespace utils {

	double initial_time = clock();
	time_t t1=0,t2=0,t3=0;
	double final_time = 0;
	double total_time = 0;


Table::Table() 
{
  clear();
}

void Table::clear()
{
  _numIndices = 0;
  _index1.clear();
  _index2.clear();
  _index3.clear();
  _values.clear();
}

bool Table::setIndex1(const vector<double> &v)
{
  if (_numIndices < 1) {
    _numIndices = 1;
  }
  if (_index1.size() != 0) {
    cerr << "Error: Index 1 already specified\n";
    return false;
  }
  _index1 = v;
  return true;
}

bool Table::setIndex2(const vector<double> &v)
{
  if (_numIndices < 2) {
    _numIndices = 2;
  }
  if (_index2.size() != 0) {
    cerr << "Error: Index 2 already specified\n";
    return false;
  }
  _index2 = v;
  return true;
}

bool Table::setIndex3(const vector<double> &v)
{
  if (_numIndices < 3) {
    _numIndices = 3;
  }
  if (_index3.size() != 0) {
    cerr << "Error: Index 3 already specified\n";
    return false;
  }
  _index3 = v;
  return true;
}

bool Table::setValues(const vector<double> &v)
{
  if (_values.size() != 0) {
    cerr << "Error: Values already specified\n";
    return false;
  }
  _values = v;
  return true;
}

bool Table::check()
{
  if (_numIndices == 0) {
    if (_values.size() != 1) {
      cerr << "Error: Incorrect number of values\n";
      return false;
    }
  } else if (_numIndices == 1) {
    if (_index1.size() == 0) {
      cerr << "Error: Index 1 not supplied\n";
      return false;
    }
    if (_values.size() != _index1.size()) {
      cerr << "Error: Incorrect number of values\n";
      return false;
    }
  } else if (_numIndices == 2) {
    if (_index1.size()==0 || _index2.size()==0) {
      cerr << "Error: All indices not supplied\n";
      return false;
    }
    if (_values.size() != _index1.size()*_index2.size()) {
      cerr << "Error: Incorrect number of values\n";
      return false;
    }
  } else if (_numIndices == 3) {
    if (_index1.size()==0 || _index2.size()==0 || _index3.size()==0) {
      cerr << "Error: All indices not supplied\n";
      return false;
    }
    if (_values.size() != _index1.size()*_index2.size()*_index3.size()) {
      cerr << "Error: Incorrect number of values\n";
      return false;
    }
  }
  return true;
}


double Table::getValue(const double &index1, const double &index2) const
{
  if (_numIndices != 2) {
    cerr << "Error: getValue called with two arguments but number of "
         << "indices was " << _numIndices << ".\n";
    //exit(1);
  }

  if (_index1.size() <= 1 || _index2.size() <= 1) {
    cerr << "Error: Too few indices\n";
    //exit(1);
  }

  unsigned i;
  bool found ;

  unsigned index1LB = 0;
  found = false;
  for (i = 0; i<_index1.size()-1; i++) {
    if ( (index1 >= _index1[i]) && (index1 <= _index1[i+1]) ) {
      found = true;
      break;
    }
  }
  if (found) {
    index1LB = i;
  } else if (index1 < _index1[0]) {
    cerr << "Warning: Index1 out of range (too small " << index1 << " " <<
      _index1[0] << ")\n";
    index1LB = 0;
  } else if (index1 > _index1[_index1.size()-1]) {
    cerr << "Warning: Index1 out of range (too large) " << index1 << " " <<
      _index1[_index1.size()-1] << ")\n";
    index1LB = _index1.size()-2;
  }

  unsigned index2LB = 0;
  found = false;
  for (i = 0; i<_index2.size()-1; i++) {
    if ( (index2 >= _index2[i]) && (index2 <= _index2[i+1]) ) {
      found = true;
      break;
    }
  }
  if (found) {
    index2LB = i;
  } else if (index2 < _index2[0]) {
    cerr << "Warning: Index2 out of range (too small " << index2 << " " <<
      _index2[0] << ")\n";    
    index2LB = 0;
  } else if (index2 > _index2[_index2.size()-1]) {
    cerr << "Warning: Index2 out of range (too large) " << index2 << " " <<
      _index2[_index2.size()-1] << ")\n";    
    index2LB = _index2.size()-2;
  }

  unsigned index1UB = index1LB + 1;
  unsigned index2UB = index2LB + 1;
  //cerr << "===============================\n";
  //cerr << _index1[index1LB] << "\t" << _index1[index1UB] << "\n";
  //cerr << _index2[index2LB] << "\t" << _index2[index2UB] << "\n";
  //cerr << "-------------------------------\n";
  //cerr << _values[_index2.size()*index1LB + index2LB] << "\t"
  //<< _values[_index2.size()*index1LB + index2UB] << "\n";
  //cerr << _values[_index2.size()*index1UB + index2LB] << "\t"
  //<< _values[_index2.size()*index1UB + index2UB] << "\n";
         

  double v_11 = _values[_index2.size()*index1LB+index2LB];
  double v_12 = _values[_index2.size()*index1LB+index2UB];
  double v_21 = _values[_index2.size()*index1UB+index2LB];
  double v_22 = _values[_index2.size()*index1UB+index2UB];  

  double v_1 = (v_11 * (_index2[index2UB]-index2) + v_12 * (index2-_index2[index2LB]))/(_index2[index2UB] - _index2[index2LB]);

  double v_2 = (v_21 * (_index2[index2UB]-index2) + v_22 * (index2-_index2[index2LB]))/(_index2[index2UB] - _index2[index2LB]);

  double v = (v_1 * (_index1[index1UB]-index1) + v_2 * (index1-_index1[index1LB]))/(_index1[index1UB] - _index1[index1LB]);
  //cerr << "Index1: " << index1 << endl;
  //cerr << "Index2: " << index2 << endl;
  //cerr << "Found value: " << v << endl;
  //cerr << "===============================\n";
  return v;
}


void Table::getTableValues(const double &index1, const double &index2,
                             double &index11, double &index12,
                             double &index21, double &index22,
                             double &v11, double &v12, double &v21, double &v22
                             )
{
  if (_numIndices != 2) {
    cerr << "Error: getTableValues called with two arguments but number of "
         << "indices was " << _numIndices << ".\n";
    exit(1);
  }

  if (_index1.size() <= 1 || _index2.size() <= 1) {
    cerr << "Error: Too few indices\n";
    exit(1);
  }

  unsigned i;
  bool found ;

  unsigned index1LB = 0;
  found = false;
  for (i = 0; i<_index1.size()-1; i++) {
    if ( (index1 >= _index1[i]) && (index1 <= _index1[i+1]) ) {
      found = true;
      break;
    }
  }
  if (found) {
    index1LB = i;
  } else if (index1 < _index1[0]) {
    cerr << "Warning: Index1 out of range (too small " << index1 << " " <<
      _index1[0] << ")\n";
    index1LB = 0;
  } else if (index1 > _index1[_index1.size()-1]) {
    cerr << "Warning: Index1 out of range (too large) " << index1 << " " <<
      _index1[_index1.size()-1] << ")\n";
    index1LB = _index1.size()-2;
  }

  unsigned index2LB = 0;
  found = false;
  for (i = 0; i<_index2.size()-1; i++) {
    if ( (index2 >= _index2[i]) && (index2 <= _index2[i+1]) ) {
      found = true;
      break;
    }
  }
  if (found) {
    index2LB = i;
  } else if (index2 < _index2[0]) {
    cerr << "Warning: Index2 out of range (too small " << index2 << " " <<
      _index2[0] << ")\n";    
    index2LB = 0;
  } else if (index2 > _index2[_index2.size()-1]) {
    cerr << "Warning: Index2 out of range (too large) " << index2 << " " <<
      _index2[_index2.size()-1] << ")\n";    
    index2LB = _index2.size()-2;
  }

  unsigned index1UB = index1LB + 1;
  unsigned index2UB = index2LB + 1;
  index11 = _index1[index1LB];
  index12 = _index1[index1UB];
  index21 = _index2[index2LB];
  index22 = _index2[index2UB];
  v11 = _values[_index2.size()*index1LB + index2LB];
  v12 = _values[_index2.size()*index1LB + index2UB];
  v21 = _values[_index2.size()*index1UB + index2LB];
  v22 = _values[_index2.size()*index1UB + index2UB];
}


ostream& operator << (ostream &os, const Table &t)
{
  if (t._numIndices == 0) {
    os << t._values[0];
  } else if (t._numIndices == 1) { 
    os << "index_1:";
    for (unsigned i=0; i<t._index1.size(); i++) {
      os << " " << t._index1[i];
    }
    os << "\n";
    for (unsigned i=0; i<t._values.size(); i++) {
      os << " " << t._values[i];

    }
  } else if (t._numIndices == 2) {
    os << "index_1:";
    for (unsigned i=0; i<t._index1.size(); i++) {
      os << " " << t._index1[i];
    }
    os << "\n";
    os << "index_2:";
    for (unsigned i=0; i<t._index2.size(); i++) {
      os << " " << t._index2[i];
    }
    os << "\n";
    for (unsigned i=0; i<t._index1.size(); i++) {
      for (unsigned j=0; j<t._index2.size(); j++) {
        os << " " << t._values[i*t._index2.size() + j];
      }
      os << "\n";
    }
  } else {
    cerr << "Error: Printing for numindices " << t._numIndices 
         << " not supported\n";
    exit(1);
  } 
  return os; 
}

/*
	old version was based on str_tok, the new shiny version is based on boost::tokenizer
	todo: write a cast class to split into strings, integer, etc.
*/
vector<double> splitMultipleValues(const string &s)
{
  vector<double> ret;
  typedef boost::tokenizer<boost::char_separator<char> > 
    tokenizer;
  boost::char_separator<char> sep(" \",");
  tokenizer tokens(s, sep);
  for (tokenizer::iterator tok_iter = tokens.begin();
       tok_iter != tokens.end(); ++tok_iter)
    try {
      ret.push_back(boost::lexical_cast<double>(*tok_iter));
    } catch (boost::bad_lexical_cast &e) {
      cerr << "Error: Could not convert " << s << " to vector of doubles\n";
      exit(1);
    }
  return ret;
}


size_t getline_replace(char **buf, size_t *len, FILE *f){
	char *fgets_rc;

	if(!*buf){
		*buf = (char*) malloc(32768);
		*len = 32767;
	}

	(*buf)[0] = 0;
	fgets_rc = fgets(*buf, 32767, f);
	if((!(*buf)[0])||(!fgets_rc)){
		return(-1);
	}else{
		return(1);
	}
}

/*this is a hash function I used on vcd ids
	** Can I use PI's node number instead of hashing? 
		problem: if so, I should take into account the other wires and internal signals. 
*/
unsigned int vcdid_hash(char *s){
	unsigned int val=0;
	int i;
	int len = strlen(s);
	s+=(len-1);
	for(i=0;i<len;i++){
		val *= 94;                         /* was 94 but XL uses '!' as right hand side chars which act as leading zeros */
		val += (((unsigned char)*s) - 32);
		s--;
	}
	return(val);
}  


/*
	the tick & tock functions are used toghether (usually in main() ) to 
	calculate the time used by a specific function. 
	they should be used like this:
	utils::tick();
		f1();
		f2();
		f3();
	utils::tock("name of process/function", result_file_pointer);
*/
void tick(){
	initial_time = clock();
	time(&t1);
}

void tock(std::string processName, FILE* resultfp){
	final_time = clock();
	total_time = (double) (final_time - initial_time) / (double) CLOCKS_PER_SEC ;
	time(&t2);
	t3 = t2 - t1;
	if (t3>1000) total_time = t3;
	cout << endl << "[time-report] Time required to " << processName << " is: " << total_time << " seconds"<<endl;
	if(resultfp!=NULL) fprintf(resultfp, "\n[time-report] Time required to compute %s is: %f seconds.\n", processName.c_str(), total_time);
}

} // namespace utils

