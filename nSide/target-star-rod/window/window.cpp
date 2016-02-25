#include "../laevateinn.hpp"
unique_pointer<WindowManager> windowManager;

WindowManager::WindowManager() {
  windowManager = this;
}

auto WindowManager::append(Window* window, const string& name) -> void {
  windowList.append({window, name, string{
    window->geometry().x(), ",",
    window->geometry().y(), ",",
    window->geometry().width(), ",",
    window->geometry().height()
  }});
}

auto WindowManager::loadGeometry() -> void {
  for(auto& window : windowList) {
    config.append(window.geometry, window.name);
  }
  config.load(locate("geometry.bml"));
  config.save(locate("geometry.bml"));
  for(auto& window : windowList) {
    lstring parts = window.geometry.split(",");
    int x = integer(parts[0]);
    int y = integer(parts[1]);
    int width = integer(parts[2]);
    int height = integer(parts[3]);
    window.window->setGeometry({x, y, width, height});
  }
}

auto WindowManager::saveGeometry() -> void {
  for(auto& window : windowList) {
    window.geometry = string{
      window.window->geometry().x(), ",",
      window.window->geometry().y(), ",",
      window.window->geometry().width(), ",",
      window.window->geometry().height()
    };
    window.window->setVisible(false);
  }
  config.save(locate("geometry.bml"));
}
