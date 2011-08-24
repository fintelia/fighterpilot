
#include "png\png.h"
#include <string>

using namespace std;

#include "pngHelper.h"



bool pngTextField(png_text* textField, const char* key, const char* text)
{
	textField->compression = PNG_TEXT_COMPRESSION_NONE;
	textField->key = (png_charp)key;
	textField->text_length = strlen(text);
	textField->text = (png_charp)text;

	textField->itxt_length = 0;
	textField->lang = 0;
	textField->lang_key = 0;

	return true;
}

bool pngUnknownChunk(png_unknown_chunk &chunk, string name, void* data, unsigned int size)
{
	if(name.size() < 4)
		return false;

	chunk.name[0] = name[0];
	chunk.name[1] = name[1];
	chunk.name[2] = name[2];
	chunk.name[3] = name[3];
	chunk.name[4] = 0;

	chunk.data = (png_byte*)data;
	chunk.size = size;

	chunk.location = 0;//unused

	return true;
}