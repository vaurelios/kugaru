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


#include <math.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <SDL.h>
#include <SDL/SDL_opengl.h>
#include <glib.h>

#include "Conf.h"
#include "Game.h"
#include "FileIO.h"
#include "Random.h"
#include "gamegl.h"
#include "Globals.h"
#include "Utility.h"
#include "openal_wrapper.h"

extern "C" {
    #include "zlib.h"
    #include "png.h"
}


using namespace std;
using namespace Kugaru;


static bool load_image        (const char *fname, TGAImageRec &tex);
static bool load_png          (const char *fname, TGAImageRec &tex);
static bool save_image        (const char *fname);
static bool save_png          (const char *fname);
int         closestResolution (int         width,
                               int         height);
int         resolutionID      (int         width,
                               int         height);
void        ReportError       (char       *strError);
void        ShutdownDSp       ();
void        DrawGL            (Game       &game);
void        CreateGLWindow    ();
bool        SetUp             (Game       &game);
void        DoUpdate          (Game       &game);
void        DoEvent           ();
void        CleanUp           ();


// Menu defs
enum 
{
    MENU_FILE_QUIT = 1
};

enum 
{
    FG_SLEEP_TIME = 10,
    BG_SLEEP_TIME = 10000
};

bool            gameFocused;
unsigned int    resolutionDepths[8][2] = {0};
static bool     g_button;
static bool     fullscreen = true;
int             kContextWidth;
int             kContextHeight;
GLuint          gFontList;
char            gcstrMode[256] = "";
uint32_t        gSleepTime = FG_SLEEP_TIME;
bool            gDone = false;
bool            gfFrontProcess = true;
Game           *pgame = 0;
Point           delta;
const RGBColor  RGB_BLACK = { 0x0000, 0x0000, 0x0000 };


static bool lookup_glsym(const char *funcname, void **func)
{
    *func = SDL_GL_GetProcAddress(funcname);

    if (*func == NULL)
    {
        fprintf(stderr, "Failed to find OpenGL symbol \"%s\"\n", funcname);
        return false;
    }

    return true;
}

static void GLAPIENTRY glDeleteTextures_doNothing(GLsizei n, const GLuint *textures)
{
    // no-op.
}

#ifdef MessageBox
#undef MessageBox
#endif
#define MessageBox(hwnd,text,title,flags) STUBBED("msgbox")
#define cmdline(c) false


void ReportError (char *strError)
{
    fprintf(stderr, "Error: %s\n", strError);
}

void ShutdownDSp ()
{
}

// OpenGL Drawing

void DrawGL(Game &game)
{
    game.DrawGLScene();
}

static void sdlEventProc(const SDL_Event &e, Game &game)
{
    int val;
    bool skipkey = false;

    switch(e.type)
    {
        case SDL_MOUSEMOTION:
            game.deltah += e.motion.xrel;
            game.deltav += e.motion.yrel;
            break;

        case SDL_MOUSEBUTTONDOWN:
            button = (SDL_MouseButtonEvent *) &e.button;
            break;

        case SDL_MOUSEBUTTONUP:
            button = NULL;
            oldbutton = 0;
            break;

        case SDL_KEYDOWN:
            /* Handle shortcuts */
            switch (e.key.keysym.sym) // we use sym instead of scancode because physical location of key does not matter for shortcuts
            {
                case SDLK_g:
                    if (e.key.keysym.mod & KMOD_CTRL)
                    {
                        skipkey = true;
                        SDL_SetRelativeMouseMode(SDL_FALSE);                    
                    }
                    break;
                case SDLK_RETURN:
                    if (e.key.keysym.mod & KMOD_ALT)
                    {
                        skipkey = true;
                        fullscreen = !fullscreen;
                        SDL_SetWindowFullscreen(screen, fullscreen);
                    }
                    break;
                case SDLK_q:
                    if (e.key.keysym.mod & KMOD_CTRL)
                    {
                        skipkey = true;
                        SDL_Quit();
                    }
                    break;
                default:
                    skipkey = false;
                    break;
            }

            if (!skipkey)
            {
                keysym = (SDL_Keysym *) &e.key.keysym;
            }

            break;

        case SDL_KEYUP:
            keysym = NULL;
            oldscancode = SDL_SCANCODE_0;
            return;
        case SDL_WINDOWEVENT:
            if (e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
                IsFocused = true;
            if (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
                IsFocused = false;
            break;
    }
}

static Point gMidPoint;

bool SetUp(Game &game)
{
    char string[10];
    SDL_GLContext glcontext;

    LOGFUNC;

    randSeed = UpTime().lo;

    slomospeed = 0.25;
    slomofreq  = 8012;
    numplayers = 1;

    kContextWidth        = cnf.getDisplayInt("width");
    kContextHeight       = cnf.getDisplayInt("height");
    usermousesensitivity = cnf.getMouseInt("mouse-speed");
    ismotionblur         = cnf.getDisplayInt("motion-blur");
    detail               = cnf.getDisplayInt("detail");
    floatjump            = cnf.getGameBool("floating-jump");
    mousejump            = cnf.getMouseBool("mouse-jump");
    ambientsound         = cnf.getSoundBool("ambient");
    blooddetail          = cnf.getDisplayInt("blood");
    autoslomo            = cnf.getDisplayInt("auto-slow-motion");
    foliage              = cnf.getDisplayBool("show-foliage");
    musictoggle          = cnf.getSoundBool("music");
    trilinear            = cnf.getDisplayBool("trilinear");
    decals               = cnf.getDisplayInt("decals");
    invertmouse          = cnf.getMouseBool("invert");
    gamespeed            = cnf.getGameInt("game-speed");
    difficulty           = cnf.getGameInt("difficulty");
    damageeffects        = cnf.getDisplayInt("damage-effects");
    texttoggle           = cnf.getGameBool("show-text");
    debugmode            = cnf.getGameBool("debug");
    vsync                = cnf.getDisplayBool("vsync");
    showpoints           = cnf.getGameBool("show-points");
    alwaysblur           = cnf.getDisplayBool("always-blur");
    immediate            = cnf.getGameBool("immediate-mode");
    velocityblur         = cnf.getDisplayInt("velocity-blur"); 
    volume               = cnf.getSoundInt("volume");
    game.forwardkey      = GetMouseOrKbd(cnf.getKeysStr("key-forward"));
    game.backkey         = GetMouseOrKbd(cnf.getKeysStr("key-back")   );
    game.leftkey         = GetMouseOrKbd(cnf.getKeysStr("key-left")   );
    game.rightkey        = GetMouseOrKbd(cnf.getKeysStr("key-right")  );
    game.jumpkey         = GetMouseOrKbd(cnf.getKeysStr("key-jump")   );
    game.crouchkey       = GetMouseOrKbd(cnf.getKeysStr("key-crouch") );
    game.drawkey         = GetMouseOrKbd(cnf.getKeysStr("key-draw")   );
    game.throwkey        = GetMouseOrKbd(cnf.getKeysStr("key-throw")  );
    game.attackkey       = GetMouseOrKbd(cnf.getKeysStr("key-attack") );
    game.chatkey         = GetMouseOrKbd(cnf.getKeysStr("key-chat")   );

    kBitsPerPixel = detail == 0 ? 16 : 32;

    if (gamespeed == 0) gamespeed = 1;
    oldgamespeed = gamespeed;

    if (detail > 2) detail = 2;
    if (detail < 0) detail = 0;
    if (kContextWidth < 0) kContextWidth = 800;
    if (kContextHeight < 0) kContextHeight = 600;
    screenwidth = kContextWidth;
    screenheight = kContextHeight;

    if (kBitsPerPixel != 32 && kBitsPerPixel != 16)
        kBitsPerPixel = 16;

    if (SDL_Init(SDL_INIT_VIDEO) == -1)
    {
        fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
        return false;
    }

    modes_count = SDL_GetNumDisplayModes(0); // TODO: Support multiple displays

    if (modes_count >= 1)
    {
        for (int i = 0; i < modes_count; i++)
        {
            SDL_DisplayMode *mode = g_new(SDL_DisplayMode, 1);
            SDL_DisplayMode *prev;

            SDL_GetDisplayMode(0, i, mode);

            if (i > 0)
            {
                prev = (SDL_DisplayMode *) g_slist_last(displaymodes)->data;

                if (prev->w == mode->w && prev->h == mode->h) // Ignore same resolutions with different refresh rate
                {
                    displaymodes = g_slist_remove(displaymodes, mode);
                    g_free(mode);
                    continue;
                }
            }

            displaymodes = g_slist_append(displaymodes, mode); 
        }
        // We set modes_count so it reflect the new value
        modes_count = g_slist_index(displaymodes, g_slist_last(displaymodes)->data) + 1;
    }
    else 
    {
        fprintf(stderr, "GetNumDisplayModes() failed: %s\n", SDL_GetError());
    }

    /* Get current display (ie system) resolution */
    SDL_DisplayMode current;
    if (SDL_GetCurrentDisplayMode(0, &current) == 0)
    {
        displaywidth = current.w;
        displaywidth = current.h;
    }

    if (cmdline("showresolutions"))
    {
        GSList *scan;

        printf("Resolutions we think are okay:\n");
        
        for (scan = displaymodes; scan != NULL; scan = scan->next)
            printf("  %d x %d\n", ((SDL_DisplayMode *) scan->data)->w, ((SDL_DisplayMode *) scan->data)->h);
    }

    if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) < 0)
    {
        fprintf(stderr, "Failed to get double buffered GL context!\n");
        SDL_Quit();
        return false;
    }

    Uint32 win_flags = 0;

    //if (!cmdline("windowed"))
    //    sdlflags |= SDL_FULLSCREEN;
    if (kContextWidth == displaywidth && kContextHeight == displayheight && fullscreen)
        win_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    else if (fullscreen)
        win_flags |= SDL_WINDOW_FULLSCREEN; 

    screen = SDL_CreateWindow("Kugaru",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              screenwidth, screenheight,
                              SDL_WINDOW_OPENGL | win_flags);

    if (screen == NULL)
    {
        fprintf(stderr, "SDL_CreateWindow() failed: %s\n", SDL_GetError());

        return false;
    }

    glcontext = SDL_GL_CreateContext(screen);

    if (glcontext == NULL)
    {
        fprintf(stderr, "Can't create a GL context: %s\n", SDL_GetError());
        return false;
    }

    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_GL_SetSwapInterval(1);

    glClear( GL_COLOR_BUFFER_BIT );
    SDL_GL_SwapWindow(screen);

    // clear all states
    glDisable( GL_ALPHA_TEST );
    glDisable( GL_BLEND );
    glDisable( GL_DEPTH_TEST) ;
    // glDisable( GL_DITHER);
    glDisable( GL_FOG );
    glDisable( GL_LIGHTING );
    glDisable( GL_LOGIC_OP );
    glDisable( GL_STENCIL_TEST );
    glDisable( GL_TEXTURE_1D );
    glDisable( GL_TEXTURE_2D );
    glPixelTransferi( GL_MAP_COLOR, GL_FALSE );
    glPixelTransferi( GL_RED_SCALE, 1 );
    glPixelTransferi( GL_RED_BIAS, 0 );
    glPixelTransferi( GL_GREEN_SCALE, 1 );
    glPixelTransferi( GL_GREEN_BIAS, 0 );
    glPixelTransferi( GL_BLUE_SCALE, 1 );
    glPixelTransferi( GL_BLUE_BIAS, 0 );
    glPixelTransferi( GL_ALPHA_SCALE, 1 );
    glPixelTransferi( GL_ALPHA_BIAS, 0 );

    // set initial rendering states
    glShadeModel( GL_SMOOTH );
    glClearDepth( 1.0f );
    glDepthFunc( GL_LEQUAL );
    glDepthMask( GL_TRUE );
    // glDepthRange( FRONT_CLIP, BACK_CLIP);
    glEnable( GL_DEPTH_TEST );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
    glCullFace( GL_FRONT );
    glEnable( GL_CULL_FACE );
    glEnable( GL_LIGHTING );
    // glEnable( GL_LIGHT_MODEL_AMBIENT);
    glEnable( GL_DITHER );
    glEnable( GL_COLOR_MATERIAL );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glAlphaFunc( GL_GREATER, 0.5f );

    GLint width = kContextWidth;
    GLint height = kContextHeight;
    gMidPoint.h = 783;//width / 2;
    gMidPoint.v = 384;//height / 2;

    game.newdetail = detail;
    game.newscreenwidth = screenwidth;
    game.newscreenheight = screenheight;

    screenwidth = 1366;
    screenheight = 768;

    game.InitGame();

    return true;
}


static void DoMouse(Game &game)
{

    if(mainmenu||(abs(game.deltah)<10*realmultiplier*1000&&abs(game.deltav)<10*realmultiplier*1000))
    {
        game.deltah *= (usermousesensitivity / 5.f);
        game.deltav *= (usermousesensitivity / 5.f);
        game.mousecoordh += game.deltah;
        game.mousecoordv += game.deltav;
        if (game.mousecoordh < 0)
            game.mousecoordh = 0;
        else if (game.mousecoordh >= kContextWidth)
            game.mousecoordh = kContextWidth - 1;
        if (game.mousecoordv < 0)
            game.mousecoordv = 0;
        else if (game.mousecoordv >= kContextHeight)
            game.mousecoordv = kContextHeight - 1;
    }

}

void DoFrameRate (int update)
{    
    static long frames = 0;

    static AbsoluteTime time = {0,0};
    static AbsoluteTime frametime = {0,0};
    AbsoluteTime currTime = UpTime ();
    double deltaTime = (float) AbsoluteDeltaToDuration (currTime, frametime);

    if (0 > deltaTime)           // if negative microseconds
        deltaTime /= -1000000.0;
    else                         // else milliseconds
        deltaTime /= 1000.0;

    multiplier=deltaTime;
    if(multiplier<.001)multiplier=.001;
    if(multiplier>10)multiplier=10;
    if(update)frametime = currTime;     // reset for next time interval

    deltaTime = (float) AbsoluteDeltaToDuration (currTime, time);

    if (0 > deltaTime)           // if negative microseconds
        deltaTime /= -1000000.0;
    else                         // else milliseconds
        deltaTime /= 1000.0;
    frames++;
    if (0.001 <= deltaTime)    // has update interval passed
    {
        if(update){
            time = currTime;    // reset for next time interval
            frames = 0;
        }
    }
}


void DoUpdate(Game &game)
{
    static float sps=200;
    static int count;
    static float oldmult;

    DoFrameRate(1);
    if(multiplier>.6)multiplier=.6;

    game.fps=1/multiplier;

    count = multiplier*sps;
    if(count<2)count=2;
    //if(count>10)count=10;

    realmultiplier=multiplier;
    multiplier*=gamespeed;
    if(difficulty==1)multiplier*=.9;
    if(difficulty==0)multiplier*=.8;

    if(game.loading==4)multiplier*=.00001;
    //multiplier*.9;
    if(slomo&&!mainmenu)multiplier*=slomospeed;
    //if(freeze)multiplier*=0.00001;
    oldmult=multiplier;
    multiplier/=(float)count;

    DoMouse(game);

    game.TickOnce();

    for(int i=0;i<count;i++)
    {
        game.Tick();
    }
    multiplier=oldmult;

    game.TickOnceAfter();
/* - Debug code to test how many channels were active on average per frame
    static long frames = 0;

    static AbsoluteTime start = {0,0};
    AbsoluteTime currTime = UpTime ();
    static int num_channels = 0;
    
    num_channels += OPENAL_GetChannelsPlaying();
    double deltaTime = (float) AbsoluteDeltaToDuration (currTime, start);

    if (0 > deltaTime)           // if negative microseconds
        deltaTime /= -1000000.0;
    else                         // else milliseconds
        deltaTime /= 1000.0;

    ++frames;

    if (deltaTime >= 1)
    {
        start = currTime;
        float avg_channels = (float)num_channels / (float)frames;

        ofstream opstream("log.txt",ios::app); 
        opstream << "Average frame count: ";
        opstream << frames;
        opstream << " frames - ";
        opstream << avg_channels;
        opstream << " per frame.\n";
        opstream.close();

        frames = 0;
        num_channels = 0;
    } */
    DrawGL (game);
}

void CleanUp (void)
{
    LOGFUNC;

    SDL_Quit();
}

#ifndef WIN32
// (code lifted from physfs: http://icculus.org/physfs/ ... zlib license.)
static char *findBinaryInPath(const char *bin, char *envr)
{
    size_t alloc_size = 0;
    char *exe = NULL;
    char *start = envr;
    char *ptr;

    do
    {
        size_t size;
        ptr = strchr(start, ':');  /* find next $PATH separator. */
        if (ptr)
            *ptr = '\0';

        size = strlen(start) + strlen(bin) + 2;
        if (size > alloc_size)
        {
            char *x = (char *) realloc(exe, size);
            if (x == NULL)
            {
                if (exe != NULL)
                    free(exe);
                return(NULL);
            } /* if */

            alloc_size = size;
            exe = x;
        } /* if */

        /* build full binary path... */
        strcpy(exe, start);
        if ((exe[0] == '\0') || (exe[strlen(exe) - 1] != '/'))
            strcat(exe, "/");
        strcat(exe, bin);

        if (access(exe, X_OK) == 0)  /* Exists as executable? We're done. */
        {
            strcpy(exe, start);  /* i'm lazy. piss off. */
            return(exe);
        } /* if */

        start = ptr + 1;  /* start points to beginning of next element. */
    } while (ptr != NULL);

    if (exe != NULL)
        free(exe);

    return(NULL);  /* doesn't exist in path. */
} /* findBinaryInPath */


char *calcBaseDir(const char *argv0)
{
    /* If there isn't a path on argv0, then look through the $PATH for it. */
    char *retval;
    char *envr;

    const char *ptr = strrchr((char *)argv0, '/');
    if (strchr(argv0, '/'))
    {
        retval = strdup(argv0);
        if (retval)
            *((char *) strrchr(retval, '/')) = '\0';
        return(retval);
    }

    envr = getenv("PATH");
    if (!envr) return NULL;
    envr = strdup(envr);
    if (!envr) return NULL;
    retval = findBinaryInPath(argv0, envr);
    free(envr);
    return(retval);
}

static inline void chdirToAppPath(const char *argv0)
{
    char *dir = calcBaseDir(argv0);
    if (dir)
    {
        #if (defined(__APPLE__) && defined(__MACH__))
        // Chop off /Contents/MacOS if it's at the end of the string, so we
        //  land in the base of the app bundle.
        const size_t len = strlen(dir);
        const char *bundledirs = "/Contents/MacOS";
        const size_t bundledirslen = strlen(bundledirs);
        if (len > bundledirslen)
        {
            char *ptr = (dir + len) - bundledirslen;
            if (strcasecmp(ptr, bundledirs) == 0)
                *ptr = '\0';
        }
        #endif
        chdir(dir);
        free(dir);
    }
}
#endif


int main(int argc, char **argv)
{
    LOGFUNC;

    try
    {
        Game game;
        pgame = &game;

        //ofstream os("error.txt");
        //os.close();
        //ofstream os("log.txt");
        //os.close();

        if (!SetUp(game))
            return 42;

        while (!gDone&&!game.quit&&(!game.tryquit))
        {
            if (IsFocused)
            {
                game.deltah = 0;
                game.deltav = 0;
                SDL_Event e;
                // message pump
                while( SDL_PollEvent( &e ) )
                {
                    if( e.type == SDL_QUIT )
                    {
                        gDone=true;
                        break;
                }
                sdlEventProc(e, game);
                }

                // game
                DoUpdate(game);
            }
            else
            {
                if (IsFocused)
                    DoUpdate(game);

                // game is not in focus, give CPU time to other apps by waiting for messages instead of 'peeking'
                STUBBED("give up CPU but sniff the event queue");
            }
        }

        pgame = 0;

        CleanUp();

        return 0;
    }
    catch (const std::exception& error)
    {
        CleanUp();

        std::string e = "Caught exception: ";
        e += error.what();

        LOG(e);

        MessageBox(g_windowHandle, error.what(), "ERROR", MB_OK | MB_ICONEXCLAMATION);
    }

    CleanUp();

    return -1;
}

extern "C" void PlaySoundEx(int chan, OPENAL_SAMPLE *sptr, OPENAL_DSPUNIT *dsp, signed char startpaused)
{
    const OPENAL_SAMPLE * currSample = OPENAL_GetCurrentSample(channels[chan]);
    if (currSample && currSample == samp[chan])
    {
        if (OPENAL_GetPaused(channels[chan]))
        {
            OPENAL_StopSound(channels[chan]);
            channels[chan] = OPENAL_FREE;
        }
        else if (OPENAL_IsPlaying(channels[chan]))
        {
            int loop_mode = OPENAL_GetLoopMode(channels[chan]);
            if (loop_mode & OPENAL_LOOP_OFF)
            {
                channels[chan] = OPENAL_FREE;
            }
        }
    }
    else
    {
        channels[chan] = OPENAL_FREE;
    }

    channels[chan] = OPENAL_PlaySoundEx(channels[chan], sptr, dsp, startpaused);
    if (channels[chan] < 0)
    {
        channels[chan] = OPENAL_PlaySoundEx(OPENAL_FREE, sptr, dsp, startpaused);
    }
}

extern "C" void PlayStreamEx(int chan, OPENAL_STREAM *sptr, OPENAL_DSPUNIT *dsp, signed char startpaused)
{
    const OPENAL_SAMPLE * currSample = OPENAL_GetCurrentSample(channels[chan]);
    if (currSample && currSample == OPENAL_Stream_GetSample(sptr))
    {
        OPENAL_StopSound(channels[chan]);
        OPENAL_Stream_Stop(sptr);
    }
    else
    {
        OPENAL_Stream_Stop(sptr);
        channels[chan] = OPENAL_FREE;
    }

    channels[chan] = OPENAL_Stream_PlayEx(channels[chan], sptr, dsp, startpaused);
    if (channels[chan] < 0)
    {
        channels[chan] = OPENAL_Stream_PlayEx(OPENAL_FREE, sptr, dsp, startpaused);
    }
}

bool LoadImage(const char * fname, TGAImageRec & tex)
{
    bool res = true;

    if ( tex.data == NULL )
    {
        return false;
    }

    res = load_image(fname, tex);

    return res;
}

void ScreenShot(const char * fname)
{
    save_image(fname);
}

static bool load_image(const char *file_name, TGAImageRec &tex)
{
    char error[256];

    const char *ptr = strrchr((char *)file_name, '.');
    if (ptr)
    {
        if (strcasecmp(ptr + 1, "png") == 0)
            return load_png(file_name, tex);
    }

    sprintf(error, "Unsupported image type for file '%s'", file_name);
    STUBBED(error);
    return false;
}

static bool load_png(const char *file_name, TGAImageRec &tex)
{
    printf("Loading png '%s' file...\n", file_name);
    bool hasalpha = false;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_uint_32 width, height;
    int bit_depth, color_type, interlace_type;
    png_byte **rows = NULL;
    bool retval = false;
    png_byte **row_pointers = NULL;
    FILE *fp = fopen(file_name, "rb");

    if (fp == NULL)
        return false;

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
        goto png_done;

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
        goto png_done;

    if (setjmp(png_jmpbuf(png_ptr)))
        goto png_done;

    png_init_io(png_ptr, fp);
    png_read_png(png_ptr, info_ptr,
                 PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING,
                 NULL);
    png_get_IHDR(png_ptr, info_ptr, &width, &height,
                 &bit_depth, &color_type, &interlace_type, NULL, NULL);

    if (bit_depth != 8)  // transform SHOULD handle this...
        goto png_done;

    if (color_type & PNG_COLOR_MASK_PALETTE)  // !!! FIXME?
        goto png_done;

    if ((color_type & PNG_COLOR_MASK_COLOR) == 0)  // !!! FIXME?
        goto png_done;

    hasalpha = ((color_type & PNG_COLOR_MASK_ALPHA) != 0);
    row_pointers = png_get_rows(png_ptr, info_ptr);
    if (!row_pointers)
        goto png_done;

    if (!hasalpha)
    {
        png_byte *dst = tex.data;
        for (int i = height-1; i >= 0; i--)
        {
            png_byte *src = row_pointers[i];
            for (int j = 0; j < width; j++)
            {
                dst[0] = src[0];
                dst[1] = src[1];
                dst[2] = src[2];
                dst[3] = 0xFF;
                src += 3;
                dst += 4;
            }
        }
    }

    else
    {
        png_byte *dst = tex.data;
        int pitch = width * 4;
        for (int i = height-1; i >= 0; i--, dst += pitch)
            memcpy(dst, row_pointers[i], pitch);
    }

    tex.sizeX = width;
    tex.sizeY = height;
    tex.bpp = 32;
    retval = true;

png_done:
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    if (fp)
        fclose(fp);

    return (retval);
}

static bool save_image(const char *file_name)
{
    const char *ptr = strrchr((char *) file_name, '.');
    char error[256];

    if (ptr)
    {
        if (strcasecmp(ptr + 1, "png") == 0)
            return save_png(file_name);
    }

    sprintf(error, "Unsupported image type for file '%s'", file_name);
    STUBBED(error);

    return false;
}

static bool save_png(const char *file_name)
{
    FILE *fp = NULL;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    bool retval = false;

    fp = fopen(file_name, "wb");
    if (fp == NULL)
        return false;

    png_bytep *row_pointers = new png_bytep[kContextHeight];
    png_bytep screenshot = new png_byte[kContextWidth * kContextHeight * 3];
    if ((!screenshot) || (!row_pointers))
        goto save_png_done;

    glGetError();
    glReadPixels(0, 0, kContextWidth, kContextHeight,
                 GL_RGB, GL_UNSIGNED_BYTE, screenshot);
    if (glGetError() != GL_NO_ERROR)
        goto save_png_done;

    for (int i = 0; i < kContextHeight; i++)
        row_pointers[i] = screenshot + ((kContextWidth * ((kContextHeight-1) - i)) * 3);

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
        goto save_png_done;

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
        goto save_png_done;

    if (setjmp(png_jmpbuf(png_ptr)))
        goto save_png_done;

    png_init_io(png_ptr, fp);

    if (setjmp(png_jmpbuf(png_ptr)))
        goto save_png_done;

    png_set_IHDR(png_ptr, info_ptr, kContextWidth, kContextHeight,
                 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    if (setjmp(png_jmpbuf(png_ptr)))
        goto save_png_done;

    png_write_image(png_ptr, row_pointers);

    if (setjmp(png_jmpbuf(png_ptr)))
        goto save_png_done;

    png_write_end(png_ptr, NULL);
    retval = true;

save_png_done:
    png_destroy_write_struct(&png_ptr, &info_ptr);
    delete[] screenshot;
    delete[] row_pointers;
    if (fp)
        fclose(fp);
    if (!retval)
        unlink(ConvertFileName(file_name));
    return retval;
}

