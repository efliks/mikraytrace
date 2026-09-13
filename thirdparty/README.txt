Vendored third-party sources
=============================

These libraries were previously tracked as git submodules and are now
vendored directly (copied in).

lodepng
-------
Upstream:  https://github.com/lvandeve/lodepng
Commit:    18964554bc769255401942e0e6dfd09f2fab2093 (2022-07-18)
Files:     lodepng.cpp, lodepng.h, LICENSE
License:   zlib (see thirdparty/lodepng/LICENSE)

toojpeg98
---------
Upstream:  https://github.com/efliks/toojpeg98 (personal fork of
           https://github.com/stbrumme/toojpeg)
Commit:    e17d471edf994e38b8f1c2e52441f6ebebfc8109
Files:     toojpeg.cpp, toojpeg.h, LICENSE, readme.md
License:   zlib (see thirdparty/toojpeg/LICENSE)
Note:      This fork ensures C++98/Watcom compatibility
