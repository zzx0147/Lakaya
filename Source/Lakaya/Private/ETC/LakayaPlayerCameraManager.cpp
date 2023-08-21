// Fill out your copyright notice in the Description page of Project Settings.


#include "ETC/LakayaPlayerCameraManager.h"

void ALakayaPlayerCameraManager::Zoom(const bool& IsZoomIn)
{
	SetFOV(IsZoomIn ? 60 : 90);
}
