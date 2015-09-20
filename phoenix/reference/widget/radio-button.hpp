namespace phoenix {

struct pRadioButton : public pWidget {
  RadioButton& radioButton;

  void setChecked();
  void setGroup(const nall::group<RadioButton>& group);
  void setImage(const image& image, Orientation orientation);
  void setText(string text);

  pRadioButton(RadioButton& radioButton) : pWidget(radioButton), radioButton(radioButton) {}
  void constructor();
  void destructor();
};

}
