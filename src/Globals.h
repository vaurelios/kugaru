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


#ifndef KUGARU_GLOBALS_H
#define KUGARU_GLOBALS_H 


#include <SDL.h>
#include <glib.h>

#include "gamegl.h"
#include "Quaternions.h"
#include "Lights.h"
#include "Conf.h"
#include "Skeleton.h"
#include "Terrain.h"
#include "Sprites.h"
#include "Frustum.h"
#include "Objects.h"
#include "Weapons.h"
#include "Person.h"
#include "TGALoader.h"
#include "Types.h"
#include "openal_wrapper.h"

#include "Constants.h"

extern SDL_Keysym *keysym;
extern SDL_Scancode oldscancode;
extern SDL_MouseButtonEvent *button;
extern uint8_t oldbutton;
extern int modes_count;
extern GSList *displaymodes;
extern int displaywidth, displayheight;
extern bool IsFocused;
extern Kugaru::Conf cnf;
extern bool visibleloading;
extern OPENAL_SAMPLE *samp[];
extern OPENAL_STREAM *strm[];
extern int channels[];

extern int volume;
extern bool ismotionblur;
extern int usermousesensitivity;
extern bool floatjump;
extern bool cellophane;
extern bool autoslomo;
extern bool decals;
extern bool invertmouse;
extern bool texttoggle;
extern float blurness;
extern float targetblurness;
extern float windvar;
extern float precipdelay;
extern float gamespeed;
extern float oldgamespeed;
extern float tintr, tintg, tintb;
extern int difficulty;
extern float multiplier;
extern float realmultiplier;
extern float screenwidth, screenheight;
extern float viewdistance;
extern XYZ viewer;
extern XYZ viewerfacing;
extern XYZ lightlocation;
extern float fadestart;
extern int environment;
extern float texscale;
extern float gravity;
extern Light light;
extern Animation animation[];
extern Skeleton testskeleton;
extern int numsounds;
extern Terrain terrain;
extern Sprites sprites;
extern float sps;

extern SDL_Window *screen;

extern int kTextureSize;
extern int detail;
extern FRUSTUM frustum;
extern float texdetail;
extern float realtexdetail;
extern float terraindetail;
extern float playerdist;
extern Objects objects;
extern int slomo;
extern float slomodelay;
extern GLubyte bloodText[];
extern GLubyte wolfbloodText[];
extern float colors[];
extern int blooddetail;
extern bool osx;
extern float camerashake;
extern float woozy;
extern float blackout;
extern bool foliage;
extern bool musictoggle;
extern bool trilinear;
extern Weapons weapons;
extern bool damageeffects;
extern Person player[];
extern int numplayers;
extern bool ambientsound;
extern bool mousejump;
extern bool freeze;
extern bool winfreeze;
extern float flashamount, flashr, flashg, flashb;
extern int flashdelay;
extern bool vsync;
extern float motionbluramount;
extern bool keyboardfrozen;
extern char mapname[];
extern bool loadingstuff;
extern bool stillloading;
extern bool showpoints;
extern bool alwaysblur;
extern bool immediate;
extern bool velocityblur;
extern int test;
extern XYZ windvector;
extern short vRefNum;
extern long dirID;
extern Kugaru::MainMenu mainmenu;
extern Kugaru::MainMenu oldmainmenu;
extern GLubyte texturearray[];
extern int loadscreencolor;
extern int whichjointstartarray[];
extern int whichjointendarray[];
extern int kBitsPerPixel;

extern int numhotspots;
extern XYZ hotspot[];
extern int hotspottype[];
extern float hotspotsize[];
extern char hotspottext[][256];
extern int currenthotspot;	
extern int winhotspot;
extern int windialogue;
extern int killhotspot;

extern float menupulse;

extern int numdialogues;
extern int numdialogueboxes[];
extern int dialoguetype[];
extern int dialogueboxlocation[][max_dialoguelength];
extern float dialogueboxcolor[][max_dialoguelength][3];
extern int dialogueboxsound[][max_dialoguelength];
extern char dialoguetext[][max_dialoguelength][128];
extern char dialoguename[][max_dialoguelength][64];
extern XYZ dialoguecamera[][max_dialoguelength];
extern XYZ participantlocation[][10];
extern int participantfocus[][max_dialoguelength];
extern int participantaction[][max_dialoguelength];
extern float participantrotation[][10];
extern XYZ participantfacing[][max_dialoguelength][10];
extern float dialoguecamerarotation[][max_dialoguelength];
extern float dialoguecamerarotation2[][max_dialoguelength];
extern int indialogue;
extern int whichdialogue;
extern int directing;
extern float dialoguetime;
extern int dialoguegonethrough[];

extern float smoketex;

extern float slomospeed;
extern float slomofreq;

extern int tutoriallevel;
extern int tutorialstage;
extern float tutorialstagetime;
extern float tutorialmaxtime;
extern float tutorialsuccess;

extern bool againbonus;

extern float damagedealt;
extern float damagetaken;

extern int maptype;

extern int editoractive;
extern int editorpathtype;

extern bool reversaltrain;
extern bool cananger;
extern bool canattack;

extern bool skyboxtexture;
extern float skyboxr;
extern float skyboxg;
extern float skyboxb;
extern float skyboxlightr;
extern float skyboxlightg;
extern float skyboxlightb;

extern float bonusnum[];

extern int hostile;
extern float hostiletime;

extern XYZ envsound[];
extern float envsoundvol[];
extern float envsoundlife[];
extern int numenvsounds;


extern bool tilt2weird;
extern bool tiltweird;
extern bool midweird;
extern bool proportionweird;
extern bool vertexweird[];
extern TGAImageRec texture;
extern bool debugmode;

extern int oldbonus;
extern int bonus;
extern float bonusvalue;
extern float bonustotal;
extern float startbonustotal;
extern float bonustime;

extern int numaccounts;
extern int accountactive;
extern int accountdifficulty[];
extern int accountprogress[];
extern float accountpoints[];
extern float accounthighscore[][50];
extern float accountfasttime[][50];
extern bool accountunlocked[][60];
extern char accountname[][256];
extern float accountcampaignhighscore[];
extern float accountcampaignfasttime[];
extern float accountcampaignscore[];
extern float accountcampaigntime[];
extern int accountcampaignchoicesmade[];
extern int accountcampaignchoices[][5000];
extern bool won;


extern bool campaign;

extern int numfalls;
extern int numflipfail;
extern int numseen;
extern int numresponded;
extern int numstaffattack;
extern int numswordattack;
extern int numknifeattack;
extern int numunarmedattack;
extern int numescaped;
extern int numflipped;
extern int numwallflipped;
extern int numthrowkill;
extern int numafterkill;
extern int numreversals;
extern int numattacks;
extern int maxalarmed;

extern bool gamestarted;

//TextureList textures;

#endif /* KUGARU_GLOBALS_H */
