This project is simply an amateur scheme for compiling ffmpeg with add-ons in gcc 15.1.0 {test msys2 Rev6}, mingw64 12.0.0 {test msys2 r1} and nasm 2.17rc0 from a .bat file. 
In this scheme I tried to eliminate duplicate add-on functions. I fixed few bugs. I don't know how it would compile in the recommended MSVC. 
I managed to add the free librsvg, avisynth or lcevc decoder. The RSVG project is omitted on Windows due to a problem with reading fonts from SVG files. It may be a redundant add-on for HTML pages these days.
