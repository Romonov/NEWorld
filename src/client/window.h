/*
* NEWorld: A free game with similar rules to Minecraft.
* Copyright (C) 2016 NEWorld Team
*
* This file is part of NEWorld.
* NEWorld is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* NEWorld is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef WINDOW_H_
#define WINDOW_H_

#include <string>
#include <SDL2/SDL.h>
#include <imgui.h>

class Window
{
public:
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    void makeCurrentDraw() const
    {
        SDL_GL_MakeCurrent(mWindow, mContext);
    }

    void swapBuffers() const
    {
        SDL_GL_SwapWindow(mWindow);
    }

    bool isKeyDown(SDL_Scancode scancode)
    {
        return SDL_GetKeyboardState(nullptr)[scancode] == 1u;
    }

    void pollEvents()
    {
        /*
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            // TODO: Process events here
        }
        */
        SDL_PumpEvents();
    }

    static Window& getInstance(const std::string& title="", int width=0, int height=0)
    {
        static Window win(title, width, height);
        return win;
    }

private:
    SDL_Window* mWindow = nullptr;
    std::string mTitle;
    int mWidth, mHeight;

    Window(const std::string& title, int width, int height);
    ~Window();

    SDL_GLContext mContext;
};

#endif
