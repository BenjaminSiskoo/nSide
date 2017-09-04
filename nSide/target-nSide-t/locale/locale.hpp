struct Locale {
  auto load(string name) -> void;

  auto operator[](const string& path) -> string;
  auto operator()(const string& path) -> string;

private:
  Markup::Node document;
};

extern Locale locale;
