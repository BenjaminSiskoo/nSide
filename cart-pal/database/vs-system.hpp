string VSSystem = R"(

database revision=2015-09-30 owner=hex_usr

release
  cartridge region=NTSC
    vs
      ppu revision=RP2C04-0002
      controller port=2 device=joypad
      controller port=1 device=joypad
      chip type=74HC32
      prg
        rom name=program.rom size=0x20000
      chr
        ram size=0x2000
      setting name="Price"
        option value=0x00 name="1 Coin - 1 Credit"
        option value=0x04 name="1 Coin - 2 Credits"
        option value=0x02 name="1 Coin - 3 Credits"
        option value=0x06 name="2 Coins - 1 Credit"
        option value=0x01 name="3 Coins - 1 Credit"
        option value=0x05 name="4 Coins - 1 Credit"
        option value=0x03 name="5 Coins - 1 Credit"
        option value=0x07 name="Free Play"
      setting name="Lives"
        option value=0x00 name="3 lives"
        option value=0x08 name="2 lives"
      setting name="Bonus ~needs verification~"
        option value=0x00 name="100000 OR Timer x 4; Heart x 40"
        option value=0x20 name="200000 OR Timer x 6; Heart x 60"
        option value=0x10 name="300000 OR Timer x 8; Heart x 80"
        option value=0x30 name="400000 OR Timer x 9; Heart x 90"
      setting name="Difficulty"
        option value=0x00 name="Easy"
        option value=0x40 name="Hard"
  information
    title:    VS. Castlevania
    name:     Castlevania
    region:   NA
    revision: 1.0
    serial:   MDS-CV
    sha256:   37ca2b6898e6c3cc96dcde5f0bf17268b907b448a45c58a8f1ea9d2c34bd7c63
    configuration
      rom name=program.rom size=0x20000
      ram name=character.ram size=0x2000

release
  cartridge
    vs
      ppu revision=RP2C04-0003
      controller port=2 device=joypad
      controller port=1 device=joypad
      chip type=MMC1B2
      prg
        rom name=program.rom size=0x10000
      chr
        rom name=character.rom size=0x8000
      setting name="Drop Rate Increase after"
        option value=0x03 name="10 Megavitamins"
        option value=0x02 name="9 Megavitamins"
        option value=0x01 name="8 Megavitamins"
        option value=0x00 name="7 Megavitamins"
      setting name="Default Virus Level"
        option value=0x04 name="3"
        option value=0x00 name="1"
        option value=0x08 name="5"
        option value=0x0c name="7"
      setting name="Drop Rate Speed Up"
        option value=0x00 name="Slow"
        option value=0x10 name="Medium"
        option value=0x20 name="Fast"
        option value=0x30 name="Fastest"
      setting name="Free Play"
        option value=0x00 name="No"
        option value=0x40 name="Yes"
      setting name="Attract Mode Music"
        option value=0x80 name="Plays"
        option value=0x00 name="Muted"
  information
    title:    VS. Dr. Mario
    name:     Dr. Mario
    region:   NA
    revision: 1.0
    serial:   MDS-VU
    sha256:   020284464fed5b7f1649c463149c810ad36ac8b6478c26620c858e1ce4078f88
    configuration
      rom name=program.rom size=0x10000
      rom name=character.rom size=0x8000

release
  cartridge region=NTSC
    vs
      ppu revision=RP2C04-0002
      controller port=2 device=joypad
      controller port=1 device=joypad
      prg
        rom name=program.rom size=0x8000
      chr
        rom name=character.rom size=0x4000
  information
    title:    Mach Rider
    name:     Mach Rider
    region:   NA
    revision: 1.0
    sha256:   74ad66ebe7c931569db0a615f2225cf45ef11da1c9b5c956ecfe11c3991ea6ee
    configuration
      rom name=program.rom size=0x8000
      ram name=character.rom size=0x4000

release
  cartridge
    vs
      prg
        rom name=program-1.rom size=0x2000
    vs
      ppu revision=RP2C04-0002
      controller port=2 device=joypad
      controller port=1 device=joypad
      prg
        rom name=program-2.rom size=0x8000
      chr
        rom name=character.rom size=0x4000
      setting name="Price"
        option value=0x00 name="1 Coin - 1 Credit"
        option value=0x01 name="2 Coins - 1 Credit"
        option value=0x05 name="3 Coins - 1 Credit"
        option value=0x03 name="4 Coins - 1 Credit"
        option value=0x04 name="1 Coin - 2 Credits"
        option value=0x02 name="1 Coin - 3 Credits"
        option value=0x06 name="1 Coin - 4 Credits"
        option value=0x07 name="Free Play"
      setting name="Lives"
        option value=0x08 name="3"
        option value=0x00 name="2"
  information
    title:    VS. Raid on Bungeling Bay
    name:     Raid on Bungeling Bay
    region:   NA
    revision: 1.0
    sha256:   be2136bed65347c7f3c0e33ea5caf7a83c29113bcabb07a6a152cdb76a2a07c4
    configuration
      rom name=program-1.rom size=0x2000
      rom name=program-2.rom size=0x8000
      ram name=character.rom size=0x4000

release
  cartridge
    vs
      cpu revision=RP2A04
    vs
      ppu revision=RP2C04-0004
      controller port=1 device=joypad
      controller port=2 device=joypad
      prg
        rom name=program.rom size=0x8000
      chr
        rom name=character.rom size=0x4000
      setting name="Price"
        option value=0x00 name="1 Coin - 1 Credit"
        option value=0x04 name="2 Coins - 1 Credit"
        option value=0x02 name="3 Coins - 1 Credit"
        option value=0x06 name="1 Coin - 2 Credits"
        option value=0x01 name="1 Coin - 3 Credits"
        option value=0x05 name="1 Coin - 4 Credits"
        option value=0x03 name="1 Coin - 5 Credits"
        option value=0x07 name="Free Play"
      setting name="Lives"
        option value=0x00 name="3 lives"
        option value=0x08 name="2 lives"
      setting name="Coins for Bonus Life"
        option value=0x10 name="200 coins"
        option value=0x00 name="100 coins"
        option value=0x20 name="150 coins"
        option value=0x30 name="250 coins"
      setting name="Timer Speed"
        option value=0x00 name="Slow"
        option value=0x40 name="Fast"
      setting name="Continue Lives"
        option value=0x00 name="4 lives"
        option value=0x80 name="3 lives"
  information
    title:    VS. Super Mario Bros.
    name:     Super Mario Bros.
    region:   NA
    revision: Easy
    serial:   MDS-SM
    sha256:   c7ae61f4e850a8363c1546402334757727f3641acf41f28a2b7406145e7561f3
    configuration
      rom name=program.rom size=0x8000
      ram name=character.rom size=0x4000

release
  cartridge
    vs
      cpu revision=RP2A04
    vs
      ppu revision=RP2C04-0004
      controller port=1 device=joypad
      controller port=2 device=joypad
      prg
        rom name=program.rom size=0x8000
      chr
        rom name=character.rom size=0x4000
      setting name="Price"
        option value=0x00 name="1 Coin - 1 Credit"
        option value=0x04 name="2 Coins - 1 Credit"
        option value=0x02 name="3 Coins - 1 Credit"
        option value=0x06 name="1 Coin - 2 Credits"
        option value=0x01 name="1 Coin - 3 Credits"
        option value=0x05 name="1 Coin - 4 Credits"
        option value=0x03 name="1 Coin - 5 Credits"
        option value=0x07 name="Free Play"
      setting name="Lives"
        option value=0x00 name="3 lives"
        option value=0x08 name="2 lives"
      setting name="Coins for Bonus Life"
        option value=0x10 name="200 coins"
        option value=0x00 name="100 coins"
        option value=0x20 name="150 coins"
        option value=0x30 name="250 coins"
      setting name="Timer Speed"
        option value=0x00 name="Fast"
        option value=0x40 name="Faster"
      setting name="Continue Lives"
        option value=0x00 name="4 lives"
        option value=0x80 name="3 lives"
  information
    title:    VS. Super Mario Bros.
    name:     Super Mario Bros.
    region:   NA
    revision: Hard
    serial:   MDS-SM
    sha256:   5eb7f18541c61eb3941b00436603b5aaad4c93a2b299918f8894963d506271dc
    configuration
      rom name=program.rom size=0x8000
      ram name=character.rom size=0x4000

)";
