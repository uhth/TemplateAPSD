#include "../include/graphics.h"

#include <stdexcept>
#include <iostream>


void* eventHandler( ALLEGRO_THREAD *thread, void *arg ) {
	GraphicsContext *graphicsContext = (GraphicsContext*) arg;
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
	ALLEGRO_EVENT event;
    if (graphicsContext->thread == nullptr) throw std::runtime_error("thread not initialized");
    if (graphicsContext->display == nullptr) throw std::runtime_error("display not initialized");
	al_register_event_source(event_queue, al_get_display_event_source(graphicsContext->display));	
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	while( true ) {
		al_wait_for_event(event_queue, &event);
		if( event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            std::cout << "Exit" << std::endl;
            graphicsContext->pushToEventQueue(EXIT);
			return NULL;
		}
        else if( event.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
            al_acknowledge_resize(graphicsContext->display);
            graphicsContext->calculateCellSize();
        }
		else if( event.type == ALLEGRO_EVENT_KEY_DOWN) { //switch automa
			switch(event.keyboard.keycode) {
				default:
					break;
				case ALLEGRO_KEY_ESCAPE :
					std::cout << "Exit";
                    graphicsContext->pushToEventQueue(EXIT);
			        return NULL;
					break;
				case ALLEGRO_KEY_LEFT :
					graphicsContext->pushToEventQueue(PREVIOUS);
					break;
				case ALLEGRO_KEY_RIGHT :
                    graphicsContext->pushToEventQueue(NEXT);
					break;
                case ALLEGRO_KEY_R :
                    graphicsContext->pushToEventQueue(RESET);
					break;
                case ALLEGRO_KEY_UP :
                    graphicsContext->incColorToneMultiplier(10);
					break;
                case ALLEGRO_KEY_DOWN :
                    graphicsContext->decColorToneMultiplier(10);
					break;
			}
		}
	}
	return NULL;
}

void GraphicsContext::init() { 
    // initialize
    if(!al_init() || !al_init_primitives_addon() || !al_init_font_addon() || !al_init_ttf_addon() || !al_install_keyboard()) 
        throw std::runtime_error("Graphics: Allegro/Addons failed to initialize");
    // load font
    ttf_font = al_load_ttf_font(PATH_TO_FONT, 22, 0);
    if(ttf_font == nullptr) std::cerr << "NO TTF Font found. It won't be possible to print text." << std::endl;
    // create display
    al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE | ALLEGRO_OPENGL_3_0);
	display = al_create_display(D_WIDTH, D_HEIGHT);
	if (display == nullptr)
        throw std::runtime_error("Graphics: Display failed to initialize");
    //resize stuff
    backBuffer = al_get_backbuffer(display);
    calculateCellSize();
    //thread
    thread = al_create_thread(eventHandler, this);
	al_start_thread(thread);
}

void GraphicsContext::exit() {
    al_destroy_thread(thread);
    al_destroy_display(display);
}

void GraphicsContext::pushToEventQueue(EVENT event) {
    mutex.lock();
    eventQueue.push(event);
    mutex.unlock();
}

EVENT GraphicsContext::popFromEventQueue() {
    EVENT event = NONE;
    mutex.lock();
    if (!eventQueue.empty()) {
        event = eventQueue.front();
        eventQueue.pop();
    }
    mutex.unlock();
    return event;
}

void GraphicsContext::calculateCellSize() {
    float displayWidth = al_get_display_width(display);
    float displayHeight = al_get_display_height(display);
    cellWidth = (displayWidth / arrWidth);
    cellHeight = (displayHeight / arrHeight);
}

void GraphicsContext::calculateVerticesCoords(int y, int x, ALLEGRO_COLOR& color) {
    float x1 = x * cellWidth;
    float y1 = y * cellHeight;
    float x2 = x1 + cellWidth;
    float y2 = y1 + cellHeight;

    //x1 += PADDING; x2 -= PADDING; y1 += PADDING; y2 -= PADDING;

    size_t pos = (x * arrWidth) + y;

    vertices[(pos * 6) + 0] = {.x=x1, .y=y1, .z=0, .u=0, .v=0, .color=color};
    vertices[(pos * 6) + 1] = {.x=x1, .y=y2, .z=0, .u=0, .v=0, .color=color};
    vertices[(pos * 6) + 2] = {.x=x2, .y=y2, .z=0, .u=0, .v=0, .color=color};
    vertices[(pos * 6) + 3] = {.x=x1, .y=y1, .z=0, .u=0, .v=0, .color=color};
    vertices[(pos * 6) + 4] = {.x=x2, .y=y1, .z=0, .u=0, .v=0, .color=color};
    vertices[(pos * 6) + 5] = {.x=x2, .y=y2, .z=0, .u=0, .v=0, .color=color};
    
}        


void GraphicsContext::printOnScreen(Array2D* array2d) {
    al_clear_to_color(al_map_rgb(0,0,0));
    for(size_t row = 0; row < array2d->getHeight(); ++row) {
        for(size_t col = 0; col < array2d->getWidth(); ++col) {
            uint_fast8_t value = *array2d->at(row, col);
            ALLEGRO_COLOR color = (value == 0) ? al_map_rgb(0,0,0) : al_color_hsl(value * colorToneMultiplier, 0.8f, 0.5f);
            calculateVerticesCoords(row, col, color);
        }
    }
    al_draw_prim(vertices, NULL, 0, 0, nVertices, ALLEGRO_PRIM_TRIANGLE_LIST);
    drawInfo();
    al_flip_display();
}

void GraphicsContext::setColorToneMultiplier(uint_fast8_t ctm) {
    mutex.lock();
    colorToneMultiplier = ctm;
    mutex.unlock();
}

void GraphicsContext::incColorToneMultiplier(size_t step) {
    mutex.lock();
    colorToneMultiplier += step;
    mutex.unlock();
}
void GraphicsContext::decColorToneMultiplier(size_t step) {
    mutex.lock();
    colorToneMultiplier -= step;
    mutex.unlock();
}
void GraphicsContext::drawInfo() {
    if(ttf_font == nullptr) return;
    //fps
    uint64_t latency = timeTracker.elapsed();
    if (latency < 1) return;
    uint64_t fps = 1000 / latency;
    al_draw_textf(ttf_font, al_map_rgb(255,255,255), 8, 8, 0, "FPS: %ld", fps);
}

uint64_t TimeTracker::elapsed() {
    uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    uint64_t diff = now - lastStoredTime;
    lastStoredTime = now;
    return diff;
}