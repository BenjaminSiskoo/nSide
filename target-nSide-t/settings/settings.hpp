struct VideoSettings : TabFrameItem {
  VideoSettings(TabFrame*);

  VerticalLayout layout{this};
    Label colorAdjustmentLabel{&layout, Size{~0, 0}};
    HorizontalLayout saturationLayout{&layout, Size{~0, 0}};
      Label saturationLabel{&saturationLayout, Size{80, 0}};
      Label saturationValue{&saturationLayout, Size{80, 0}};
      HorizontalSlider saturationSlider{&saturationLayout, Size{~0, 0}};
    HorizontalLayout gammaLayout{&layout, Size{~0, 0}};
      Label gammaLabel{&gammaLayout, Size{80, 0}};
      Label gammaValue{&gammaLayout, Size{80, 0}};
      HorizontalSlider gammaSlider{&gammaLayout, Size{~0, 0}};
    HorizontalLayout luminanceLayout{&layout, Size{~0, 0}};
      Label luminanceLabel{&luminanceLayout, Size{80, 0}};
      Label luminanceValue{&luminanceLayout, Size{80, 0}};
      HorizontalSlider luminanceSlider{&luminanceLayout, Size{~0, 0}};
    Label overscanMaskLabel{&layout, Size{~0, 0}};
    HorizontalLayout horizontalMaskLayout{&layout, Size{~0, 0}};
      Label horizontalMaskLabel{&horizontalMaskLayout, Size{80, 0}};
      Label horizontalMaskValue{&horizontalMaskLayout, Size{80, 0}};
      HorizontalSlider horizontalMaskSlider{&horizontalMaskLayout, Size{~0, 0}};
    HorizontalLayout verticalMaskLayout{&layout, Size{~0, 0}};
      Label verticalMaskLabel{&verticalMaskLayout, Size{80, 0}};
      Label verticalMaskValue{&verticalMaskLayout, Size{80, 0}};
      HorizontalSlider verticalMaskSlider{&verticalMaskLayout, Size{~0, 0}};

  auto update() -> void;
};

struct AudioSettings : TabFrameItem {
  AudioSettings(TabFrame*);

  VerticalLayout layout{this};
    HorizontalLayout controlLayout{&layout, Size{~0, 0}};
      Label frequencyLabel{&controlLayout, Size{0, 0}};
      ComboButton frequencyCombo{&controlLayout, Size{~0, 0}};
      Label latencyLabel{&controlLayout, Size{0, 0}};
      ComboButton latencyCombo{&controlLayout, Size{~0, 0}};
      Label resamplerLabel{&controlLayout, Size{0, 0}};
      ComboButton resamplerCombo{&controlLayout, Size{~0, 0}};
    HorizontalLayout volumeLayout{&layout, Size{~0, 0}};
      Label volumeLabel{&volumeLayout, Size{80, 0}};
      Label volumeValue{&volumeLayout, Size{80, 0}};
      HorizontalSlider volumeSlider{&volumeLayout, Size{~0, 0}};

  auto update() -> void;
  auto updateVolume() -> void;
};

struct InputSettings : TabFrameItem {
  InputSettings(TabFrame*);
  auto updateControls() -> void;
  auto activeEmulator() -> InputEmulator&;
  auto activePort() -> InputPort&;
  auto activeDevice() -> InputDevice&;
  auto reloadPorts() -> void;
  auto reloadDevices() -> void;
  auto reloadMappings() -> void;
  auto refreshMappings() -> void;
  auto assignMapping() -> void;
  auto assignMouseInput(unsigned id) -> void;
  auto inputEvent(shared_pointer<HID::Device> device, unsigned group, unsigned input, int16 oldValue, int16 newValue, bool allowMouseInput = false) -> void;

  InputMapping* activeMapping = nullptr;

  VerticalLayout layout{this};
    HorizontalLayout selectionLayout{&layout, Size{~0, 0}};
      ComboButton emulatorList{&selectionLayout, Size{~0, 0}};
      ComboButton portList{&selectionLayout, Size{~0, 0}};
      ComboButton deviceList{&selectionLayout, Size{~0, 0}};
    ListView mappingList{&layout, Size{~0, ~0}};
    HorizontalLayout controlLayout{&layout, Size{~0, 0}};
      Button assignMouse1{&controlLayout, Size{100, 0}};
      Button assignMouse2{&controlLayout, Size{100, 0}};
      Button assignMouse3{&controlLayout, Size{100, 0}};
      Widget spacer{&controlLayout, Size{~0, 0}};
      Button resetButton{&controlLayout, Size{80, 0}};
      Button eraseButton{&controlLayout, Size{80, 0}};
};

struct HotkeySettings : TabFrameItem {
  HotkeySettings(TabFrame*);
  auto reloadMappings() -> void;
  auto refreshMappings() -> void;
  auto assignMapping() -> void;
  auto inputEvent(shared_pointer<HID::Device> device, unsigned group, unsigned input, int16 oldValue, int16 newValue) -> void;

  InputMapping* activeMapping = nullptr;

  VerticalLayout layout{this};
    ListView mappingList{&layout, Size{~0, ~0}};
    HorizontalLayout controlLayout{&layout, Size{~0, 0}};
      Widget spacer{&controlLayout, Size{~0, 0}};
      Button resetButton{&controlLayout, Size{80, 0}};
      Button eraseButton{&controlLayout, Size{80, 0}};
};

struct TimingSettings : TabFrameItem {
  TimingSettings(TabFrame*);

  VerticalLayout layout{this};
    HorizontalLayout videoLayout{&layout, Size{~0, 0}};
      Label videoLabel{&videoLayout, Size{40, 0}};
      LineEdit videoValue{&videoLayout, Size{80, 0}};
      Button videoAssign{&videoLayout, Size{80, 0}};
    HorizontalLayout audioLayout{&layout, Size{~0, 0}};
      Label audioLabel{&audioLayout, Size{40, 0}};
      LineEdit audioValue{&audioLayout, Size{80, 0}};
      Button audioAssign{&audioLayout, Size{80, 0}};

  auto update() -> void;
};

struct AdvancedSettings : TabFrameItem {
  AdvancedSettings(TabFrame*);

  VerticalLayout layout{this};
    Label driverLabel{&layout, Size{~0, 0}, 2};
    HorizontalLayout driverLayout{&layout, Size{~0, 0}};
      Label videoLabel{&driverLayout, Size{0, 0}};
      ComboButton videoDriver{&driverLayout, Size{~0, 0}};
      Label audioLabel{&driverLayout, Size{0, 0}};
      ComboButton audioDriver{&driverLayout, Size{~0, 0}};
      Label inputLabel{&driverLayout, Size{0, 0}};
      ComboButton inputDriver{&driverLayout, Size{~0, 0}};
    Label libraryLabel{&layout, Size{~0, 0}, 2};
    HorizontalLayout libraryLayout{&layout, Size{~0, 0}};
      Label libraryPrefix{&libraryLayout, Size{0, 0}};
      LineEdit libraryLocation{&libraryLayout, Size{~0, 0}};
      Button libraryChange{&libraryLayout, Size{0, 0}};
};

struct SettingsManager : Window {
  SettingsManager();
  auto setVisible(bool visible = true) -> SettingsManager&;
  auto show(unsigned setting) -> void;

  VerticalLayout layout{this};
    TabFrame panel{&layout, Size{~0, ~0}};
      VideoSettings video{&panel};
      AudioSettings audio{&panel};
      InputSettings input{&panel};
      HotkeySettings hotkeys{&panel};
      TimingSettings timing{&panel};
      AdvancedSettings advanced{&panel};

  StatusBar statusBar{this};
};

extern SettingsManager* settingsManager;
