/*
 * This file is part of Kugaru.
 *
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


#include <stdio.h>

#include "binio.h"
#include "Constants.h"


typedef struct
{
    float x;
    float y;
    float z;
} XYZ;

typedef struct
{
    int texture;

    float r;
    float g;
    float b;

    float lr;
    float lg;
    float lb;
} Skybox;

typedef struct
{
    XYZ cords;

    float rot;
    float tgrot;
    
    int weapons[5]; /* Weapons number are limited to 5; GameTick.cpp:ch_save */
    int num_weapons;

    float armorhead;
    float armorhigh;
    float armorlow;

    float protectionhead;
    float protectionhigh;
    float protectionlow;

    float metalhead;
    float metalhigh;
    float metallow;

    float power;
    float speedmult;

    char clothes[5][256];
    int num_clothes;

    float clothestintr[5];
    float clothestintg[5];
    float clothestintb[5];

    int skin;
    int creature;
} Player;

int main(int argc, char **argv)
{
    FILE *fp;
    int mapver;
    int maptype;
    int hostile;
    float viewdist;
    float fadestart;
    Skybox sb;
    Player player;
    int numdialogues;
    int numdialogueboxes[max_dialogues];
    int dialoguetype[max_dialogues];
    int dialogueboxlocation[max_dialogues][max_dialoguelength];
    float dialogueboxcolor[max_dialogues][max_dialoguelength][3];
    int dialogueboxsound[max_dialogues][max_dialoguelength];
    char dialoguetext[max_dialogues][max_dialoguelength][128];
    char dialoguename[max_dialogues][max_dialoguelength][64];
    XYZ dialoguecamera[max_dialogues][max_dialoguelength];
    XYZ participantlocation[max_dialogues][10];
    int participantfocus[max_dialogues][max_dialoguelength];
    int participantaction[max_dialogues][max_dialoguelength];
    float participantrotation[max_dialogues][10];
    XYZ participantfacing[max_dialogues][max_dialoguelength][10];
    float dialoguecamerarotation[max_dialogues][max_dialoguelength];
    float dialoguecamerarotation2[max_dialogues][max_dialoguelength];
    

    if (argc < 2)
    {
        printf("Usage: mapinfo filename\n");
        return 256;
    }

    fp = fopen(argv[1], "rb");

    if (fp == NULL)
    {
        fprintf(stderr, "File '%s' not found...\n", argv[1]);

        return 1;
    }

    /* general stuff */
    funpackf(fp, "Bi", &mapver);
    funpackf(fp, "Bi", &maptype);
    funpackf(fp, "Bi", &hostile);
    funpackf(fp, "Bf Bf", &viewdist, &fadestart);

    /* skybox related stuff */
    funpackf(fp, "Bb Bf Bf Bf", &sb.texture, &sb.r, &sb.g, &sb.b);
    funpackf(fp, "Bf Bf Bf", &sb.lr, &sb.lg, &sb.lb);

    /* player related stuff */
    funpackf(fp, "Bf Bf Bf Bf Bf Bi", &player.cords.x, &player.cords.y, &player.cords.z,
                                      &player.rot,   &player.tgrot, &player.num_weapons);
    if (player.num_weapons > 0 && player.num_weapons < 5)
    {
        for (int i = 0; i < player.num_weapons; i++)
            funpackf(fp, "Bi", &(player.weapons[i]));
    }
    funpackf(fp, "Bf Bf Bf", &player.armorhead,      &player.armorhigh,      &player.armorlow);
    funpackf(fp, "Bf Bf Bf", &player.protectionhead, &player.protectionhigh, &player.protectionlow);
    funpackf(fp, "Bf Bf Bf", &player.metalhead,      &player.metalhigh,      &player.metallow);
    funpackf(fp, "Bf Bf",    &player.power,          &player.speedmult);
    funpackf(fp, "Bi",       &player.num_clothes);
    funpackf(fp, "Bi Bi",    &player.skin,           &player.creature);

    /* Dialogues related stuff */
    funpackf(fp, "Bi", &numdialogues);
    if (numdialogues > 0)
    {
        for (int i = 0; i < numdialogues; i++)
        {
            funpackf(fp, "Bi", &numdialogueboxes[i]);
            funpackf(fp, "Bi", &dialoguetype[i]);

            for (int j = 0; j < 10; j++)
            {
                funpackf(fp, "Bf Bf Bf", &participantlocation[i][j].x, &participantlocation[i][j].y, &participantlocation[i][j].z);
                funpackf(fp, "Bi",       &participantrotation[i][j]);
            }

            if (numdialogueboxes[i] > 0)
            {
                for (int j = 0; j < numdialogueboxes[i]; j++)
                {
                    int tmplen;

                    funpackf(fp, "Bi", &dialogueboxlocation[i][j]);
	                funpackf(fp, "Bf", &dialogueboxcolor[i][j][0]);
	                funpackf(fp, "Bf", &dialogueboxcolor[i][j][1]);
	                funpackf(fp, "Bf", &dialogueboxcolor[i][j][2]);
	                funpackf(fp, "Bi", &dialogueboxsound[i][j]);

                    funpackf(fp, "Bi", &tmplen);
                    for (int k = 0; k < tmplen; k++)
                    {
                        funpackf(fp, "Bb", &dialoguetext[i][j][k]);
	                    
                        if (dialoguetext[i][j][k] == '\r') dialoguetext[i][j][k] = ' '; // CR -> space
                        if (dialoguetext[i][j][k] == '\0') break;
                    }

                    funpackf(fp, "Bi", &tmplen);
                    printf("len: %d\n", tmplen);
                    for (int k = 0; k < tmplen; k++)
                    {
                        funpackf(fp, "Bb", &dialoguename[i][j][k]);
	                    
                        if (dialoguename[i][j][k] == '\0') break;
                    }

                    funpackf(fp, "Bf Bf Bf", &dialoguecamera[i][j].x, &dialoguecamera[i][j].y, &dialoguecamera[i][j].z);
	                funpackf(fp, "Bi", &participantfocus[i][j]);
	                funpackf(fp, "Bi", &participantaction[i][j]);

	                for(int k = 0; k < 10; k++)
	                    funpackf(fp, "Bf Bf Bf", &participantfacing[i][j][k].x, &participantfacing[i][j][k].y, &participantfacing[i][j][k].z);

	                funpackf(fp, "Bf Bf", &dialoguecamerarotation[i][j], &dialoguecamerarotation2[i][j]);
                }
            }
        }
    }

    /* clothes */
    if(player.num_clothes > 0)
    {
        for(int i = 0; i < player.num_clothes; i++)
        {
            int tmplen;

            funpackf(fp, "Bi", &tmplen);
            for(int j = 0; j < tmplen; j++)
	            funpackf(fp, "Bb", &player.clothes[i][j]);

            funpackf(fp, "Bf Bf Bf", &player.clothestintr[i], &player.clothestintg[i], &player.clothestintb[i]);
        }
    }

    /* general stuff */
    printf("Map Version: %d\n", mapver);
    printf("Map Type: %d\n", maptype);
    printf("Hostile: %d\n", hostile);
    printf("View Distance: %f | Fade start: %f\n", viewdist, fadestart);

    /* skybox stuff */
    printf("Skybox: \n");
    printf("  -> Texture: %d -> r:%f g:%f b:%f\n", sb.texture, sb.r, sb.g, sb.b);
    printf("  -> Light: r:%f g:%f b:%f\n", sb.lr, sb.lg, sb.lb);

    /* player stuff */
    printf("Player: \n");
    printf("  -> Cords: x:%f y:%f z:%f\n", player.cords.x, player.cords.y, player.cords.z);
    printf("  -> Rotation: %f | Target Rotation: %f\n", player.rot, player.tgrot);
    printf("  -> # Weapons: %d\n", player.num_weapons);
    if (player.num_weapons > 0 && player.num_weapons < 5)
    {
        printf("  -> Weapons: \n");
        for (int i = 0; i < player.num_weapons; i++)
            printf("     -> #%d: %d\n", i, player.weapons[i]);
    }
    printf("  -> Power: %f | Speed: %f\n", player.power, player.speedmult);
    printf("  -> # Clothes: %d\n", player.num_clothes);
    printf("  -> Skin: %d | Creature: %d\n", player.skin, player.creature);

    /* dialogues stuff */
    printf("# Dialogues: %d\n", numdialogues);
    if (numdialogues > 0)
    {
        for (int i = 0; i < numdialogues; i++)
        {
            printf("  -> Dialogue #%d:\n", i + 1);

            for (int j = 0; j < numdialogueboxes[i]; j++)
            {
                printf("     -> %s: %s\n", dialoguename[i][j], dialoguetext[i][j]);
            }
        }
    }

    return 0;
}
