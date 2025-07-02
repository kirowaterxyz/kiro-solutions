
#include "..\kmbox\kmbox.hpp"
#include "..\settings.h"

inline e_weapon_core_animation get_local_weapon_type() {
	static e_weapon_core_animation cachedType = e_weapon_core_animation::unarmed;
	static std::chrono::steady_clock::time_point lastUpdate = std::chrono::steady_clock::now();
	static uint64_t lastWeapon = 0;

	uint64_t currentWeapon = mem.Read<uint64_t>(point::Player + offsets::CurrentWeapon);

	// Only update if weapon changed or 500ms passed
	auto now = std::chrono::steady_clock::now();
	if (currentWeapon != lastWeapon || std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate).count() > 500) {
		lastWeapon = currentWeapon;
		if (!currentWeapon) {
			cachedType = e_weapon_core_animation::unarmed;
		}
		else {
			cachedType = static_cast<e_weapon_core_animation>(mem.Read<std::uint8_t>(currentWeapon + offsets::WeaponCoreAnimation));
		}
		lastUpdate = now;
	}
	return cachedType;
}


namespace aim {

	Vector3 predictLocation(Vector3 target, Vector3 target_velocity, float projectile_speed, float projectile_gravity_scale, float distance)
	{
		float horizontalTime = distance / projectile_speed;
		float verticalTime = distance / projectile_speed;

		target.x += target_velocity.x * horizontalTime;
		target.y += target_velocity.y * horizontalTime;
		target.z += target_velocity.z * verticalTime + abs(-980 * projectile_gravity_scale) * 0.5f * (verticalTime * verticalTime);

		return target;
	}

	bool isHit(Vector3 loc, double margin = 20) {
		if (mainCamera.LocationUnderReticle.x >= loc.x - margin && mainCamera.LocationUnderReticle.x <= loc.x + margin && mainCamera.LocationUnderReticle.y >= loc.y - margin && mainCamera.LocationUnderReticle.y <= loc.y + margin)
			return true;
		else
			return false;
	}

	bool isHit2D(Vector3 loc, double margin = 20) {
		if (settings::window::Width / 2 >= loc.x - margin && settings::window::Width / 2 <= loc.x + margin && settings::window::Height / 2 >= loc.y - margin && settings::window::Height / 2 <= loc.y + margin)
			return true;
		else
			return false;
	}

	double isClose(Vector3 loc2D) {

		const double maxDistance = std::sqrt(std::pow(settings::window::Width, 2) + std::pow(settings::window::Height, 2)) / 2.0;

		double distance = std::sqrt(std::pow(loc2D.x - settings::window::Width / 2, 2) + std::pow(loc2D.y - settings::window::Height / 2, 2));

		double closeness = 1.0f - (distance / maxDistance);

		closeness = std::clamp(closeness, 0.0, 1.0);

		return closeness;
	}

	constexpr float RadiansToDegrees(float radians) {
		return radians * (180.0f / M_PI);
	}

	Rotation targetRotation(const Vector3& currentPosition, const Vector3& targetPosition) {
		float directionX = targetPosition.x - currentPosition.x;
		float directionY = targetPosition.y - currentPosition.y;
		float directionZ = targetPosition.z - currentPosition.z;

		float yawRadians = std::atan2(directionY, directionX);
		float yawDegrees = RadiansToDegrees(yawRadians);

		float distanceXY = std::sqrt(directionX * directionX + directionY * directionY); // Horizontal distance
		float pitchRadians = std::atan2(directionZ, distanceXY);
		float pitchDegrees = RadiansToDegrees(pitchRadians);

		return { yawDegrees, pitchDegrees };
	}

	PlayerCache target{};
	bool Targetting = false;

	void updateAimbot()
	{
		if (!settings::runtime::hotKeys)
			return;
		if (!settings::kmbox::SerialKmbox && !settings::kmbox::NetKmbox && !settings::config::MoonlightAim)
			return;
		if (!settings::config::Aimbot)
			return;

		// not in game / alive
		if (!point::Player || !point::PlayerState)
			return;

		e_weapon_core_animation weaponType = get_local_weapon_type();
		//std::cout << "Current weapon: " << weapon_core_animation_to_string(weaponType) << std::endl;

		// Set defaults
		bool enabled = settings::config::Aimbot;
		bool visCheck = settings::config::AimbotVisCheck;
		bool ignoreKnocked = settings::config::AimbotIgnoreknocked;
		int aimBone = settings::config::AimBone;
		int fov = settings::config::AimFov;
		bool showFov = settings::config::ShowAimFov;
		float smoothing = settings::config::AimSmoothing;
		int aimKey = settings::config::AimKey;
		int aimKey2 = settings::config::AimKey2;

		switch (weaponType) {
		case e_weapon_core_animation::pistol:
		case e_weapon_core_animation::machine_pistol:
		case e_weapon_core_animation::dual_wield:
			enabled = settings::config::AimbotPistol;
			visCheck = settings::config::AimbotVisCheckPistol;
			ignoreKnocked = settings::config::AimbotIgnoreknockedPistol;
			aimBone = settings::config::AimBonePistol;
			fov = settings::config::AimFovPistol;
			showFov = settings::config::ShowAimFovPistol;
			smoothing = settings::config::AimSmoothingPistol;
			aimKey = settings::config::AimKeyPistol;
			aimKey2 = settings::config::AimKey2Pistol;
			break;
		case e_weapon_core_animation::shotgun:
		case e_weapon_core_animation::tactical_shotgun:
			enabled = settings::config::AimbotShotgun;
			visCheck = settings::config::AimbotVisCheckShotgun;
			ignoreKnocked = settings::config::AimbotIgnoreknockedShotgun;
			aimBone = settings::config::AimBoneShotgun;
			fov = settings::config::AimFovShotgun;
			showFov = settings::config::ShowAimFovShotgun;
			smoothing = settings::config::AimSmoothingShotgun;
			aimKey = settings::config::AimKeyShotgun;
			aimKey2 = settings::config::AimKey2Shotgun;
			break;
		case e_weapon_core_animation::rifle:
		case e_weapon_core_animation::assault_rifle:
		case e_weapon_core_animation::ar_bullpup:
		case e_weapon_core_animation::ar_forward_grip:
		case e_weapon_core_animation::ar_drum_gun:
			enabled = settings::config::AimbotRifle;
			visCheck = settings::config::AimbotVisCheckRifle;
			ignoreKnocked = settings::config::AimbotIgnoreknockedRifle;
			aimBone = settings::config::AimBoneRifle;
			fov = settings::config::AimFovRifle;
			showFov = settings::config::ShowAimFovRifle;
			smoothing = settings::config::AimSmoothingRifle;
			aimKey = settings::config::AimKeyRifle;
			aimKey2 = settings::config::AimKey2Rifle;
			break;
		case e_weapon_core_animation::sniper_rifle:
			enabled = settings::config::AimbotSniper;
			visCheck = settings::config::AimbotVisCheckSniper;
			ignoreKnocked = settings::config::AimbotIgnoreknockedSniper;
			aimBone = settings::config::AimBoneSniper;
			fov = settings::config::AimFovSniper;
			showFov = settings::config::ShowAimFovSniper;
			smoothing = settings::config::AimSmoothingSniper;
			aimKey = settings::config::AimKeySniper;
			aimKey2 = settings::config::AimKey2Sniper;
			break;
		default:
			// Use global/default
			break;
		}

		if (enabled && showFov)
			ImGui::GetBackgroundDrawList()->AddCircle(
				ImVec2(settings::window::Width / 2, settings::window::Height / 2),
				fov,
				ImColor(settings::config::FovColor[0], settings::config::FovColor[1], settings::config::FovColor[2], settings::config::FovColor[3]),
				1000,
				1.f
			);

		if (enabled && showFov) {
			if (settings::config::RainbowFov) {
				// Rainbow FOV circle
				const int num_segments = 200;
				float centerX = settings::window::Width / 2.0f;
				float centerY = settings::window::Height / 2.0f;
				float radius = fov;
				float thickness = 1.0f;
				float time = ImGui::GetTime();

				for (int i = 0; i < num_segments; ++i) {
					float a0 = (i / (float)num_segments) * 2.0f * IM_PI;
					float a1 = ((i + 1) / (float)num_segments) * 2.0f * IM_PI;

					// Hue shifts over time for animation
					float hue = fmodf((a0 / (2.0f * IM_PI)) + time * 0.4, 1.0f);
					ImColor color = ImColor::HSV(hue, 1.0f, 1.0f);

					ImVec2 p0(centerX + cosf(a0) * radius, centerY + sinf(a0) * radius);
					ImVec2 p1(centerX + cosf(a1) * radius, centerY + sinf(a1) * radius);

					ImGui::GetBackgroundDrawList()->AddLine(p0, p1, color, thickness);
				}
			}
			else {
				// Default white FOV circle
				ImGui::GetBackgroundDrawList()->AddCircle(
					ImVec2(settings::window::Width / 2, settings::window::Height / 2),
					fov,
					ImColor(settings::config::FovColor[0], settings::config::FovColor[1], settings::config::FovColor[2], settings::config::FovColor[3]),
					1000,
					1.f
				);
			}
		}


		if (mem.IsKeyDown(aimKey) || mem.IsKeyDown(aimKey2)) {
			const auto& PlayerList = secondPlayerList;

			bool ValidTarget = true;

			if (target.PlayerState) {

				// check if player still exist
				auto it = secondPlayerList.find(target.PlayerState);
				if (it != secondPlayerList.end()) {
					// get lil bro
					target = it->second;

					// check if he's good
					double dist = std::sqrt(std::pow(target.Head2D.x - settings::window::Width / 2, 2) + std::pow(target.Head2D.y - settings::window::Height / 2, 2));

					bool IsVis = point::Seconds - target.last_render <= 0.06f;

					if (target.ignore || (dist > fov) || (visCheck && !IsVis) || (ignoreKnocked && (target.bisDBNO || target.bisDying)))
						ValidTarget = false;

				}
				else {
					ValidTarget = false;
				}

				Targetting = true;
			}
			if (Targetting && !ValidTarget) {
				target.PlayerState = 0;
				Targetting = false;
				}

			double closest = HUGE;
			PlayerCache closestPlayer{};
			if (!Targetting) {
				bool closePlayerFound = false;
				for (const auto& it : PlayerList) {
					const PlayerCache& player = it.second;

					if (player.ignore)
						continue;

					if (!player.Pawn || !player.Mesh || !player.BoneArray) continue;

					bool IsVis = point::Seconds - player.last_render <= 0.06f;

					//if (player.isDying) continue; this caused issue with not aiming when glider
					if (visCheck && !IsVis) continue;

					if (player.PlayerState == point::PlayerState) continue;

					if (ignoreKnocked && (player.bisDBNO || player.bisDying)) // Skip knocked & dying
						continue;

					if (player.TeamId == local_player::localTeam) continue;

					double distance2D = std::sqrt(std::pow(player.Head2D.x - settings::window::Width / 2, 2) + std::pow(player.Head2D.y - settings::window::Height / 2, 2));

					if (distance2D < fov) {
						if (distance2D < closest) {
							closest = distance2D;
							closestPlayer = player;
							closePlayerFound = true;
						}
					}
				}

				if (!closePlayerFound)
					return;
			}
			else {
				closestPlayer = target;
			}

			target = closestPlayer;

			Vector3 originalTarget3D;
			Vector3 originalTarget2D;
			switch (aimBone) {
			case 0: // Head
				originalTarget3D = closestPlayer.Head3D;
				originalTarget2D = closestPlayer.Head2D;
				break;
			case 1: // Top
				originalTarget3D = closestPlayer.Neck3D;
				originalTarget2D = closestPlayer.Neck2D;
				break;
			case 2: // Middle
				originalTarget3D = closestPlayer.Hip3D;
				originalTarget2D = closestPlayer.Hip2D;
				break;
			default:
				originalTarget3D = closestPlayer.Head3D;
				originalTarget2D = closestPlayer.Head2D;
				break;
			}

			Vector3 target3D = originalTarget3D;
			Vector3 target2D = originalTarget2D;

			Rotation target = targetRotation(mainCamera.Location, target3D);

			float currentY = mainCamera.Rotation.y;
			float currentX = mainCamera.Rotation.x;

			float targety = target.yaw - currentY;
			float targetx = target.pitch - currentX;

			while (targety > 180.0f) targety -= 360.0f;
			while (targety < -180.0f) targety += 360.0f;

			std::clamp(targetx, -89.9f, 89.9f);

			float xPercent = point::MouseSensX / 0.25f;
			float xStep = 1.8f * (1.f / xPercent);

			float yPercent = point::MouseSensY / 0.25f;
			float yStep = -1.8f * (1.f / yPercent);

			float fovScale = 80.f / mainCamera.FieldOfView;

			xStep *= fovScale;
			yStep *= fovScale;

			float AngleX = targety * (xStep / smoothing); // settings::config::StepPerDegreeX
			float AngleY = targetx * (yStep / smoothing); // settings::config::StepPerDegreeY

			if (settings::config::MoonlightAim) {
				mouse_event(MOUSEEVENTF_MOVE, AngleX, AngleY, 0, 0);
			}
			else if (settings::kmbox::NetKmbox) {
				kmNet_mouse_move(AngleX, AngleY);
			}
			else {
				km_move(AngleX, AngleY);
			}
		}
		else {
			target.PlayerState = 0;
			Targetting = false;
		}
	}

	void updateTriggerBot()
	{
		if (!settings::runtime::hotKeys)
			return;

		if (!settings::kmbox::SerialKmbox && !settings::kmbox::NetKmbox && !settings::config::MoonlightAim)
			return;

		if (!settings::config::TriggerBot)
			return;

		static bool clicked = false;
		static auto lastClick = std::chrono::steady_clock::now();
		static auto clickStart = std::chrono::steady_clock::now();

		if (clicked) {
			auto now = std::chrono::steady_clock::now();
			if (std::chrono::duration_cast<std::chrono::milliseconds>(now - clickStart).count() > 25) {
				if (settings::config::MoonlightAim)
					mouse_event(MOUSEEVENTF_LEFTUP, mainCamera.Rotation.x, mainCamera.Rotation.y, 0, 0);
				else
					kmNet_mouse_left(false);
				km_unclick();
				clicked = false;
			}
		}

		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastClick).count() < settings::config::TriggerDelay)
			return;

		if (mem.IsKeyDown(settings::config::TriggerKey) || mem.IsKeyDown(settings::config::TriggerKey2) || settings::config::TriggerBotAlwaysOn) {
			// Get local player's weapon type
			e_weapon_core_animation weaponType = get_local_weapon_type();

			// "All" option: if enabled, allow any weapon
			if (!settings::config::TriggerBotAll) {
				bool allowed = false;
				switch (weaponType) {
				case e_weapon_core_animation::pistol:
				case e_weapon_core_animation::machine_pistol:
				case e_weapon_core_animation::dual_wield:
					allowed = settings::config::TriggerBotPistol;
					break;
				case e_weapon_core_animation::shotgun:
				case e_weapon_core_animation::tactical_shotgun:
					allowed = settings::config::TriggerBotShotgun;
					break;
				case e_weapon_core_animation::rifle:
				case e_weapon_core_animation::assault_rifle:
				case e_weapon_core_animation::ar_bullpup:
				case e_weapon_core_animation::ar_forward_grip:
				case e_weapon_core_animation::ar_drum_gun:
					allowed = settings::config::TriggerBotRifle;
					break;
				case e_weapon_core_animation::sniper_rifle:
					allowed = settings::config::TriggerBotSniper;
					break;
				default:
					allowed = false;
					break;
				}
				if (!allowed)
					return;
			}
			const auto& PlayerList = secondPlayerList;

			for (const auto& it : PlayerList) {
				const PlayerCache& player = it.second;

				if (player.ignore)
					continue;

				if (!player.Pawn || !player.Mesh || !player.BoneArray) continue;

				bool IsVis = point::Seconds - player.last_render <= 0.06f;
				if (!IsVis) continue;
				if (settings::config::TriggerBotIgnoreknocked && player.bisDBNO || player.bisDying) continue;
				if (player.PlayerState == point::PlayerState) continue;
				if (player.TeamId == local_player::localTeam) continue;

				float distance = static_cast<float>(mainCamera.Location.Distance(player.Head3D)) / 100.0f;
				if (distance > settings::config::TriggerBotMaxDistance)
					continue;

				if (isHit(player.Head3D)) {
					lastClick = std::chrono::steady_clock::now();
					clickStart = std::chrono::steady_clock::now();

					if (settings::config::MoonlightAim) {
						mouse_event(MOUSEEVENTF_LEFTDOWN, NULL, NULL, NULL, NULL);
						clicked = true;
					}
					else if (settings::kmbox::NetKmbox) {
						kmNet_mouse_left(true);
						clicked = true;
					}
					else {
						km_click();
					}

					break;
				}
			}
		}
	}
}
