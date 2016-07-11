# StarHonor
Star Honor game for Arduboy by Wenceslao Villanueva Jr

Star Honor

Star Honor is a Roguelike space adventure where you are field promoted into the Captain’s chair of the USS Arduino.  Your home world has become the victim of the a biological weapon deployed by enemies.  You posses the only hope for your world, a cure for the phage.  With a skeleton crew, a limping vessel and time running out, you must travel world to world, seeing supplies, upgrades and doing battle to make it home and save your planet.

Instructions

Up 		      - Forward
Down 		    - Backwards
Left/Right	- Turn
A Button	  - Status Screen (Map)
		        - Back
B Button	  - Hail (Map)
		        - Confirm
Up + B		  - Warp (When you’ve gathered at least 3 crystal)

The goal is the get home before time runs out.  Navigate the sector to visit planets to find upgrades and Crystal.  You need 3 Crystals to power your warp drives.  Each warp jump gets you to a sector closer to home.  Get to Sector 1, save your planet, easy right?

Unfortunately it isn’t that easy, while some species are more than willing to help, those loyal to the Empire will attack you.  When that happens you will have to do battle.  

Attacking reduces a planet’s defense
When that reaches zero, you win the battle
However a planet retaliates after you attack and they will strike with their attack power
Your shields absorb all the damage first
After that, various systems may take damage such as your engines, weapons and hull
When the hull reaches zero, the USS Arduino will lose containment and well… space is harsh.

During battle you may check the status of your ship and issue emergency repairs.  You must have at least 30 crew to issue a repair once per battle.  So keep them alive and well!  For every 10 crew you find during your voyage, you will have more emergency repairs available.


Version 1.0

I started by writing a tool called Image2Arduboy which I used extensively to get assets into the game.  It can take an image, down res it to fit on the screen, reduce to 1 bit and output a hex byte array that is suitable for drawBitmap and drawSlowXYBitmap.  Using Image2Arduboy I wrote my own font system and a text system so that I could include a lot of text in the game and display it.

This game was a long road of trying and testing several things out and trying to scope the project for the Arduboy.  The scope often shifted due to the amount of code I could fit in there.  You can browse the code and find code for random encounters, AI Ships, Live weapon firing code.  When I started moving in a direction I thought was do-able I still ran out of progmem space.  Even strings are expensive!  So I began to learn exactly what all my code did.  I did so many optimizations and if you look at the code, I left a lot of them in there commented out as examples for myself and maybe others.  I will clean up the code at this point but I felt it was important to understand what everything really does when resources are short.  I eventually took the core and Arduboy libraries and optimized and removed out of there too.  Hey, the Arduboy intro is neat but it’s 1k!  That’s a lot of text I could have!  The only thing I’d do different is probably eliminate the use of floats, they make function calls and operations cost a lot of space!  I eliminated a lot of them but they’re still convenient to use in places.

I had a lot of fun making this game and I think I’ve taken it as far as I can with the resources available.  In a time where I use a lot of engines and higher level languages, I’m proud of the game and building it from the ground up.  I feel like a crammed a lot of stuff in there and got hit a good spot between gameplay and code footprint.  I again have mad respect for those who make games early on in console days with much less.

Please enjoy, comment and post ideas that can fit into exactly 0 bytes
Sketch uses 28,672 bytes (100%) of program storage space. Maximum is 28,672 bytes.


TLDR: Please let me know if you have issues compiling, I had some issues with the latest Arduino IDE updates.  Plus the project includes a custom version of Arduboy and Core libraries that strip out some features to make room for more game!  (Sorry Flashlight!)
