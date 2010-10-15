
#include <string>
#include <vector>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

#include "script.h"





struct var
{
	enum varType{
		VOID	=0x0,
		FLOAT	=0x1,
		INT		=0x2,
		STRING	=0x4,
		VECTOR	=0x8
	};

	string name;
	varType type;
	void* ptr;
	var(): name(""), type(VOID), ptr(NULL) {}
	var(string n, varType t, void* p): name(n), type(t), ptr(p) {}
};

struct token
{
	enum tokenType{
		NONE,
		VAR_NAME,
		EQUALS,
		VAR_VALUE
	};
	tokenType type;
	string myString;
	token(): type(NONE), myString("") {}
	token(tokenType t, string s): type(t), myString(s) {}
};
void loadScript(string filename, vector<scriptVar> vars)
{
	string line;
	vector<token> tokens;

	ifstream fin(filename, ios::in);

	if(!fin.is_open())
	{
		__debugbreak();//could not open script file
		exit(0);
	}
	while(!fin.eof())
	{
		getline(fin,line);
		line=line.substr(0,line.find_first_of("'#"));


		char_separator<char> sep(" ", "=");
		tokenizer<char_separator<char> > Tokenizer(line, sep);
		token::tokenType lastType=token::NONE;
		for (tokenizer<char_separator<char> >::iterator tok_iter = Tokenizer.begin(); tok_iter != Tokenizer.end(); ++tok_iter)
		{
			if((*tok_iter).compare("=")==0)
			{
				tokens.push_back(token(token::EQUALS,*tok_iter));
				lastType=token::EQUALS;
			}
			else if(lastType==token::EQUALS)
			{
				tokens.push_back(token(token::VAR_VALUE,*tok_iter));
				lastType=token::VAR_VALUE;
			}
			else
			{
				for(vector<scriptVar>::iterator i = vars.begin();i!=vars.end();i++)
				{
					if((*tok_iter).compare((*i).getName())==0)
					{
						tokens.push_back(token(token::VAR_NAME,*tok_iter));
						lastType=token::VAR_NAME;
					}
				}
			}
		}

		if(tokens.size()==3 && tokens[0].type==token::VAR_NAME && tokens[1].type==token::EQUALS && tokens[2].type==token::VAR_VALUE)
		try{
			for(vector<scriptVar>::iterator i = vars.begin();i!=vars.end();i++)
			{
				if((tokens[0].myString).compare((*i).getName())==0)
					(*i).set(tokens[2].myString);
			}
		}catch(bad_lexical_cast &){
			__debugbreak();//boost type conversion failed
			exit(0);
		}
	}
	fin.close();
}