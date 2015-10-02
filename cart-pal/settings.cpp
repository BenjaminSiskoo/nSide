struct Settings : Configuration::Document {
  Settings();
  ~Settings();

  Configuration::Node root;
  string activePath;
  string libraryPath;
  bool createManifests = false;
  bool useDatabase = true;
  bool useHeuristics = true;
};

Settings::Settings() {
  root.append(activePath, "ActivePath");
  root.append(libraryPath, "LibraryPath");
  root.append(createManifests, "CreateManifests");
  root.append(useDatabase, "UseDatabase");
  root.append(useHeuristics, "UseHeuristics");
  append(root, "Settings");

  directory::create({configpath(), "cart-pal/"});
  load({configpath(), "cart-pal/settings.bml"});
  save({configpath(), "cart-pal/settings.bml"});

  if(!activePath) activePath = userpath();
  if(!libraryPath) libraryPath = {userpath(), "Emulation/"};
}

Settings::~Settings() {
  save({configpath(), "cart-pal/settings.bml"});
}
