cmake_minimum_required(VERSION 3.16)
project(FICSITContractsCore VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(FICSITContractsCore
    Source/FICSITContractsCore/ContractEngine.cpp
    Source/FICSITContractsCore/EndgameProgression.cpp
)
target_include_directories(FICSITContractsCore PUBLIC Source/FICSITContractsCore)

# Compile the exact adapter translation unit used by Unreal Build Tool. This
# catches broken relative source includes even without the proprietary UE SDK.
add_library(FICSITContractsPluginCoreCompileCheck OBJECT
    Mods/GameFeatures/FICSITContracts/Source/FICSITContracts/Private/FICSITContractsCore.cpp
)
target_include_directories(FICSITContractsPluginCoreCompileCheck PRIVATE Source/FICSITContractsCore)

if(MSVC)
    target_compile_options(FICSITContractsCore PRIVATE /W4)
else()
    target_compile_options(FICSITContractsCore PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(FICSITContractsCoreTests Tests/ContractEngineTests.cpp)
target_link_libraries(FICSITContractsCoreTests PRIVATE FICSITContractsCore)

if(MSVC)
    target_compile_options(FICSITContractsCoreTests PRIVATE /W4)
else()
    target_compile_options(FICSITContractsCoreTests PRIVATE -Wall -Wextra -Wpedantic)
endif()

enable_testing()
add_test(NAME FICSITContractsCoreTests COMMAND FICSITContractsCoreTests)
