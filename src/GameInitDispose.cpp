/*
 * This file is part of Kugaru.
 *
 * Copyright (C) 2003, 2010 - Wolfire Games
 * Copyright (C) 2014 Victor A. Santos
 *
 * Kugaru is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Kugaru is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Kugaru.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "Game.h"
#include "Random.h"
#include "Utility.h"
#include "openal_wrapper.h"

extern float screenwidth,screenheight;
extern float viewdistance;
extern XYZ viewer;
extern XYZ lightlocation;
extern float lightambient[3],lightbrightness[3];
extern float fadestart;
extern float texscale;
extern float gravity;
extern Light light;
extern Animation animation[animation_count];
extern Skeleton testskeleton;
extern int numsounds;
extern OPENAL_SAMPLE	*samp[100];
extern int channels[100];
extern Terrain terrain;
extern Sprites sprites;
extern int kTextureSize;
extern float texdetail;
extern float realtexdetail;
extern float terraindetail;
extern float volume;
extern Objects objects;
extern int detail;
extern bool cellophane;
extern GLubyte bloodText[512*512*3];
extern GLubyte wolfbloodText[512*512*3];
extern bool ismotionblur;
extern bool trilinear;
extern bool osx;
extern bool musictoggle;
extern Weapons weapons;
extern Person player[maxplayers];
extern int numplayers;
extern int environment;
extern bool ambientsound;
extern float multiplier;
extern int netdatanew;
extern float mapinfo;
extern bool stillloading;
extern TGAImageRec texture;
extern short vRefNum;
extern long dirID;
extern int mainmenu;
extern int oldmainmenu;
extern bool visibleloading;
extern int loadscreencolor;
extern float flashamount,flashr,flashg,flashb;
extern int flashdelay;
extern int whichjointstartarray[26];
extern int whichjointendarray[26];
extern int difficulty;
extern float tintr,tintg,tintb;
extern float slomospeed;
extern char mapname[256];
extern bool gamestarted;

extern int numaccounts;
extern int accountactive;
extern int accountdifficulty[10];
extern int accountprogress[10];
extern float accountpoints[10];
extern float accounthighscore[10][50];
extern float accountfasttime[10][50];
extern bool accountunlocked[10][60];
extern char accountname[10][256];

extern int numdialogues;
extern int numdialogueboxes[20];
extern int dialoguetype[20];
extern int dialogueboxlocation[20][20];
extern float dialogueboxcolor[20][20][3];
extern int dialogueboxsound[20][20];
extern char dialoguetext[20][20][128];
extern char dialoguename[20][20][64];
extern XYZ dialoguecamera[20][20];
extern float dialoguecamerarotation[20][20];
extern float dialoguecamerarotation2[20][20];
extern int indialogue;
extern int whichdialogue;
extern float dialoguetime;

extern float accountcampaignhighscore[10];
extern float accountcampaignfasttime[10];
extern float accountcampaignscore[10];
extern float accountcampaigntime[10];

extern int accountcampaignchoicesmade[10];
extern int accountcampaignchoices[10][5000];

extern OPENAL_STREAM * strm[20];

extern "C"	void PlaySoundEx(int channel, OPENAL_SAMPLE *sptr, OPENAL_DSPUNIT *dsp, signed char startpaused);
extern "C" void PlayStreamEx(int chan, OPENAL_STREAM *sptr, OPENAL_DSPUNIT *dsp, signed char startpaused);

using namespace Kugaru;

void LOG(const std::string &fmt, ...)
{
    // !!! FIXME: write me.
}


Game::TextureList Game::textures;

void Game::Dispose()
{
	int i, j;

	LOGFUNC;

	if(endgame==2){
		accountcampaignchoicesmade[accountactive]=0;
		accountcampaignscore[accountactive]=0;
		accountcampaigntime[accountactive]=0;
		endgame=0;
	}


	sprintf (mapname, "data/Users");

	FILE *tfile;
	tfile = fopen( ConvertFileName(mapname), "wb" );
	if (tfile)
	{
		fpackf(tfile, "Bi", numaccounts);
		fpackf(tfile, "Bi", accountactive);
		if(numaccounts>0)
		{
			for(i=0;i<numaccounts;i++)
			{
				fpackf(tfile, "Bf", accountcampaigntime[i]);
				fpackf(tfile, "Bf", accountcampaignscore[i]);
				fpackf(tfile, "Bf", accountcampaignfasttime[i]);
				fpackf(tfile, "Bf", accountcampaignhighscore[i]);
				fpackf(tfile, "Bi", accountdifficulty[i]);
				fpackf(tfile, "Bi", accountprogress[i]);
				fpackf(tfile, "Bi", accountcampaignchoicesmade[i]);
				for(j=0;j<accountcampaignchoicesmade[i];j++)
				{
					fpackf(tfile, "Bi", accountcampaignchoices[i][j]);
				}
				fpackf(tfile, "Bf", accountpoints[i]);
				for(j=0;j<50;j++)
				{
					fpackf(tfile, "Bf", accounthighscore[i][j]);
					fpackf(tfile, "Bf", accountfasttime[i][j]);
				}
				for(j=0;j<60;j++)
				{
					fpackf(tfile, "Bb",  accountunlocked[i][j]);
				}
				fpackf(tfile, "Bi",  strlen(accountname[i]));
				if(strlen(accountname[i])>0)
				{
					for(j=0;j<(int)strlen(accountname[i]);j++)
					{
						fpackf(tfile, "Bb",  accountname[i][j]);
					}
				}
			}
		}

		fclose(tfile);
	}

	TexIter it = textures.begin();
	for (; it != textures.end(); ++it)
	{
		if (glIsTexture(it->second))
			glDeleteTextures(1, &it->second);
	}
	textures.clear();

	LOG("Shutting down sound system...");

	OPENAL_StopSound(OPENAL_ALL);

// this is causing problems on Linux, but we'll force an _exit() a little
//  later in the shutdown process.  --ryan.
#if !PLATFORM_LINUX
#define streamcount 20
#define samplecount 100

	for (i=0; i < samplecount; ++i)
	{
		OPENAL_Sample_Free(samp[i]);
	}

	for (i=0; i < streamcount; ++i)
	{
		OPENAL_Stream_Close(strm[i]);
	}

	OPENAL_Close();
	if (texture.data)
	{
		free(texture.data);
	}
	texture.data = 0;
#endif
}


//void Game::LoadSounds();
void Game::LoadSounds()
{
	LOGFUNC;

	LOG(std::string("Loading sounds..."));

	OPENAL_3D_SetDopplerFactor(0);

	OPENAL_SetSFXMasterVolume((int)(volume*255));

	samp[footstepsound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/footstepsnow1.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[footstepsound], 4.0f, 1000.0f);

	samp[footstepsound2] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/footstepsnow2.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[footstepsound2], 4.0f, 1000.0f);

	samp[footstepsound3] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/footstepstone1.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[footstepsound3], 4.0f, 1000.0f);

	samp[footstepsound4] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/footstepstone2.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[footstepsound4], 4.0f, 1000.0f);

	samp[landsound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/land.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[landsound], 4.0f, 1000.0f);

	samp[jumpsound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/jump.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[jumpsound], 4.0f, 1000.0f);

	samp[hawksound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/hawk.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[hawksound], 40.0f, 10000.0f);

	samp[whooshsound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/whoosh.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[whooshsound], 4.0f, 1000.0f);
	OPENAL_Sample_SetMode(samp[whooshsound], OPENAL_LOOP_NORMAL);

	samp[landsound1] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/land1.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[landsound1], 4.0f, 1000.0f);



	samp[landsound2] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/land2.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[landsound2], 4.0f, 1000.0f);

	samp[breaksound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/broken.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[breaksound], 8.0f, 2000.0f);

	samp[lowwhooshsound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/lowwhoosh.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[lowwhooshsound], 8.0f, 2000.0f);

	samp[midwhooshsound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/midwhoosh.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[midwhooshsound], 8.0f, 2000.0f);

	samp[highwhooshsound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/highwhoosh.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[highwhooshsound], 8.0f, 2000.0f);

	samp[movewhooshsound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/movewhoosh.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[movewhooshsound], 8.0f, 2000.0f);

	samp[heavyimpactsound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/heavyimpact.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[heavyimpactsound], 8.0f, 2000.0f);

	samp[whooshhitsound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/whooshhit.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[whooshhitsound], 8.0f, 2000.0f);

	samp[thudsound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/thud.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[thudsound], 8.0f, 2000.0f);

	samp[alarmsound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/alarm.ogg"), OPENAL_2D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[alarmsound], 8.0f, 2000.0f);

	samp[breaksound2] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/break.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[breaksound2], 8.0f, 2000.0f);

	samp[knifedrawsound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/knifedraw.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[knifedrawsound], 8.0f, 2000.0f);

	samp[knifesheathesound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/knifesheathe.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[knifesheathesound], 8.0f, 2000.0f);

	samp[fleshstabsound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/fleshstab.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[fleshstabsound], 8.0f, 2000.0f);

	samp[fleshstabremovesound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/fleshstabremove.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[fleshstabremovesound], 8.0f, 2000.0f);

	samp[knifeswishsound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/knifeswish.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[knifeswishsound], 8.0f, 2000.0f);

	samp[knifeslicesound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/knifeslice.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[knifeslicesound], 8.0f, 2000.0f);

	samp[swordslicesound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/swordslice.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[swordslicesound], 8.0f, 2000.0f);

	samp[skidsound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/skid.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[skidsound], 8.0f, 2000.0f);

	samp[snowskidsound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/snowskid.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[snowskidsound], 8.0f, 2000.0f);

	samp[bushrustle] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/bushrustle.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[bushrustle], 4.0f, 1000.0f);

	samp[clank1sound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/clank1.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[clank1sound], 8.0f, 2000.0f);

	samp[clank2sound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/clank2.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[clank2sound], 8.0f, 2000.0f);

	samp[clank3sound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/clank3.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[clank3sound], 8.0f, 2000.0f);

	samp[clank4sound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/clank4.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[clank4sound], 8.0f, 2000.0f);

	samp[consolesuccesssound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/consolesuccess.ogg"), OPENAL_2D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[consolesuccesssound], 4.0f, 1000.0f);

	samp[consolefailsound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/consolefail.ogg"), OPENAL_2D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[consolefailsound], 4.0f, 1000.0f);

	samp[metalhitsound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/metalhit.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[metalhitsound], 8.0f, 2000.0f);

	samp[clawslicesound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/clawslice.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[clawslicesound], 8.0f, 2000.0f);

	samp[splattersound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/splatter.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[splattersound], 8.0f, 2000.0f);

	samp[growlsound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/growl.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[growlsound], 1000.0f, 2000.0f);

	samp[growl2sound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/growl2.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[growl2sound], 1000.0f, 2000.0f);

	samp[barksound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/bark.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[barksound], 1000.0f, 2000.0f);

	samp[bark2sound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/bark2.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[bark2sound], 1000.0f, 2000.0f);

	samp[bark3sound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/bark3.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[bark3sound], 1000.0f, 2000.0f);

	samp[snarlsound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/snarl.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[snarlsound], 1000.0f, 2000.0f);


	samp[snarl2sound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/snarl2.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[snarl2sound], 1000.0f, 2000.0f);

	samp[barkgrowlsound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/barkgrowl.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[barkgrowlsound], 1000.0f, 2000.0f);

	samp[rabbitattacksound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/rabbitattack.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[rabbitattacksound], 1000.0f, 2000.0f);

	samp[rabbitattack2sound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/rabbitattack2.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[rabbitattack2sound], 1000.0f, 2000.0f);

	samp[rabbitattack3sound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/rabbitattack3.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[rabbitattack3sound], 1000.0f, 2000.0f);

	samp[rabbitattack4sound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/rabbitattack4.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[rabbitattack4sound], 1000.0f, 2000.0f);

	samp[rabbitpainsound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/rabbitpain.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[rabbitpainsound], 1000.0f, 2000.0f);

	samp[rabbitpain1sound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/rabbitpain2.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[rabbitpain1sound], 1000.0f, 2000.0f);

	/*samp[rabbitpain2sound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/rabbitpain2.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[rabbitpain2sound], 1000.0f, 2000.0f);
	*/
	samp[rabbitchitter] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/rabbitchitter.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[rabbitchitter], 1000.0f, 2000.0f);

	samp[rabbitchitter2] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/rabbitchitter2.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[rabbitchitter2], 1000.0f, 2000.0f);

	samp[swordstaffsound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/swordstaff.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[swordstaffsound], 8.0f, 2000.0f);

	samp[staffbodysound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/staffbody.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[staffbodysound], 8.0f, 2000.0f);

	samp[staffheadsound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/staffhead.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[staffheadsound], 8.0f, 2000.0f);

	samp[staffbreaksound] = OPENAL_Sample_LoadEx(OPENAL_FREE, ConvertFileName("data/sounds/staffbreak.ogg"), OPENAL_HW3D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[staffbreaksound], 8.0f, 2000.0f);
}

void Game::LoadTexture(const char *fileName, GLuint *textureid,int mipmap, bool hasalpha)
{
	GLuint		type;

	LOGFUNC;

	LOG(std::string("Loading texture...") + fileName);

	// Fix filename so that is os appropreate
	const char *fixedFN = ConvertFileName(fileName);

	//Load Image
	upload_image(fixedFN, hasalpha);

//	std::string fname(fileName);
//	std::transform(fname.begin(), fname.end(), tolower);
//	TexIter it = textures.find(fname);

	//Is it valid?
	if(1==1)
	//if(textures.end() == it)
	{
		//Alpha channel?
		if ( texture.bpp == 24 )
			type = GL_RGB;
		else
			type = GL_RGBA;

		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

		if(!*textureid)glGenTextures( 1, textureid );
		glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

		glBindTexture( GL_TEXTURE_2D, *textureid);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		if(trilinear)if(mipmap)glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		if(!trilinear)if(mipmap)glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
		if(!mipmap)glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.sizeX, texture.sizeY, 0,
		//          GL_BGRA_EXT, GL_UNSIGNED_INT_8_8_8_8_REV, texture.data);

		//gluBuild2DMipmaps( GL_TEXTURE_2D, type, texture.sizeX, texture.sizeY, GL_BGRA_EXT, GL_UNSIGNED_INT_8_8_8_8_REV, texture.data );

		gluBuild2DMipmaps( GL_TEXTURE_2D, type, texture.sizeX, texture.sizeY, type, GL_UNSIGNED_BYTE, texture.data );

//		textures.insert(std::make_pair(fname, *textureid));
	}
//	else
//	{
//		*textureid = it->second;
//	}
}

void Game::LoadTextureSave(const char *fileName, GLuint *textureid,int mipmap,GLubyte *array, int *skinsize)
{
	GLuint		type;
	int i;
	int bytesPerPixel;

	LOGFUNC;

	LOG(std::string("Loading texture (S)...") + fileName);

	//Load Image
	const char *fixedFN = ConvertFileName(fileName);
	upload_image(fixedFN, 0);
	//LoadTGA( fileName );

//	std::string fname(fileName);
//	std::transform(fname.begin(), fname.end(), tolower);
//	TexIter it = textures.find(fname);

	//Is it valid?
	if(1==1)
	//if(textures.end() == it)
	{
		bytesPerPixel=texture.bpp/8;

		//Alpha channel?
		if ( texture.bpp == 24 )
			type = GL_RGB;
		else
			type = GL_RGBA;

		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

		if(!*textureid)glGenTextures( 1, textureid );
		glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

		glBindTexture( GL_TEXTURE_2D, *textureid);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		if(trilinear)if(mipmap)glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		if(!trilinear)if(mipmap)glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
		if(!mipmap)glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

		int tempnum=0;
		for(i=0;i<(int)(texture.sizeY*texture.sizeX*bytesPerPixel);i++){
			if((i+1)%4||type==GL_RGB){
				array[tempnum]=texture.data[i];
				tempnum++;
			}
		}

		*skinsize=texture.sizeX;

		gluBuild2DMipmaps( GL_TEXTURE_2D, type, texture.sizeX, texture.sizeY, GL_RGB, GL_UNSIGNED_BYTE, array );

//		textures.insert(std::make_pair(fname, *textureid));
	}
//	else
//	{
//		*textureid = it->second;
//	}
}

void Game::LoadSave(const char *fileName, GLuint *textureid,bool mipmap,GLubyte *array, int *skinsize)
{
	int i;
	int bytesPerPixel;

	LOGFUNC;

	LOG(std::string("Loading (S)...") + fileName);

	//Load Image
	float temptexdetail=texdetail;
	texdetail=1;
	//upload_image( fileName );
	//LoadTGA( fileName );

	// Converting file to something os specific
	const char *fixedFN = ConvertFileName(fileName);

	//Load Image
	upload_image(fixedFN, 0);
	texdetail=temptexdetail;

	//Is it valid?
	if(1==1){
		bytesPerPixel=texture.bpp/8;

		int tempnum=0;
		for(i=0;i<(int)(texture.sizeY*texture.sizeX*bytesPerPixel);i++){
			if((i+1)%4||bytesPerPixel==3){
				array[tempnum]=texture.data[i];
				tempnum++;
			}
		}
	}
}

bool Game::AddClothes(const char *fileName, GLuint *textureid,bool mipmap,GLubyte *array, int *skinsize)
{
	int i;
	int bytesPerPixel;

	LOGFUNC;

	//upload_image( fileName );
	//LoadTGA( fileName );
	//Load Image
	bool opened;
	opened = upload_image(fileName, 1);

	float alphanum;
	//Is it valid?
	if(opened){
		if(tintr>1)tintr=1;
		if(tintg>1)tintg=1;
		if(tintb>1)tintb=1;

		if(tintr<0)tintr=0;
		if(tintg<0)tintg=0;
		if(tintb<0)tintb=0;

		bytesPerPixel=texture.bpp/8;

		int tempnum=0;
		alphanum=255;
		for(i=0;i<(int)(texture.sizeY*texture.sizeX*bytesPerPixel);i++){
			if(bytesPerPixel==3)alphanum=255;
			else if((i+1)%4==0)alphanum=texture.data[i];
			//alphanum/=2;
			if((i+1)%4||bytesPerPixel==3){
				if((i%4)==0)texture.data[i]*=tintr;
				if((i%4)==1)texture.data[i]*=tintg;
				if((i%4)==2)texture.data[i]*=tintb;
				array[tempnum]=(float)array[tempnum]*(1-alphanum/255)+(float)texture.data[i]*(alphanum/255);
				tempnum++;
			}
		}
	}
	else return 0;
	return 1;
}


//***************> ResizeGLScene() <******/
GLvoid Game::ReSizeGLScene(float fov, float pnear)
{
	if (screenheight==0)
	{
		screenheight=1;
	}

	glViewport(0,0,screenwidth,screenheight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(fov,(GLfloat)screenwidth/(GLfloat)screenheight,pnear,viewdistance);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Game::LoadingScreen()
{
	static float loadprogress,minprogress,maxprogress;
	static AbsoluteTime time = {0,0};
	static AbsoluteTime frametime = {0,0};
	AbsoluteTime currTime = UpTime ();
	double deltaTime = (float) AbsoluteDeltaToDuration (currTime, frametime);

	if (0 > deltaTime)	// if negative microseconds
		deltaTime /= -1000000.0;
	else				// else milliseconds
		deltaTime /= 1000.0;

	multiplier=deltaTime;
	if(multiplier<.001)multiplier=.001;
	if(multiplier>10)multiplier=10;
	if(multiplier>.05){
		frametime = currTime;	// reset for next time interval

		float size=1;
		glLoadIdentity();
		//Clear to black
		glClearColor(0,0,0,1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		loadtime+=multiplier*4;

		loadprogress=loadtime;
		if(loadprogress>100)loadprogress=100;

		//loadprogress=abs(Random()%100);

		//Background

		glEnable(GL_TEXTURE_2D);
		glBindTexture( GL_TEXTURE_2D, loadscreentexture);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
		glDisable(GL_CULL_FACE);
		glDisable(GL_LIGHTING);
		glDepthMask(0);
		glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
		glPushMatrix();										// Store The Projection Matrix
		glLoadIdentity();									// Reset The Projection Matrix
		glOrtho(0,screenwidth,0,screenheight,-100,100);						// Set Up An Ortho Screen
		glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
		glPushMatrix();										// Store The Modelview Matrix
		glLoadIdentity();								// Reset The Modelview Matrix
		glTranslatef(screenwidth/2,screenheight/2,0);
		glScalef((float)screenwidth/2,(float)screenheight/2,1);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_BLEND);
		glColor4f(loadprogress/100,loadprogress/100,loadprogress/100,1);
		//glColor4f(1,1,1,1);
		/*if(loadscreencolor==0)glColor4f(1,1,1,1);
		if(loadscreencolor==1)glColor4f(1,0,0,1);
		if(loadscreencolor==2)glColor4f(0,1,0,1);
		if(loadscreencolor==3)glColor4f(0,0,1,1);
		if(loadscreencolor==4)glColor4f(1,1,0,1);
		if(loadscreencolor==5)glColor4f(1,0,1,1);
		*/
		glPushMatrix();
		//glScalef(.25,.25,.25);
		glBegin(GL_QUADS);
		glTexCoord2f(.1-loadprogress/100,0+loadprogress/100+.3);
		glVertex3f(-1,		-1,	 0.0f);
		glTexCoord2f(.1-loadprogress/100,0+loadprogress/100+.3);
		glVertex3f(1,	-1,	 0.0f);
		glTexCoord2f(.1-loadprogress/100,1+loadprogress/100+.3);
		glVertex3f(1,	1, 0.0f);
		glTexCoord2f(.1-loadprogress/100,1+loadprogress/100+.3);
		glVertex3f(-1,	1, 0.0f);
		glEnd();
		glPopMatrix();
		glEnable(GL_BLEND);
		glPushMatrix();
		//glScalef(.25,.25,.25);
		glBegin(GL_QUADS);
		glTexCoord2f(.4+loadprogress/100,0+loadprogress/100);
		glVertex3f(-1,		-1,	 0.0f);
		glTexCoord2f(.4+loadprogress/100,0+loadprogress/100);
		glVertex3f(1,	-1,	 0.0f);
		glTexCoord2f(.4+loadprogress/100,1+loadprogress/100);
		glVertex3f(1,	1, 0.0f);
		glTexCoord2f(.4+loadprogress/100,1+loadprogress/100);
		glVertex3f(-1,	1, 0.0f);
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
		glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
		glPopMatrix();										// Restore The Old Projection Matrix
		glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
		glPopMatrix();										// Restore The Old Projection Matrix
		glDisable(GL_BLEND);
		glDepthMask(1);

		glEnable(GL_TEXTURE_2D);
		glBindTexture( GL_TEXTURE_2D, loadscreentexture);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
		glDisable(GL_CULL_FACE);
		glDisable(GL_LIGHTING);
		glDepthMask(0);
		glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
		glPushMatrix();										// Store The Projection Matrix
		glLoadIdentity();									// Reset The Projection Matrix
		glOrtho(0,screenwidth,0,screenheight,-100,100);						// Set Up An Ortho Screen
		glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
		glPushMatrix();										// Store The Modelview Matrix
		glLoadIdentity();								// Reset The Modelview Matrix
		glTranslatef(screenwidth/2,screenheight/2,0);
		glScalef((float)screenwidth/2*(1.5-(loadprogress)/200),(float)screenheight/2*(1.5-(loadprogress)/200),1);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		glEnable(GL_BLEND);
		//glColor4f(loadprogress/100,loadprogress/100,loadprogress/100,1);
		glColor4f(loadprogress/100,loadprogress/100,loadprogress/100,1);
		/*if(loadscreencolor==0)glColor4f(1,1,1,1);
		if(loadscreencolor==1)glColor4f(1,0,0,1);
		if(loadscreencolor==2)glColor4f(0,1,0,1);
		if(loadscreencolor==3)glColor4f(0,0,1,1);
		if(loadscreencolor==4)glColor4f(1,1,0,1);
		if(loadscreencolor==5)glColor4f(1,0,1,1);
		*/
		glPushMatrix();
		//glScalef(.25,.25,.25);
		glBegin(GL_QUADS);
		glTexCoord2f(0+.5,0+.5);
		glVertex3f(-1,		-1,	 0.0f);
		glTexCoord2f(1+.5,0+.5);
		glVertex3f(1,	-1,	 0.0f);
		glTexCoord2f(1+.5,1+.5);
		glVertex3f(1,	1, 0.0f);
		glTexCoord2f(0+.5,1+.5);
		glVertex3f(-1,	1, 0.0f);
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
		glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
		glPopMatrix();										// Restore The Old Projection Matrix
		glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
		glPopMatrix();										// Restore The Old Projection Matrix
		glDisable(GL_BLEND);
		glDepthMask(1);

		glEnable(GL_TEXTURE_2D);
		glBindTexture( GL_TEXTURE_2D, loadscreentexture);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
		glDisable(GL_CULL_FACE);
		glDisable(GL_LIGHTING);
		glDepthMask(0);
		glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
		glPushMatrix();										// Store The Projection Matrix
		glLoadIdentity();									// Reset The Projection Matrix
		glOrtho(0,screenwidth,0,screenheight,-100,100);						// Set Up An Ortho Screen
		glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
		glPushMatrix();										// Store The Modelview Matrix
		glLoadIdentity();								// Reset The Modelview Matrix
		glTranslatef(screenwidth/2,screenheight/2,0);
		glScalef((float)screenwidth/2*(100+loadprogress)/100,(float)screenheight/2*(100+loadprogress)/100,1);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		glEnable(GL_BLEND);
		glColor4f(loadprogress/100,loadprogress/100,loadprogress/100,.4);
		glPushMatrix();
		//glScalef(.25,.25,.25);
		glBegin(GL_QUADS);
		glTexCoord2f(0+.2,0+.8);
		glVertex3f(-1,		-1,	 0.0f);
		glTexCoord2f(1+.2,0+.8);
		glVertex3f(1,	-1,	 0.0f);
		glTexCoord2f(1+.2,1+.8);
		glVertex3f(1,	1, 0.0f);
		glTexCoord2f(0+.2,1+.8);
		glVertex3f(-1,	1, 0.0f);
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
		glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
		glPopMatrix();										// Restore The Old Projection Matrix
		glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
		glPopMatrix();										// Restore The Old Projection Matrix
		glDisable(GL_BLEND);
		glDepthMask(1);

		//Text
		/*
		glEnable(GL_TEXTURE_2D);
		static char string[256]="";
		sprintf (string, "LOADING... %d%",(int)loadprogress);
		glColor4f(1,1,1,.2);
		text.glPrint(280-280*loadprogress/100/2/4,125-125*loadprogress/100/2/4,string,1,1+loadprogress/100,640,480);
		glColor4f(1.2-loadprogress/100,1.2-loadprogress/100,1.2-loadprogress/100,1);
		text.glPrint(280,125,string,1,1,640,480);
		*/

		if(flashamount>0){
			if(flashamount>1)flashamount=1;
			if(flashdelay<=0)flashamount-=multiplier;
			flashdelay--;
			if(flashamount<0)flashamount=0;
			glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
			glDisable(GL_CULL_FACE);
			glDisable(GL_LIGHTING);
			glDisable(GL_TEXTURE_2D);
			glDepthMask(0);
			glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
			glPushMatrix();										// Store The Projection Matrix
			glLoadIdentity();									// Reset The Projection Matrix
			glOrtho(0,screenwidth,0,screenheight,-100,100);						// Set Up An Ortho Screen
			glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
			glPushMatrix();										// Store The Modelview Matrix
			glLoadIdentity();								// Reset The Modelview Matrix
			glScalef(screenwidth,screenheight,1);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
			glColor4f(flashr,flashg,flashb,flashamount);
			glBegin(GL_QUADS);
			glVertex3f(0,		0,	 0.0f);
			glVertex3f(256,	0,	 0.0f);
			glVertex3f(256,	256, 0.0f);
			glVertex3f(0,	256, 0.0f);
			glEnd();
			glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
			glPopMatrix();										// Restore The Old Projection Matrix
			glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
			glPopMatrix();										// Restore The Old Projection Matrix
			glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
			glEnable(GL_CULL_FACE);
			glDisable(GL_BLEND);
			glDepthMask(1);
		}

		swap_gl_buffers();
		loadscreencolor=0;
	}
}

void Game::FadeLoadingScreen(float howmuch)
{
	static float loadprogress,minprogress,maxprogress;

	float size=1;
	glLoadIdentity();
	//Clear to black
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	loadprogress=howmuch;

	//loadprogress=abs(Random()%100);

	//Background

	//glEnable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_2D);
	//glBindTexture( GL_TEXTURE_2D, loadscreentexture);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glDepthMask(0);
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	glOrtho(0,screenwidth,0,screenheight,-100,100);						// Set Up An Ortho Screen
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPushMatrix();										// Store The Modelview Matrix
	glLoadIdentity();								// Reset The Modelview Matrix
	glTranslatef(screenwidth/2,screenheight/2,0);
	glScalef((float)screenwidth/2,(float)screenheight/2,1);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
	glColor4f(loadprogress/100,0,0,1);
	/*if(loadscreencolor==0)glColor4f(1,1,1,1);
	if(loadscreencolor==1)glColor4f(1,0,0,1);
	if(loadscreencolor==2)glColor4f(0,1,0,1);
	if(loadscreencolor==3)glColor4f(0,0,1,1);
	if(loadscreencolor==4)glColor4f(1,1,0,1);
	if(loadscreencolor==5)glColor4f(1,0,1,1);
	*/
	glPushMatrix();
	//glScalef(.25,.25,.25);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex3f(-1,		-1,	 0.0f);
	glTexCoord2f(1,0);
	glVertex3f(1,	-1,	 0.0f);
	glTexCoord2f(1,1);
	glVertex3f(1,	1, 0.0f);
	glTexCoord2f(0,1);
	glVertex3f(-1,	1, 0.0f);
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glDisable(GL_BLEND);
	glDepthMask(1);
	//Text
	/*
	glEnable(GL_TEXTURE_2D);
	static char string[256]="";
	sprintf (string, "LOADING... %d%",(int)loadprogress);
	glColor4f(1,1,1,.2);
	text.glPrint(280-280*loadprogress/100/2/4,125-125*loadprogress/100/2/4,string,1,1+loadprogress/100,640,480);
	glColor4f(1.2-loadprogress/100,1.2-loadprogress/100,1.2-loadprogress/100,1);
	text.glPrint(280,125,string,1,1,640,480);
	*/
	swap_gl_buffers();
	loadscreencolor=0;
}


void Game::InitGame()
{
#if PLATFORM_MACOSX
	ProcessSerialNumber PSN;
	ProcessInfoRec pinfo;
	FSSpec pspec;
	OSStatus err;
	/* set up process serial number */
	PSN.highLongOfPSN = 0;
	PSN.lowLongOfPSN = kCurrentProcess;
	/* set up info block */
	pinfo.processInfoLength = sizeof(pinfo);
	pinfo.processName = NULL;
	pinfo.processAppSpec = &pspec;
	/* grab the vrefnum and directory */
	err = GetProcessInformation(&PSN, &pinfo);
	if (err == noErr) {
		vRefNum = pspec.vRefNum;
		dirID = pspec.parID;
	}
#endif

	LOGFUNC;

	autocam=0;

	int i,j;

	numchallengelevels=14;

	/*char tempstring[256];
	sprintf (tempstring, "%s", registrationname);
	long num1;
	long num2;
	long num3;
	long num4;
	long long longnum;
	longnum = MD5_string ( tempstring);
	//longnum = 1111111111111111;
	num1 = longnum/100000000;
	num2 = longnum%100000000;
	sprintf (tempstring, "%d-%d-%d-%d", num1/10000, num1%10000, num2/10000, num2%10000);
	*/

	FILE			*tfile;

	accountactive=-1;

	sprintf (mapname, "data/Users");
	tfile=fopen( ConvertFileName(mapname), "rb" );
	if(tfile)
	{
		funpackf(tfile, "Bi", &numaccounts);
		funpackf(tfile, "Bi", &accountactive);
		if(numaccounts>0)
		{
			for(i=0;i<numaccounts;i++)
			{
				funpackf(tfile, "Bf", &accountcampaigntime[i]);
				funpackf(tfile, "Bf", &accountcampaignscore[i]);
				funpackf(tfile, "Bf", &accountcampaignfasttime[i]);
				funpackf(tfile, "Bf", &accountcampaignhighscore[i]);
				funpackf(tfile, "Bi", &accountdifficulty[i]);
				funpackf(tfile, "Bi", &accountprogress[i]);
				funpackf(tfile, "Bi", &accountcampaignchoicesmade[i]);
				for(j=0;j<accountcampaignchoicesmade[i];j++)
				{
					funpackf(tfile, "Bi", &accountcampaignchoices[i][j]);
					if (accountcampaignchoices[i][j] >= 10)
					{
						accountcampaignchoices[i][j] = 0;
					}
				}
				funpackf(tfile, "Bf", &accountpoints[i]);
				for(j=0;j<50;j++)
				{
					funpackf(tfile, "Bf", &accounthighscore[i][j]);
					funpackf(tfile, "Bf", &accountfasttime[i][j]);
				}
				for(j=0;j<60;j++)
				{
					funpackf(tfile, "Bb",  &accountunlocked[i][j]);
				}
				int temp;
				funpackf(tfile, "Bi",  &temp);
				if(temp>0)
				{
					for(j=0;j<temp;j++)
					{
						funpackf(tfile, "Bb",  &accountname[i][j]);
					}
				}
			}
		}

		fclose(tfile);
	}

	tintr=1;
	tintg=1;
	tintb=1;

	whichjointstartarray[0]=righthip;
	whichjointendarray[0]=rightfoot;

	whichjointstartarray[1]=righthip;
	whichjointendarray[1]=rightankle;

	whichjointstartarray[2]=righthip;
	whichjointendarray[2]=rightknee;

	whichjointstartarray[3]=rightknee;
	whichjointendarray[3]=rightankle;

	whichjointstartarray[4]=rightankle;
	whichjointendarray[4]=rightfoot;

	whichjointstartarray[5]=lefthip;
	whichjointendarray[5]=leftfoot;

	whichjointstartarray[6]=lefthip;
	whichjointendarray[6]=leftankle;

	whichjointstartarray[7]=lefthip;
	whichjointendarray[7]=leftknee;

	whichjointstartarray[8]=leftknee;
	whichjointendarray[8]=leftankle;

	whichjointstartarray[9]=leftankle;
	whichjointendarray[9]=leftfoot;

	whichjointstartarray[10]=abdomen;
	whichjointendarray[10]=rightshoulder;

	whichjointstartarray[11]=abdomen;
	whichjointendarray[11]=rightelbow;

	whichjointstartarray[12]=abdomen;
	whichjointendarray[12]=rightwrist;

	whichjointstartarray[13]=abdomen;
	whichjointendarray[13]=righthand;

	whichjointstartarray[14]=rightshoulder;
	whichjointendarray[14]=rightelbow;

	whichjointstartarray[15]=rightelbow;
	whichjointendarray[15]=rightwrist;

	whichjointstartarray[16]=rightwrist;
	whichjointendarray[16]=righthand;

	whichjointstartarray[17]=abdomen;
	whichjointendarray[17]=leftshoulder;

	whichjointstartarray[18]=abdomen;
	whichjointendarray[18]=leftelbow;

	whichjointstartarray[19]=abdomen;
	whichjointendarray[19]=leftwrist;

	whichjointstartarray[20]=abdomen;
	whichjointendarray[20]=lefthand;

	whichjointstartarray[21]=leftshoulder;
	whichjointendarray[21]=leftelbow;

	whichjointstartarray[22]=leftelbow;
	whichjointendarray[22]=leftwrist;

	whichjointstartarray[23]=leftwrist;
	whichjointendarray[23]=lefthand;

	whichjointstartarray[24]=abdomen;
	whichjointendarray[24]=neck;

	whichjointstartarray[25]=neck;
	whichjointendarray[25]=head;

	FadeLoadingScreen(0);

	stillloading=1;

	texture.data = ( GLubyte* )malloc( 1024*1024*4 );

	int temptexdetail=texdetail;
	texdetail=1;
	text.LoadFontTexture("data/textures/font.png");
	text.BuildFont();
	texdetail=temptexdetail;

	FadeLoadingScreen(10);

	if(detail==2){
		texdetail=1;
		terraindetail=1;
	}
	if(detail==1){
		texdetail=2;
		terraindetail=1;
	}
	if(detail==0){
		texdetail=4;
		terraindetail=1;
		//terraindetail=2;
	}

	for (int it = 0; it < 100; ++it)
	{
		channels[it] = -1;
		samp[it] = NULL;
	}
	for (int it = 0; it < 20; ++it)
	{
		strm[it] = NULL;
	}

	LOG("Initializing sound system...");

    int output = -1;

    #if PLATFORM_LINUX
    extern bool cmdline(const char *cmd);
    unsigned char rc = 0;
    output = OPENAL_OUTPUT_ALSA;  // Try alsa first...
    if (cmdline("forceoss"))      //  ...but let user override that.
        output = OPENAL_OUTPUT_OSS;
    else if (cmdline("nosound"))
        output = OPENAL_OUTPUT_NOSOUND;

    OPENAL_SetOutput(output);
	if ((rc = OPENAL_Init(44100, 32, 0)) == false)
    {
        // if we tried ALSA and failed, fall back to OSS.
        if ( (output == OPENAL_OUTPUT_ALSA) && (!cmdline("forcealsa")) )
        {
            OPENAL_Close();
            output = OPENAL_OUTPUT_OSS;
            OPENAL_SetOutput(output);
	        rc = OPENAL_Init(44100, 32, 0);
        }
    }

    if (rc == false)
    {
        OPENAL_Close();
        output = OPENAL_OUTPUT_NOSOUND;  // we tried! just do silence.
        OPENAL_SetOutput(output);
	    rc = OPENAL_Init(44100, 32, 0);
    }
    #else
	OPENAL_Init(44100, 32, 0);
    #endif

	OPENAL_SetSFXMasterVolume((int)(volume*255));

	strm[stream_music3] = OPENAL_Stream_Open(ConvertFileName("data/sounds/music3.ogg"), OPENAL_2D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=0;}
//	OPENAL_Sample_SetMinMaxDistance(strm[stream_music3], 4.0f, 1000.0f);
	OPENAL_Stream_SetMode(strm[stream_music3], OPENAL_LOOP_NORMAL);

	if(musictoggle){
//		PlaySoundEx( stream_music3, strm[stream_music3], NULL, true);
		PlayStreamEx(stream_music3, strm[stream_music3], 0, true);
		OPENAL_SetPaused(channels[stream_music3], false);
		OPENAL_SetVolume(channels[stream_music3], 256);
	}

	FadeLoadingScreen(20);

	if(ambientsound){
		strm[stream_wind] = OPENAL_Stream_Open(ConvertFileName("data/sounds/wind.ogg"), OPENAL_2D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
//		OPENAL_Sample_SetMinMaxDistance(strm[stream_wind], 4.0f, 1000.0f);
		OPENAL_Stream_SetMode(strm[stream_wind], OPENAL_LOOP_NORMAL);

		FadeLoadingScreen(30);

		strm[stream_desertambient] = OPENAL_Stream_Open(ConvertFileName("data/sounds/desertambient.ogg"), OPENAL_2D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
//		OPENAL_Sample_SetMinMaxDistance(strm[stream_desertambient], 4.0f, 1000.0f);
		OPENAL_Stream_SetMode(strm[stream_desertambient], OPENAL_LOOP_NORMAL);
	}

	FadeLoadingScreen(40);

	samp[firestartsound] = OPENAL_Sample_Load(OPENAL_FREE, ConvertFileName("data/sounds/firestart.ogg"), OPENAL_2D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[firestartsound], 8.0f, 2000.0f);

	strm[stream_firesound] = OPENAL_Stream_Open(ConvertFileName("data/sounds/fire.ogg"), OPENAL_2D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
//	OPENAL_Sample_SetMinMaxDistance(strm[stream_firesound], 8.0f, 2000.0f);
	OPENAL_Stream_SetMode(strm[stream_firesound], OPENAL_LOOP_NORMAL);

	FadeLoadingScreen(50);

	samp[fireendsound] = OPENAL_Sample_Load(OPENAL_FREE, ConvertFileName("data/sounds/fireend.ogg"), OPENAL_2D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=5;}
	OPENAL_Sample_SetMinMaxDistance(samp[fireendsound], 8.0f, 2000.0f);

	//if(musictoggle){
	strm[stream_music1grass] = OPENAL_Stream_Open(ConvertFileName("data/sounds/music1grass.ogg"), OPENAL_2D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=1;}
//	OPENAL_Sample_SetMinMaxDistance(strm[stream_music1grass], 4.0f, 1000.0f);
	OPENAL_Stream_SetMode(strm[stream_music1grass], OPENAL_LOOP_NORMAL);

	strm[stream_music1snow] = OPENAL_Stream_Open(ConvertFileName("data/sounds/music1snow.ogg"), OPENAL_2D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=2;}
//	OPENAL_Sample_SetMinMaxDistance(strm[stream_music1snow], 4.0f, 1000.0f);
	OPENAL_Stream_SetMode(strm[stream_music1snow], OPENAL_LOOP_NORMAL);

	FadeLoadingScreen(60);

	strm[stream_music1desert] = OPENAL_Stream_Open(ConvertFileName("data/sounds/music1desert.ogg"), OPENAL_2D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=3;}
//	OPENAL_Sample_SetMinMaxDistance(strm[stream_music1desert], 4.0f, 1000.0f);
	OPENAL_Stream_SetMode(strm[stream_music1desert], OPENAL_LOOP_NORMAL);

	FadeLoadingScreen(80);
	strm[stream_music2] = OPENAL_Stream_Open(ConvertFileName("data/sounds/music2.ogg"), OPENAL_2D, 0, 0); if(visibleloading){LoadingScreen(); loadscreencolor=4;}
//	OPENAL_Sample_SetMinMaxDistance(strm[stream_music2], 4.0f, 1000.0f);
	OPENAL_Stream_SetMode(strm[stream_music2], OPENAL_LOOP_NORMAL);

	//}


	FadeLoadingScreen(90);


	LoadTexture("data/textures/cursor.png",&cursortexture,0,1);

	LoadTexture("data/textures/mapcircle.png",&Mapcircletexture,0,1);
	LoadTexture("data/textures/mapbox.png",&Mapboxtexture,0,1);
	LoadTexture("data/textures/maparrow.png",&Maparrowtexture,0,1);

	temptexdetail=texdetail;
	if(texdetail>2)texdetail=2;
	LoadTexture("data/textures/lugaru.png",&Mainmenuitems[0],0,0);
	LoadTexture("data/textures/newgame.png",&Mainmenuitems[1],0,0);
	LoadTexture("data/textures/options.png",&Mainmenuitems[2],0,0);
	LoadTexture("data/textures/quit.png",&Mainmenuitems[3],0,0);
	LoadTexture("data/textures/world.png",&Mainmenuitems[7],0,0);
	LoadTexture("data/textures/eyelid.png",&Mainmenuitems[4],0,1);
	//LoadTexture("data/textures/eye.jpg",&Mainmenuitems[5],0,1);
	texdetail=temptexdetail;

	loaddistrib=0;
	anim=0;

	FadeLoadingScreen(95);


	gameon=0;
	mainmenu=1;

	stillloading=0;
	firstload=0;
	oldmainmenu=0;

	newdetail=detail;
	newscreenwidth=screenwidth;
	newscreenheight=screenheight;



	/*
	float gLoc[3]={0,0,0};
	float vel[3]={0,0,0};
	OPENAL_Sample_SetMinMaxDistance(samp[firestartsound], 9999.0f, 99999.0f);
	PlaySoundEx( firestartsound, samp[firestartsound], NULL, true);
	OPENAL_3D_SetAttributes(channels[firestartsound], gLoc, vel);
	OPENAL_SetVolume(channels[firestartsound], 256);
	OPENAL_SetPaused(channels[firestartsound], false);
	OPENAL_Sample_SetMinMaxDistance(samp[firestartsound], 8.0f, 2000.0f);

	flashr=1;
	flashg=0;
	flashb=0;
	flashamount=1;
	flashdelay=1;
	*/
}


void Game::LoadStuff()
{
	static float temptexdetail;
	static float viewdistdetail;
	static int i,j,texsize;
	float megascale =1;

	LOGFUNC;

	visibleloading=1;

	/*musicvolume[3]=512;
	PlaySoundEx( music4, samp[music4], NULL, true);
	OPENAL_SetPaused(channels[music4], false);
	OPENAL_SetVolume(channels[music4], 512);
	*/
	loadtime=0;

	stillloading=1;

	//texture.data = ( GLubyte* )malloc( 1024*1024*4 );

	for(i=0;i<maxplayers;i++)
	{
		if (glIsTexture(player[i].skeleton.drawmodel.textureptr))
		{
			glDeleteTextures(1, &player[i].skeleton.drawmodel.textureptr);
		}
		player[i].skeleton.drawmodel.textureptr=0;;
	}

	//temptexdetail=texdetail;
	//texdetail=1;
	i=abs(Random()%4);
	LoadTexture("data/textures/fire.png",&loadscreentexture,1,0);
	//texdetail=temptexdetail;

	temptexdetail=texdetail;
	texdetail=1;
	text.LoadFontTexture("data/textures/font.png");
	text.BuildFont();
	texdetail=temptexdetail;

	numsounds=71;

	viewdistdetail=2;
	viewdistance=50*megascale*viewdistdetail;

	brightness=100;



	if(detail==2){
		texdetail=1;
		terraindetail=1;
	}
	if(detail==1){
		texdetail=2;
		terraindetail=1;
	}
	if(detail==0){
		texdetail=4;
		terraindetail=1;
		//terraindetail=2;
	}

	realtexdetail=texdetail;

	/*texdetail/=4;
	if(texdetail<1)texdetail=1;
	realtexdetail=texdetail*4;
	*/
	numplayers=1;



	/*LoadTexture("data/textures/snow.png",&terraintexture,1);

	LoadTexture("data/textures/rock.png",&terraintexture2,1);

	LoadTexture("data/textures/detail.png",&terraintexture3,1);
	*/


	LOG("Loading weapon data...");

	LoadTexture("data/textures/knife.png",&weapons.knifetextureptr,0,1);
	LoadTexture("data/textures/bloodknife.png",&weapons.bloodknifetextureptr,0,1);
	LoadTexture("data/textures/lightbloodknife.png",&weapons.lightbloodknifetextureptr,0,1);
	LoadTexture("data/textures/sword.png",&weapons.swordtextureptr,1,0);
	LoadTexture("data/textures/swordblood.png",&weapons.bloodswordtextureptr,1,0);
	LoadTexture("data/textures/swordbloodlight.png",&weapons.lightbloodswordtextureptr,1,0);
	LoadTexture("data/textures/staff.png",&weapons.stafftextureptr,1,0);

	weapons.throwingknifemodel.load(ConvertFileName("data/models/throwingknife.solid"), true);
	weapons.throwingknifemodel.Scale(.001,.001,.001);
	//weapons.throwingknifemodel.Rotate(0,0,-90);
	weapons.throwingknifemodel.Rotate(90,0,0);
	weapons.throwingknifemodel.Rotate(0,90,0);
	weapons.throwingknifemodel.flat=0;
	weapons.throwingknifemodel.CalculateNormals(1);
	//weapons.throwingknifemodel.ScaleNormals(-1,-1,-1);

	weapons.swordmodel.load(ConvertFileName("data/models/sword.solid"), true);
	weapons.swordmodel.Scale(.001,.001,.001);
	//weapons.swordmodel.Rotate(0,0,-90);
	weapons.swordmodel.Rotate(90,0,0);
	weapons.swordmodel.Rotate(0,90,0);
	weapons.swordmodel.Rotate(0,0,90);
	weapons.swordmodel.flat=1;
	weapons.swordmodel.CalculateNormals(1);
	//weapons.swordmodel.ScaleNormals(-1,-1,-1);

	weapons.staffmodel.load(ConvertFileName("data/models/staff.solid"), true);
	weapons.staffmodel.Scale(.005,.005,.005);
	//weapons.staffmodel.Rotate(0,0,-90);
	weapons.staffmodel.Rotate(90,0,0);
	weapons.staffmodel.Rotate(0,90,0);
	weapons.staffmodel.Rotate(0,0,90);
	weapons.staffmodel.flat=1;
	weapons.staffmodel.CalculateNormals(1);
	//weapons.staffmodel.ScaleNormals(-1,-1,-1);

	//temptexdetail=texdetail;
	//if(texdetail>4)texdetail=4;
	LoadTexture("data/textures/shadow.png",&terrain.shadowtexture,0,1);

	LoadTexture("data/textures/blood.png",&terrain.bloodtexture,0,1);

	LoadTexture("data/textures/break.png",&terrain.breaktexture,0,1);

	LoadTexture("data/textures/blood.png",&terrain.bloodtexture2,0,1);


	LoadTexture("data/textures/footprint.png",&terrain.footprinttexture,0,1);

	LoadTexture("data/textures/bodyprint.png",&terrain.bodyprinttexture,0,1);

	/*LoadTexture("data/textures/cloud.png",&sprites.cloudtexture,1);

	LoadTexture("data/textures/cloudimpact.png",&sprites.cloudimpacttexture,1);

	LoadTexture("data/textures/bloodparticle.png",&sprites.bloodtexture,1);

	LoadTexture("data/textures/snowflake.png",&sprites.snowflaketexture,1);

	LoadTexture("data/textures/flame.png",&sprites.flametexture,1);

	LoadTexture("data/textures/smoke.png",&sprites.smoketexture,1);
	//texdetail=temptexdetail;
	LoadTexture("data/textures/shine.png",&sprites.shinetexture,1);*/



	LoadTexture("data/textures/hawk.png",&hawktexture,0,1);

	LoadTexture("data/textures/logo.png",&logotexture,0,1);


	//LoadTexture("data/textures/box.jpg",&objects.boxtextureptr,1,0);


	LoadTexture("data/textures/cloud.png",&sprites.cloudtexture,1,1);
	LoadTexture("data/textures/cloudimpact.png",&sprites.cloudimpacttexture,1,1);
	LoadTexture("data/textures/bloodparticle.png",&sprites.bloodtexture,1,1);
	LoadTexture("data/textures/snowflake.png",&sprites.snowflaketexture,1,1);
	LoadTexture("data/textures/flame.png",&sprites.flametexture,1,1);
	LoadTexture("data/textures/bloodflame.png",&sprites.bloodflametexture,1,1);
	LoadTexture("data/textures/smoke.png",&sprites.smoketexture,1,1);
	LoadTexture("data/textures/shine.png",&sprites.shinetexture,1,0);
	LoadTexture("data/textures/splinter.png",&sprites.splintertexture,1,1);
	LoadTexture("data/textures/leaf.png",&sprites.leaftexture,1,1);
	LoadTexture("data/textures/tooth.png",&sprites.toothtexture,1,1);

	rotation=0;
	rotation2=0;
	ReSizeGLScene(90,.01);

	viewer=0;




	if(detail)kTextureSize=1024;
	if(detail==1)kTextureSize=512;
	if(detail==0)kTextureSize=256;


	//drawmode=motionblurmode;

	//Set up distant light
	light.color[0]=.95;
	light.color[1]=.95;
	light.color[2]=1;
	light.ambient[0]=.2;
	light.ambient[1]=.2;
	light.ambient[2]=.24;
	light.location.x=1;
	light.location.y=1;
	light.location.z=-.2;
	Normalise(&light.location);

	LoadingScreen();

	SetUpLighting();


	fadestart=.6;
	gravity=-10;

	texscale=.2/megascale/viewdistdetail;
	terrain.scale=3*megascale*terraindetail*viewdistdetail;

	viewer.x=terrain.size/2*terrain.scale;
	viewer.z=terrain.size/2*terrain.scale;

	hawk.load((char *)"data/models/hawk.solid",1);
	hawk.Scale(.03,.03,.03);
	hawk.Rotate(90,1,1);
	hawk.CalculateNormals(0);
	hawk.ScaleNormals(-1,-1,-1);
	hawkcoords.x=terrain.size/2*terrain.scale-5-7;
	hawkcoords.z=terrain.size/2*terrain.scale-5-7;
	hawkcoords.y=terrain.getHeight(hawkcoords.x,hawkcoords.z)+25;


	eye.load((char *)"data/models/eye.solid",1);
	eye.Scale(.03,.03,.03);
	eye.CalculateNormals(0);

	cornea.load((char *)"data/models/cornea.solid",1);
	cornea.Scale(.03,.03,.03);
	cornea.CalculateNormals(0);

	iris.load((char *)"data/models/iris.solid",1);
	iris.Scale(.03,.03,.03);
	iris.CalculateNormals(0);

	LoadSave("data/textures/bloodfur.png",0,1,&bloodText[0],0);
	LoadSave("data/textures/wolfbloodfur.png",0,1,&wolfbloodText[0],0);

	oldenvironment=-4;

	gameon=1;
	mainmenu=0;

	firstload=0;
	//if(targetlevel!=7)
		Loadlevel(targetlevel);


	rabbitcoords=player[0].coords;
	rabbitcoords.y=terrain.getHeight(rabbitcoords.x,rabbitcoords.z);

	animation[runanim].Load((char *)"data/animations/run",middleheight,neutral);

	animation[bounceidleanim].Load((char *)"data/animations/idle",middleheight,neutral);
	animation[stopanim].Load((char *)"data/animations/stop",middleheight,neutral);

	animation[jumpupanim].Load((char *)"data/animations/jumpup",highheight,neutral);
	animation[jumpdownanim].Load((char *)"data/animations/jumpdown",highheight,neutral);

	animation[landanim].Load((char *)"data/animations/landing",lowheight,neutral);
	animation[landhardanim].Load((char *)"data/animations/landhard",lowheight,neutral);
	animation[climbanim].Load((char *)"data/animations/climb",lowheight,neutral);
	animation[hanganim].Load((char *)"data/animations/hangon",lowheight,neutral);
	animation[spinkickanim].Load((char *)"data/animations/spinkick",middleheight,normalattack);

	animation[getupfromfrontanim].Load((char *)"data/animations/getupfromfront",lowheight,neutral);
	animation[getupfrombackanim].Load((char *)"data/animations/getupfromback",lowheight,neutral);
	animation[crouchanim].Load((char *)"data/animations/crouch",lowheight,neutral);
	animation[sneakanim].Load((char *)"data/animations/sneak",lowheight,neutral);
	animation[rollanim].Load((char *)"data/animations/roll",lowheight,neutral);
	animation[flipanim].Load((char *)"data/animations/flip",highheight,neutral);
	animation[frontflipanim].Load((char *)"data/animations/flip",highheight,neutral);
	animation[spinkickreversedanim].Load((char *)"data/animations/spinkickcaught",middleheight,reversed);

	animation[spinkickreversalanim].Load((char *)"data/animations/spinkickcatch",middleheight,reversal);
	animation[lowkickanim].Load((char *)"data/animations/lowkick",middleheight,normalattack);
	animation[sweepanim].Load((char *)"data/animations/sweep",lowheight,normalattack);
	animation[sweepreversedanim].Load((char *)"data/animations/sweepcaught",lowheight,reversed);
	animation[sweepreversalanim].Load((char *)"data/animations/sweepcatch",middleheight,reversal);
	animation[rabbitkickanim].Load((char *)"data/animations/rabbitkick",middleheight,normalattack);
	animation[rabbitkickreversedanim].Load((char *)"data/animations/rabbitkickcaught",middleheight,reversed);
	animation[rabbitkickreversalanim].Load((char *)"data/animations/rabbitkickcatch",lowheight,reversal);
	animation[upunchanim].Load((char *)"data/animations/upunch",middleheight,normalattack);
	animation[staggerbackhighanim].Load((char *)"data/animations/staggerbackhigh",middleheight,neutral);
	animation[upunchreversedanim].Load((char *)"data/animations/upunchcaught",middleheight,reversed);

	animation[upunchreversalanim].Load((char *)"data/animations/upunchcatch",middleheight,reversal);
	animation[hurtidleanim].Load((char *)"data/animations/hurtidle",middleheight,neutral);
	animation[backhandspringanim].Load((char *)"data/animations/backhandspring",middleheight,neutral);
	animation[fightidleanim].Load((char *)"data/animations/fightidle",middleheight,neutral);
	animation[walkanim].Load((char *)"data/animations/walk",middleheight,neutral);

	animation[fightsidestep].Load((char *)"data/animations/fightsidestep",middleheight,neutral);
	animation[killanim].Load((char *)"data/animations/kill",middleheight,normalattack);
	animation[sneakattackanim].Load((char *)"data/animations/sneakattack",middleheight,reversal);
	animation[sneakattackedanim].Load((char *)"data/animations/sneakattacked",middleheight,reversed);
	animation[drawrightanim].Load((char *)"data/animations/drawright",middleheight,neutral);
	animation[knifeslashstartanim].Load((char *)"data/animations/slashstart",middleheight,normalattack);
	animation[crouchdrawrightanim].Load((char *)"data/animations/crouchdrawright",lowheight,neutral);
	animation[crouchstabanim].Load((char *)"data/animations/crouchstab",lowheight,normalattack);

	animation[knifefollowanim].Load((char *)"data/animations/slashfollow",middleheight,reversal);
	animation[knifefollowedanim].Load((char *)"data/animations/slashfollowed",middleheight,reversed);
	animation[knifethrowanim].Load((char *)"data/animations/knifethrow",middleheight,normalattack);
	animation[removeknifeanim].Load((char *)"data/animations/removeknife",middleheight,neutral);
	animation[crouchremoveknifeanim].Load((char *)"data/animations/crouchremoveknife",lowheight,neutral);
	animation[jumpreversedanim].Load((char *)"data/animations/jumpcaught",middleheight,reversed);
	animation[jumpreversalanim].Load((char *)"data/animations/jumpcatch",middleheight,reversal);
	animation[staggerbackhardanim].Load((char *)"data/animations/staggerbackhard",middleheight,neutral);

	animation[dropkickanim].Load((char *)"data/animations/dropkick",middleheight,normalattack);
	animation[winduppunchanim].Load((char *)"data/animations/winduppunch",middleheight,normalattack);
	animation[winduppunchblockedanim].Load((char *)"data/animations/winduppunchblocked",middleheight,normalattack);
	animation[blockhighleftanim].Load((char *)"data/animations/blockhighleft",middleheight,normalattack);
	animation[blockhighleftstrikeanim].Load((char *)"data/animations/blockhighleftstrike",middleheight,normalattack);
	animation[backflipanim].Load((char *)"data/animations/backflip",highheight,neutral);
	animation[walljumpbackanim].Load((char *)"data/animations/walljumpback",highheight,neutral);
	animation[walljumpfrontanim].Load((char *)"data/animations/walljumpfront",highheight,neutral);
	animation[rightflipanim].Load((char *)"data/animations/rightflip",highheight,neutral);
	animation[walljumprightanim].Load((char *)"data/animations/walljumpright",highheight,neutral);
	animation[leftflipanim].Load((char *)"data/animations/leftflip",highheight,neutral);
	animation[walljumpleftanim].Load((char *)"data/animations/walljumpleft",highheight,neutral);
	animation[walljumprightkickanim].Load((char *)"data/animations/walljumprightkick",highheight,neutral);
	animation[walljumpleftkickanim].Load((char *)"data/animations/walljumpleftkick",highheight,neutral);
	animation[knifefightidleanim].Load((char *)"data/animations/knifefightidle",middleheight,neutral);
	animation[knifesneakattackanim].Load((char *)"data/animations/knifesneakattack",middleheight,reversal);
	animation[knifesneakattackedanim].Load((char *)"data/animations/knifesneakattacked",middleheight,reversed);
	animation[swordfightidleanim].Load((char *)"data/animations/swordfightidle",middleheight,neutral);
	animation[drawleftanim].Load((char *)"data/animations/drawleft",middleheight,neutral);
	animation[swordslashanim].Load((char *)"data/animations/swordslash",middleheight,normalattack);
	animation[swordgroundstabanim].Load((char *)"data/animations/swordgroundstab",lowheight,normalattack);
	animation[dodgebackanim].Load((char *)"data/animations/dodgeback",middleheight,neutral);
	animation[swordsneakattackanim].Load((char *)"data/animations/swordsneakattack",middleheight,reversal);
	animation[swordsneakattackedanim].Load((char *)"data/animations/swordsneakattacked",middleheight,reversed);
	animation[swordslashreversedanim].Load((char *)"data/animations/swordslashcaught",middleheight,reversed);
	animation[swordslashreversalanim].Load((char *)"data/animations/swordslashcatch",middleheight,reversal);
	animation[knifeslashreversedanim].Load((char *)"data/animations/knifeslashcaught",middleheight,reversed);
	animation[knifeslashreversalanim].Load((char *)"data/animations/knifeslashcatch",middleheight,reversal);
	animation[swordfightidlebothanim].Load((char *)"data/animations/swordfightidleboth",middleheight,neutral);
	animation[swordslashparryanim].Load((char *)"data/animations/sworduprightparry",middleheight,normalattack);
	animation[swordslashparriedanim].Load((char *)"data/animations/swordslashparried",middleheight,normalattack);
	animation[wolfidle].Load((char *)"data/animations/wolfidle",middleheight,neutral);
	animation[wolfcrouchanim].Load((char *)"data/animations/wolfcrouch",lowheight,neutral);
	animation[wolflandanim].Load((char *)"data/animations/wolflanding",lowheight,neutral);
	animation[wolflandhardanim].Load((char *)"data/animations/wolflandhard",lowheight,neutral);
	animation[wolfrunanim].Load((char *)"data/animations/wolfrun",middleheight,neutral);
	animation[wolfrunninganim].Load((char *)"data/animations/wolfrunning",middleheight,neutral);
	animation[rabbitrunninganim].Load((char *)"data/animations/rabbitrunning",middleheight,neutral);
	animation[wolfstopanim].Load((char *)"data/animations/wolfstop",middleheight,neutral);
	animation[rabbittackleanim].Load((char *)"data/animations/rabbittackle",middleheight,neutral);
	animation[rabbittacklinganim].Load((char *)"data/animations/rabbittackling",middleheight,reversal);
	animation[rabbittackledbackanim].Load((char *)"data/animations/rabbittackledback",middleheight,reversed);
	animation[rabbittackledfrontanim].Load((char *)"data/animations/rabbittackledfront",middleheight,reversed);
	animation[wolfslapanim].Load((char *)"data/animations/wolfslap",middleheight,normalattack);
	animation[staffhitanim].Load((char *)"data/animations/staffhit",middleheight,normalattack);
	animation[staffgroundsmashanim].Load((char *)"data/animations/staffgroundsmash",lowheight,normalattack);
	animation[staffspinhitanim].Load((char *)"data/animations/spinwhack",middleheight,normalattack);
	animation[staffhitreversedanim].Load((char *)"data/animations/staffhitcaught",middleheight,reversed);
	animation[staffhitreversalanim].Load((char *)"data/animations/staffhitcatch",middleheight,reversal);
	animation[staffspinhitreversedanim].Load((char *)"data/animations/spinwhackcaught",middleheight,reversed);
	animation[staffspinhitreversalanim].Load((char *)"data/animations/spinwhackcatch",middleheight,reversal);

	animation[sitanim].Load((char *)"data/animations/sit",lowheight,neutral);
	animation[sleepanim].Load((char *)"data/animations/sleep",lowheight,neutral);
	animation[talkidleanim].Load((char *)"data/animations/talkidle",middleheight,neutral);

	animation[sitwallanim].Load((char *)"data/animations/dying",lowheight,neutral);
	animation[dead1anim].Load((char *)"data/animations/dead1",lowheight,neutral);
	animation[dead2anim].Load((char *)"data/animations/dead2",lowheight,neutral);
	animation[dead3anim].Load((char *)"data/animations/dead3",lowheight,neutral);
	animation[dead4anim].Load((char *)"data/animations/dead4",lowheight,neutral);
	//Fix knife stab, too lazy to do it manually
	XYZ moveamount;
	moveamount=0;
	moveamount.z=2;
	for(i=0;i<player[0].skeleton.num_joints;i++){
		for(j=0;j<animation[knifesneakattackanim].numframes;j++){
			animation[knifesneakattackanim].position[i][j]+=moveamount;
		}
	}

	loadscreencolor=4;
	LoadingScreen();

	for(i=0;i<player[0].skeleton.num_joints;i++){
		for(j=0;j<animation[knifesneakattackedanim].numframes;j++){
			animation[knifesneakattackedanim].position[i][j]+=moveamount;
		}
	}

	loadscreencolor=4;
	LoadingScreen();

	for(i=0;i<player[0].skeleton.num_joints;i++){
		animation[dead1anim].position[i][1]=animation[dead1anim].position[i][0];
		animation[dead2anim].position[i][1]=animation[dead2anim].position[i][0];
		animation[dead3anim].position[i][1]=animation[dead3anim].position[i][0];
		animation[dead4anim].position[i][1]=animation[dead4anim].position[i][0];
	}
	animation[dead1anim].speed[0]=0.001;
	animation[dead2anim].speed[0]=0.001;
	animation[dead3anim].speed[0]=0.001;
	animation[dead4anim].speed[0]=0.001;

	animation[dead1anim].speed[1]=0.001;
	animation[dead2anim].speed[1]=0.001;
	animation[dead3anim].speed[1]=0.001;
	animation[dead4anim].speed[1]=0.001;

	for(i=0;i<player[0].skeleton.num_joints;i++){
		for(j=0;j<animation[swordsneakattackanim].numframes;j++){
			animation[swordsneakattackanim].position[i][j]+=moveamount;
		}
	}
	loadscreencolor=4;
	LoadingScreen();
	for(j=0;j<animation[swordsneakattackanim].numframes;j++){
		animation[swordsneakattackanim].weapontarget[j]+=moveamount;
	}

	loadscreencolor=4;
	LoadingScreen();

	for(i=0;i<player[0].skeleton.num_joints;i++){
		for(j=0;j<animation[swordsneakattackedanim].numframes;j++){
			animation[swordsneakattackedanim].position[i][j]+=moveamount;
		}
	}
	/*
	for(i=0;i<player[0].skeleton.num_joints;i++){
	for(j=0;j<animation[sleepanim].numframes;j++){
	animation[sleepanim].position[i][j]=DoRotation(animation[sleepanim].position[i][j],0,180,0);
	}
	}
	*/
	loadscreencolor=4;
	LoadingScreen();
	temptexdetail=texdetail;
	texdetail=1;
	texdetail=temptexdetail;

	loadscreencolor=4;
	LoadingScreen();

	//if(ismotionblur){
	if(!screentexture){
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

		glGenTextures( 1, &screentexture );
		glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );


		glEnable(GL_TEXTURE_2D);
		glBindTexture( GL_TEXTURE_2D, screentexture);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, kTextureSize, kTextureSize, 0);
	}
	//}

	LoadSounds();

	/*PlaySoundEx( consolesuccesssound, samp[consolesuccesssound], NULL, true);
	OPENAL_SetVolume(channels[consolesuccesssound], 256);
	OPENAL_SetPaused(channels[consolesuccesssound], false);
	*/
	if(targetlevel!=7){
		float gLoc[3]={0,0,0};
		float vel[3]={0,0,0};
		OPENAL_Sample_SetMinMaxDistance(samp[fireendsound], 9999.0f, 99999.0f);
		PlaySoundEx( fireendsound, samp[fireendsound], NULL, true);
		OPENAL_3D_SetAttributes(channels[fireendsound], gLoc, vel);
		OPENAL_SetVolume(channels[fireendsound], 256);
		OPENAL_SetPaused(channels[fireendsound], false);
		OPENAL_Sample_SetMinMaxDistance(samp[fireendsound], 8.0f, 2000.0f);
	}

	stillloading=0;
	loading=0;
	changedelay=1;

	visibleloading=0;
}

Game::Game()
{
	terraintexture = 0;
	terraintexture2 = 0;
	terraintexture3 = 0;
	screentexture = 0;
	screentexture2 = 0;
	logotexture = 0;
	loadscreentexture = 0;
	Maparrowtexture = 0;
	Mapboxtexture = 0;
	Mapcircletexture = 0;
	cursortexture = 0;

	memset(Mainmenuitems, 0, sizeof(Mainmenuitems));

	nummenuitems = 0;

	memset(startx, 0, sizeof(startx));
	memset(starty, 0, sizeof(starty));
	memset(endx, 0, sizeof(endx));
	memset(endy, 0, sizeof(endy));

	memset(selectedlong, 0, sizeof(selectedlong));
	memset(offsetx, 0, sizeof(offsetx));
	memset(offsety, 0, sizeof(offsety));
	memset(movex, 0, sizeof(movex));
	memset(movey, 0, sizeof(movey));
	memset(endy, 0, sizeof(endy));

	transition = 0;
	anim = 0;
	selected = 0;
	loaddistrib = 0;
	keyselect = 0;
	indemo = 0;

	won = 0;

	entername = 0;

	memset(menustring, 0, sizeof(menustring));
	memset(registrationname, 0, sizeof(registrationname));
	registrationnumber = 0;

	newdetail = 0;
	newscreenwidth = 0;
	newscreenheight = 0;

	gameon = 0;
	deltah = 0,deltav = 0;
	mousecoordh = 0,mousecoordv = 0;
	oldmousecoordh = 0,oldmousecoordv = 0;
	rotation = 0,rotation2 = 0;

//	SkyBox skybox;

	cameramode = 0;
	cameratogglekeydown = 0;
	chattogglekeydown = 0;
	olddrawmode = 0;
	drawmode = 0;
	drawmodetogglekeydown = 0;
	explodetogglekeydown = 0;
	detailtogglekeydown = 0;
	firstload = 0;
	oldbutton = 0;

	leveltime = 0;
	loadtime = 0;

//	Model hawk;

//	XYZ hawkcoords;
//	XYZ realhawkcoords;

	hawktexture = 0;
	hawkrotation = 0;
	hawkcalldelay = 0;
/*
	Model eye;
	Model iris;
	Model cornea;
*/
	stealthloading = 0;

	campaignnumlevels = 0;

	memset(campaignmapname, 0, sizeof(campaignmapname));
	memset(campaigndescription, 0, sizeof(campaigndescription));
	memset(campaignchoosenext, 0, sizeof(campaignchoosenext));
	memset(campaignnumnext, 0, sizeof(campaignnumnext));
	memset(campaignnextlevel, 0, sizeof(campaignnextlevel));
	int campaignchoicesmade;
	memset(campaignchoices, 0, sizeof(campaignchoices));
	memset(campaignlocationx, 0, sizeof(campaignlocationx));
	memset(campaignlocationy, 0, sizeof(campaignlocationy));
	memset(campaignlocationy, 0, sizeof(campaignlocationy));

	campaignchoicenum = 0;

	memset(campaignchoicewhich, 0, sizeof(campaignchoicewhich));

	whichchoice = 0;

	numlevelspassed = 0;

	memset(levelorder, 0, sizeof(levelorder));
	memset(levelvisible, 0, sizeof(levelvisible));
	memset(levelhighlight, 0, sizeof(levelhighlight));

	minimap = 0;

	musictype = 0,oldmusictype = 0,oldoldmusictype = 0;
	realthreat = 0;

//	Model rabbit;
//	XYZ rabbitcoords;

//	XYZ mapcenter;
	mapradius = 0;

//	Text text;
	fps = 0;

//	XYZ cameraloc;
	cameradist = 0;

	envtogglekeydown = 0;
	slomotogglekeydown = 0;
	texturesizetogglekeydown = 0;
	freezetogglekeydown = 0;
	drawtoggle = 0;

	editorenabled = 0;
	editortype = 0;
	editorsize = 0;
	editorrotation = 0;
	editorrotation2 = 0;

	brightness = 0;

	quit = 0;
	tryquit = 0;

//	XYZ pathpoint[30];
	numpathpoints = 0;
	memset(numpathpointconnect, 0, sizeof(numpathpointconnect));
	memset(pathpointconnect, 0, sizeof(pathpointconnect));
	pathpointselected = 0;

	endgame = 0;
	scoreadded = 0;
	numchallengelevels = 0;

	console = 0;
	archiveselected = 0;

	memset(consoletext, 0, sizeof(consoletext));
	memset(consolechars, 0, sizeof(consolechars));
	chatting = 0;
	memset(displaytext, 0, sizeof(displaytext));
	memset(displaychars, 0, sizeof(displaychars));
	memset(displaytime, 0, sizeof(displaytime));
	displayblinkdelay = 0;
	displayblink = 0;
	displayselected = 0;
	consolekeydown = 0;
	consoletogglekeydown = 0;
	consoleblinkdelay = 0;
	consoleblink = 0;
	consoleselected = 0;
	memset(togglekey, 0, sizeof(togglekey));
	memset(togglekeydelay, 0, sizeof(togglekeydelay));
	registernow = 0;
	autocam = 0;

	crouchkey = 0,jumpkey = 0,forwardkey = 0,chatkey = 0,backkey = 0,leftkey = 0,rightkey = 0,drawkey = 0,throwkey = 0,attackkey = 0;
	oldattackkey = 0;

	loading = 0;
	talkdelay = 0;

	numboundaries = 0;
//	XYZ boundary[360];

	whichlevel = 0;
	oldenvironment = 0;
	targetlevel = 0;
	changedelay = 0;

	memset(musicvolume, 0, sizeof(musicvolume));
	memset(oldmusicvolume, 0, sizeof(oldmusicvolume));
	musicselected = 0;
	change = 0;
}

