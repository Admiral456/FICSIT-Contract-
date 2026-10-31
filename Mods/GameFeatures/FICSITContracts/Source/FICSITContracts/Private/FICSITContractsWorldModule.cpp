#include "FICSITContractsWorldModule.h"
#include "FICSITContractsChatCommand.h"
#include "FICSITContractsSubsystem.h"
#include "FICSITDispatchTerminalContent.h"
#include "FICSITFerraviumContent.h"

UFICSITContractsWorldModule::UFICSITContractsWorldModule()
{
    bRootModule = true;
    ModSubsystems.Add(AFICSITContractsSubsystem::StaticClass());
    mChatCommands.Add(AFICSITContractsChatCommand::StaticClass());
    mSchematics.Add(UFICSITDispatchTerminalSchematic::StaticClass());
    mSchematics.Add(UFICSITFrontierSurveySchematic::StaticClass());
}
