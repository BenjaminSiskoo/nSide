nSide v009r12 (2017-01-19)

A fork of higan v102 by byuu (http://byuu.org/emulation/higan/), which was
renamed to exclude "higan" at byuu's request.

nSide adds new devices to the Famicom emulator's controller ports. The supported
devices are:
*Gamepad
*Four Score
*Zapper (port 2 only)
*Power Pad (port 2 only)
*Arkanoid Vaus (port 2 only)
*SFC Gamepad (unlicensed)
*Mouse (unlicensed, only supported by homebrew)

In addition, it adds the Famicom expansion port for its own devices. The only
expansion port devices supported are:
*Gamepad (counts as Player 3)
*JoyPair
*4-Players Adaptor (special multitap that may allow 6-player homebrew)
*Beam Gun (Zapper)
*Family BASIC Keyboard
*Family Trainer (Power Pad)
*Mouse (unlicensed, only supported by homebrew)

Finally, it supports the VS. UniSystem, PlayChoice-10, and FamicomBox, each with
their own arcade panels and buttons. The PlayChoice-10 support includes being
able to view game instructions, but only with the right BIOS files.

Please be aware that a great majority of Famicom and NES games support the use
of Player 3 and Player 4 (Famicom) as substitutes for Player 1 and Player 2,
including ones that use the NES Four Score such as A Nightmare on Elm Street. If
the expansion slot is set to Gamepad, JoyPair, or 4-Players Adaptor, this can
result in controller inputs meant for Player 3 controlling Player 1 as well.

The interface also has a Save Screenshot function, which can save a screenshot
in PNG format at any time while a game is running. Please be aware that the PNG
compression is somewhat poor.

nSide uses different directories for storing configuration settings, save
states, and shaders so as not to conflict with higan. However, nSide will import
higan's settings upon first load.
In Linux or BSD, the configuration files will be in "~/.local/share/nSide/".
In Windows, the configuration files will be in "%LocalAppData%\nSide\".

You will need the GBA BIOS to play Game Boy Advance games. This is no different
from higan.

Game Boy Advance.sys/bios.rom
sha256: fd2547724b505f487e6dcb29ec2ecff3af35a841a77ab2e85fd87350abd36570

The PlayChoice-10 takes 3 files, a BIOS, a character ROM, and a palette ROM.
The BIOS varies depending on the screen setup.

PlayChoice-10.sys/bios-dual.rom
ROM 8T on the PCB. MAME calls it "pch1-c__8t_e-2.8t".
sha256: 12200cee0965b871d2a47ac09ef563214d1b1a8355beda8bd477e21a561682e8
The Dual Screen version is ready to use when this BIOS is installed.

PlayChoice-10.sys/bios-single.rom
ROM 8T on the PCB. MAME calls it "pck1-c.8t".
sha256: 64c1a7debf4729941c60e4c722ed61282ebd2f36928a30e55c9631477ce522ac
To use the Single Screen BIOS, you need to edit the file
"PlayChoice-10.sys/manifest.bml".

PlayChoice-10.sys/character.rom
Concatenate the 3 ROMs 8P + 8M + 8K for a 24KB ROM.
sha256: a8bf9c58e31dee0a60a2d480bd707c323212fba8963cc35a47423cadd1d7ed26
Used by both screen setups.

PlayChoice-10.sys/palette.rom
Concatenate the 3 ROMs 6F + 6E + 6D for a 768 byte ROM.
Each byte is the corresponding nybble in the low 4 bits and all 0's in the high
4 bits. MAME's expected palette ROMs come in this format already, except split
into 3 files.
sha256: 9f639da2e0248431b59a9344769a38fad8b64742ce6e0e44534e6918b8977a0a
Used by both screen setups.

If you do not wish to emulate the PlayChoice-10, then these files are not
necessary and can safely be ignored.

The FamicomBox takes only a program ROM and character ROM for its BIOS.

FamicomBox.sys/bios.program.rom
sha256: cc34e51798c0ff45d21a793b78f20143587ee0ac398f979e22a32809b273a470

FamicomBox.sys/bios.character.rom
sha256: 227b3acecf78c47927ca76125a0d3564f0bc6e1ae9a9ab5935e34acbe4022032

Known Bugs:
Famicom:
  *Saving a state while the Zapper or Beam Gun is connected will cause the
  emulator to hang. If the cursor was captured, it can be hard to rescue.

Super Famicom:
  *The SA-1 has no bus conflict emulation. Adding bus conflict emulation will
  slow the entire emulator down to unplayable speeds that no computer currently
  in existence will be able to run.
  All official SA-1 games will work, but homebrew writers are advised to use
  flash cartridges on their actual Super Famicoms or SNESes instead.
  *Super Bonk's demo sequence is mis-timed. Bonk falls short of entering a pipe,
  which ruins the rest of the demo as he continues reading inputs and gets stuck
  in the area above the pipe.
  Known to affect actual hardware.

Master System:
  *Many, many bugs. This core is still in alpha.

Mega Drive:
  *Super Street Fighter II: The New Challengers has missing sprites.
  *Many, many bugs. This core is still in alpha.

PC Engine:
  *Many, many bugs. This core is still in alpha.

Game Boy:
  *When loading a Game Boy game in Game Boy Color mode, colors are applied to
  the wrong areas, making game displays terrible on the eyes.
  For example, in Donkey Kong Land III, Dixie Kong's yellow and dark brown
  colors are swapped, while her mid-brown color is not.
  Inherited from higan v094.

Game Boy Advance:
  *The audio in many games is lacking. There is still much to know about how the
  Game Boy Advance generates audio.

Game Gear:
  *Many, many bugs. This core is still in alpha.

WonderSwan:
  Many bugs in different spots. higan's WonderSwan emulator is still new and
  full of bugs, all of which affect nSide's version of the emulator.
  *Card Captor Sakura has columns of corruption when characters speak.
  *Meitantei Conan - Nishi no Meitantei Saidai no Kiki!? will not start because
  it relies on correct prefetch emulation.

VS. System:
  *No support for games using DRM such as R.B.I. Baseball.

PlayChoice-10:
  *When the Z80 tries to stop the CPU, it never actually does so, so games keep
  running for 20 more seconds even when the timer runs out, albeit with no sound
  (which is a separate disable that works correctly).

===========================
Changes from higan: General
===========================
   Changed "higan" to "nSide" and added a Contributors field on the
  Configuration Settings Advanced tab and Cocoa About box. This list has
  multiple names of people who contributed to higan (and whose contributions
  appear in nSide), but there may still be more that I am missing, so let me
  know on byuu's forums if you or someone else needs to be credited.

   Reformatted the "About" text to show contributors to higan and show which
  program nSide branched from.

   Renamed "{game}/higan" folder for save states to "{game}/nSide".

   Renamed the tomoko-based UI to "nSide-t".

   Renamed icarus to "cart-pal".

===========================
Changes from higan: Famicom
===========================
   The read registers at $4016 and $4017 only have 5 bits each, not 6, so
  cpu.mdr() is ANDed with 0xe0 instead of 0xc0.

   Added emulation of PPU open bus behavior according to the notes blargg wrote
  when he published his ppu_open_bus demo. ppu.status.mdr will decay to 0x00 if
  not refreshed after about 600 milliseconds, and reading from the palette will
  put the 2 highest MDR bits into the read value.

   Added direct color support (when rendering is disabled and PPUADDR points to
  the palette at $3F00-$3FFF, the selected color will render instead of the
  background color).

   Added PAL support, which reduces games to 50Hz and swaps the red and green
  color emphasis bits.

   Added Four Score support. It is split into 2 devices, so for best results,
  plug a Four Score into both slots.
   Added support for the Zapper/Beam Gun.
  Light is defined as any Famicom color with a luma of 0x20 or greater and a
  chroma less than 0x0D, not taking into account emphasis or RGB PPUs.
   Added support for the Power Pad/Family Trainer.
   Added support for the Arkanoid Vaus.
   Added support for the Family BASIC Keyboard.
   Added support for the JoyPair.
   Added support for the 4-Players Adaptor.

   Added new board types:
    NES-EVENT              // Nintendo World Championships 1990
    NES-QJ                 // Super Spike V'Ball / Nintendo World Cup
    NES-SF1ROM             // Bases Loaded II (1.2)
    HVC-UN1ROM             // 戦場の狼 (Senjou no Ookami) (JPN)
    PAL-ZZ                 // Super Mario Bros. / Tetris / Nintendo World Cup
    BANDAI-74*161/02/74    // Oeka Kids (JPN)
    BANDAI-74*161/161/32   // Kamen Rider Club (JPN)
    BANDAI-PT-554          // Family Trainer 3 - Aerobics Studio (JPN)
    IREM-74*161/161/21/138 // ナポレオン戦記 (Napoleon Senki) (JPN)
    IREM-G101              // Image Fight (JPN)
    IREM-H3001             // 大工の源さん2 (Daiku no Gen-san 2) (JPN)
    IREM-HOLYDIVER         // Holy Diver (JPN)
    IREM-TAM-S1            // 快傑ヤンチャ丸 (Kaiketsu Yanchamaru 2) (JPN)
    JALECO-JF-05..08       // 忍者じゃじゃ丸くん (Ninja Jajamaru-kun) (JPN)
    JALECO-JF-09           // じゃじゃ丸の大冒険 (Jajamaru no Daibouken) (JPN)
    JALECO-JF-10           // うる生やつら ルムのウエヂングベル (Urusei Yatsura) (JPN)
    JALECO-JF-11           // 妖怪倶楽部 (Youkai Club) (JPN)
    JALECO-JF-14           // バイオ戦士DAN インクリーサーとの闘い (Bio Senshi DAN) (JPN)
    JALECO-JF-16           // 宇宙船コスモキャリア (Uchuusen: Cosmo Carrier) (JPN)
    JALECO-JF-24           // Magic John (JPN)
    JALECO-JF-25           // 忍者じゃじゃ丸 銀河大作戦 (Ninja Jajamaru G.D.) (JPN)
    NAMCOT-129
    NAMCOT-163             // 貝獣物語 (Kaijuu Monogatari) (JPN)
    NAMCOT-175
    NAMCOT-340
    NAMCOT-34xx            // SkyKid (JPN), The Quest of Ki (JPN)
    SUNSOFT-1              // アトランチスの謎 (Atlantis no Nazo) (JPN)
    SUNSOFT-4              // After Burner (JPN)
    TAITO-TC0190FMC
    TAITO-TC0350FMR
    TAITO-TC0690FMC

   Added aliases for existing board classes:
    HVC-??????      to NES-??????  // Games released in Japan
    NES-HROM        to NES-NROM    // Donkey Kong Jr. (JPN)
    NES-RROM        to NES-NROM    // Duck Hunt (JPN)
    NES-RROM-128    to NES-NROM    // Clu Clu Land (USA)
    NES-RTROM       to NES-NROM    // Excitebike (JPN)
    NES-SNWEPROM    to NES-SxROM   // Final Fantasy II Prototype (USA)
    NES-SROM        to NES-NROM    // Mario Bros. (JPN)
    NES-STROM       to NES-NROM    // Pinball (JPN)
    BANDAI-FCG-1    to BANDAI-FCG  // ドラゴンボール　大魔王復活 (JPN)
                                   // (Dragon Ball: Dai Maou Fukkatsu)
    BANDAI-FCG-2    to BANDAI-FCG  // ドラゴンボール３　悟空伝 (JPN)
                                   // (Dragon Ball 3: Gokuu Den)
    BANDAI-JUMP2    to BANDAI-FCG  // ファミコンジャンプII 最強の７人 (JPN)
                                   // (Famicom Jump II: Saikyou no 7 Nin)
    BANDAI-LZ93D50  to BANDAI-FCG  // クレヨンしんちゃん オラとポイポイ (JPN)
                                   // (Crayon Shin-chan: Ora to Poi Poi)
    IREM-BNROM      to NES-BNROM   // 魔鐘 (JPN)
                                   // (Mashou)
    IREM-FCG-1      to BANDAI-FCG  // 西村京太郎ミステリー ブルートレイン殺人事件 (JPN)
                                   // (Nishimura Kyoutarou Mystery: Blue Train
                                   // Satsujin Jiken)
    IREM-NROM-128   to NES-NROM    // Zippy Race (JPN)
    IREM-NROM-256   to NES-NROM    // Spelunker (JPN)
    IREM-UNROM      to NES-UNROM   // Spelunker II (JPN)
    KONAMI-CNROM    to NES-CNROM   // Gradius (JPN)
    KONAMI-NROM-128 to NES-NROM    // Road Fighter (JPN)
    KONAMI-SLROM    to NES-SLROM   // The Adventures of Bayou Billy (USA)
    KONAMI-TLROM    to NES-TLROM   // Super C (USA)
    KONAMI-UNROM    to NES-UNROM   // Life Force (USA)
    NAMCOT-3301     to NES-NROM    // Galaxian (JPN)
    NAMCOT-3302     to NES-NROM    // Mappy (JPN)
    NAMCOT-3305     to NES-NROM    // The Tower of Druaga (JPN)
    NAMCOT-3311     to NES-NROM    // Tag Team Pro-Wrestling (JPN)
    NAMCOT-3312     to NES-NROM    // Dig Dug II (JPN)
    TENGEN-800008

   Added unlicensed aliases for existing board classes:
    TENGEN-800003 to NES-NROM     // Pac-Man (Unlicensed) (USA)
    TENGEN-800008 to NES-CNROM    // Tetris: The Soviet Mind Game (USA)
    TENGEN-800042 to SUNSOFT-4    // After Burner (USA)

   Added unlicensed board types: (note that they are sectioned off separately
   from the licensed ones for easy removal):
    CAMERICA-BF909x        // Micro Machines
    CAMERICA-ALGN
    CAMERICA-ALGQ
    COLORDREAMS-74*377.cpp // Bible Adventures
    MLT-ACTION52           // Action 52
    AVE-NINA-xx            // Impossible Mission II
    SingleChip             // Magic Floor (nocash)
   Many games published on Camerica boards were made by Codemasters who is
  notorious for exploiting rarely-used aspects of the NES. This is not a problem
  with the Camerica board but with Famicom hardware timing.

   Prevented saving to non-existent SRAM/WRAM for the following board families:
    SxROM
    TxROM

   The board definitions for the following board families now read the board and
  chip types and choose a board and chip revisions accordingly:
    NROM
      HROM has no solder pads to select mirroring and is locked to vertical
      mirroring.
    SxROM (MMC1)
      SEROM, SHROM, and SH1ROM have pin A14 hooked directly between the control
      deck and the program ROM, which prevents the ROM from being bankswitched.
      Prevented reading from and writing to non-existent PRG RAM. This bug
      prevented Bionic Commando from booting.
      Added tracking of the last CHR bank used, so that the legacy PRG RAM
      disable bit and 256 KB PRG bank bit will dynamically switch as the PPU
      renders. Can be used for software IRQs but will catastrophically crash the
      game if misused.
    TxROM (MMC3)
      Added TQROM support, which has both CHR-ROM and CHR-RAM (selected via bit
      6 of the bank number). Needed to play High Speed and Pin Bot.
      Added TxSROM support, which selects mirroring via bit 7 of whichever CHR
      banks control PPU $0000..$0FFF. Needed to play Armadillo.
      Added support for TVROM. TR1ROM support already existed, which provides
      four-screen mirroring, but it did not recognize TVROM as also having it.
      Needed to play Rad Racer II.
      Added support for Acclaim's MC-ACC chip, which fires IRQs on PPU A12
      falling edges instead of rising edges. Needed to properly display The
      Incredible Crash Dummies's messages during its intro.
    ExROM (MMC5)
      Fixed a bug in CHR mode 0 concerning background access to the PPU.
      Originally, though an increment of 1 to $512B advanced by 0x2000 bytes,
      PPU $1000..$1FFF acted as a mirror of $0000..$0FFF, making the latter half
      of each bank inaccessible. It now accesses the latter half of the bank.
      Sprite access was not affected by the bug.
    AxROM
    PxROM
    FxROM

   Added bus conflict emulation to the following board families (assuming that
  the written value is ANDed with the ROM byte):
    UxROM
      Added support for the 74HC08 chip, which is required to play Crazy
      Climber. Normal UxROM games use the 74HC32 chip.
    CxROM
      Added support for CPROM, which restricts PPU $0000-0FFF to the first bank
      while $1000-1FFF is a 4KB switchable bank. Needed to play Videomation.
      Added support for CNROM security CHR chip enables. Each game has a
      specific CHR bank that is read from
      manifest["cartridge/board/security/pass"], and that bank is the only one
      that maps in the CHR. Spy vs. Spy uses 0x1.
    AxROM (AMROM only; AOROM has no quality database coverage)
    BNROM
    GNROM/MHROM

   Made the MMC3 read the PPU's address bus (status.chr_abus) instead of ticking
  on every CHR ROM/RAM read.

   Added support for VRC2a, which is similar to VRC2c except that CHR banks are
  right-shifted by 1. "cartridge/chip/pinout/chr-shift" indicates the shift
  amount, with 0 used by VRC2b and VRC2c, and 1 used by VRC2a.
  VRC2a was used in ツインビー3 ポコポコ大魔王 (TwinBee 3: The Aimless Demon King).
  VRC2b was used in 魂斗羅 (Contra).
  VRC2c was used in がんばれゴエモン外伝 けいた黄金キセル (Ganbare Goemon Gaiden).

   The variants VRC4a, VRC4b, VRC4d, VRC4e, and VRC4f (if it exists) were all
  supported already, but VRC4c had been locked out, making it impossible to
  run がんばれゴエモン外伝2 天下の財宝 (Ganbare Goemon Gaiden 2). Not anymore.
  VRC4c's pinout is: "pinout a0=6 a1=7"

   Added the ability to read the VRC6 pinout from manifest.bml. Before, it would
  dynamically determine the pinout based on the presence of SRAM.
  悪魔城伝説 (Akumajou Densetsu/Castlevania 3) uses "pinout a0=0 a1=1".
  Esper Dream 2 and 魍魎戦記マダラ (Mouryou Senki Madara) use "pinout a0=1 a1=0".

   Split chip code for the Bandai chips into a separate file alongside the MMCs
  and VRCs.
  Made register placement in the Bandai boards more strict based on board type.
  "board type=???" determines how to lay out the registers.
  FCG-1 and FCG-2 use $6000-7FFF, and LZ93D50 and JUMP2 uses $8000-FFFF.
  Additionally, a second "chip type=???" line determines which type of EEPROM
  to use (24C01 for 128-byte and 24C02 for 256-byte). Leave the second line out
  to exclude EEPROM (and use SRAM if it exists). True EEPROM support is still
  not in, however.
  クレヨンしんちゃん オラとポイポイ (Crayon Shin-chan: Ora to Poi Poi) is the only game using
  the LZ93D50 to not have any way of saving data, and
  ファミコンジャンプII 最強の７人 (Famicom Jump II) is the only game on any variant of this
  board to use normal SRAM at $6000-7FFF.

   Added support for the Camerica boards ALGQ, BF9096 and BF9097. Needed to play
  Fire Hawk and the Quattro multicarts.

   Added the exportMemory function, which can dump WRAM, CIRAM, CGRAM, OAM,
  cartridge PRG RAM, and cartridge CHR RAM.

   Added limited support for the VS. System. Only UniSystem games will work
  right now (Tennis and Baseball are DualSystem games that would require
  emulating 2 Famicoms). The Start and Select buttons are disabled in favor of
  Buttons 1, 2, 3, and 4, which can be configured in the new Famicom Arcade
  Panel device, the VS. Panel. The VS. Panel also controls the service button
  and coin slots. It is connected automatically and cannot be disconnected.
   VS. games are stored in a separate "VS. System" folder in the library. The
  VS. manifest format selects the PPU revision in "side/ppu/revision"
  and specifies DIP switch settings in the same format as the Nintendo Super
  System's DIP switch settings ("side/setting/option/value").

   Added support for loading games with the .pc10 extension.
  The PlayChoice-10 loads the BIOS from the PlayChoice-10.sys folder and throws
  a warning if one is not found. Using the BIOS is required to play
  PlayChoice-10 games, which always occupy the 1st slot.
   In Famicom mode, which PPU to use is decided by the manifest in Famicom.sys.
  Just edit that file and replace "RP2C02G" with "RP2C03B" to use the RGB PPU
  in Famicom games.

   Prepared the video renderer for dynamic adjustment of screen width for
  VS. DualSystem and PlayChoice-10 games. To set the width to 512 pixels for a
  VS. DualSystem game, add a second "side" node with a "ppu" child. It is not
  enough to simply have a 2nd "side" node, said node needs to have a "ppu" node
  as a child to activate the double width. This is currently not useful because
  of lack of true DualSystem support. The height is set to 464 if using the
  PlayChoice-10's dual screen mode (set in PlayChoice-10.sys/manifest.bml).

=====================================================
Changes from higan: processor/r6502 (affects Famicom)
=====================================================
   BRK was improperly clearing the Decimal flag when it should leave it alone.
  Only the interrupt flag is set now.
   An NMI can now redirect a BRK if it occurs after BRK's 4th cycle (after
  pushing the return vector into the stack but before pushing register P).
   Opcode 0x80 was being treated as NOP abs, when it should be NOP #imm.
  NOP #imm now advances the program counter properly, fixing Puzznic's columns
  of corruption bug.
   Added support for the unofficial opcodes ALR, ANC, AXS, DCP, ISC (ISB), LAX,
  LXA, RLA, RRA, SAX, SHA, SHX, SHY, SLO, SRE, STP (KIL), and XAA (ANE).

=================================
Changes from higan: Super Famicom
=================================
   Expanded exportMemory to dump expansion chip-specific memory.

   Added a second cursor design that will be shown whenever the Super Scope is
  in Turbo mode. This is not just a simple recolor.

============================================================
Changes from higan: processor/r65816 (affects Super Famicom)
============================================================
   Changed the decode() function in the disassembler to take 3 arguments instead
  of 2. The new argument is the address of the opcode being decoded. In higan,
  this function uses the program counter to figure out the opcode's location,
  which produces incorrect results for opcodes with relative addresses such as
  branching opcodes.

=========================================================
Changes from higan: processor/z80 (affects Master System)
=========================================================
   Supported "ld A,i"'s and "ld A,r"'s unique flag effects, including setting
  the zero flag if i or r are 0x00 when being copied into A. This is needed for
  PlayChoice-10 emulation.

==============================
Changes from higan: Mega Drive
==============================
   Added support for the Fighting Pad 6B, the 6-button controller. While higan
  allows mapping the X, Y, and Z buttons but not MODE, they are never actually
  polled in higan.

============================
Changes from higan: Game Boy
============================
   Added the exportMemory function, which can dump WRAM, HRAM (internal RAM),
  VRAM, OAM, and the palette, which is different for the Game Boy and Game Boy
  Color.

====================================
Changes from higan: Game Boy Advance
====================================
   Added the exportMemory function, which can dump External WRAM, Internal WRAM,
  VRAM, and PRAM (Palette RAM).
  However, dumping External WRAM causes a crash with about 3/4 of the data
  dumped, so it has been temporarily disabled (External WRAM is 262144 bytes
  long, the longest of any file dumped by any emulator's exportMemory function).

   Added cheat code support. In the Game Boy Advance, it is slightly annoying
  to use compared to the Famicom, Super Famicom, and Game Boy; a cheat code
  will only take effect if its data width matches the CPU's access width
  exactly (8-bit, 16-bit, or 32-bit).

   Added video rotation to support games such as Dr. Mario & Puzzle League.
  This support is emulator-agnostic.

============================
Changes from higan: cart-pal
============================
   Renamed "icarus" to "cart-pal".

   Added support for the following Famicom boards:
    NES-CPROM
    HVC-UN1ROM
    HVC-UNROM+74HC08
    NES-TQROM
    NES-TVROM
    IREM-G101
    IREM-H3001
    IREM-74*161/161/21/138
    IREM-HOLYDIVER (NES 2.0 only)
    IREM-TAM-S1
    JALECO-JF-16
    NAMCOT-34xx

   Revised the header information for iNES mappers 21, 23, 24, 25, and 26 to
  correctly detect VRC4 and VRC6 pinouts more often (but in VRC4's case, still
  not 100% of the time)

   Revised the header information for iNES mappers 16, 153, and 159 to select
  which type of EEPROM/SRAM to use for the Bandai FCG series of boards.

   Added preliminary support for NES 2.0. This allows submappers to influence
  purification of Konami VRC games, distinguish between IREM-HOLYDIVER and
  JALECO-JF-16 (Cosmo Carrier), among other things.

No version of nSide will be called "v010" until the Famicom emulator has proper
sprite priority evaluation, or until the Atari 2600 is properly emulated.
