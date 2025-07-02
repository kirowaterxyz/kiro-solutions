#pragma once



namespace esp {


	// idk :(
	bool shouldDisplayPlayer(const PlayerCache& player) {



		auto it = secondPlayerList.find(player.PlayerState);
		if (it != secondPlayerList.end()) {

			double dist = std::sqrt(std::pow(player.Head2D.x - settings::window::Width / 2, 2) + std::pow(player.Head2D.y - settings::window::Height / 2, 2));


			if (player.bisDying)
				return false;

		}
		else {
			return false;
		}

		return true;
	}

	void renderPlayers()
	{

		// stats
		int playersRendered = 0;
		int playersLooped = 0;
		int validPlayersLooped = 0;
		int invalidPlayersLooped = 0;
		int teammatesSkipped = 0;
		int bots = 0;

		for (const auto& it : secondPlayerList) {
			const PlayerCache& player = it.second;

			if (player.ignore)
				continue;

			playersLooped++;

			// valid?
			if (!isPlayerValid(player)) {
				invalidPlayersLooped++;
				continue;
			}

			validPlayersLooped++;

			// should be in the updates
			bool IsVis = true;
			if (settings::config::isVis) {
				IsVis = point::Seconds - player.last_render <= 0.06f;
			}

			if (player.isBot) {
				bots++;
				if (settings::config::Ignorenpc) {
					continue;
				}
			}

			// player.isDBNO

			//if (player.isBot) {
			//	bots++;
			//}


			// check if its bot
			//if (player.isDBNO)
			//{
				//ImGui::GetBackgroundDrawList()->AddText((ImFont*)fonts::notosans_font, 18.5f, ImVec2(player.Neck2D.x, player.Neck2D.y), ImColor(255, 255, 255, 255), "is bot");
				//continue;
			//}//

			// check if its me ༼ つ ◕_◕ ༽つ
			if (player.PlayerState == point::PlayerState) {
				//ImGui::GetBackgroundDrawList()->AddText((ImFont*)fonts::notosans_font, 18.5f, ImVec2(player.Neck2D.x, player.Neck2D.y), ImColor(255, 255, 255, 255), "me");
				continue;
			}

			// chcek if not in lobby and check if its a teammate (omg friends) (◕‿◕✿)
			if (point::Player && player.TeamId == local_player::localTeam) {
				//ImGui::GetBackgroundDrawList()->AddText((ImFont*)fonts::notosans_font, 18.5f ,ImVec2(player.Neck2D.x, player.Neck2D.y), ImColor(255,255,255,255), "teammate");
				teammatesSkipped++;
				continue;
			}

			// check if dying (dead) ◉_◉
			//if (player.bisDying) {
				//ImGui::GetBackgroundDrawList()->AddText((ImFont*)fonts::notosans_font, 18.5f, ImVec2(player.Neck2D.x, player.Neck2D.y), ImColor(255, 255, 255, 255), "dying");
				//continue;
			//}

			// check if knocked (dead) ◉_◉
			//if (player.isDBNO) {
				//ImGui::GetBackgroundDrawList()->AddText((ImFont*)fonts::notosans_font, 18.5f, ImVec2(player.Neck2D.x, player.Neck2D.y), ImColor(255, 255, 255, 255), "knocked");
				//continue;
			//}
			//if (player.bisDBNO) {
				// Show as DBNO (orange)
				//ImGui::GetBackgroundDrawList()->AddText((ImFont*)fonts::notosans_font, 18.5f, ImVec2(player.Neck2D.x, player.Neck2D.y), ImColor(255, 100, 0, 255), "knocked");
				//continue;
			//}

			//if (player.last_render > 0.f && (player.isDBNO & 0x40)) {
				// Show as DBNO (orange)
				//ImGui::GetBackgroundDrawList()->AddText((ImFont*)fonts::notosans_font, 18.5f, ImVec2(player.Neck2D.x, player.Neck2D.y), ImColor(255, 255, 255, 255), "knocked");
				//continue;
			//}

			// info we might want to show

			int distanceMeters = mainCamera.Location.Distance(player.Head3D) / 100;

			// ok maybe we can draw now

			playersRendered++;

			// skeleton
			if (settings::config::Skeleton)
			{
				ImColor colSK = ImColor::HSV(0, 1, 1); // Default fallback
				if (IsVis)
					colSK = ImColor(settings::config::skeletonColorVisible[0], settings::config::skeletonColorVisible[1], settings::config::skeletonColorVisible[2], settings::config::skeletonColorVisible[3]);
				else
					colSK = ImColor(settings::config::skeletonColor[0], settings::config::skeletonColor[1], settings::config::skeletonColor[2], settings::config::skeletonColor[3]);


				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.Neck2D.x, player.Neck2D.y), ImVec2(player.Head2D.x, player.Head2D.y), colSK, settings::config::SkeletonThickness);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.Hip2D.x, player.Hip2D.y), ImVec2(player.Neck2D.x, player.Neck2D.y), colSK, settings::config::SkeletonThickness);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.UpperArmLeft2D.x, player.UpperArmLeft2D.y), ImVec2(player.Neck2D.x, player.Neck2D.y), colSK, settings::config::SkeletonThickness);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.UpperArmRight2D.x, player.UpperArmRight2D.y), ImVec2(player.Neck2D.x, player.Neck2D.y), colSK, settings::config::SkeletonThickness);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.LeftHand2D.x, player.LeftHand2D.y), ImVec2(player.UpperArmLeft2D.x, player.UpperArmLeft2D.y), colSK, settings::config::SkeletonThickness);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.RightHand2D.x, player.RightHand2D.y), ImVec2(player.UpperArmRight2D.x, player.UpperArmRight2D.y), colSK, settings::config::SkeletonThickness);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.LeftHand2D.x, player.LeftHand2D.y), ImVec2(player.LeftHandT2D.x, player.LeftHandT2D.y), colSK, settings::config::SkeletonThickness);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.RightHand2D.x, player.RightHand2D.y), ImVec2(player.RightHandT2D.x, player.RightHandT2D.y), colSK, settings::config::SkeletonThickness);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.LeftThigh2D.x, player.LeftThigh2D.y), ImVec2(player.Hip2D.x, player.Hip2D.y), colSK, settings::config::SkeletonThickness);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.RightThigh2D.x, player.RightThigh2D.y), ImVec2(player.Hip2D.x, player.Hip2D.y), colSK, settings::config::SkeletonThickness);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.LeftCalf2D.x, player.LeftCalf2D.y), ImVec2(player.LeftThigh2D.x, player.LeftThigh2D.y), colSK, settings::config::SkeletonThickness);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.RightCalf2D.x, player.RightCalf2D.y), ImVec2(player.RightThigh2D.x, player.RightThigh2D.y), colSK, settings::config::SkeletonThickness);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.LeftFoot2D.x, player.LeftFoot2D.y), ImVec2(player.LeftCalf2D.x, player.LeftCalf2D.y), colSK, settings::config::SkeletonThickness);
				ImGui::GetBackgroundDrawList()->AddLine(ImVec2(player.RightFoot2D.x, player.RightFoot2D.y), ImVec2(player.RightCalf2D.x, player.RightCalf2D.y), colSK, settings::config::SkeletonThickness);
			}

			// box
			if (settings::config::Box) {
				ImColor colBox = ImColor::HSV(0, 1, 1); // fallback color
				if (IsVis)
					colBox = ImColor(settings::config::boxColorVisible[0], settings::config::boxColorVisible[1], settings::config::boxColorVisible[2], settings::config::boxColorVisible[3]);
				else
					colBox = ImColor(settings::config::boxColor[0], settings::config::boxColor[1], settings::config::boxColor[2], settings::config::boxColor[3]);

				float box_height = abs(player.Top2D.y - player.Bottom2D.y);
				float box_width = 0.5f * box_height;

				ImVec2 topLeft = ImVec2(player.Bottom2D.x - box_width / 2, player.Top2D.y);
				ImVec2 bottomRight = ImVec2(player.Bottom2D.x + box_width / 2, player.Bottom2D.y);

				// Draw box
				ImGui::GetBackgroundDrawList()->AddRect(topLeft, bottomRight, colBox, settings::config::BoxRoundness, 0, settings::config::BoxThickness);
			}

			// Corner box
			if (settings::config::CornerBox) {
				ImColor colCornerBox = ImColor::HSV(0, 1, 1); // fallback color
				if (IsVis)
					colCornerBox = ImColor(settings::config::CornerboxColorVisible[0], settings::config::CornerboxColorVisible[1], settings::config::CornerboxColorVisible[2], settings::config::CornerboxColorVisible[3]);
				else
					colCornerBox = ImColor(settings::config::CornerboxColor[0], settings::config::CornerboxColor[1], settings::config::CornerboxColor[2], settings::config::CornerboxColor[3]);

				float box_height = abs(player.Top2D.y - player.Bottom2D.y);
				float box_width = 0.5f * box_height;

				ImVec2 topLeft = ImVec2(player.Bottom2D.x - box_width / 2, player.Top2D.y);
				ImVec2 bottomRight = ImVec2(player.Bottom2D.x + box_width / 2, player.Bottom2D.y);

				float lineLength = box_width * 0.25f;
				float thickness = settings::config::BoxThickness;

				// Top left
				ImGui::GetBackgroundDrawList()->AddLine(topLeft, ImVec2(topLeft.x + lineLength, topLeft.y), colCornerBox, thickness);
				ImGui::GetBackgroundDrawList()->AddLine(topLeft, ImVec2(topLeft.x, topLeft.y + lineLength), colCornerBox, thickness);

				// Top right
				ImVec2 topRight = ImVec2(bottomRight.x, topLeft.y);
				ImGui::GetBackgroundDrawList()->AddLine(topRight, ImVec2(topRight.x - lineLength, topRight.y), colCornerBox, thickness);
				ImGui::GetBackgroundDrawList()->AddLine(topRight, ImVec2(topRight.x, topRight.y + lineLength), colCornerBox, thickness);

				// Bottom left
				ImVec2 bottomLeft = ImVec2(topLeft.x, bottomRight.y);
				ImGui::GetBackgroundDrawList()->AddLine(bottomLeft, ImVec2(bottomLeft.x + lineLength, bottomLeft.y), colCornerBox, thickness);
				ImGui::GetBackgroundDrawList()->AddLine(bottomLeft, ImVec2(bottomLeft.x, bottomLeft.y - lineLength), colCornerBox, thickness);

				// Bottom right
				ImGui::GetBackgroundDrawList()->AddLine(bottomRight, ImVec2(bottomRight.x - lineLength, bottomRight.y), colCornerBox, thickness);
				ImGui::GetBackgroundDrawList()->AddLine(bottomRight, ImVec2(bottomRight.x, bottomRight.y - lineLength), colCornerBox, thickness);
			}


			if (settings::config::Name) {
				ImColor colName = ImColor::HSV(0, 1, 1); // fallback color
				if (IsVis)
					colName = ImColor(settings::config::NameColorVisible[0], settings::config::NameColorVisible[1], settings::config::NameColorVisible[2], settings::config::NameColorVisible[3]);
				else
					colName = ImColor(settings::config::NameColor[0], settings::config::NameColor[1], settings::config::NameColor[2], settings::config::NameColor[3]);
				float box_height = abs(player.Top2D.y - player.Bottom2D.y);
				float box_width = 0.5f * box_height;

				ImVec2 topLeft = ImVec2(player.Bottom2D.x - box_width / 2, player.Top2D.y);
				ImVec2 bottomRight = ImVec2(player.Bottom2D.x + box_width / 2, player.Bottom2D.y);

				std::string playerName = player.PlayerName;
				ImVec2 nameSize = ImGui::CalcTextSize(playerName.c_str());
				ImVec2 namePos = ImVec2(
					topLeft.x + (box_width / 2.0f) - (nameSize.x / 2.0f),
					bottomRight.y + 2.0f // Just below the box
				);
				ImGui::GetBackgroundDrawList()->AddText(namePos, colName, playerName.c_str());
			}


			//distance
			if (settings::config::Distance)
			{
				ImColor colTxt = ImColor::HSV(0, 1, 1); // fallback color
				if (IsVis)
					colTxt = ImColor(settings::config::DistanceColorVisible[0], settings::config::DistanceColorVisible[1], settings::config::DistanceColorVisible[2], settings::config::DistanceColorVisible[3]);
				else
					colTxt = ImColor(settings::config::DistanceColor[0], settings::config::DistanceColor[1], settings::config::DistanceColor[2], settings::config::DistanceColor[3]);

				float box_height = (abs(player.Top2D.y - player.Bottom2D.y));
				float box_width = 0.5f * box_height;

				ImVec2 topLeft = ImVec2(player.Bottom2D.x - box_width / 2, player.Top2D.y);
				ImVec2 bottomRight = ImVec2(player.Bottom2D.x + box_width / 2, player.Bottom2D.y);

				string distanceStr = std::format("{:d}m", distanceMeters);
				ImVec2 distanceSize = ImGui::CalcTextSize(distanceStr.c_str());
				ImVec2 distancePos = ImVec2(
					topLeft.x + (box_width / 2.0f) - (distanceSize.x / 2.0f),
					bottomRight.y + 18.0f // adjust with the height of box name
				);

				ImGui::GetBackgroundDrawList()->AddText(distancePos, colTxt, distanceStr.c_str());
			}
			if (settings::config::Snaplines)
			{
				ImColor colSnap = ImColor::HSV(0, 1, 1); // Default fallback
				if (IsVis)
					colSnap = ImColor(settings::config::SnapColorVisible[0], settings::config::SnapColorVisible[1], settings::config::SnapColorVisible[2], settings::config::SnapColorVisible[3]);
				else
					colSnap = ImColor(settings::config::SnapColor[0], settings::config::SnapColor[1], settings::config::SnapColor[2], settings::config::SnapColor[3]);

				ImVec2 start;
				if (settings::config::SnaplineMiddle) {
					start = ImVec2(settings::window::Width / 2, settings::window::Height / 2);
				}
				else if (settings::config::SnaplineTop) {
					start = ImVec2(settings::window::Width / 2, 0);
				}
				else {
					// Default: bottom
					start = ImVec2(settings::window::Width / 2, settings::window::Height);
				}

				ImGui::GetBackgroundDrawList()->AddLine(start, ImVec2(player.Head2D.x, player.Head2D.y), colSnap, 1.0f);
			}
			if (settings::config::KillScore)
			{
				ImColor colKillScore = ImColor::HSV(0, 1, 1); // Default fallback
				if (IsVis)
					colKillScore = ImColor(settings::config::KillScoreColorVisible[0], settings::config::KillScoreColorVisible[1], settings::config::KillScoreColorVisible[2], settings::config::KillScoreColorVisible[3]);
				else
					colKillScore = ImColor(settings::config::KillScoreColor[0], settings::config::KillScoreColor[1], settings::config::KillScoreColor[2], settings::config::KillScoreColor[3]);
				float box_height = (abs(player.Top2D.y - player.Bottom2D.y));
				float box_width = 0.5f * box_height;
				ImVec2 topLeft = ImVec2(player.Bottom2D.x - box_width / 2, player.Top2D.y);
				std::string killScoreStr = std::to_string(player.KillScore) + " kills";
				ImVec2 killScoreSize = ImGui::CalcTextSize(killScoreStr.c_str());
				ImVec2 killScorePos = ImVec2(
					topLeft.x + (box_width / 2.0f) - (killScoreSize.x / 2.0f),
					topLeft.y - killScoreSize.y - 10.0f  // Adjust as needed (higher/lower than platform text)
				);
				ImGui::GetBackgroundDrawList()->AddText(killScorePos, colKillScore, killScoreStr.c_str());
			}
			if (settings::config::FovArrows) {
				ImColor colFovArrow = ImColor::HSV(0, 1, 1); // Default fallback
				if (IsVis)
					colFovArrow = ImColor(settings::config::FovArrowsColorVisible[0], settings::config::FovArrowsColorVisible[1], settings::config::FovArrowsColorVisible[2], settings::config::FovArrowsColorVisible[3]);
				else
					colFovArrow = ImColor(settings::config::FovArrowsColor[0], settings::config::FovArrowsColor[1], settings::config::FovArrowsColor[2], settings::config::FovArrowsColor[3]);

				float centerX = settings::window::Width / 2.0f;
				float centerY = settings::window::Height / 2.0f;
				float fovRadius = static_cast<float>(settings::config::FovArrowsSlider);

				float dx = player.Head2D.x - centerX;
				float dy = player.Head2D.y - centerY;
				float dist = std::sqrt(dx * dx + dy * dy);

				if (dist > fovRadius) {
					float dirX = dx / dist;
					float dirY = dy / dist;

					float arrowX = centerX + dirX * fovRadius;
					float arrowY = centerY + dirY * fovRadius;

					float arrowLength = 18.0f;
					float arrowWidth = 8.0f;

					ImVec2 tip(arrowX, arrowY);
					ImVec2 base(arrowX - dirX * arrowLength, arrowY - dirY * arrowLength);
					ImVec2 perp(-dirY, dirX);

					ImVec2 left(base.x + perp.x * arrowWidth, base.y + perp.y * arrowWidth);
					ImVec2 right(base.x - perp.x * arrowWidth, base.y - perp.y * arrowWidth);

					ImGui::GetBackgroundDrawList()->AddTriangleFilled(tip, left, right, colFovArrow);
				}
			}

			if (settings::config::Platform) {
				if (!player.Platform.empty())
				{
					ImColor platformTextColor = ImColor::HSV(0, 1, 1); // fallback color
					if (IsVis)
						platformTextColor = ImColor(settings::config::PlatformColorVisible[0], settings::config::PlatformColorVisible[1], settings::config::PlatformColorVisible[2], settings::config::PlatformColorVisible[3]);
					else
						platformTextColor = ImColor(settings::config::PlatformColor[0], settings::config::PlatformColor[1], settings::config::PlatformColor[2], settings::config::PlatformColor[3]);

					float box_height = (abs(player.Top2D.y - player.Bottom2D.y));
					float box_width = 0.5f * box_height;

					ImVec2 topLeft = ImVec2(player.Bottom2D.x - box_width / 2, player.Top2D.y);

					// Display platform text
					ImVec2 textSize = ImGui::CalcTextSize(player.Platform.c_str());
					ImVec2 textPos = ImVec2(
						topLeft.x + (box_width / 2.0f) - (textSize.x / 2.0f),
						topLeft.y - textSize.y - 40.0f  // Position it above other text
					);

					ImGui::GetBackgroundDrawList()->AddText(textPos, platformTextColor, player.Platform.c_str());
				}
			}
			if (settings::config::Weaponname) {
				ImColor weaponColor = ImColor::HSV(0, 1, 1); // fallback color
				if (IsVis)
					weaponColor = ImColor(settings::config::WeaponColorVisible[0], settings::config::WeaponColorVisible[1], settings::config::WeaponColorVisible[2], settings::config::WeaponColorVisible[3]);
				else
					weaponColor = ImColor(settings::config::WeaponColor[0], settings::config::WeaponColor[1], settings::config::WeaponColor[2], settings::config::WeaponColor[3]);

				float box_height = abs(player.Top2D.y - player.Bottom2D.y);
				float box_width = 0.5f * box_height;

				ImVec2 topLeft = ImVec2(player.Bottom2D.x - box_width / 2, player.Top2D.y);
				ImVec2 bottomRight = ImVec2(player.Bottom2D.x + box_width / 2, player.Bottom2D.y);

				ImVec2 weaponTextSize = ImGui::CalcTextSize(player.Weaponname.c_str());
				ImVec2 weaponTextPos = ImVec2(
					topLeft.x + (box_width / 2.0f) - (weaponTextSize.x / 2.0f),
					bottomRight.y + 34.0f // Adjust height: Name is at +2, Distance at +18, so +34 is good
				);

				ImGui::GetBackgroundDrawList()->AddText(weaponTextPos, weaponColor, player.Weaponname.c_str());
			}
			if (settings::config::Rank) {
				if (!player.Rank.empty()) {
					ImColor rankColor = ImColor::HSV(0, 1, 1); // fallback color

					if (IsVis)
						rankColor = ImColor(settings::config::RankColorVisible[0], settings::config::RankColorVisible[1], settings::config::RankColorVisible[2], settings::config::RankColorVisible[3]);
					else
						rankColor = ImColor(settings::config::RankColor[0], settings::config::RankColor[1], settings::config::RankColor[2], settings::config::RankColor[3]);

					float box_height = (abs(player.Top2D.y - player.Bottom2D.y));
					float box_width = 0.5f * box_height;

					ImVec2 topLeft = ImVec2(player.Bottom2D.x - box_width / 2, player.Top2D.y);

					// Display rank text
					ImVec2 textSize = ImGui::CalcTextSize(player.Rank.c_str());
					ImVec2 textPos = ImVec2(
						topLeft.x + (box_width / 2.0f) - (textSize.x / 2.0f),
						topLeft.y - textSize.y - 25.0f  // Adjust as needed (higher/lower than platform text)
					);

					ImGui::GetBackgroundDrawList()->AddText(textPos, rankColor, player.Rank.c_str());
				}
			}

			if (settings::config::Spectators) {
				auto spectatorNames = GetSpectatorNamesCached();

				if (!spectatorNames.empty()) {
					auto draw_list = ImGui::GetBackgroundDrawList();

					// Settings
					float padding = 4.0f;
					float lineHeight = 18.0f; // Height per spectator line
					float headerHeight = 20.0f;
					float baseX = 6.0f;
					float baseY = 30.0f; // below watermark

					// Dynamically calculate box width based on longest text
					float maxWidth = ImGui::CalcTextSize("Spectators:").x;
					for (const auto& name : spectatorNames) {
						float nameWidth = ImGui::CalcTextSize(name.c_str()).x;
						if (nameWidth > maxWidth)
							maxWidth = nameWidth;
					}
					float boxWidth = maxWidth + (padding * 2); // account for padding

					// Background rectangle height based on number of spectators
					float totalHeight = headerHeight + (spectatorNames.size() * lineHeight);

					// Header rectangle
					draw_list->AddRectFilled(
						ImVec2(baseX, baseY),
						ImVec2(baseX + boxWidth, baseY + headerHeight),
						ImColor(0.07f, 0.07f, 0.09f, 1.0f)
					);

					// Background for names
					draw_list->AddRectFilled(
						ImVec2(baseX, baseY + headerHeight),
						ImVec2(baseX + boxWidth, baseY + totalHeight),
						ImColor(0.07f, 0.07f, 0.09f, 1.0f)
					);

					// Header text
					draw_list->AddText(ImVec2(baseX + padding, baseY), ImColor(255, 255, 255), "Spectators:");

					// Draw each spectator name
					for (int i = 0; i < spectatorNames.size(); ++i) {
						const std::string& name = spectatorNames[i];
						ImVec2 textPos = ImVec2(baseX + padding, baseY + headerHeight + (i * lineHeight));
						draw_list->AddText(textPos, ImColor(255, 255, 255), name.c_str());
					}
				}
			}
		}

		// stats
		info::render::playersRendered = playersRendered;
		info::render::playersLooped = playersLooped;
		info::render::validPlayersLooped = validPlayersLooped;
		info::render::invalidPlayersLooped = invalidPlayersLooped;
		info::render::teammatesSkipped = teammatesSkipped;
		info::render::validBots = bots;
	}


	void Debug()
	{
		if (settings::config::Crosshair)
		{
			ImColor colCH = ImColor(settings::config::CrosshairColor[0], settings::config::CrosshairColor[1], settings::config::CrosshairColor[2], settings::config::CrosshairColor[3]);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(settings::window::Width / 2 - 10, settings::window::Height / 2), ImVec2(settings::window::Width / 2 + 10, settings::window::Height / 2), colCH, 1.0f);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(settings::window::Width / 2, settings::window::Height / 2 - 10), ImVec2(settings::window::Width / 2, settings::window::Height / 2 + 10), colCH, 1.0f);
		}

		const auto& MainValues = stats::mainThreadData.getValues();
		float mainAvgMs = std::accumulate(MainValues.begin(), MainValues.end(), 0.0f) / MainValues.size();
		float mainFPS = (mainAvgMs > 0) ? (1000.0f / mainAvgMs) : 0.0f;

		// Combine watermark and FPS into one string
		std::string mainStr = std::format("discord.gg/d4ZhmGTMNW | FPS: {:.0f}", mainFPS);

		auto draw_list = ImGui::GetBackgroundDrawList();
		auto textSize = ImGui::CalcTextSize(mainStr.c_str());

		auto yOffset = 0;
		auto offset = 4;

		// Draw background rectangles
		draw_list->AddRectFilled(
			ImVec2(6, 4 + yOffset),
			ImVec2((4 * 2) + textSize.x + 6, 6 + yOffset),
			ImColor(settings::config::WatermarkAccentColor[0], settings::config::WatermarkAccentColor[1], settings::config::WatermarkAccentColor[2], settings::config::WatermarkAccentColor[3])
		);

		draw_list->AddRectFilled(
			ImVec2(6, 6 + yOffset),
			ImVec2((4 * 2) + textSize.x + 6, 25 + yOffset),
			ImColor(settings::config::WatermarkBackgroundColor[0], settings::config::WatermarkBackgroundColor[1], settings::config::WatermarkBackgroundColor[2], settings::config::WatermarkBackgroundColor[3])
		);

		// Draw combined text
		draw_list->AddText(ImVec2(10, 6 + yOffset), ImColor(255, 255, 255, 255), mainStr.c_str());
	}
}
