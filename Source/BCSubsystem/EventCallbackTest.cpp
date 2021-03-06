// Fill out your copyright notice in the Description page of Project Settings.

#include "BCSubsystem.h"
#include "EventCallbackTest.h"
#include "BrainCloudWrapper.h"
#include "ServiceName.h"
#include "ServiceOperation.h"

// Sets default values
AEventCallbackTest::AEventCallbackTest()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    m_wrapper = NewObject<UBrainCloudWrapper>();
}

// Called when the game starts or when spawned
void AEventCallbackTest::BeginPlay()
{
    Super::BeginPlay();

    m_wrapper->initialize(
        "https://internal.braincloudservers.com/dispatcherv2",
        "91c3a097-4697-4787-ba1c-ff6e737ff8b3",
        "10299",
        "1.0.0");

    BrainCloudClient *bcClient = m_wrapper->getBCClient();
    bcClient->enableLogging(true);
    bcClient->getAuthenticationService()->authenticateUniversal("UnrealUser", "UnrealUser", true, this);
    bcClient->registerEventCallback(this);
    bcClient->registerRewardCallback(this);
}

// Called every frame
void AEventCallbackTest::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    m_wrapper->runCallbacks();
}

void AEventCallbackTest::eventCallback(const FString &jsonData)
{
    UE_LOG(LogTemp, Display, TEXT("Event return: %s"), *jsonData);
}

void AEventCallbackTest::rewardCallback(const FString &jsonData)
{
    UE_LOG(LogTemp, Display, TEXT("Reward return: %s"), *jsonData);
}

void AEventCallbackTest::serverCallback(ServiceName serviceName, ServiceOperation serviceOperation, const FString &jsonData)
{
    if (serviceName == ServiceName::AuthenticateV2)
    {
        m_wrapper->getEventService()->sendEvent(
            m_wrapper->getBCClient()->getAuthenticationService()->getProfileId(),
            "test",
            "{ \"RoundsWon\" : 0 }",
            nullptr);
        m_wrapper->getPlayerStatisticsEventService()->triggerStatsEvent("test", 1, nullptr);
    }
}

void AEventCallbackTest::serverError(ServiceName serviceName, ServiceOperation serviceOperation, int32 statusCode, int32 reasonCode, const FString &jsonError)
{
}
