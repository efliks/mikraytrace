#!/bin/sh

wget https://opengameart.org/sites/default/files/02camino.png
wget https://opengameart.org/sites/default/files/04univ2.png
wget https://opengameart.org/sites/default/files/04univ3.png
wget https://opengameart.org/sites/default/files/01tizeta_floor_f.png
wget https://opengameart.org/sites/default/files/01tizeta_floor_g.png
wget https://opengameart.org/sites/default/files/02intonacopr_nt3.jpg
wget https://opengameart.org/sites/default/files/01tizeta_floor_d.png
wget https://opengameart.org/sites/default/files/metal%20plate%20tex.png
wget https://opengameart.org/sites/default/files/trak2_wall1b.tga
wget https://opengameart.org/sites/default/files/trak2_tile1b.tga
wget https://opengameart.org/sites/default/files/trak2_gr81b.tga
wget https://opengameart.org/sites/default/files/trak_tile_g.jpg

for x in *.tga ; do convert $x `basename $x .tga`.png ; done

for x in *.jpg ; do convert $x `basename $x .jpg`.png ; done

rm *.tga *.jpg
