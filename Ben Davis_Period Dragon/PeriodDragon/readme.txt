Period Dragon
=============

Well here we are - my entry into TINS 2012 :)

http://tins.amarillion.org/2012/

I've written my entry in D this time. D is a wonderful language which offers
a lot of the safety you get from Java or C# - things like garbage collection,
bounds checking, everything initialised to a default value - while still
compiling to native. Also, the deeper I get into it, the more fantastically
good design decisions I find - global variables being thread-local by
default, type syntax like const(char)* being less confusing ...

However, that does mean it might be one of the less trivial entries to
compile.

As I write this, it's getting on for 5 a.m., and I don't have Monday off
work. So unfortunately, I'm going to have to give just the bare minimum of
instructions and assume a lot of prior knowledge. Massive apologies for this!
I'll make a Windows version available after life has settled down a bit. But
if I'm not available, SiegeLord's entry is also in D, and I'm using his
Allegro bindings, so he is the person to ask!


Licence
-------

- You may distribute this game unmodified, or with minimal portability fixes,
  or with an added executable, or both - provided that you do it strictly for
  non-profit purposes.

- You may of course enjoy the game to your heart's content.

- All other rights are reserved.


Compiling the game
------------------

You can get D from:

http://ftp.digitalmars.com/dmd.2.059.zip

(Note that version 2.060 gave me problems.)

If you want to know more about D in general, go to:

http://dlang.org/

You will also need to get SiegeLord's D bindings from here:

https://github.com/SiegeLord/DAllegro5

Follow SiegeLord's instructions to get up and running. You'll probably need
to generate D-format 'lib' files that map on to Allegro's DLLs. You might
also need other D dependencies.

D has the concept of 'import' paths (or does it call them 'include'?) -
these are directories where other .d files can be found, sort of. To be
precise, if my file says "import allegro5.allegro;" in it, and you've
specified an import path of "C:\DAllegro5", then the D compiler will need to
be able to find "C:\DAllegro5\allegro5\allegro.d".

D also has library search paths - just specify the directories containing
Allegro 5's lib files.

You may need to mess around with my 'pragma(lib, "...")' lines - search for
pragma. It depends how your Allegro build is arranged.

Other than that, note that D tends to like to compile everything together.
Specify all the .d files at once.

For Windows, I recommend compiling with -L/SUBSYSTEM:WINDOWS:4.0
- this will make it a Windows application instead of a console one.


The rules
---------

Theme: creative anachronism: combine two elements that are obviously from
different time eras

- Well, dragons are prehistoric, right, and they wouldn't exist in a world
  where electricity has been invented. That's my story and I'm sticking by
  it.

artistical rule #56 - Here be dragons

- You control a dragon in this game. An eponymous one, no less. (Note how I
  also phrased that last sentence in such a way that you can decide whether
  your dragon is a he or a she.)

technical rule #42 - must have way to record progress / save games

- Not only will it remember which level you're up to, but you can save and
  reload the exact state of the level at any time!

technical rule #61
multilayer scrolling (=parallax scrolling)

- Well, this is the rule that fundamentally defined the game for me!

technical rule #64
prominent use of prime numbers

- All the planes have prime numbers as their period of repetition. That means
  it takes a heck of a long time for the whole thing to wrap round. Don't get
  lost. In the provided levels, the solutions are always quite close to the
  starting position.

bonus rule #9
Act of Guttenberg - swap a rule for a rule of an earlier competition, if you
re-use code from an entry from that competition (not your own). You can apply
this rule several times if you re-use code from several entries.

- Not invoked.


Code re-use
-----------

I started with the example that comes with SiegeLord's D bindings. It's
pretty minimal. Everything on top of that was written fresh during the
competition.


Me
--

- Ben 'Bruce "entheh" Perry' Davis
- entheh@cantab.net
