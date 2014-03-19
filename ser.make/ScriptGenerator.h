#pragma once

#include <string>
class ScriptGenerator
{
	string name ;
	int count ;
	string test ;
	int time ;
public:
	ScriptGenerator(void);
	ScriptGenerator(string _name, int _t);
	~ScriptGenerator(void);

	void generateModelSimScript();
	void generateMLETScript();
	string generateFileName(int,int);

};
