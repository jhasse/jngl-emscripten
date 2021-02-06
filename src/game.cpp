#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#ifdef __EMSCRIPTEN__

#include <emscripten.h>
#include <SDL.h>

#define GL_GLEXT_PROTOTYPES 1
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#else

#include <SDL2/SDL.h>

#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL_opengles2.h>

#endif

#include <jngl.hpp>

std::function<void()> loop;
void main_loop() {
	loop();
}

JNGL_MAIN_BEGIN {
	jngl::showWindow("Tutorial 2 - Printing Text", 800, 600);
    jngl::setFont("data/Poppins-Bold.ttf");

	loop = [&] {
		jngl::updateInput();
		jngl::setBackgroundColor(jngl::keyDown('s') ? 0 : 255, 0, 0);
		jngl::print("Hello World!", -100, -10);
		// jngl::draw("data/jngl.png", jngl::getMousePos());
		jngl::setColor(120, 120, 120, 120);
		jngl::drawRect(-90000, -90000, 2000000, 2000000);
		jngl::setColor(0, 0, 0);
		jngl::drawCircle(jngl::getMousePos(), 30);
		if (jngl::keyDown('a')) {
			jngl::debugLn("asdf");
		}
		jngl::swapBuffers();
	};
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, true);
#else
	while (jngl::running()) {
		loop();
	}
#endif
}
JNGL_MAIN_END
