#include "../src/pugixml.hpp"

#include <stdint.h>
#include <string.h>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
	char* text = new char[Size + 1];
	memcpy(text, Data, Size);
	text[Size] = 0;

#ifdef PUGIXML_NO_EXCEPTIONS
	pugi::xpath_query q(text);
#else
	try
	{
		pugi::xpath_query q(text);
	}
	catch (pugi::xpath_exception&)
	{
	}
#endif

	delete[] text;
	return 0;
}
