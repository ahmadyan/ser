#include <boost/algorithm/string/predicate.hpp>
#include <algorithm>
#include "library.h"
#include "debug.h"

// Read timing library and extract timing information
void library::read_library(string library_file){
	cout << "reading library " << library_file << endl ;
	double initial_time = clock();
	time_t t1,t2,t3;
	double final_time;
	double total_time;
	initial_time = clock();
	time(&t1);

	lib = liberty::parse(library_file);

	final_time = clock();
	total_time = (double) (final_time - initial_time) / (double) CLOCKS_PER_SEC ;
	time(&t2);
	t3 = t2 - t1;
	if (t3>1000) total_time = t3;
	cout << "finished reading library " << library_file << ". time required=" << total_time << " seconds."<< endl ;
}

liberty::Library library::getLib(){
	return lib;
}

void library::analyze(){
	cout << "Analyzing library ... " << endl ;

	std::vector<liberty::Cell> cell = lib.getCells();
	cout << "Total Cell in library = " << cell.size() << endl ;
	for (int i=0;i<cell.size();i++)
	{
		cout << cell[i].getName() << endl ;
		std::vector<liberty::Pin> pins = cell[i].getPins();
		for(int j=0;j<pins.size();j++){
			cout << pins[j].getName() << " " << pins[j].getDirection() << endl ;
			try
			{
				std::vector<liberty::Timing> timing = pins[j].getTiming();
				utils::Table tableRise = timing[0].getCellRiseTable();
				utils::Table tableFall = timing[0].getCellFallTable();
				//cout << tableRise << endl ;
				//cout << "---------------" << endl ;
				//cout << tableFall << endl ;
			}
			catch (...)
			{
				cout << "Pin has to timing information" << endl ;
			}
		}

		//cout << "--------------" << endl;
	}
	return;
}

// getTimingforCell
// Returns a duo rise & fall time, usually it's based on nano seconds, 
// however it may differs in standard cell library.
timingData library::getTimingForCell(string c, int numberOfInput, double outputCapacitance)
{
	timingData data;
	string cellName = c;
	data.rise=0; 
	data.fall=0;
	//cout << "cell name=" << cellName << endl ;

#ifdef NAN45
	if(cellName=="not"){ cellName="inv";}
#endif

	std::vector<liberty::Cell> cell = lib.getCells();
	for (int i=0;i<cell.size();i++)
	{
		//cout << cell[i].getName() << endl ;
		string name = cell[i].getName();
		transform ( name.begin(), name.end(), name.begin(), ptr_fun(::tolower) );
		if( boost::starts_with(name, cellName) && ( cell[i].getName().find("X1")) != string::npos)
		{
			//cout << cell[i].getName() << endl ;
			if( (cell[i].getInputPinCount() == numberOfInput) || (cellName=="dff"))
			{
				std::vector<liberty::Pin> pins = cell[i].getPins();
				for(int j=0;j<pins.size();j++){
					if (pins[j].getDirection()==output)
					{
						try
						{
							std::vector<liberty::Timing> timing = pins[j].getTiming();
							utils::Table tableRise = timing[0].getCellRiseTable();
							utils::Table tableFall = timing[0].getCellFallTable();
							//cout << tableRise << endl ;
							//cout << "---------------" << endl ;
							//cout << tableFall << endl ;
							//0.0004 0.0008 0.0016 0.0032 0.0064 0.0128 0.0256
							/*
							std::vector<double> cap = tableRise._index2; 
							int c=0;
							do { 
							c++;
							} while (c<cap.size() && cap[c]<outputCapacitance);
							data.rise= tableRise.getValue(0,c);
							cap = tableFall._index2; 
							c=0;
							do { 
							c++;
							} while (c<cap.size() && cap[c]<outputCapacitance);
							*/
							data.rise= 1000* tableRise.getValue((tableRise._index1)[0],outputCapacitance);
							data.fall= 1000* tableFall.getValue((tableFall._index1)[0],outputCapacitance);

							return data;
							//cout << tableRise._numIndices << endl ;
							//std::vector<double> idx1 = tableRise._values ;
							//for(int c=0;c<idx1.size();c++) cout << idx1[c] << " " ;
							//cout << endl ;
							//cout << tableRise.getValue(2,3) << endl ;
						}
						catch (...)
						{
							cout << "Warning! Pin " << pins[j].getName() << " in cell " << cell[i].getName() <<" has no timing information." << endl ;
							return data;
						}
					}
				}
			}
		}
	}
	return data;
}

// getCapacitanceForCell
float library::getCapacitanceForCell(string c, int numberOfInput)
{
	string cellName = c ;

#ifdef NAN45
	if(cellName=="not"){ cellName="inv";}
#endif

	//cout << "cell name=" << cellName << endl ;
	std::vector<liberty::Cell> cell = lib.getCells();
	for (int i=0;i<cell.size();i++)
	{
		//cout << cell[i].getName() << endl ;
		string name = cell[i].getName();
		transform ( name.begin(), name.end(), name.begin(), ptr_fun(::tolower) );
		if( boost::starts_with(name, cellName) && ( cell[i].getName().find("X1")) != string::npos)
		{
			//cout << cell[i].getName() << endl ;
			if( cell[i].getInputPinCount() == numberOfInput )
			{
				std::vector<liberty::Pin> pins = cell[i].getPins();
				for(int j=0;j<pins.size();j++){
					if (pins[j].getDirection()==input)
					{
						return (double)( pins[j].getCapacitance() );
					}
				}
			}
		}
	}
	return 0.001; //1pf
}

