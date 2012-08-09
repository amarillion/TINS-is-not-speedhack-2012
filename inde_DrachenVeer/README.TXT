Kiwi
====

Information
-----------

Dennis Busch            <Dennis-Busch@gmx.net>
Karthik Kumar V		<karthikkumar@gmail.com>
Nikhil Karnad		<nikhil.karnad@gmail.com>

Usage
-----

On Windows, run Launch.cmd. On Linux/Mac, run Launch.

How to Play
-----------

WASD to Move, P to Attack. Esc to Quit.

How we Implement Rules
-----------------------

1. genre rule #79  - Theme: creative anachronism: combine two elements that are obviously from different time area's.
Notes: We bring in a culmination of skyscrapers and cavemen, with victorian era lighting.

2. technical rule #42
must have way to record progress / save games
Notes: We aren't doing it. We are swapping with: artistical rule #132
Hand drawn. Feature hand-drawn or hand-painted graphics.
You may use actual scanned drawings, or just simulate them.

3. technical rule #61 - multilayer scrolling (=parallax scrolling)
Notes: Our game uses a parallax system for scrolling.

4.  technical rule #64 - prominent use of prime numbers
Notes: Our attacks are based on prime numbers.

5. artistical rule #56 - Here be dragons
Notes: Our primary character is an evolving dragon.

6. bonus rule #9
Act of Guttenberg - swap a rule for a rule of an earlier competition,
if you re-use code from an entry from that competition (not your own). 
You can apply this rule several times if you re-use code from several entries.
Notes: We used string_from_file code in Hotel Hell. We made it more generic to read larger files.

Build Notes
----- -----

We need one of these compilers to build:

 1. MSVC (Windows)
 2. gcc (Linux/Windows)

We need these libraries to build:

 1. Allegro 5.1.0
 2. Lua 5.1.* - Not Lua 5.2
 3. Chipmunk 6.0.3+ - Git

We require these utilities to build:

 1. GNU Make 3.8*
 2. CMake 2.8.*
 3. Swig 2.0*

We need these source control utilities to build:

 1. Mercurial 2.0*
 2. Git 1.7.8+

We need this in-code library to build:

 1. WidgetZ - Git

In order to build, issue these commands:

make clean
make

To build with an IDE, issue:

make clean
make IDE=true

To find prebuilt versions of Libraries, check:
http://guilt.bafsoft.net/downloads/dist/

CMake Build Notes
----- ----- -----
If there are errors finding Allegro, here are a few
of the common ones and how to fix them:

1. CMake uses the CMake/Modules/*.cmake
custom modules to look for include and lib
files. Ensure that the Allegro version is correct,
e.g. 5.1.1 vs 5.1.3

2. If you have not installed these libraries to your system
lib directory with pkg-config files, you need to
supply the director paths manually through
environment variables.

$ export ALLEGRO_HOME=/home/my_name/allegro5/
$ export LUA_HOME=/home/my_name/lua51/
$ export CHIPMUNK_HOME=/home/my_name/chipmunk/
$ export SWIG_HOME=/home/my_name/swig/
$ export PKG_CONFIG_PATH=$ALLEGRO_HOME/lib/pkgconfig/:$LUA_HOME/lib/pkgconfig/:$CHIPMUNK_HOME/lib/pkgconfig:$PKG_CONFIG_PATH

ALLEGRO_HOME, LUA_HOME and CHIPMUNK_HOME contain include/
and lib/ subdirectories respectively. For Allegro library, it is
the DESTDIR of make install on the source code. SWIG_HOME
must contain the swig executable.

3. Alternatively, You can also get a precompiled dependencies
bundle and extract it in the directory. Deps/include and Deps/lib
will be used in the search process.

You can also try setting the EXTDEV variable, and
 dependencies are fetched from it.

$export EXTDEV=/home/my_name

4. WidgetZ needs to be fetched into the Src/ directory.
 We maintain a Git Fork, from where we push patches.

$ cd Src
$ git clone git@github.com:guilt/WidgetZ.git

If permissions are denied from GitHub, the following might work for read-only access:
$ git clone git://github.com/guilt/WidgetZ.git
