#include "Synergy/Core/Resources.hpp"

#include "Synergy/Core/Exception.hpp"

#include <cstdlib>

using namespace std;

string Synergy::getSynergyDir() {
	char * synergyDir = getenv("SYNERGY_DIR");
	if(synergyDir == nullptr) {
		throw Synergy::Exception(
				"Environment variable SYNERGY_DIR has not been set.\n"
						"User must set this environment variable to the top level directory "
						"containing the Synergy library before attempting to run a program that "
						"is linked with Synergy."
		);
	}
	return string(synergyDir);
}