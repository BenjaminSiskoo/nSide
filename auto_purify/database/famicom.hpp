string Famicom = R"(

database revision=2012-10-22

release
  cartridge region=NTSC
    board type=KONAMI-VRC-6
    chip type=VRC6
      pinout a0=0 a1=1
    prg
      rom name=program.rom size=0x40000
    chr
      rom name=character.rom size=0x20000
  information
    title:    悪魔城伝説
    name:     Akumajou Densetsu
    region:   JP
    revision: 1.0
    board:    351951
    serial:   KON-RC845
    sha256:   c70f0f5d4054ce7c4850259879c9823add73ccc234ddcf96d95681bb78bd2c58
    configuration
      rom name=program.rom size=0x40000
      rom name=character.rom size=0x20000

release
  cartridge region=NTSC
    board type=KONAMI-UNROM
    chip type=74HC32
    mirror mode=vertical
    prg
      rom name=program.rom size=0x20000
    chr
      ram size=0x2000
  information
    title:    悪魔城ドラキュラ
    name:     Akumajou Dracula
    region:   JP
    revision: 1.0
    board:    353525
    serial:   KON-RV003
    sha256:   60b1aebdc0a19afc5d3e7dc4f09d8a968580e007197461a8a142656702c27f0d
    configuration
      rom name=program.rom size=0x20000
      ram name=character.ram size=0x2000

release
  cartridge region=NTSC
    board type=HVC-TLSROM
    chip type=MMC3B
    prg
      rom name=program.rom size=0x40000
    chr
      rom name=character.rom size=0x20000
  information
    title:    アルマジロ
    name:     Armadillo
    region:   JP
    revision: 1.0
    board:    HVC-TLSROM-01
    serial:   IGS-9T
    sha256:   54526dc9444c0eb4b0e5814f98b5e522bcb9881a6f2c0644fc7a21ca8c03502b
    configuration
      rom name=program.rom size=0x40000
      rom name=character.rom size=0x20000

release
  cartridge region=NTSC
    board type=NAMCOT-3401
    mirror mode=horizontal
    prg
      rom name=program.rom size=0x8000
    chr
      rom name=character.rom size=0x8000
  information
    title:    バベルの塔
    name:     Babel no Tou
    region:   JP
    revision: 1.0
    board:    3401
    serial:   NAM-NBL-3900-16
    sha256:   a9d7e89bd4ea28cfd169e32c4516ef5d059e19afb9dfa4ede8412f2373dfb0a7
    configuration
      rom name=program.rom size=0x8000
      rom name=character.rom size=0x8000

release
  cartridge region=NTSC
    board type=HVC-CNROM
      security pass=0x3
    mirror mode=horizontal
    prg
      rom name=program.rom size=0x8000
    chr
      rom name=character.rom size=0x2000
  information
    title:    B-ワイング
    name:     B-Wings
    region:   JP
    revision: 1.0
    board:    HVC-CNROM-256K-01
    serial:   DFC-BW
    sha256:   2b4ac20082e2f45a8f8fd4922a0e995829719a523e118a9eec891c3206adf25b
    configuration
      rom name=program.rom size=0x8000
      rom name=character.rom size=0x2000

release
  cartridge region=NTSC
    board type=HVC-CNROM
      security pass=0x3
    mirror mode=vertical
    prg
      rom name=program.rom size=0x4000
    chr
      rom name=character.rom size=0x2000
  information
    title:    バード・ウィーク
    name:     Bird Week
    region:   JP
    revision: 1.0
    board:    HVC-CNROM-256K-01
    serial:   TFS-BK
    sha256:   acf054b0886a2ca74a0280fc36bc1d55e9845acc29759f1893c1da4c1389f9c2
    configuration
      rom name=program.rom size=0x4000
      rom name=character.rom size=0x2000

release
  cartridge region=NTSC
    board type=JALECO-JF-05
    mirror mode=vertical
    prg
      rom name=program.rom size=0x4000
    chr
      rom name=character.rom size=0x4000
  information
    title:    シティコネクション
    name:     City Connection
    region:   JP
    revision: 1.0
    board:    JF-05
    serial:   JF-05
    sha256:   72c69f068099e42844bfe17e7d37f439243fad10b283650cac041e78443e0300
    configuration
      rom name=program.rom size=0x4000
      rom name=character.rom size=0x4000

release
  cartridge region=NTSC
    board type=KONAMI-VRC-2
    chip type=VRC2
      pinout a0=0 a1=1
    prg
      rom name=program.rom size=0x20000
    chr
      rom name=character.rom size=0x20000
  information
    title:    コントラ 魂斗羅
    name:     Contra
    region:   JP
    revision: 1.0
    board:    LROG009-00
    serial:   KON-RC826
    sha256:   62c9d4e0578cb1e615ce9bb2c8ebc15b1e8de4c928c5c07ba9a85c11aa36ae4d
    configuration
      rom name=program.rom size=0x20000
      rom name=character.rom size=0x20000

release
  cartridge region=NTSC
    board type=HVC-UNROM
    chip type=74HC08
    mirror mode=horizontal
    prg
      rom name=program.rom size=0x20000
    chr
      ram size=0x2000
  information
    title:    クレイジークライマー
    name:     Crazy Climber
    region:   JP
    revision: 1.0
    board:    HVC-UNROM-02
    serial:   NBF-CY (04)
    sha256:   e24d3c754dce20e27046afeacb2dfc217950d4be766ded80c20283392cb3891e
    configuration
      rom name=program.rom size=0x20000
      ram name=character.ram size=0x2000

release
  cartridge region=NTSC
    board type=IREM-H3001
    chip type=IF-H3001
    prg
      rom name=program.rom size=0x40000
    chr
      rom name=character.rom size=0x40000
  information
    title:    大工の源さん2 赤毛のダンの逆襲
    name:     Daiku no Gen-san 2 - Akage no Dan no Gyakushuu
    region:   JP
    revision: 1.0
    board:    FC-00-017C
    serial:   IF-29
    sha256:   a1d9bcf389d6837581cd33a26b4e13917d084e3e69dbe89453e443fc9976fa22
    configuration
      rom name=program.rom size=0x40000
      rom name=character.rom size=0x40000

release
  cartridge region=NTSC
    board type=NAMCOT-3453
    chip type=118
    prg
      rom name=program.rom size=0x20000
    chr
      rom name=character.rom size=0x20000
  information
    title:    デビルマン
    name:     Devil Man
    region:   JP
    revision: 1.0
    board:    UNK-NAMCOT-DM
    serial:   NAM-DM-5500
    sha256:   d2140fc2e6354a9f4d0154dabac757e5559890edba4885799c1c979d8b7a8b20
    configuration
      rom name=program.rom size=0x20000
      rom name=character.rom size=0x20000

release
  cartridge region=NTSC
    board type=HVC-NROM-128
    mirror mode=horizontal
    prg
      rom name=program.rom size=0x4000
    chr
      rom name=character.rom size=0x2000
  information
    title:    ドンキーコング
    name:     Donkey Kong
    region:   JP
    revision: 1.0
    board:    9011-01
    serial:   HVC-DK
    sha256:   dd108e55b60070b4c0147f7dba31844ed83065255466bfacd5038e6382483026
    configuration
      rom name=program.rom size=0x4000
      rom name=character.rom size=0x2000

release
  cartridge region=NTSC
    board type=HVC-NROM-128
    mirror mode=horizontal
    prg
      rom name=program.rom size=0x4000
    chr
      rom name=character.rom size=0x2000
  information
    title:    ドンキーコング
    name:     Donkey Kong
    region:   JP
    revision: 1.1
    board:    HVC-NROM-02
    serial:   HVC-DK
    sha256:   aa408f5a6b97c0d738e7e8b489a5617ad4a9ecdee2b05c4ee504210ce31b2825
    configuration
      rom name=program.rom size=0x4000
      rom name=character.rom size=0x2000

release
  cartridge region=NTSC
    board type=HVC-HROM
    prg
      rom name=program.rom size=0x4000
    chr
      rom name=character.rom size=0x2000
  information
    title:    ドンキーコングJr
    name:     Donkey Kong Jr.
    region:   JP
    revision: 1.0
    board:    HVC-HROM-01
    serial:   HVC-JR
    sha256:   950ebe68e7f74219b9e5e104200b03165d59c24264f02e32c12be967fd311ac2
    configuration
      rom name=program.rom size=0x4000
      rom name=character.rom size=0x2000

release
  cartridge region=NTSC
    board type=HVC-RROM
    mirror mode=vertical
    prg
      rom name=program.rom size=0x4000
    chr
      rom name=character.rom size=0x2000
  information
    title:    ダックハント
    name:     Duck Hunt
    region:   JP
    revision: 1.0
    board:    HVC-RROM-05
    serial:   HVC-DH
    sha256:   7026334a7e8742b61b450f4b3b182922c6a69fc723d7cd19c83db365f15e45ba
    configuration
      rom name=program.rom size=0x4000
      rom name=character.rom size=0x2000

release
  cartridge region=NTSC
    board type=FDS
    prg
      rom name=program.rom size=0x2000
      ram size=0x8000
    chr
      ram size=0x2000
  information
    title:    Famicom Disk System
    name:     Famicom Disk System
    region:   JP
    revision: 1.0
    board:    Famicom Disk System
    serial:   N/A
    sha256:   99c18490ed9002d9c6d999b9d8d15be5c051bdfa7cc7e73318053c9a994b0178
    configuration
      rom name=program.rom size=0x2000
      ram name=work.ram size=0x8000
      ram name=character.ram size=0x2000

release
  cartridge region=NTSC
    board type=BANDAI-JUMP2
    chip type=LZ93D50
    prg
      rom name=program.rom size=0x80000
      ram name=save.ram size=0x2000
    chr
      ram size=0x2000
  information
    title:    ファミコンジャンプII 最強の7人
    name:     Famicom Jump II - Saikyou no 7 Nin
    region:   JP
    revision: 1.0
    board:    BA-JUMP2
    serial:   BA-FJ2
    sha256:   dd031b72924e1d080f8758412c73224a274ae5e5a50d90310d578975df74101f
    configuration
      rom name=program.rom size=0x80000
      ram name=save.ram size=0x2000
      ram name=character.ram size=0x2000

release
  cartridge region=NTSC
    board type=HVC-FJROM
    chip type=MMC4
    prg
      rom name=program.rom size=0x20000
      ram name=save.ram size=0x2000
    chr
      rom name=character.rom size=0x10000
  information
    title:    ファミコンウォーズ
    name:     Famicom Wars
    region:   JP
    revision: 1.0
    board:    HVC-FJROM-01
    serial:   HVC-FW
    sha256:   7586cdd8b742ba3c4f0ea3eefaa2f6f2215af197a5269e3de9026bcdf236e981
    configuration
      rom name=program.rom size=0x20000
      ram name=save.ram size=0x2000
      rom name=character.rom size=0x10000

release
  cartridge region=NTSC
    board type=HVC-SNROM
    chip type=MMC1A
    prg
      rom name=program.rom size=0x40000
      ram name=save.ram size=0x2000
    chr
      ram size=0x2000
  information
    title:    ファイナルファンタジーII
    name:     Final Fantasy II
    region:   JP
    revision: 1.0
    board:    HVC-SNROM-07
    serial:   SQF-FY
    sha256:   60c65138a0edecee77e7fa808291d6198ef2ebd10b1fd2bc4f047298d93a4cc0
    configuration
      rom name=program.rom size=0x40000
      ram name=save.ram size=0x2000
      ram name=character.ram size=0x2000

release
  cartridge region=NTSC
    board type=HVC-TNROM
    chip type=MMC3B
    prg
      rom name=program.rom size=0x80000
      ram name=save.ram size=0x2000
    chr
      ram size=0x2000
  information
    title:    ファイナルファンタジーIII
    name:     Final Fantasy III
    region:   JP
    revision: 1.0
    board:    HVC-TNROM-01
    serial:   SQF-FC
    sha256:   7ff89b50156b6f5b3d78d3d2eeec8a9221d9f7b18f8350abf89b7867a205f710
    configuration
      rom name=program.rom size=0x80000
      ram name=save.ram size=0x2000
      ram name=character.ram size=0x2000

release
  cartridge region=NTSC
    board type=HVC-FKROM
    chip type=MMC4
    prg
      rom name=program.rom size=0x40000
      ram name=save.ram size=0x2000
    chr
      rom name=character.rom size=0x20000
  information
    title:    ファイアーエムブレム 暗黒竜と光の剣
    name:     Fire Emblem 1 - Ankoku Ryuu to Hikari no Tsurugi
    region:   JP
    revision: 1.0
    board:    HVC-FKROM-01
    serial:   HVC-VX
    sha256:   60db7fd78cc849658a42ca648f0d294ebf21e477ddf0753f0e6bbffaad6192ab
    configuration
      rom name=program.rom size=0x40000
      ram name=save.ram size=0x2000
      rom name=character.rom size=0x20000

release
  cartridge region=NTSC
    board type=HVC-FKROM
    chip type=MMC4
    prg
      rom name=program.rom size=0x40000
      ram name=save.ram size=0x2000
    chr
      rom name=character.rom size=0x20000
  information
    title:    ファイアーエムブレム外伝
    name:     Fire Emblem 2 - Gaiden
    region:   JP
    revision: 1.0
    board:    HVC-FKROM-01
    serial:   HVC-2I
    sha256:   0d899d67c7a61b13151f465c38c63a0d2d6a42d6be2d4ecac0fef87d55d92f92
    configuration
      rom name=program.rom size=0x40000
      ram name=save.ram size=0x2000
      rom name=character.rom size=0x20000

release
  cartridge region=NTSC
    board type=NAMCOT-3301
    mirror mode=horizontal
    prg
      rom name=program.rom size=0x4000
    chr
      rom name=character.rom size=0x2000
  information
    title:    ギャラガ
    name:     Galaga
    region:   JP
    revision: 1.0
    board:    3301
    serial:   NAM-NGG-4500-05
    sha256:   e6fe68b9f12578e74ba016ca146aaf8232b20475fb675c7d32e0ea4e47eb1cc8
    configuration
      rom name=program.rom size=0x4000
      rom name=character.rom size=0x2000

release
  cartridge region=NTSC
    board type=NAMCOT-3301
    mirror mode=horizontal
    prg
      rom name=program.rom size=0x2000
    chr
      rom name=character.rom size=0x2000
  information
    title:    ギャラクシアン
    name:     Galaxian
    region:   JP
    revision: 1.0
    board:    3301
    serial:   NAM-NGX-4500-01
    sha256:   50178a2856f8ed3574b4e7fd45b9d1ec44c660d51fe9783d0012a19df5892cce
    configuration
      rom name=program.rom size=0x2000
      rom name=character.rom size=0x2000

release
  cartridge region=NTSC
    board type=SUNSOFT-5B
    chip type=Sunsoft-5B
    prg
      rom name=program.rom size=0x40000
    chr
      rom name=character.rom size=0x20000
  information
    title:    ギミック!
    name:     Gimmick!
    region:   JP
    revision: 1.0
    board:    FC-GMK
    serial:   SUN-GMK-6200
    sha256:   1bbe4b3e20a004a4f741018e31e6ae291772b8876d6fb6f01494c9c5b0917c6c
    configuration
      rom name=program.rom size=0x40000
      rom name=character.rom size=0x20000

release
  cartridge region=NTSC
    board type=KONAMI-74*139/74
    mirror mode=vertical
    prg
      rom name=program.rom size=0x8000
    chr
      rom name=character.rom size=0x4000
  information
    title:    グーニーズ
    name:     Goonies, The
    region:   JP
    revision: 1.0
    board:    302047A
    serial:   KON-RC809
    sha256:   f837e0807ade4bb8036d40746e2433ed66986f043fc2be722e765e45a676cd5d
    configuration
      rom name=program.rom size=0x8000
      rom name=character.rom size=0x4000

release
  cartridge region=NTSC
    board type=HVC-SROM
    mirror mode=vertical
    prg
      rom name=program.rom size=0x4000
    chr
      rom name=character.rom size=0x2000
  information
    title:    ホーガンズアレイ
    name:     Hogan's Alley
    region:   JP
    revision: 1.0
    board:    HVC-SROM-03
    serial:   HVC-HA
    sha256:   8e4a04076b6a728a7e65a09737776dcb9defed7922bf0437d9a89bbe8c724b55
    configuration
      rom name=program.rom size=0x4000
      rom name=character.rom size=0x2000

release
  cartridge region=NTSC
    board type=IREM-G101
    chip type=G-101
    prg
      rom name=program.rom size=0x20000
      ram size=0x2000
    chr
      rom name=character.rom size=0x20000
  information
    title:    イメージファイト
    name:     Image Fight
    region:   JP
    revision: 1.0
    board:    FC-00-015
    serial:   IF-14
    sha256:   c9707547555d8c38014540204649125f7b36d3990433bfebb63c49742e2dc3a2
    configuration
      rom name=program.rom size=0x20000
      ram name=work.ram size=0x2000
      rom name=character.rom size=0x20000

release
  cartridge region=NTSC
    board type=JALECO-JF-27
    chip type=SS88006
    prg
      rom name=program.rom size=0x20000
      ram name=save.ram size=0x2000
    chr
      rom name=character.rom size=0x20000
  information
    title:    じゃじゃ丸撃魔伝 ~幻の金魔城~
    name:     Jajamaru Gekimaden - Maboroshi no Kinmajou
    region:   JP
    revision: 1.0
    board:    JF-27B
    serial:   JF-27
    sha256:   ea770788f68e4bb089e4205807931d64b83175a0106e7563d0a6d3ebac369991
    configuration
      rom name=program.rom size=0x20000
      ram name=save.ram size=0x2000
      rom name=character.rom size=0x20000

release
  cartridge region=NTSC
    board type=HVC-EKROM
    chip type=MMC5A
    prg
      rom name=program.rom size=0x80000
      ram name=save.ram size=0x2000
    chr
      rom name=character.rom size=0x40000
  information
    title:    ジャストブリード
    name:     Just Breed
    region:   JP
    revision: 1.0
    board:    HVC-EKROM-01
    serial:   EFC-I5
    sha256:   f40341cf7e76480465ba8ab2d09e44644c3e3af7d647dc04c8b38357b26e014a
    configuration
      rom name=program.rom size=0x80000
      ram name=save.ram size=0x2000
      rom name=character.rom size=0x40000

release
  cartridge region=NTSC
    board type=JALECO-JF-24
    chip type=SS88006
    prg
      rom name=program.rom size=0x20000
    chr
      rom name=character.rom size=0x20000
  information
    title:    マジック・ジョン
    name:     Magic John
    region:   JP
    revision: 1.0
    board:    JF-24A
    serial:   JF-30
    sha256:   497c3015516cb6ae2f20d70b8fb1b70b8c4cfcd64e118992c438cfe7b0579f2b
    configuration
      rom name=program.rom size=0x20000
      rom name=character.rom size=0x20000

release
  cartridge region=NTSC
    board type=IREM-G101
    chip type=G-101
    mirror mode=screen-1
    prg
      rom name=program.rom size=0x20000
    chr
      rom name=character.rom size=0x20000
  information
    title:    メジャーリーグ
    name:     Major League
    region:   JP
    revision: 1.0
    board:    UNK-IF-13
    serial:   IF-13
    sha256:   4ef61de405406bfa9eeaf19ed1d882444c41bb606ac78673b7ec8ee323d0e073
    configuration
      rom name=program.rom size=0x20000
      rom name=character.rom size=0x20000

release
  cartridge region=NTSC
    board type=HVC-TKROM
    chip type=MMC3A
    prg
      rom name=program.rom size=0x40000
      ram name=save.ram size=0x2000
    chr
      rom name=character.rom size=0x20000
  information
    title:    MOTHER
    name:     MOTHER
    region:   JP
    revision: 1.0
    board:    HVC-TKROM-01
    serial:   HVC-MX
    sha256:   aa0fe1b0b1e0c3a0a01695f1914fbb5e92649db33b74a9bdcb51be1481221f49
    configuration
      rom name=program.rom size=0x40000
      ram name=save.ram size=0x2000
      rom name=character.rom size=0x20000

release
  cartridge region=NTSC
    board type=HVC-TLROM
    chip type=MMC3C
    prg
      rom name=program.rom size=0x20000
    chr
      rom name=character.rom size=0x20000
  information
    title:    熱血格闘伝説
    name:     Nekketsu Kakutou Densetsu
    region:   JP
    revision: 1.0
    board:    HVC-TLROM-04
    serial:   TJC-NA
    sha256:   3c419e3ecf328c03364afbcf5bd15bf0029a525db9e8f74379ae1cce4062b3c3
    configuration
      rom name=program.rom size=0x20000
      rom name=character.rom size=0x20000

release
  cartridge region=NTSC
    board type=JALECO-JF-06
    mirror mode=vertical
    prg
      rom name=program.rom size=0x4000
    chr
      rom name=character.rom size=0x4000
  information
    title:    忍者じゃじゃ丸くん
    name:     Ninja Jajamaru-kun
    region:   JP
    revision: 1.0
    board:    JF-06
    serial:   JF-06
    sha256:   bf546b0ce7fc60d89020ff7c0fadc7369925ba68a84d25b03de4190a91c46a95
    configuration
      rom name=program.rom size=0x4000
      rom name=character.rom size=0x4000

release
  cartridge region=NTSC
    board type=JALECO-JF-25
    chip type=SS88006
    prg
      rom name=program.rom size=0x20000
    chr
      rom name=character.rom size=0x20000
  information
    title:    忍者じゃじゃ丸 銀河大作戦
    name:     Ninja Jajamaru - Ginga Daisakusen
    region:   JP
    revision: 1.0
    board:    JF-25
    serial:   JF-32
    sha256:   648cf7ac553517573cc9b3955ab50566a91974b2348154910bfa53ef15d55b56
    configuration
      rom name=program.rom size=0x20000
      rom name=character.rom size=0x20000

release
  cartridge region=NTSC
    board type=IREM-G101
    chip type=G-101
    prg
      rom name=program.rom size=0x20000
    chr
      rom name=character.rom size=0x20000
  information
    title:    パーマン えんばんを とりかえせ!!
    name:     Paaman - Enban o Torikaese!!
    region:   JP
    revision: 1.0
    board:    FC-00-016
    serial:   IF-20
    sha256:   6d2887d978262401f315836411537a208272cba7721f598308d92983f1f5731a
    configuration
      rom name=program.rom size=0x20000
      rom name=character.rom size=0x20000

release
  cartridge region=NTSC
    board type=NAMCOT-3416
    chip type=118
    mirror mode=vertical
    prg
      rom name=program.rom size=0x20000
    chr
      rom name=character.rom size=0x10000
  information
    title:    カイの冒険: The Quest of Ki
    name:     Quest of Ki, The
    region:   JP
    revision: 1.0
    board:    3416
    serial:   NAM-QK-3900
    sha256:   67123fe28cf5fbadeafc77400a0812f0135ab36706ec7d1267f84931d044e71d
    configuration
      rom name=program.rom size=0x20000
      rom name=character.rom size=0x10000

release
  cartridge region=NTSC
    board type=NAMCOT-3417
    chip type=109
    mirror mode=vertical
    prg
      rom name=program.rom size=0x8000
    chr
      rom name=character.rom size=0x8000
  information
    title:    スカイキッド
    name:     SkyKid
    region:   JP
    revision: 1.0
    board:    3417
    serial:   NAM-NSK-3900-18
    sha256:   4fbd928f06fbc4ea537cdae7f6a906053df7cd80c35b5cfeac9dfb9d3033efb8
    configuration
      rom name=program.rom size=0x8000
      rom name=character.rom size=0x8000

release
  cartridge region=NTSC
    board type=HVC-CNROM
      security pass=0x1
    mirror mode=vertical
    prg
      rom name=program.rom size=0x8000
    chr
      rom name=character.rom size=0x2000
  information
    title:    スパイVSスパイ
    name:     Spy vs. Spy
    region:   JP
    revision: 1.0
    board:    HVC-CNROM-256K-01
    serial:   KSC-SP
    sha256:   a9cf92ec1a080faa195d0b3dcb48fbb5ee3362f0f2f14e14e4257def48ac4346
    configuration
      rom name=program.rom size=0x8000
      rom name=character.rom size=0x2000

release
  cartridge region=NTSC
    board type=HVC-NROM-256
    mirror mode=vertical
    prg
      rom name=program.rom size=0x8000
    chr
      rom name=character.rom size=0x2000
  information
    title:    スーパーマリオブラザーズ
    name:     Super Mario Bros.
    region:   JP
    revision: 1.0
    board:    HVC-NROM-256K-02
    serial:   HVC-SM
    sha256:   fcb6a0ef3a20c19b356005fbb21dc8009563b1cb5a9aaebc8e9386b4a8c5912e
    configuration
      rom name=program.rom size=0x8000
      rom name=character.rom size=0x2000

release
  cartridge region=NTSC
    board type=HVC-NROM-256
    mirror mode=horizontal
    prg
      rom name=program.rom size=0x8000
    chr
      rom name=character.rom size=0x2000
  information
    title:    テグザー
    name:     Thexder
    region:   JP
    revision: 1.0
    board:    HVC-NROM-256K-02S
    serial:   SQF-TX
    sha256:   18d41a2dc65d8afce295eaf29c391539a69d7cfe6dd32503713ae13d4495a545
    configuration
      rom name=program.rom size=0x8000
      rom name=character.rom size=0x2000

release
  cartridge region=NTSC
    board type=KONAMI-VRC-4
    chip type=VRC4
      pinout a0=2 a1=3
    prg
      rom name=program.rom size=0x20000
    chr
      rom name=character.rom size=0x20000
  information
    title:    タイニートゥーンアドベンチャー
    name:     Tiny Toon Adventures
    region:   JP
    revision: 1.0
    board:    352396
    serial:   KON-RC860
    sha256:   4b80a1db42ecde039f01c52a74146887f9dfc2ad54fe3706bcdf625ec3e2de97
    configuration
      rom name=program.rom size=0x20000
      rom name=character.rom size=0x20000

release
  cartridge region=NTSC
    board type=KONAMI-VRC-7
    chip type=VRC7
    prg
      rom name=program.rom size=0x20000
    chr
      rom name=character.rom size=0x20000
  information
    title:    タイニートゥーンアドベンチャー2 モンタナランドへようこそ
    name:     Tiny Toon Adventures 2 - Montana Land e Youkoso
    region:   JP
    revision: 1.0
    board:    353429
    serial:   KON-RV051
    sha256:   dabde0dbf12d01237350cf6ad6463ce79651ac1e80d57b7b3b55c1819e72fd8a
    configuration
      rom name=program.rom size=0x20000
      rom name=character.rom size=0x20000

release
  cartridge region=NTSC
    board type=FDS
    prg
      rom name=program.rom size=0x2000
      ram size=0x8000
    chr
      ram size=0x2000
  information
    title:    Twin Famicom Disk System
    name:     Twin Famicom Disk System
    region:   JP
    revision: 1.0
    board:    Famicom Disk System
    serial:   N/A
    sha256:   a0a9d57cbace21bf9c85c2b85e86656317f0768d7772acc90c7411ab1dbff2bf
    configuration
      rom name=program.rom size=0x2000
      ram name=work.ram size=0x8000
      ram name=character.ram size=0x2000

release
  cartridge region=NTSC
    board type=KONAMI-VRC-2
    chip type=VRC2
      pinout a0=1 a1=0 chr-shift=1
    prg
      rom name=program.rom size=0x20000
    chr
      rom name=character.rom size=0x20000
  information
    title:    ツインビー3 ポコポコ大魔王
    name:     TwinBee 3 - Poko Poko Dai Maou
    region:   JP
    revision: 1.0
    board:    351618
    serial:   KON-RC841
    sha256:   092db8cfadf0a96c67dd0734c966c47dfb80ffb830485d8ee702284ae8eac4aa
    configuration
      rom name=program.rom size=0x20000
      rom name=character.rom size=0x20000

release
  cartridge region=NTSC
    board type=HVC-NROM-128
    mirror mode=vertical
    prg
      rom name=program.rom size=0x4000
    chr
      rom name=character.rom size=0x2000
  information
    title:    Wild Gunman
    name:     Wild Gunman
    region:   JP
    revision: 1.1
    board:    2I
    serial:   HVC-WG
    sha256:   adff304553b64384f86f6c2b63571f43972b9d087f92359a1b9b93b54d523542
    configuration
      rom name=program.rom size=0x4000
      rom name=character.rom size=0x2000

release
  cartridge region=NTSC
    board type=NES-SGROM
    chip type=MMC1A
    prg
      rom name=program.rom size=0x40000
    chr
      ram size=0x2000
  information
    title:    Bionic Commando
    name:     Bionic Commando
    region:   NA
    revision: 1.0
    board:    NES-SGROM-03
    serial:   NES-CM-USA
    sha256:   aeb61fd5cf5a5ed73344c46a43f8a8d539f601ff57e8f56c49bc1caea4ab3d9e
    configuration
      rom name=program.rom size=0x40000
      ram name=character.ram size=0x2000

release
  cartridge region=NTSC
    board type=NES-SLROM
    chip type=MMC1B2
    prg
      rom name=program.rom size=0x20000
    chr
      rom name=character.rom size=0x20000
  information
    title:    Clash at Demonhead
    name:     Clash at Demonhead
    region:   NA
    revision: 1.0
    board:    NES-SLROM-05
    serial:   NES-B6-USA
    sha256:   cf226f0d9486103bbaa19ee124b673d47aa2b3766334b6b7587d704c03e6649e
    configuration
      rom name=program.rom size=0x20000
      rom name=character.rom size=0x20000

release
  cartridge region=NTSC
    board type=NES-TKROM
    chip type=MMC3B
    prg
      rom name=program.rom size=0x40000
      ram name=save.ram size=0x2000
    chr
      rom name=character.rom size=0x20000
  information
    title:    Crystalis
    name:     Crystalis
    region:   NA
    revision: 1.0
    board:    NES-TKROM-01
    serial:   NES-G0-USA
    sha256:   5ad644d368f70b61700b74a1d04778888efcbbf98d5435d79f9fcefd23ac39c2
    configuration
      rom name=program.rom size=0x40000
      ram name=save.ram size=0x2000
      rom name=character.rom size=0x20000

release
  cartridge region=NTSC
    board type=NES-CNROM
    mirror mode=vertical
    prg
      rom name=program.rom size=0x8000
    chr
      rom name=character.rom size=0x8000
  information
    title:    Cybernoid: The Fighting Machine
    name:     Cybernoid - The Fighting Machine
    region:   NA
    revision: 1.0
    board:    NES-CNROM-06
    serial:   NES-YN-USA
    sha256:   ad1e14d08657d99c8b70f779931f62524b4beb529090b82b368925d8b642e40c
    configuration
      rom name=program.rom size=0x8000
      rom name=character.rom size=0x8000

release
  cartridge region=NTSC
    board type=NES-BNROM
    mirror mode=horizontal
    prg
      rom name=program.rom size=0x20000
    chr
      ram size=0x2000
  information
    title:    Deadly Towers
    name:     Deadly Towers
    region:   NA
    revision: 1.0
    board:    NES-BN-ROM-01
    serial:   NES-DE-USA
    sha256:   0115356b0791cc8ddcb7d3163d6ef7aa664f3ff4e68dba561ffffb79eefcbca9
    configuration
      rom name=program.rom size=0x20000
      ram name=character.ram size=0x2000

release
  cartridge region=NTSC
    board type=NES-SNROM
    chip type=MMC1B2
    prg
      rom name=program.rom size=0x40000
      ram name=save.ram size=0x2000
    chr
      ram size=0x2000
  information
    title:    Destiny of an Emperor
    name:     Destiny of an Emperor
    region:   NA
    revision: 1.0
    board:    NES-SNROM-05
    serial:   NES-YZ-USA
    sha256:   6d082c801942ce6787b471428ab4c8a6acb3e21f3f38fa197f2aeb698d9a2d7e
    configuration
      rom name=program.rom size=0x40000
      ram name=save.ram size=0x2000
      ram name=character.ram size=0x2000

release
  cartridge region=NTSC
    board type=NES-NROM-128
    mirror mode=horizontal
    prg
      rom name=program.rom size=0x4000
    chr
      rom name=character.rom size=0x2000
  information
    title:    Donkey Kong
    name:     Donkey Kong
    region:   NA
    revision: 1.0
    board:    NES-NROM-128-01
    serial:   NES-DK-USA
    sha256:   aa408f5a6b97c0d738e7e8b489a5617ad4a9ecdee2b05c4ee504210ce31b2825
    configuration
      rom name=program.rom size=0x4000
      rom name=character.rom size=0x2000

release
  cartridge region=NTSC
    board type=NES-NROM-128
    mirror mode=vertical
    prg
      rom name=program.rom size=0x4000
    chr
      rom name=character.rom size=0x2000
  information
    title:    Donkey Kong Jr.
    name:     Donkey Kong Jr.
    region:   NA
    revision: 1.0
    board:    NES-NROM-128-03
    serial:   NES-JR-USA
    sha256:   950ebe68e7f74219b9e5e104200b03165d59c24264f02e32c12be967fd311ac2
    configuration
      rom name=program.rom size=0x4000
      rom name=character.rom size=0x2000

release
  cartridge region=NTSC
    board type=NES-SAROM
    chip type=MMC1B2
    prg
      rom name=program.rom size=0x10000
      ram name=save.ram size=0x2000
    chr
      rom name=character.rom size=0x4000
  information
    title:    Dragon Warrior
    name:     Dragon Warrior
    region:   NA
    revision: 1.0
    board:    NES-SAROM-01
    serial:   NES-DQ-USA
    sha256:   abc5bcb459316a7d245065149ea72b5a8317f62fa6ed578569e15b670d3c0022
    configuration
      rom name=program.rom size=0x10000
      ram name=save.ram size=0x2000
      rom name=character.rom size=0x4000

release
  cartridge region=NTSC
    board type=NES-SAROM
    chip type=MMC1B2
    prg
      rom name=program.rom size=0x10000
      ram name=save.ram size=0x2000
    chr
      rom name=character.rom size=0x4000
  information
    title:    Dragon Warrior
    name:     Dragon Warrior
    region:   NA
    revision: 1.1
    board:    NES-SAROM-02
    serial:   NES-DQ-USA
    sha256:   d968a138b19d39dad24aceb3464c7b90e8d50416297273586d3faf1f094eb35c
    configuration
      rom name=program.rom size=0x10000
      ram name=save.ram size=0x2000
      rom name=character.rom size=0x4000

release
  cartridge region=NTSC
    board type=NES-SNROM
    chip type=MMC1B2
    prg
      rom name=program.rom size=0x40000
      ram name=save.ram size=0x2000
    chr
      ram size=0x2000
  information
    title:    Dragon Warrior II
    name:     Dragon Warrior II
    region:   NA
    revision: 1.0
    board:    NES-SNROM-05
    serial:   NES-D2-USA
    sha256:   c15ab051ff066f018cf4b0159780c58026114bb47a6376ef81c1571a39a8fe9b
    configuration
      rom name=program.rom size=0x40000
      ram name=save.ram size=0x2000
      ram name=character.ram size=0x2000

release
  cartridge region=NTSC
    board type=NES-SUROM
    chip type=MMC1B3
    prg
      rom name=program.rom size=0x80000
      ram name=save.ram size=0x2000
    chr
      ram size=0x2000
  information
    title:    Dragon Warrior III
    name:     Dragon Warrior III
    region:   NA
    revision: 1.0
    board:    NES-SUROM-01
    serial:   NES-D3-USA
    sha256:   f91a8bfc25bd267f5ae77bafa7fc650f77f8e50067869e99682b32d5b410644e
    configuration
      rom name=program.rom size=0x80000
      ram name=save.ram size=0x2000
      ram name=character.ram size=0x2000

release
  cartridge region=NTSC
    board type=NES-SUROM
    chip type=MMC1B3
    prg
      rom name=program.rom size=0x80000
      ram name=save.ram size=0x2000
    chr
      ram size=0x2000
  information
    title:    Dragon Warrior IV
    name:     Dragon Warrior IV
    region:   NA
    revision: 1.0
    board:    NES-SUROM-02
    serial:   NES-D4-USA
    sha256:   e49cb745370065a40aff078ae52b5de1c0db137fedcbe93b78ab18d76479deed
    configuration
      rom name=program.rom size=0x80000
      ram name=save.ram size=0x2000
      ram name=character.ram size=0x2000

release
  cartridge region=NTSC
    board type=NES-NROM-128
    mirror mode=vertical
    prg
      rom name=program.rom size=0x4000
    chr
      rom name=character.rom size=0x2000
  information
    title:    Duck Hunt
    name:     Duck Hunt
    region:   NA
    revision: 1.0
    board:    NES-NROM-128-03
    serial:   NES-DH-USA
    serial:   NES-DH-CAN
    sha256:   7026334a7e8742b61b450f4b3b182922c6a69fc723d7cd19c83db365f15e45ba
    configuration
      rom name=program.rom size=0x4000
      rom name=character.rom size=0x2000

release
  cartridge region=NTSC
    board type=NES-UNROM
    chip type=74HC32
    mirror mode=vertical
    prg
      rom name=program.rom size=0x20000
    chr
      ram size=0x2000
  information
    title:    Disney's DuckTales
    name:     DuckTales
    region:   NA
    revision: 1.0
    board:    NES-UNROM-09
    serial:   NES-UK-USA
    sha256:   8ba8baed01a9fbaf1e9ff29e0c9825db1963ac2aff211d6f1f3bcfd3839e2013
    configuration
      rom name=program.rom size=0x20000
      ram name=character.ram size=0x2000

release
  cartridge region=NTSC
    board type=NES-UNROM
    chip type=74HC32
    mirror mode=vertical
    prg
      rom name=program.rom size=0x20000
    chr
      ram size=0x2000
  information
    title:    Disney's DuckTales 2
    name:     DuckTales 2
    region:   NA
    revision: 1.0
    board:    NES-UNROM-10
    serial:   NES-DL-USA
    sha256:   54c70628739c9cfab40b8d79555e9076adae34127ef369988ca91635b4a688bf
    configuration
      rom name=program.rom size=0x20000
      ram name=character.ram size=0x2000

release
  cartridge region=NTSC
    board type=NES-NROM-128
    mirror mode=vertical
    prg
      rom name=program.rom size=0x4000
    chr
      rom name=character.rom size=0x2000
  information
    title:    Excitebike
    name:     Excitebike
    region:   NA
    revision: 1.0
    board:    NES-NROM-128-04
    serial:   NES-EB-USA
    sha256:   e9f4c9d1b7c66c6af83f2db5d4f704cf5f4b3c86e26a49c05539237807d8875e
    configuration
      rom name=program.rom size=0x4000
      rom name=character.rom size=0x2000

release
  cartridge region=NTSC
    board type=NES-SNROM
    chip type=MMC1B2
    prg
      rom name=program.rom size=0x40000
      ram name=save.ram size=0x2000
    chr
      ram size=0x2000
  information
    title:    Final Fantasy
    name:     Final Fantasy
    region:   NA
    revision: 1.0
    board:    NES-SNROM-05
    serial:   NES-FF-USA
    serial:   NES-FF-CAN
    sha256:   fa456d852372173ea31b192459ba1a2026f779df67793327ba6e132476c1d034
    configuration
      rom name=program.rom size=0x40000
      ram name=save.ram size=0x2000
      ram name=character.ram size=0x2000

release
  cartridge region=NTSC
    board type=NES-GNROM
    mirror mode=vertical
    prg
      rom name=program.rom size=0x20000
    chr
      rom name=character.rom size=0x8000
  information
    title:    Gumshoe
    name:     Gumshoe
    region:   NA
    revision: 1.0
    board:    NES-GN-ROM-01
    serial:   NES-GS-USA
    sha256:   4628f32db9b826d19fe5dd8e2c45a9f70e1041f15b7b44b06dee2f01731566e8
    configuration
      rom name=program.rom size=0x20000
      rom name=character.rom size=0x8000

release
  cartridge region=NTSC
    board type=NES-TQROM
    chip type=MMC3B
    prg
      rom name=program.rom size=0x20000
    chr
      rom name=character.rom size=0x10000
      ram size=0x2000
  information
    title:    High Speed
    name:     High Speed
    region:   NA
    revision: 1.0
    board:    NES-TQROM-01
    serial:   NES-8H-USA
    sha256:   710e907230bbd82143286b40a56a298b25cf326697a9f07bfd8e043c1936a4b1
    configuration
      rom name=program.rom size=0x20000
      rom name=character.rom size=0x10000
      rom name=nametable.ram size=0x2000

release
  cartridge region=NTSC
    board type=NES-NROM-128
    mirror mode=vertical
    prg
      rom name=program.rom size=0x4000
    chr
      rom name=character.rom size=0x2000
  information
    title:    Hogan's Alley
    name:     Hogan's Alley
    region:   NA
    revision: 1.0
    board:    NES-NROM-128-04
    serial:   NES-HA-USA
    sha256:   8e4a04076b6a728a7e65a09737776dcb9defed7922bf0437d9a89bbe8c724b55
    configuration
      rom name=program.rom size=0x4000
      rom name=character.rom size=0x2000

release
  cartridge region=NTSC
    board type=ACCLAIM-MC-ACC
    chip type=MC-ACC
    prg
      rom name=program.rom size=0x20000
    chr
      rom name=character.rom size=0x40000
  information
    title:    The Incredible Crash Dummies
    name:     Incredible Crash Dummies, The
    region:   NA
    revision: 1.0
    board:    55741
    serial:   NES-CQ-USA
    sha256:   c42fc592821b474b486ae32d1d63e8938f1735a6d45db026f7b78b2ec51427ac
    configuration
      rom name=program.rom size=0x20000
      rom name=character.rom size=0x40000

release
  cartridge region=NTSC
    board type=NES-SNROM
    chip type=MMC1A
    prg
      rom name=program.rom size=0x20000
      ram name=save.ram size=0x2000
    chr
      ram size=0x2000
  information
    title:    The Legend of Zelda
    name:     Legend of Zelda, The
    region:   NA
    revision: 1.0
    board:    NES-SNROM-02
    serial:   NES-ZL-USA
    sha256:   085e5397a3487357c263dfa159fb0fe20a5f3ea8ef82d7af6a7e848d3b9364e8
    configuration
      rom name=program.rom size=0x20000
      ram name=save.ram size=0x2000
      ram name=character.ram size=0x2000

release
  cartridge region=NTSC
    board type=NES-SNROM
    chip type=MMC1B3
    prg
      rom name=program.rom size=0x20000
      ram name=save.ram size=0x2000
    chr
      ram size=0x2000
  information
    title:    The Legend of Zelda
    name:     Legend of Zelda, The
    region:   NA
    revision: 1.1
    board:    NES-SNROM-05
    serial:   NES-ZL-USA
    sha256:   ec0d4ebf6d2fcecd1d95fef7329954efe79676959bc281ea908b226459bc6dc2
    configuration
      rom name=program.rom size=0x20000
      ram name=save.ram size=0x2000
      ram name=character.ram size=0x2000

release
  cartridge region=NTSC
    board type=NES-TLROM
    chip type=MMC3C
    prg
      rom name=program.rom size=0x40000
    chr
      rom name=character.rom size=0x20000
  information
    title:    Little Samson
    name:     Little Samson
    region:   NA
    revision: 1.0
    board:    NES-TLROM-03
    serial:   NES-LT-USA
    sha256:   a5165565263eaf8bdc45a8e6a615704f9bf271cd6d547d22c098c80cbaffd879
    configuration
      rom name=program.rom size=0x40000
      rom name=character.rom size=0x20000

release
  cartridge region=NTSC
    board type=NES-SGROM
    chip type=MMC1B2
    prg
      rom name=program.rom size=0x40000
    chr
      ram size=0x2000
  information
    title:    Mega Man 2
    name:     Mega Man 2
    region:   NA
    revision: 1.0
    board:    NES-SGROM-04
    serial:   NES-XR-USA
    sha256:   1e588d435e75d80c5c0b578b4fa8d196f2cf4346c11c9a7b7e435d768828ad01
    configuration
      rom name=program.rom size=0x40000
      ram name=character.ram size=0x2000

release
  cartridge region=NTSC
    board type=NES-TQROM
    chip type=MMC3A
    prg
      rom name=program.rom size=0x20000
    chr
      rom name=character.rom size=0x10000
      ram size=0x2000
  information
    title:    Pin-Bot
    name:     Pin-Bot
    region:   NA
    revision: 1.0
    board:    NES-TQROM-01
    serial:   NES-IO-USA
    sha256:   f4ddb0f1a02f823ebed30b55547344de3c8fb9d87254ff993584373ecadd9141
    configuration
      rom name=program.rom size=0x20000
      rom name=character.rom size=0x10000
      rom name=nametable.ram size=0x2000

release
  cartridge region=NTSC
    board type=NES-CNROM
    mirror mode=horizontal
    prg
      rom name=program.rom size=0x8000
    chr
      rom name=character.rom size=0x8000
  information
    title:    Puzznic
    name:     Puzznic
    region:   NA
    revision: 1.0
    board:    NES-CNROM-07
    serial:   NES-ZP-USA
    sha256:   0cf2fc17a59a0932ce43e6b2e9ea4e2570f03139784b5c9df429a499e734b92e
    configuration
      rom name=program.rom size=0x8000
      rom name=character.rom size=0x8000

release
  cartridge region=NTSC
    board type=NES-HKROM
    chip type=MMC6B
    prg
      rom name=program.rom size=0x40000
      ram name=save.ram size=0x400
    chr
      rom name=character.rom size=0x40000
  information
    title:    StarTropics
    name:     StarTropics
    region:   NA
    revision: 1.0
    board:    NES-HKROM-01
    serial:   NES-OC-USA
    sha256:   69de2c7552fa81ca5921da0e457abf1be35f18ffbad159788a76141be59c9f6b
    configuration
      rom name=program.rom size=0x40000
      ram name=save.ram size=0x400
      rom name=character.rom size=0x40000

release
  cartridge region=NTSC
    board type=NES-NROM-256
    mirror mode=vertical
    prg
      rom name=program.rom size=0x8000
    chr
      rom name=character.rom size=0x2000
  information
    title:    Super Mario Bros.
    name:     Super Mario Bros.
    region:   NA
    revision: 1.0
    board:    NES-NROM-256-04
    serial:   NES-SM-USA
    sha256:   fcb6a0ef3a20c19b356005fbb21dc8009563b1cb5a9aaebc8e9386b4a8c5912e
    configuration
      rom name=program.rom size=0x8000
      rom name=character.rom size=0x2000

release
  cartridge region=NTSC
    board type=NES-MHROM
    mirror mode=vertical
    prg
      rom name=program.rom size=0x10000
    chr
      rom name=character.rom size=0x4000
  information
    title:    Super Mario Bros. / Duck Hunt
    name:     Super Mario Bros. + Duck Hunt
    region:   NA
    revision: 1.0
    board:    NES-MHROM-04
    serial:   NES-MH-USA
    sha256:   5dde385041aa7364c78205f2ba49615f416c701b6025e38aa1d7b9c4f99a62db
    configuration
      rom name=program.rom size=0x10000
      rom name=character.rom size=0x4000

release
  cartridge region=NTSC
    board type=NES-TSROM
    chip type=MMC3A
    prg
      rom name=program.rom size=0x20000
      ram size=0x2000
    chr
      rom name=character.rom size=0x20000
  information
    title:    Super Mario Bros. 2
    name:     Super Mario Bros. 2
    region:   NA
    revision: 1.0
    board:    NES-TSROM-01
    serial:   NES-MW-USA
    sha256:   cba920f9394733c82253685d7783f26a3033ba58a94623e9abf7892329b969b9
    configuration
      rom name=program.rom size=0x20000
      ram name=work.ram size=0x2000
      rom name=character.rom size=0x20000

release
  cartridge region=NTSC
    board type=NES-TSROM
    chip type=MMC3B
    prg
      rom name=program.rom size=0x20000
      ram size=0x2000
    chr
      rom name=character.rom size=0x20000
  information
    title:    Super Mario Bros. 2
    name:     Super Mario Bros. 2
    region:   NA
    revision: 1.1
    board:    NES-TSROM-08
    serial:   NES-MW-USA
    serial:   NES-MW-CAN
    sha256:   728d0ca6751b0c039fc3e34f2e7f27a870afcab30f5e270244ac40979c5f69ca
    configuration
      rom name=program.rom size=0x20000
      ram name=work.ram size=0x2000
      rom name=character.rom size=0x20000

release
  cartridge region=NTSC
    board type=NES-TSROM
    chip type=MMC3B
    prg
      rom name=program.rom size=0x40000
      ram size=0x2000
    chr
      rom name=character.rom size=0x20000
  information
    title:    Super Mario Bros. 3
    name:     Super Mario Bros. 3
    region:   NA
    revision: 1.0
    board:    NES-TSROM-06
    serial:   NES-UM-USA
    sha256:   d77d17d34af24871d7ce1160ccd3330555835c8e940b7100e095ac38973d927a
    configuration
      rom name=program.rom size=0x40000
      ram name=work.ram size=0x2000
      rom name=character.rom size=0x20000

release
  cartridge region=NTSC
    board type=NES-TSROM
    chip type=MMC3B
    prg
      rom name=program.rom size=0x40000
      ram size=0x2000
    chr
      rom name=character.rom size=0x20000
  information
    title:    Super Mario Bros. 3
    name:     Super Mario Bros. 3
    region:   NA
    revision: 1.1
    board:    NES-TSROM-07
    serial:   NES-UM-USA
    serial:   NES-UM-CAN
    sha256:   959fdd32c71735d6fb2bd16a646d39f4ee65623273dd035e6a968e991bd13ef8
    configuration
      rom name=program.rom size=0x40000
      ram name=work.ram size=0x2000
      rom name=character.rom size=0x20000

release
  cartridge region=NTSC
    board type=NES-SEROM
    chip type=MMC1B2
    prg
      rom name=program.rom size=0x8000
    chr
      rom name=character.rom size=0x4000
  information
    title:    Tetris
    name:     Tetris
    region:   NA
    revision: 1.0
    board:    NES-SEROM-04
    serial:   NES-EI-USA
    serial:   NES-EI-CAN
    sha256:   2ae5fb18a1bf841077e3872ba05060f030ea0bfc573994b2f8fe2fb570dc7853
    configuration
      rom name=program.rom size=0x8000
      rom name=character.rom size=0x4000

release
  cartridge region=NTSC
    board type=NES-TLROM
    chip type=MMC3B
    prg
      rom name=program.rom size=0x20000
    chr
      rom name=character.rom size=0x20000
  information
    title:    Totally Rad
    name:     Totally Rad
    region:   NA
    revision: 1.0
    board:    NES-TLROM-03
    serial:   NES-6T-USA
    sha256:   0bd29c5b583570e195e05a4272e69b74257db1f4ff928d59c8eb1022bad871ce
    configuration
      rom name=program.rom size=0x20000
      rom name=character.rom size=0x20000

release
  cartridge region=NTSC
    board type=NES-TKROM
    chip type=MMC3C
    prg
      rom name=program.rom size=0x40000
      ram name=save.ram size=0x2000
    chr
      rom name=character.rom size=0x40000
  information
    title:    Wario's Woods
    name:     Wario's Woods
    region:   NA
    revision: 1.0
    board:    NES-TKROM-10
    serial:   NES-WB-USA
    sha256:   c12771e8155b030eff0081bfabd98e57a162d6592899f29dd16f141f0e6e08a3
    configuration
      rom name=program.rom size=0x40000
      ram name=save.ram size=0x2000
      rom name=character.rom size=0x40000

release
  cartridge region=NTSC
    board type=NES-NROM-128
    mirror mode=vertical
    prg
      rom name=program.rom size=0x4000
    chr
      rom name=character.rom size=0x2000
  information
    title:    Wild Gunman
    name:     Wild Gunman
    region:   NA
    revision: 1.0
    board:    NES-NROM-128-01
    serial:   NES-WG-USA
    sha256:   62aec65696ecf24a487b7cdd19bad5cbd19f4229a89a7888634d468c67da378a
    configuration
      rom name=program.rom size=0x4000
      rom name=character.rom size=0x2000

release
  cartridge region=NTSC
    board type=NES-NROM-128
    mirror mode=vertical
    prg
      rom name=program.rom size=0x4000
    chr
      rom name=character.rom size=0x2000
  information
    title:    Wild Gunman
    name:     Wild Gunman
    region:   NA
    revision: 1.1
    board:    NES-NROM-128-04
    serial:   NES-WG-USA
    sha256:   adff304553b64384f86f6c2b63571f43972b9d087f92359a1b9b93b54d523542
    configuration
      rom name=program.rom size=0x4000
      rom name=character.rom size=0x2000

release
  cartridge region=NTSC
    board type=NES-CNROM
    mirror mode=horizontal
    prg
      rom name=program.rom size=0x8000
    chr
      rom name=character.rom size=0x8000
  information
    title:    World Class Track Meet
    name:     World Class Track Meet
    region:   NA
    revision: 1.0
    board:    NES-CN-ROM-256-05
    serial:   NES-WT-USA
    sha256:   6c74bcb9ac50428266cfc4d13892b6e93c6936ba569bf3f0f2068e45a9b675fc
    configuration
      rom name=program.rom size=0x8000
      rom name=character.rom size=0x8000

release
  cartridge region=NTSC
    board type=NES-CNROM
    mirror mode=horizontal
    prg
      rom name=program.rom size=0x8000
    chr
      rom name=character.rom size=0x8000
  information
    title:    World Class Track Meet
    name:     World Class Track Meet
    region:   NA
    revision: 1.1
    board:    NES-CNROM-07
    serial:   NES-WT-USA
    sha256:   ac766a8d99bfd8e95c02b9a9c68279c72ba5b3307b78edc67b52781ed185fa89
    configuration
      rom name=program.rom size=0x8000
      rom name=character.rom size=0x8000

release
  cartridge region=NTSC
    board type=NES-HKROM
    chip type=MMC6B
    prg
      rom name=program.rom size=0x40000
      ram name=save.ram size=0x400
    chr
      rom name=character.rom size=0x40000
  information
    title:    Zoda's Revenge: StarTropics 2
    name:     Zoda's Revenge - StarTropics 2
    region:   NA
    revision: 1.0
    board:    NES-HKROM-02
    serial:   NES-6C-USA
    sha256:   d0850075065ecbd125a33accc952de5d012527be45aa14a1b8223a9adf1643ae
    configuration
      rom name=program.rom size=0x40000
      ram name=save.ram size=0x400
      rom name=character.rom size=0x40000

release
  cartridge region=PAL
    board type=NES-NROM-128
    mirror mode=vertical
    prg
      rom name=program.rom size=0x4000
    chr
      rom name=character.rom size=0x2000
  information
    title:    Hogan's Alley
    name:     Hogan's Alley
    region:   EU
    revision: 1.0
    board:    NES-NROM-128-03
    serial:   NES-HA-EEC
    sha256:   8e4a04076b6a728a7e65a09737776dcb9defed7922bf0437d9a89bbe8c724b55
    configuration
      rom name=program.rom size=0x4000
      rom name=character.rom size=0x2000

release
  cartridge region=PAL
    board type=NES-TSROM
    chip type=MMC3C
    prg
      rom name=program.rom size=0x20000
      ram size=0x2000
    chr
      rom name=character.rom size=0x20000
  information
    title:    Noah's Ark
    name:     Noah's Ark
    region:   EU
    revision: 1.0
    board:    NES-TSROM-08
    serial:   NES-NH-NOE
    sha256:   6157c99fe7a214025c65fd3649e4afe9cd2d38c333e65af028b935e49fbeb500
    configuration
      rom name=program.rom size=0x20000
      ram name=work.ram size=0x2000
      rom name=character.rom size=0x20000

release
  cartridge region=PAL
    board type=NES-TKROM
    chip type=MMC3C
    prg
      rom name=program.rom size=0x40000
      ram name=save.ram size=0x2000
    chr
      rom name=character.rom size=0x40000
  information
    title:    Wario's Woods
    name:     Wario's Woods
    region:   EU
    revision: 1.0
    board:    NES-TKROM-10
    serial:   NES-WB-NOE
    sha256:   9fe815d8fd175ef9ef03fb010638f2b6b7aa9d11d5a40eda2476450918543e6f
    configuration
      rom name=program.rom size=0x40000
      ram name=save.ram size=0x2000
      rom name=character.rom size=0x40000

)";
