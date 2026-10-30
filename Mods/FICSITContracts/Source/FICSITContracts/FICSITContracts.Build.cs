using System.IO;
using UnrealBuildTool;

public class FICSITContracts : ModuleRules
{
    public FICSITContracts(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp17;
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "FactoryGame", "SML" });
        PublicIncludePaths.Add(Path.GetFullPath(Path.Combine(
            ModuleDirectory, "..", "..", "..", "..", "Source", "FICSITContractsCore")));
    }
}
