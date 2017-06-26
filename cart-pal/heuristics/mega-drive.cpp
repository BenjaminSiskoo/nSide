struct MegaDriveCartridge {
  MegaDriveCartridge(string location, uint8_t* data, uint size);

  string manifest;

//private:
  struct Information {
  } information;
};

MegaDriveCartridge::MegaDriveCartridge(string location, uint8_t* data, uint size) {
  manifest.append(
    "board region=", regions[0], "  //\"", regionCodes, "\"\n"
    "  rom name=program.rom size=0x", hex(size), "\n"
    "    map address=000000-3fffff\n"
  );

  bool ramVolatile;
  bool ramDepth;

  enum : bool { Byte, Word };

  if(data[0x0001b0] == 'R'
  && data[0x0001b1] == 'A'
  && (data[0x0001b2] & 0xa7) == 0xa0) {
    uint32_t lo = data[0x0001b5] << 16 | data[0x0001b6] << 8 | data[0x0001b7] << 0;
    uint32_t hi = data[0x0001b9] << 16 | data[0x0001ba] << 8 | data[0x0001bb] << 0;

    if((data[0x0001b3] & 0x20) && data[0x0001b4] == data[0x0001b8]) {  //SRAM
      uint8_t ramFlags = data[0x0001b2];
      ramVolatile = !(ramFlags & 0x40);
      ramDepth = (ramFlags & 0x10) ? Byte : Word;
      bool ramAlign = ramFlags & 0x08;

      uint size;
      if(ramDepth == Byte) size = (((hi + 2) & ~1) - (lo & ~1)) >> 1;
      if(ramDepth == Word) size = (hi + 1) - lo;

      lo = ramDepth == Byte ? 0x200000 + ramAlign : 0x200000;
      hi = ramDepth == Byte ? 0x3ffffe + ramAlign : 0x3fffff;
      manifest.append(
        "  ram type=sram name=save.ram size=0x", hex(size), ramVolatile ? " volatile" : "", "\n"
        "    map address=", hex(lo, 6L), "-", hex(hi, 6L), ramDepth == Byte ? " mask=0x0001" : "", "\n"
      );

    } else if((data[0x0001b3] & 0x40) && memory::compare(data + 0x0001b4, data + 0x0001b8, 4)) {
    //manifest.append(
    //  "  ram type=eeprom name=save.ram size=0x2000\n"
    //  "    map address=", hex(lo, 6L), "-", hex(hi, 6L), "\n"
    //);
    }
  }

  manifest.append("\n");
  manifest.append("information\n");
  manifest.append("  title:  ", Location::prefix(location), "\n");
  manifest.append("  sha256: ", Hash::SHA256(data, size).digest(), "\n");
  manifest.append("\n");
  manifest.append("note: heuristically generated by cart-pal\n");
}
