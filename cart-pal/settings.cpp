struct Settings : Markup::Node {
  Settings();
  ~Settings();
};

Settings::Settings() {
  Markup::Node::operator=(BML::unserialize(string::read(locate({configpath(), "cart-pal/"}, "settings.bml"))));

  auto set = [&](const string& name, const string& value) {
    //create node and set to default value only if it does not already exist
    if(!operator[](name)) operator()(name).setValue(value);
  };

  set("Library/Location", {userpath(), "Emulation/"});

  set("cart-pal/Path", userpath());
  set("cart-pal/CreateManifests", false);
  set("cart-pal/UseDatabase", true);
  set("cart-pal/UseHeuristics", true);
}

Settings::~Settings() {
  file::write(locate({configpath(), "cart-pal/"}, "settings.bml"), BML::serialize(*this));
}
