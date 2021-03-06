//
// Copyright 2017 Timo Kloss
//
// This file is part of LowRes NX.
//
// LowRes NX is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LowRes NX is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with LowRes NX.  If not, see <http://www.gnu.org/licenses/>.
//

#include "settings.h"
#include <string.h>

#ifdef __EMSCRIPTEN__
#include <SDL2/SDL.h>
#elif defined(__APPLE__) && defined(__MACH__)
#include <SDL2/SDL.h>
#elif defined(__LINUX__)
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

#ifdef _WIN32
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif
const char *defaultProgramsPath = "programs" PATH_SEPARATOR;

const char *defaultSettings = "# Lines starting with a # character are comments. Remove the # to enable an option.\n\n# Starts the application in fullscreen mode\n#fullscreen yes\n\n# Path for the tool programs and virtual disk file\n#programs path\n";

void settings_setValue(struct Settings *settings, const char *key, const char *value);


void settings_init(struct Settings *settings, int argc, const char * argv[])
{
    // default values
    
    char *basePath = SDL_GetBasePath();
    if (basePath)
    {
        strncpy(settings->programsPath, basePath, FILENAME_MAX - 1);
        strncat(settings->programsPath, defaultProgramsPath, FILENAME_MAX - 1);
        SDL_free(basePath);
        basePath = NULL;
    }
    else
    {
        strncpy(settings->programsPath, defaultProgramsPath, FILENAME_MAX - 1);
    }
    
    // load settings file
    
    char *prefPath = SDL_GetPrefPath("Inutilis Software", "LowRes NX");
    if (prefPath)
    {
        char filename[FILENAME_MAX];
        strncpy(filename, prefPath, FILENAME_MAX - 1);
        strncat(filename, "settings.txt", FILENAME_MAX - 1);
        SDL_free((void *)prefPath);
        prefPath = NULL;
        
        FILE *file = fopen(filename, "r");
        if (file)
        {
            // load settings
            char line[256];
            while (fgets(line, 256, file))
            {
                if (line[0] != '#')
                {
                    char *space = strchr(line, ' ');
                    if (space)
                    {
                        *space = 0; // separate into two strings
                        char *value = space + 1;
                        
                        // remove EOL characters
                        char *eolChar = strchr(value, '\n');
                        if (eolChar)
                        {
                            *eolChar = 0;
                        }
                        eolChar = strchr(value, '\r');
                        if (eolChar)
                        {
                            *eolChar = 0;
                        }
                        
                        settings_setValue(settings, line, value);
                    }
                }
            }
            
            fclose(file);
        }
        else
        {
            // write default settings file
            FILE *file = fopen(filename, "w");
            if (file)
            {
                fputs(defaultSettings, file);
                fclose(file);
            }
        }
    }
    
    // parse arguments
    
    for (int i = 1; i < argc; i++)
    {
        const char *arg = argv[i];
        if (*arg == '-') {
            i++;
            if (i < argc)
            {
                settings_setValue(settings, arg + 1, argv[i]);
            }
            else
            {
                SDL_Log("missing value for parameter %s", arg);
            }
        } else {
            settings->program = arg;
        }
    }
}

void settings_setValue(struct Settings *settings, const char *key, const char *value)
{
    if (strcmp(key, "fullscreen") == 0) {
        if (strcmp(value, "yes") == 0)
        {
            settings->fullscreen = true;
        }
        else if (strcmp(value, "no") == 0)
        {
            settings->fullscreen = false;
        }
    }
    else if (strcmp(key, "programs") == 0)
    {
        strncpy(settings->programsPath, value, FILENAME_MAX - 1);
		size_t len = strlen(settings->programsPath);
		if (settings->programsPath[len - 1] != PATH_SEPARATOR[0])
		{
			strncat(settings->programsPath, PATH_SEPARATOR, FILENAME_MAX - 1);
		}

    }
}
