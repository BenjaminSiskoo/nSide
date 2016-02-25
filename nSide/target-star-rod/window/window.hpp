struct WindowManager {
  WindowManager();

  struct WindowItem {
    Window* window;
    string name;
    string geometry;
  };
  vector<WindowItem> windowList;
  Configuration::Document config;

  auto append(Window* window, const string& name) -> void;
  auto loadGeometry() -> void;
  auto saveGeometry() -> void;
};

extern unique_pointer<WindowManager> windowManager;
