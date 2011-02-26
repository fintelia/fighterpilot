
#include <string>
#include <vector>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;


#include "enums.h"
#include "script.h"

map<string,scriptVar> loadScript(string filename)
{
	map<string,scriptVar> m;

	string line;

	ifstream fin(filename, ios::in);

	if(!fin.is_open())
	{
		debugBreak();//could not open script file
		return m;
	}
	while(!fin.eof())
	{
		getline(fin,line);
		line=line.substr(0,line.find_first_of("'#"));

		char_separator<char> sep(" \t", "=");
		tokenizer<char_separator<char> > Tokenizer(line, sep);
		auto tok_iter = Tokenizer.begin();

		if(tok_iter != Tokenizer.end()) 	
			continue;

		string name = (*tok_iter);

		if(++tok_iter !=Tokenizer.end() || (*tok_iter) != "=" || ++tok_iter !=Tokenizer.end()) 
			continue;

		m[name] = *tok_iter;
	}
	fin.close();
	return m;
}