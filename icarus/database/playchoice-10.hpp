string PlayChoice10 = R"(

database revision=2015-09-30

release
  cartridge region=NTSC
    board type=JALECO-JF-09
    mirror mode=vertical
    prg
      rom name=program.rom size=0x8000
    chr
      rom name=character.rom size=0x4000
    pc10
      rom name=instructions.rom size=0x2000
      rom name=key.rom size=0x9
  information
    title:    The Goonies
    name:     Goonies, The
    region:   NA
    revision: 1.0
    sha256:   967620e98d157d70c6cd4b72d88877d2aba49bc2e2d9008482b940ffd48f6b25

release
  cartridge region=NTSC
    board type=NES-NROM-256
    mirror mode=vertical
    prg
      rom name=program.rom size=0x8000
    chr
      rom name=character.rom size=0x2000
    pc10
      rom name=instructions.rom size=0x2000
      rom name=key.rom size=0x9
  information
    title:    Super Mario Bros.
    name:     Super Mario Bros.
    region:   NA
    revision: 1.0
    sha256:   613da8bfaaa0be4b8308292acc5973d409f54ed143e14f082e1ef95442520ac2
    configuration
      rom name=program.rom size=0x8000
      rom name=character.rom size=0x2000
      rom name=instructions.rom size=0x2000
      rom name=key.rom size=0x9

release
  cartridge region=NTSC
    board type=NES-TSROM
    chip type=MMC3B
    prg
      rom name=program.rom size=0x40000
      ram size=0x2000
    chr
      rom name=character.rom size=0x20000
    pc10 mode=single
      rom name=instructions.rom size=0x2000
      rom name=key.rom size=0x9
  information
    title:    Super Mario Bros. 3
    name:     Super Mario Bros. 3
    region:   NA
    revision: 1.0
    sha256:   01d456bcd2389a33cb536a86a9fcd524db52b6dbb30628e6d78297cdec4e3e29
    configuration
      rom name=program.rom size=0x40000
      ram name=work.ram size=0x2000
      rom name=character.rom size=0x20000
      rom name=instructions.rom size=0x2000
      rom name=key.rom size=0x9

)";
