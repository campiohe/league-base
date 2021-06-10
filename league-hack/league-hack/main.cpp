﻿#include "includes.h"

Hack hack;
Drawing d3d;

// Settings
struct ESP_CFG
{
	bool autoAttackRange = true;
	bool snaplines = true;
	bool box2D = true;
	bool statusText = true;
	bool predEsp = true;
};

struct AimLock_CFG
{
	int smooth = 200;
	int delay = 15;
};

struct Settings {
	ESP_CFG esp;
	AimLock_CFG aim;
}settings;


// Features
void ESP() {
	// Dummy vars
	vec2 vBoxOffset{ 0, 50 };

	// colors
	D3DCOLOR d3dRed = D3DCOLOR_ARGB(255, 255, 0, 0);
	D3DCOLOR d3dWhite = D3DCOLOR_ARGB(255, 255, 255, 255);
	D3DCOLOR d3dGreen = D3DCOLOR_ARGB(255, 0, 255, 0);

	d3d.sRender();

	// AA Range fake
	if (hack.eLocalEnt && settings.esp.autoAttackRange) {
		d3d.drawEllipse(hack.eLocalEnt->vPos.x, hack.eLocalEnt->vPos.y, hack.eLocalEnt->vScreenAARange.x, hack.eLocalEnt->vScreenAARange.y, 50, 2, d3dWhite);
	}

	for (int i = 0; i < hack.aEnemyEntList.size(); i++) {

		if (hack.eLocalEnt && settings.esp.statusText) {
			std::stringstream index, distance;
			std::string s;
			
			// Enemy Id
			index << hack.aEnemyEntList[i].Id;
			s = index.str();
			d3d.drawText((char*)s.c_str(), hack.aEnemyEntList[i].vPos.x, hack.aEnemyEntList[i].vPos.y, D3DCOLOR_ARGB(255, 255, 255, 255));

			// Enemy distance
			vec2 d = hack.aEnemyEntList[i].vPos - hack.eLocalEnt->vPos;
			distance << sqrt((d.x * d.x + d.y * d.y));
			s = distance.str();
			d3d.drawText((char*)s.c_str(), hack.aEnemyEntList[i].vPos.x + 22, hack.aEnemyEntList[i].vPos.y - 22, d3dWhite);
		}

		// box2d
		if (settings.esp.box2D)
			d3d.drawEspBox2D(hack.aEnemyEntList[i].vPos + vBoxOffset, hack.aEnemyEntList[i].vPos - vBoxOffset, 2, d3dRed);

		// snaplines
		if (hack.eLocalEnt && settings.esp.snaplines)
			d3d.drawLine(hack.eLocalEnt->vPos, hack.aEnemyEntList[i].vPos, 2, d3dRed);
		
		// predict lines
		if (settings.esp.predEsp) {
			vec2 vPredict = hack.PredictEnt(hack.aEnemyEntList[i], 0.3f);
			d3d.drawLine(hack.aEnemyEntList[i].vPos, hack.aEnemyEntList[i].vPos + vPredict, 2, d3dGreen);
		}

	}

	d3d.fRender();
	
}

void AimLock() {

	if (GetAsyncKeyState((short)'Q') || GetAsyncKeyState((short)'W') || GetAsyncKeyState((short)'R')) {
		Ent* enemy = hack.GetClosestEnemy(hack.MousePos());

		if (enemy) {
			hack.MouseMoveSmooth(settings.aim.smooth, settings.aim.delay, enemy->vPos);
			//hack.MouseMove(enemy->vPos);
		}	
	}
}

void Orbwalker​() {

	static bool bIsOrbAttackable = true;
	static bool bIsInside = false;
	static float fDelay = 0.0f;
	static float fAttackSpeed = 0.0f;

	vec2 vCursorPos = hack.MousePos();

	if (GetAsyncKeyState(VK_SPACE)) {

		if (!hack.eLocalEnt)
			return;

		Ent* enemy = hack.GetClosestEnemy(hack.eLocalEnt->vPos);

		if (!enemy)
			return;

		fAttackSpeed = hack.eLocalEnt->fAttackSpeed;

		bIsInside = hack.eLocalEnt->IsInside(*enemy, 1.5f);

		if (bIsOrbAttackable && bIsInside) {

			hack.MouseMove(enemy->vPos);
			hack.KeyboardPressKey('l');
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
			hack.MouseMove(vCursorPos);
			std::this_thread::sleep_for(std::chrono::milliseconds(80));
			hack.MouseRightClick(vCursorPos);

			fDelay = (hack.fGameTime * 1000.0f) + (1000.0f / fAttackSpeed);

			bIsOrbAttackable = false;
		}
		else if (hack.fGameTime * 1000.0f >= fDelay) {
			bIsOrbAttackable = true;
		}
	}
}


int main() {
    
	/*
		[?] Game Settings:
		 -  colorblind mode (on)
		 -  window mode (to overlay)
		 -  nivel de cor (75)
		 -  exibir barra de vida (on)
		 -  cam mode (fixed)
		 -  movimentação com attack (set any key conjunto 2)
		 -  bind auto attack move to left-click (off)
	*/

	static bool bClear = true;

	std::future<void> aim, orb;

    hack.Init();

    while (!GetAsyncKeyState(VK_HOME))
	{

    	if (hack.IsGameRunning()) 
		{
    		hack.Update();
			
    		ESP();
    		
			aim = std::async(std::launch::async, AimLock);
			orb = std::async(std::launch::async, Orbwalker​);

			bClear = false;
    	}
    	else if (!bClear) 
		{
    		d3d.clear();
			bClear = true;
    	}
    }

    d3d.clear();

    return 0;
}