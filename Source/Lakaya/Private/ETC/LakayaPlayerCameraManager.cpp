// Fill out your copyright notice in the Description page of Project Settings.


#include "ETC/LakayaPlayerCameraManager.h"

void ALakayaPlayerCameraManager::Zoom_Implementation(const bool IsZoomIn, const float& ZoomFov)
{
	K2_SetFov(IsZoomIn ? ZoomFov : DefaultFOV);
}

void ALakayaPlayerCameraManager::K2_SetFov(const float& NewFov)
{
	SetFOV(NewFov);
}
