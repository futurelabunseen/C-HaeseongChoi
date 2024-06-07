// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/SSLandingCameraShake.h"

USSLandingCameraShake::USSLandingCameraShake()
{
	OscillationDuration = 0.25f;

	// Rotation oscillation
	RotOscillation.Pitch.Amplitude = 15.0f;
	RotOscillation.Pitch.Frequency = 30.0f;

	RotOscillation.Yaw.Amplitude = 15.0f;
	RotOscillation.Yaw.Frequency = 30.0f;

	RotOscillation.Roll.Amplitude = 15.0f;
	RotOscillation.Roll.Frequency = 30.0f;

	// Location oscillation
	LocOscillation.Z.Amplitude = 10.0f;
	LocOscillation.Z.Frequency = 25.0f;
}