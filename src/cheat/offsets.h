#pragma once
#include <cstdint>

namespace offsets {
    uintptr_t Uworld = 0x1488D840;                                  // 6 08 2025 Class Globals
    uintptr_t GameInstance = 0x228;                                 // 6 08 2025 Class World
    uintptr_t GameState = 0x1B0;                                    // 6 08 2025 Class World
    uintptr_t PersistentLevel = 0x30;                             	// 5 29 2025 Class World
    uintptr_t LocalPlayers = 0x38;                                  // 5 29 2025 Class GameInstance
    uintptr_t PlayerController = 0x30;                              // 5 29 2025 Class Player
    uintptr_t LocalPawn = 0x350;                                    // 5 29 2025 AcknowledgedPawn
    uintptr_t Cameramanager = 0x360;                                // 5 29 2025 Class PlayerController
    uintptr_t PlayerState = 0x2C8;                                  // 5 29 2025 Class Controller
    uintptr_t PawnPrivate = 0x320;                                  // 5 29 2025 Class PlayerState
    uintptr_t PlayerArray = 0x2C0;                                  // 5 29 2025 Class GameStateBase
    uintptr_t Mesh = 0x328;                                         // 5 29 2025 Class Character
    uintptr_t ComponentToWorld = 0x1E0;                             // 5 29 2025 Class Globals
    uintptr_t Velocity = 0x180;                                     // 5 29 2025 Class SceneComponent Component Velocity
    uintptr_t TeamId = 0x1291;                                      // 6 08 2025 Class FortPlayerStateAthena
    uintptr_t RootComponent = 0x1B0;                                // 5 29 2025 Class Actor
    uintptr_t OwningWorld = 0x118;                                   // 5 29 2025 Class Level // 200

    // World
    uintptr_t RelativeLocation = 0x138;                           // 5 29 2025 Class SceneComponent

    // Visibility
    uintptr_t Seconds = 0x180;                                      // 6 08 2025 CameraRotation + 0x10
    uintptr_t LastRenderTime = 0x32c;                               // 5 29 2025 BoundScale + 08x (BoundScale in FXSystemComponent

    // Camera
    uintptr_t CameraLocationPointer = 0x160;                        // 6 08 2025 Class CameraLocation
    uintptr_t CameraRotationPointer = 0x170;                        // 6 08 2025 Class CameraRotation
    uintptr_t FieldOfView = 0x4AC;                                  // 5 29 2025 point::LocalPlayer + 0x4AC

    //Bone
    uintptr_t BoneArray = 0x5E8;                                    // 5 29 2025 Class DynamicMeshComponent

    // Health // Status
    uintptr_t IsDBNO = 0x9A1;                                       // 6 08 2025 Class FortPawn //0x5e8 ?? UFortAiAnimInstance.bISDBNO
    uintptr_t isDying = 0x728;                                      // 5 29 2025 Class FortPawn
    uintptr_t bIsABot = 0x2b2;                                      // 5 29 2025 Class PlayerState

    // Reticle // Targeting
    uintptr_t LocationUnderReticle = 0x3018;                        // 6 08 2025 Class FortPlayerController

    // Weapon
    uintptr_t WeaponData = 0x548;                                   // 6 08 2025 Class FortWeapon
    uintptr_t WeaponCoreAnimation = 0x16d8;							// 6 08 2025 Class FortWeapon
    uintptr_t CurrentWeapon = 0xAF0;                                // 6 08 2025 Class FortPawn

    // Spectator
    uintptr_t Spectators = 0xbb8;                                   // 5 29 2025 Class FortPlayerStatePvE or FortSpectatorCameraComponent
    uintptr_t SpectatorArray = 0x108;                               // 5 29 2025 Offset to spectator array from Spectators

    // Player name
    uintptr_t PlayerName = 0xB40;                                   // 6 08 2025 Class Globals or UFortExperienceSettingsComponent.NameplateDisplayMode

    // Platform
    uintptr_t Platform = 0x430;                                     // 5 29 2025 Class FortPlayerState

    // Rank
    uintptr_t HabaneroComponent = 0xa80;                            // 5 29 2025 FortPlayerState
    uintptr_t RankedProgress = 0xd0;                                // 5 29 2025 Class FortPlayerStateComponent_Habanero or UFortPlayerStateComponent_Habanero.RankedProgress
    uintptr_t CurrentRank = 0x10;                                   // 5 29 2025

    // Killscore
    uintptr_t KillScore = 0x12a8;                                   // 5 29 2025 Class FortPlayerStateAthena

    // Mouse
    uintptr_t MouseStuff = 0x378;                                   // 6 08 2025 Class FortLocalPlayer
    uintptr_t MouseSensitivityX = 0x784;                            // 6 08 2025 Class FortClientSettingsRecord
    uintptr_t MouseSensitivityY = 0x788;                            // 6 08 2025 Class FortClientSettingsRecord


    // useful

    //uintptr_t MouseSensitivity = 0x748;                           // 5 21 2025 Class FortClientSettingsRecord
    // https://offsets.host/?class=PlayerController&property=TargetViewRotation = 0x378
    //0x74c https://offsets.host/?class=FortClientSettingsRecord&property=MouseSensitivityX
    //0x750 https://offsets.host/?class=FortClientSettingsRecord&property=MouseSensitivityY
    //0x748 // 378 ?? https://offsets.host/?class=FortClientSettingsRecord&property=MouseSensitivity
    //0x378 // lol https://offsets.host/?class=FortLocalPlayer&property=ClientSettingsRecord
    //ClientSettingsRecord: 0x378 
    //SeasonLevelUIDisplay = 0x128C;
}
