#include "Interactable/Interactable.h"

AInteractable::AInteractable()
{
	Tags.Add("Interactable");

	InteractingPawn = nullptr;
}