#include "DeckBuilderGameMode.h"

#include "DeckBuilderPlayerController.h"

ADeckBuilderGameMode::ADeckBuilderGameMode()
{
	DefaultPawnClass = nullptr;
	PlayerControllerClass = ADeckBuilderPlayerController::StaticClass();
	bStartPlayersAsSpectators = true;
}
