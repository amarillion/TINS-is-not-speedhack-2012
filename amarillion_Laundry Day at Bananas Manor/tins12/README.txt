=================================
  Laundry day at bananas manor
=================================

Oh no! It's laundry day at bananas manor, 
but the laundry machine is going horribly awry!

This is our entry for the  TINS 2012 Competition

How to play:

After the intro sequence, you get the level select 
screen, where you can choose one of four levels 
(a.k.a. laundry machines).

The last level is disabled until you have finished
the first three.

The default key bindings are as follows

Left key = move left
Rigth key = move right
Down key = toggle weapon (if available)
Up key = Jump
Space key = Shoot

You pass a level simply by exiting it to the right.

After passing a level, you get access to a more
powerful weapon.

The fourth level is an epic battle with Dragoncat!

Have Fun!

Max & Amarillion

=================================
			Rules
=================================

Here are the TINS 2012 Rules:

**** There will be 1 genre rule

genre rule #79
Theme: creative anachronism: combine two elements that are obviously from different time area's


**** There will be 1 artistical rule

artistical rule #56
Here be dragons


technical rule #61
multilayer scrolling (=parallax scrolling)


technical rule #64
prominent use of prime numbers


**** There will be 1 bonus rule

bonus rule #9
Act of Guttenberg - swap a rule for a rule of an earlier competition,
if you re-use code from an entry from that competition (not your own). 
You can apply this rule several times if you re-use code from several entries.


Here is how we implement them:

1. Creative anachronism: the main plot is about a laundry machine in the victorian age. 
On top of that, there is a anachronistic mix of weapons from a catapult to a laser gun.

2. The final boss is a dragon. 

3. Store progress: we use a passcode system to store progress (which was more challenging 
and interesting than saving to disk). This rule has inspired us to create a megaman-like game, 
where you can play levels in any order, and obtain a new weapon at the end of each.

4. Parallax scrolling: yup. The game is supposed to take place inside the laundry machine.

5. Prime numbers: we used prime numbers everywhere in the game,
from tilemap dimensions to game constants. 

I've marked the use of prime numbers in the code with the comment "PRIME"
You can see for yourself if you run the following bash command:

grep PRIME src/*.cpp include/*.h shared/src/*.cpp shared/include/*.h | wc -l
> 19

The result is 19, which is of course also a prime.

But that is just small stuff. The main use is in the passcode. 
Pass codes are only valid if they are a multiplication of 
four prime numbers. The code you enter is decomposed into
it's four prime factors, and which those are determines
if you can pass or not.

We're not invoking the bonus rule. 

=================================
         Installation
=================================

Libraries used:

Allegro 4.4.2 with logg extension
Alfont
Tegel 0.82

=================================
         	Info
=================================

Font "megaman 2" is (c) Caterkiller 2010, from http://en.fonts2u.com/megaman-2-regular.font

http://dl.openhandhelds.org/cgi-bin/gp32.cgi?0,1,0,0,23,452

I actually heavily re-used code from 
our own previous entries (not just Mexica, the last one. I grabbed code from nearly 
all my past entries, including Eleven monkeys, Xelda, Evil Dr. F and Fole & Raul go bananas.
