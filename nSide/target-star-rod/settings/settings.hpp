struct Settings : Configuration::Document {
  struct Video : Configuration::Node {
    bool synchronize;
  } video;

  struct Audio : Configuration::Node {
    bool synchronize;
    bool mute;
  } audio;

  struct Input : Configuration::Node {
  } input;

  string folderpath;

  Settings();
  auto load() -> void;
  auto unload() -> void;
};

extern unique_pointer<Settings> settings;
