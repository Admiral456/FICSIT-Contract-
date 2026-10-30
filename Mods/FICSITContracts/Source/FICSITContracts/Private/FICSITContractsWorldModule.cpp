#include "FICSITContractsWorldModule.h"
#include "FICSITContractsChatCommand.h"
#include "FICSITContractsSubsystem.h"

UFICSITContractsWorldModule::UFICSITContractsWorldModule()
{
    bRootModule = true;
    ModSubsystems.Add(AFICSITContractsSubsystem::StaticClass());
    mChatCommands.Add(AFICSITContractsChatCommand::StaticClass());
}
