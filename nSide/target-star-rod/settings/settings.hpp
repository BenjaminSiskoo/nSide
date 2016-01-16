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
  void load();
  void unload();
};

extern Settings* settings;
