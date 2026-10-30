#include "FICSITContractsGameInstanceModule.h"
#include "FICSITContractsRemoteCallObject.h"

UFICSITContractsGameInstanceModule::UFICSITContractsGameInstanceModule()
{
    bRootModule = true;
    RemoteCallObjects.Add(UFICSITContractsRemoteCallObject::StaticClass());
}
