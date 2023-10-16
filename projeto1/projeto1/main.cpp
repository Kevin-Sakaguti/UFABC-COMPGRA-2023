#include "window.hpp"

int main(int argc, char **argv) {
  try {
    abcg::Application app(argc, argv);

    Window window;
    window.setOpenGLSettings({.samples = 2, .doubleBuffering = false});
    window.setWindowSettings({.width = 400,
                              .height = 400,
                              .showFullscreenButton = false,
                              .title = "projeto1"});

    app.run(window);
  } catch (std::exception const &exception) {
    fmt::print(stderr, "{}\n", exception.what());
    return -1;
  }
  return 0;
}