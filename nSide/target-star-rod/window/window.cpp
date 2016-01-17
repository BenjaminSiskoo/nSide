#include "../laevateinn.hpp"
WindowManager* windowManager = nullptr;

WindowManager::WindowManager() {
  windowManager = this;
}

void WindowManager::append(Window* window, const string& name) {
  windowList.append({window, name, window->geometry().text()});
}

void WindowManager::loadGeometry() {
  for(auto& window : windowList) {
    config.append(window.geometry, window.name);
  }
  config.load(locate({localpath(), "star-rod/"}, "star-rod-geometry.bml"));
  config.save(locate({localpath(), "star-rod/"}, "star-rod-geometry.bml"));
  for(auto& window : windowList) {
    window.window->setGeometry(window.geometry);
  }
}

void WindowManager::saveGeometry() {
  for(auto& window : windowList) {
    window.geometry = window.window->geometry().text();
    window.window->setVisible(false);
  }
  config.save(locate({localpath(), "star-rod/"}, "star-rod-geometry.bml"));
}
