auto FamicomBox::serialize(serializer& s) -> void {
  s.array(bios_ram);
  s.array(test_ram);

  s.integer(dip);
  s.integer(keyswitch);

  s.integer(exceptionEnable);
  s.integer(exceptionTrap);
  s.integer(ramProtect);
  s.integer(zapperGND);
  s.integer(warmboot);
  s.integer(enableControllers);
  s.integer(swapControllers);

  s.integer(counter);
  s.integer(attractionTimer);
  s.integer(watchdog);

  s.integer(cartridgeSelect);
  s.integer(cartridgeRowSelect);
  s.integer(registerLock);
}
