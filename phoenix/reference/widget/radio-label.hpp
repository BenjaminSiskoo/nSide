namespace phoenix {

struct pRadioLabel : public pWidget {
  RadioLabel& radioLabel;

  void setChecked();
  void setGroup(const Group<RadioLabel>& group);
  void setText(string text);

  pRadioLabel(RadioLabel& radioLabel) : pWidget(radioLabel), radioLabel(radioLabel) {}
  void constructor();
  void destructor();
};

}
