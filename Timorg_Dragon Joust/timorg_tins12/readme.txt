Dragon Joust, Tins 2012
by Timorg (anthonytcassidy@hotmail.com)

Tools and Libraries Used
------------------------------------------------------------------------
Compiler/IDE: Code::Blocks (GCC 4.5.2)
Platform: Windows
Language: C++
Allegro Version: 4.4.2

Notes on Compiling
------------------------------------------------------------------------
Each part of the program (the game and sprite editor) is self contained
its just a matter of compiling and linking against allegro 4.4 (4.2
should work fine)

Eg: Under windows
g++ -ojoust joust.cpp -lalleg
g++ -ospr_edit spr_edit.cpp -lalleg

Tins Rules
------------------------------------------------------------------------
Genre Rule #79: Creative Anachronism
The game features a knight as the player character, you can tell he is a
knight as he is wearing a grey suit of armour. The goal of the game is
to destroy the robots that are being built by the platforms. Robots and
knights are from different time eras.

Artistic rule #56: Here be Dragons
The game features a knight riding a dragon.

Technical Rule #42: Must have way to record progress / save games
The game saves and restores the current program state when it is exited
and restarted. The game can be continued where it left off when the
program loads.

Technical Rule #61: Multilayer Scrolling
The artwork in the game is vector based, a second smaller version of the
sprite is rendered behind the main one. Its offset is determined by the
sprites position on the screen, giving a parallax (depth) effect.

Technical Rule #64: Prominent use of Prime Numbers
Where robots spawn is based on prime numbers. A list of 32 prime numbers
is used, this number is converted to binary and the bits are used to
decide how many robots to spawn, and which spawners they come from. The
score allocated for different game events are also prime. (3, 7 and 11.)

About the Game
------------------------------------------------------------------------
Dragon Joust is a clone of the game Joust.
(http://en.wikipedia.org/wiki/Joust_%28video_game%29)

The player is in control of a knight riding a dragon, he has somehow
found his way into a 1980s graphics card factory. The factory has been
taken over by killer robots who have modified the factory so instead of
producing CGA cards, it produces killer robots instead.

How to Play
------------------------------------------------------------------------
The goal is to kick the robots off the sides at the bottom of the
screen. At the start of each level the factory produces a number of
robots that are created in their shutdown state. After the activation
process has completed they become operational and try and kill any
intruders that may have found their way into the factory. The robots
have emergency shutdown buttons on the top of their head, when pushed
the robot return to their shutdown state. Unfortunately the factory
automatically restarts their activation process, the only way to
permanently destroy them is kick them down into the waste furnaces at
the sides of the bottom of the screen. A shutdown robot can be pushed
across the floor by standing on top of them.

Scoring:
3 points are awarded for shutting a robot down.
7 points are awarded for incinerating a shut down robot.
11 points are awarded for an active robot incinerating itself.

Controls:
Left Cursor - Move the dragon to the left.
Right Cursor - Move the dragon to the right.
Up Cursor - Cause the dragon to flap their wings.*
F3 - Reset the state of the game.
F4 - Take a screenshot.

* When the dragon flaps their wings, it will cause them to accend a
small distance, to continue upwards they will need to repeatable flap
their wings.

Easter Eggs:
------------------------------------------------------------------------
If the program is started with a command line argument, it will run in
an emulated CGA mode.

Known Bugs:
------------------------------------------------------------------------
There is quite a few problems with the collision detection in the game,
this is mostly only visible when trying to kick the shutdown robots,
they can not be kicked when the player is standing on the ground, the
player is required to fall down onto them. Fast objects can also pass
through platforms.

When running the game in the emulated cga mode, the font is still the
inbuilt allegro one, it has not been changed.