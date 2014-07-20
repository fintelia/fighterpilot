
#include "game.h"

void SettingsManager::load(const map<string,map<string,string>>& m)
{
	for(auto i = m.begin(); i != m.end(); i++)
	{
		categories[i->first].insert(i->second.begin(), i->second.end());
	}
}