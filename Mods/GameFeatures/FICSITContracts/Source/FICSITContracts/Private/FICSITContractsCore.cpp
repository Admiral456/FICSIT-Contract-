// Compile the portable domain engines into the SML runtime module.
// Unreal Build Tool only discovers translation units inside this module's Source tree;
// CMake separately builds the same originals for platform-independent tests.
#include "../../../../../../Source/FICSITContractsCore/ContractEngine.cpp"
#include "../../../../../../Source/FICSITContractsCore/EndgameProgression.cpp"
