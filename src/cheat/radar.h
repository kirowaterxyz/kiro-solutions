#pragma once

namespace radar {
	static Vector3 RotatePoint(Vector3 EntityPos, Vector3 LocalPlayerPos, int posX, int posY, int sizeX, int sizeY, float angle, float zoom, bool* viewCheck)
	{
		float r_1, r_2;
		float x_1, y_1;

		r_1 = -(EntityPos.y - LocalPlayerPos.y);
		r_2 = (EntityPos.x - LocalPlayerPos.x);
		float Yaw = angle - 90;

		float yawToRadian = Yaw * (float)(M_PI / 180.0F);
		x_1 = (float)(r_2 * (float)cos((double)(yawToRadian)) - r_1 * sin((double)(yawToRadian))) / 20;
		y_1 = (float)(r_2 * (float)sin((double)(yawToRadian)) + r_1 * cos((double)(yawToRadian))) / 20;

		if(viewCheck)
		*viewCheck = y_1 < 0;

		x_1 *= -zoom;
		y_1 *= zoom;

		int sizX = sizeX / 2;
		int sizY = sizeY / 2;

		x_1 += sizX;
		y_1 += sizY;

		if (x_1 < 5)
			x_1 = 5;

		if (x_1 > sizeX - 5)
			x_1 = sizeX - 5;

		if (y_1 < 5)
			y_1 = 5;

		if (y_1 > sizeY - 5)
			y_1 = sizeY - 5;


		x_1 += posX;
		y_1 += posY;


		return Vector3(x_1, y_1, 0);
	}

	void DrawPoint(ImDrawList* drawlist, Vector3 position, int distance, Vector3 localPosition, int scale, float zoom, double localYaw, ImVec2 pos, ImVec2 siz, ImColor color)
	{
		bool out = false;
		bool ck = false;
		Vector3 single = RotatePoint(position, localPosition, pos.x, pos.y, siz.x, siz.y, localYaw, zoom, &ck);
		if (distance >= 0.f && distance < scale)
		{
			drawlist->AddCircleFilled(ImVec2(single.x, single.y), 5, color);

			if (settings::config::RadarDistance)
			{
				std::string distanceStr = std::format("{}m", distance);
				ImVec2 textSize = ImGui::CalcTextSize(distanceStr.c_str());

				ImVec2 textPos = ImVec2(
					single.x - textSize.x / 2.0f,
					single.y + 6.0f // Below the dot
				);

				drawlist->AddText(textPos, IM_COL32(255, 255, 255, 255), distanceStr.c_str());
			}
		}
	}

	void Render()
	{

		if (!settings::config::Radar)
			return;

		std::unordered_map<uintptr_t, PlayerCache> PlayerList = secondPlayerList;

		ImGui::Begin("Radar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);

		ImVec2 RealSiz = ImGui::GetWindowSize();

		// minimum size
		{
			float minSize = 150;
			if (RealSiz.x < minSize) {
				ImGui::SetWindowSize(ImVec2(minSize, RealSiz.y));
			}
			if (RealSiz.y < minSize) {
				ImGui::SetWindowSize(ImVec2(RealSiz.x, minSize));
			}
		}

		ImVec2 siz = ImVec2(RealSiz.x - 10, RealSiz.y - 10);

		ImVec2 pos = ImGui::GetWindowPos();

		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 windowSize = ImGui::GetWindowSize();

		pos.x += 5;
		pos.y += 5;

		ImVec2 padding = ImVec2(5, 5);

		ImVec2 contentPos = ImVec2(windowPos.x + padding.x, windowPos.y + padding.y);
		ImVec2 contentSize = ImVec2(windowSize.x - 2 * padding.x, windowSize.y - 2 * padding.y);
		ImVec2 RadarCenter = ImVec2(contentPos.x + contentSize.x / 2, contentPos.y + contentSize.y / 2);

		// Access the window draw list
		ImDrawList* drawList = ImGui::GetWindowDrawList();



		if (!settings::config::RadarFuser) {
			ImU32 fillColor = IM_COL32(18, 18, 23, 255);
			ImU32 outlineColor = IM_COL32(204, 204, 212, 225); // rounding
			drawList->AddRectFilled(windowPos, ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y), fillColor);
			drawList->AddRect(windowPos, ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y), outlineColor, 10.f, 0, 2.0f); // rounding
			drawList->AddRectFilled(pos, ImVec2(pos.x + siz.x, pos.y + siz.y), fillColor);
		}

		if (settings::config::RadarDrawCenter)
		{
			ImU32 lineColor = IM_COL32(75, 75, 75, 255);
			drawList->AddLine(RadarCenter, ImVec2(pos.x + (siz.x * 0.10), pos.y), lineColor, 4.f);
			drawList->AddLine(RadarCenter, ImVec2(pos.x + (siz.x * 0.90), pos.y), lineColor, 4.f);
			drawList->AddLine(RadarCenter, ImVec2(pos.x + (siz.x / 2), pos.y + siz.y), lineColor, 5.f);
		}

		// draw the points in the radar
		for (auto it : PlayerList) {
			PlayerCache player = it.second;

			if (player.ignore)
				continue;

			if (!isPlayerValid(player))
				continue;

			if (player.PlayerState == point::PlayerState) {
				continue;
			}

			if (point::Player && player.TeamId == local_player::localTeam) {
				continue;
			}

			if (player.bisDying)
				continue;

			// should be in the updates
			bool IsVis = point::Seconds - player.last_render <= 0.06f;

			ImColor color = ImColor(255, 0, 0, 255);

			// the order matters

			// red not visible
			if (IsVis)
				color = ImColor(0, 255, 0, 255);

			// orange knocked players
			if (player.bisDBNO)
				color = ImColor(255, 100, 0, 255);

			// blue bots
			if (player.isBot)
				color = ImColor(0, 0, 255, 255);

			DrawPoint(drawList, player.Head3D, static_cast<int>(mainCamera.Location.Distance(player.Head3D) / 100.0f), mainCamera.Location, settings::config::RadarScale, settings::config::RadarZoom, mainCamera.Rotation.y, pos, siz, color);

		}

		ImGui::End();
	}
}