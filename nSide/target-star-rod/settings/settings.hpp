struct Settings : Configuration::Document {
  struct Video : Configuration::Node {
    string driver;
    bool synchronize;
  } video;

  struct Audio : Configuration::Node {
    string driver;
    bool synchronize;
    bool mute;
  } audio;

  struct Input : Configuration::Node {
    string driver;
  } input;

  string folderpath;

  Settings();
  auto load() -> void;
  auto unload() -> void;
};

extern unique_pointer<Settings> settings;
