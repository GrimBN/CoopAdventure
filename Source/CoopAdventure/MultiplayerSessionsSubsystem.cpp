// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "Online/OnlineSessionNames.h"

void PrintString(const FString& StringToPrint)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, StringToPrint);
	}
}

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
{
	//PrintString("MSS Constructor");

	CreateServerAfterDestroy = false;
	DestroyedServerName = "";
	ServerNameToSearch = "";
}

void UMultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	//Super::Initialize(Collection);
	//PrintString("MSS Initialize");
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();

	if (OnlineSubsystem)
	{
		PrintString(OnlineSubsystem->GetSubsystemName().ToString());

		SessionInterface = OnlineSubsystem->GetSessionInterface();

		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMultiplayerSessionsSubsystem::OnFindSessionComplete);
		}
	}
}

void UMultiplayerSessionsSubsystem::Deinitialize()
{
	//Super::Deinitialize();
	//UE_LOG(LogTemp, Warning, TEXT("MSS Deinitialize"));
}

void UMultiplayerSessionsSubsystem::CreateServer(FString ServerName)
{
	PrintString("Create Server: " + ServerName);

	if (ServerName.IsEmpty())
	{
		PrintString("Server name cannot be empty!");
		return;
	}

	FName SessionName = FName("Co-op Adventure Session Name");

	FNamedOnlineSession *ExistingSession = SessionInterface->GetNamedSession(SessionName);

	if (ExistingSession)
	{
		PrintString(FString::Printf(TEXT("Session with name %s already exists, destroying..."), *SessionName.ToString()));
		CreateServerAfterDestroy = true;
		DestroyedServerName = ServerName;
		SessionInterface->DestroySession(SessionName);
		return;
	}

	FOnlineSessionSettings SessionSettings;

	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.NumPublicConnections = 2;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bAllowJoinViaPresence = true;
	bool IsLan = false;

	if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
	{
		IsLan = true;
	}
	SessionSettings.bIsLANMatch = IsLan;

	SessionSettings.Set(FName("SERVER_NAME"), ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	SessionInterface->CreateSession(0, SessionName, SessionSettings);

}

void UMultiplayerSessionsSubsystem::FindServer(FString ServerName)
{
	PrintString("Find Server: " + ServerName);

	if (ServerName.IsEmpty())
	{
		PrintString("Server name cannot be empty!");
		return;
	}

	SessionSearch = MakeShareable(new FOnlineSessionSearch());

	bool IsLan = false;

	if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
	{
		IsLan = true;
	}

	SessionSearch->bIsLanQuery = IsLan;
	SessionSearch->MaxSearchResults = 9999;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	ServerNameToSearch = ServerName;

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	PrintString(FString::Printf(TEXT("OnCreateSessionComplete: %d"), bWasSuccessful));

	if (bWasSuccessful)
	{
		GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/ThirdPersonMap?listen");
	}
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	
	PrintString(FString::Printf(TEXT("OnDestroySession: %s, Successful: %d"), *SessionName.ToString(), bWasSuccessful));
	

	if (CreateServerAfterDestroy)
	{
		CreateServerAfterDestroy = false;
		CreateServer(DestroyedServerName);
	}
}

void UMultiplayerSessionsSubsystem::OnFindSessionComplete(bool bWasSuccessful)
{
	if (!bWasSuccessful) return;
	if (ServerNameToSearch.IsEmpty()) return;

	TArray<FOnlineSessionSearchResult> Results = SessionSearch->SearchResults;

	if (Results.Num() > 0)
	{
		FString Msg = FString::Printf(TEXT("%d sessions found."), Results.Num());
		PrintString(Msg);

		for (FOnlineSessionSearchResult Result : Results)
		{
			if (Result.IsValid())
			{
				FString ServerName = "No-Name";
				Result.Session.SessionSettings.Get(FName("SERVER_NAME"), ServerName);

				FString Msg2 = FString::Printf(TEXT("Server_Name: %s"), *ServerName);
				PrintString(Msg2);
			}
		}

	}
	else
	{
		PrintString("Zero Sessions found.");
	}
}
