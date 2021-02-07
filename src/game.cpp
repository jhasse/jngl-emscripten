#include <jngl.hpp>
#include <jngl/init.hpp>

class MyWork : public jngl::Work {
	void step() override {
	}
	void draw() const override {
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
	}
};

std::function<std::shared_ptr<jngl::Work>()> jnglInit(jngl::AppParameters& params) {
	params.screenSize = { 800, 600 };
	return []() {
		jngl::setFont("data/Poppins-Bold.ttf");
		return std::make_shared<MyWork>();
	};
}
