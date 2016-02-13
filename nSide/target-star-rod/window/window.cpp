#include "../laevateinn.hpp"
WindowManager* windowManager = nullptr;

WindowManager::WindowManager() {
  windowManager = this;
}

void WindowManager::append(Window* window, const string& name) {
  windowList.append({window, name, string{
    window->geometry().x(), ",",
    window->geometry().y(), ",",
    window->geometry().width(), ",",
    window->geometry().height()
  }});
}

void WindowManager::loadGeometry() {
  for(auto& window : windowList) {
    config.append(window.geometry, window.name);
  }
  config.load(locate({localpath(), "star-rod/"}, "star-rod-geometry.bml"));
  config.save(locate({localpath(), "star-rod/"}, "star-rod-geometry.bml"));
  for(auto& window : windowList) {
    lstring parts = window.geometry.split(",");
    int x = integer(parts[0]);
    int y = integer(parts[1]);
    int width = integer(parts[2]);
    int height = integer(parts[3]);
    window.window->setGeometry({x, y, width, height});
  }
}

void WindowManager::saveGeometry() {
  for(auto& window : windowList) {
    window.geometry = string{
      window.window->geometry().x(), ",",
      window.window->geometry().y(), ",",
      window.window->geometry().width(), ",",
      window.window->geometry().height()
    };
    window.window->setVisible(false);
  }
  config.save(locate({localpath(), "star-rod/"}, "star-rod-geometry.bml"));
}
