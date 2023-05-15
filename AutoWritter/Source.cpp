#include "Autowritter.h"

auto main() -> int
{
	AutoWritterSetup setup;
	setup.sleepBetweenTypes = 500;

	AutoWritter writer(setup);
	writer.RunAutoclicker();

}