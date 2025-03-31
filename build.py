#!/usr/bin/env python3

# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2024

from architectds import *

nitrofs = NitroFS()
#convert the .png and .jpg in assets/bgtiled to 8bit backgrounds and output into bg
nitrofs.add_nflib_bg_tiled(['assets/bgtiled'], 'bg')
#convert the files in sprites
nitrofs.add_nflib_sprite_256(['assets/sprites'], 'sp')
nitrofs.generate_image()

arm9 = Arm9Binary(
    sourcedirs=['source'],
    libs=['nds9', 'nflib'],
    libdirs=['${BLOCKSDS}/libs/libnds', '${BLOCKSDSEXT}/nflib']
)
arm9.generate_elf()

nds = NdsRom(
    binaries=[arm9, nitrofs],
    game_title='Cliff Flier',
)
nds.generate_nds()

nds.run_command_line_arguments()
