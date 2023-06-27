#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <mutex>
#include <queue>

#include "uint8_array2d.h"
#include "config.h"
#include "common.h"

class TimeTracker {
    private:
        std::recursive_mutex r_mutex;
        uint64_t lastStoredTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    public:
        uint64_t elapsed();
};

class GraphicsContext {
    private:
        ALLEGRO_FONT *ttf_font;
        ALLEGRO_DISPLAY *display;
        ALLEGRO_BITMAP *backBuffer;
        ALLEGRO_THREAD *thread;
        ALLEGRO_VERTEX *vertices;
        std::recursive_mutex r_mutex;
        std::queue<EVENT> eventQueue; //Non-blocking shared res
        uint_fast8_t colorToneMultiplier = 1;
        TimeTracker timeTracker;
        float cellWidth;
        float cellHeight;
        size_t arrWidth;
        size_t arrHeight;
        size_t nVertices;
    protected:
        void calculateVerticesCoords(int,int,ALLEGRO_COLOR&);
        void calculateCellSize();
    public:
        GraphicsContext(size_t arrWidthIn, size_t arrHeightIn) : arrWidth(arrWidthIn), arrHeight(arrHeightIn), nVertices(arrWidthIn * arrHeightIn * 6) { vertices = new ALLEGRO_VERTEX[nVertices]; }
        void init();
        void exit();
        void printOnScreen(Array2D*);
        void setColorToneMultiplier(uint_fast8_t);
        void incColorToneMultiplier(size_t=1);
        void decColorToneMultiplier(size_t=1);
        void pushToEventQueue(EVENT);
        void drawInfo();
        EVENT popFromEventQueue();
        ~GraphicsContext() { delete[] vertices; }

        std::function<void(int,int,uint_fast8_t,uint_fast8_t)> getPrintFunc();
        friend void* eventHandler(ALLEGRO_THREAD *thread, void *arg);
        
};

#endif