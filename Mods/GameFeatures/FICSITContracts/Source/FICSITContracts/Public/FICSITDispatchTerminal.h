#pragma once
#include "CoreMinimal.h"
#include "Buildables/FGBuildable.h"
#include "FICSITDispatchTerminal.generated.h"
class UFGColoredInstanceMeshProxy;
class UBoxComponent;
class UPointLightComponent;
class UStaticMeshComponent;
class AFGCharacterPlayer;

UCLASS()
class FICSITCONTRACTS_API AFICSITDispatchTerminal : public AFGBuildable
{
    GENERATED_BODY()
public:
    AFICSITDispatchTerminal(const FObjectInitializer& ObjectInitializer);
    virtual bool IsUseable_Implementation() const override;
    virtual void OnUse_Implementation(AFGCharacterPlayer* ByCharacter, const FUseState& State) override;
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FICSIT Dispatch Terminal")
    TObjectPtr<UBoxComponent> SolidCollision;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FICSIT Dispatch Terminal")
    TObjectPtr<UBoxComponent> InteractionVolume;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FICSIT Dispatch Terminal")
    TObjectPtr<UFGColoredInstanceMeshProxy> ImportedTerminalMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FICSIT Dispatch Terminal")
    TObjectPtr<UStaticMeshComponent> FallbackBase;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FICSIT Dispatch Terminal")
    TObjectPtr<UStaticMeshComponent> FallbackPedestal;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FICSIT Dispatch Terminal")
    TObjectPtr<UStaticMeshComponent> FallbackDesk;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FICSIT Dispatch Terminal")
    TObjectPtr<UStaticMeshComponent> FallbackMonitor;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FICSIT Dispatch Terminal")
    TObjectPtr<UStaticMeshComponent> FallbackScreen;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FICSIT Dispatch Terminal")
    TObjectPtr<UStaticMeshComponent> FallbackStatusBar;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FICSIT Dispatch Terminal")
    TObjectPtr<UStaticMeshComponent> FallbackCanopy;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FICSIT Dispatch Terminal")
    TObjectPtr<UStaticMeshComponent> FallbackLeftModule;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FICSIT Dispatch Terminal")
    TObjectPtr<UStaticMeshComponent> FallbackRightModule;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FICSIT Dispatch Terminal")
    TObjectPtr<UStaticMeshComponent> FallbackFrontGrille;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FICSIT Dispatch Terminal")
    TObjectPtr<UStaticMeshComponent> FallbackBeacon;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FICSIT Dispatch Terminal")
    TObjectPtr<UPointLightComponent> ScreenLight;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FICSIT Dispatch Terminal")
    TObjectPtr<UPointLightComponent> BeaconLight;
private:
    void ConfigureFallbackMesh(UStaticMeshComponent* Component, UStaticMesh* Cube,
        const FVector& Location, const FVector& Scale, const FRotator& Rotation = FRotator::ZeroRotator);
};