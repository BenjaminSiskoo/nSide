auto APU::serialize(serializer& s) -> void {
  Thread::serialize(s);

  filter.serialize(s);

  pulse[0].serialize(s);
  pulse[1].serialize(s);
  triangle.serialize(s);
  dmc.serialize(s);
  frame.serialize(s);

  s.integer(enabledChannels);
  s.integer(cartridgeSample);
}

auto APU::Filter::serialize(serializer& s) -> void {
  s.integer(hipassStrong);
  s.integer(hipassWeak);
  s.integer(lopass);
}

auto APU::FrameCounter::serialize(serializer& s) -> void {
  s.integer(irqPending);

  s.integer(mode);
  s.integer(counter);
  s.integer(divider);
}
