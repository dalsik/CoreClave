#include "DeckBuilderPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "DeckBuilderSubsystem.h"
#include "DeckBuilderWidget.h"
#include "Kismet/GameplayStatics.h"
#include "InputCoreTypes.h"

ADeckBuilderPlayerController::ADeckBuilderPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void ADeckBuilderPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
	{
		return;
	}

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	bShowMouseCursor = true;

	CachedDeckBuilderSubsystem = GetGameInstance() ? GetGameInstance()->GetSubsystem<UDeckBuilderSubsystem>() : nullptr;

	OpenDeckBuilderWidget();
	LoadDeckFromSlot();
}

void ADeckBuilderPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!InputComponent)
	{
		return;
	}

	InputComponent->BindKey(EKeys::Enter, IE_Pressed, this, &ADeckBuilderPlayerController::HandleStartBattleInput);
	InputComponent->BindKey(EKeys::F5, IE_Pressed, this, &ADeckBuilderPlayerController::HandleSaveInput);
	InputComponent->BindKey(EKeys::F9, IE_Pressed, this, &ADeckBuilderPlayerController::HandleLoadInput);
}

void ADeckBuilderPlayerController::OpenDeckBuilderWidget()
{
	if (!IsLocalController() || !DeckBuilderWidgetClass || DeckBuilderWidget)
	{
		return;
	}

	DeckBuilderWidget = CreateWidget<UDeckBuilderWidget>(this, DeckBuilderWidgetClass);
	if (DeckBuilderWidget)
	{
		DeckBuilderWidget->AddToViewport();
	}
}

UDeckBuilderSubsystem* ADeckBuilderPlayerController::GetDeckBuilderSubsystem() const
{
	if (!CachedDeckBuilderSubsystem && GetGameInstance())
	{
		CachedDeckBuilderSubsystem = GetGameInstance()->GetSubsystem<UDeckBuilderSubsystem>();
	}

	return CachedDeckBuilderSubsystem;
}

void ADeckBuilderPlayerController::AddCardToWorkingDeck(FName CardId)
{
	UDeckBuilderSubsystem* DeckBuilderSubsystem = GetDeckBuilderSubsystem();
	if (!DeckBuilderSubsystem || CardId.IsNone())
	{
		return;
	}

	DeckBuilderSubsystem->AddCard(CardId);
}

void ADeckBuilderPlayerController::RemoveCardFromWorkingDeck(int32 CardIndex)
{
	UDeckBuilderSubsystem* DeckBuilderSubsystem = GetDeckBuilderSubsystem();
	if (!DeckBuilderSubsystem)
	{
		return;
	}

	DeckBuilderSubsystem->RemoveCardAt(CardIndex);
}

void ADeckBuilderPlayerController::SaveCurrentDeckToSlot()
{
	if (UDeckBuilderSubsystem* DeckBuilderSubsystem = GetDeckBuilderSubsystem())
	{
		DeckBuilderSubsystem->SaveToSlot();
	}
}

void ADeckBuilderPlayerController::LoadDeckFromSlot()
{
	if (UDeckBuilderSubsystem* DeckBuilderSubsystem = GetDeckBuilderSubsystem())
	{
		DeckBuilderSubsystem->LoadFromSlot();
	}
}

void ADeckBuilderPlayerController::RequestStartBattle()
{
	UDeckBuilderSubsystem* DeckBuilderSubsystem = GetDeckBuilderSubsystem();
	if (!DeckBuilderSubsystem)
	{
		return;
	}

	FText ValidationError;
	if (!DeckBuilderSubsystem->ValidateWorkingDeck(ValidationError))
	{
		UE_LOG(LogTemp, Warning, TEXT("Deck validation failed: %s"), *ValidationError.ToString());
		return;
	}

	DeckBuilderSubsystem->SaveToSlot();

	UGameplayStatics::OpenLevel(this, BattleMapName);
}

void ADeckBuilderPlayerController::HandleStartBattleInput()
{
	RequestStartBattle();
}

void ADeckBuilderPlayerController::HandleSaveInput()
{
	SaveCurrentDeckToSlot();
}

void ADeckBuilderPlayerController::HandleLoadInput()
{
	LoadDeckFromSlot();
}
