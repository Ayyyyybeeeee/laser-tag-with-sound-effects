# Crunchlabs Hack Pack Laser Tag

This is a hack (or a mod) for the [Crunchlabs](https://www.crunchlabs.com/) Hack Pack Build #5 [Laser Tag](https://www.crunchlabs.com/products/laser-tag) kit.

This hack adds sound effects using [DFR1173](https://www.dfrobot.com/product-2862.html) :
- startup sound
- shoot sound (one for each team)
- sound when you are 'hit' (one for each team) 
- an alarm sound if you try and shoot when in timeout

I used the [DFR1173](https://www.dfrobot.com/product-2862.html) sound module from DFROBOT, possibly the least documented arduino module ever.  Most of the time on this project was dedicated to making this thing work.

The sound files I used are in the ZIP file.  I also found it helpful to format the DFR1173 first using windows format command from DOS prompt.  It's really important that the bit rate does not exceed 192, and the filenames are in a numbered (two digit) sub-folder, and the filenames begin with a three digit number and the total filename length does not exceed 12 bytes.  You load the sound files using a special USB cable that comes with the DFR1173 (make sure the arrow on the cable is pointing to the 5V pin).

You can choose whatever sounds you want by loading the MP3 files on to the DFR1173:
* `E:\02\001_powr.mp3` - :battery: Proton pack startup sound from Ghostbusters[^1] 
* `E:\02\002_blt1.mp3` - :gun: Blaster sound from Star Wars[^2]
* `E:\02\003_blt2.mp3` - :gun: Blaster not violent[^3]
* `E:\02\004_beep.mp3` - :rotating_light: Proton pack overheat sound from Ghostbusters[^4]
* `E:\02\005_blt3.mp3` - :gun: TIE Balster Canon from Star Wars[^5]
* `E:\02\006_die1.mp3` - :fire: R2D2 sound from Star Wars[^6]
* `E:\02\007_die2.mp3` - :fire: R2D2 sound from Star Wars[^7]
* `E:\02\008_die3.mp3` - :fire: R2D2 sound from Star Wars[^8]


I simply taped the DFR1173 on to the front of the battery with electrical tape.  An upgrade would be to 3D print a proper holder for this, or even use heatshrink tubing.  I've used 3 long (40cm) dupont wires to take the 5v/gnd and signal from the crunchlabs breadboard to the DFR1173 5v, gnd and Rx pins.

My code includes a couple of 'fixes' to the original code:
1. in sendIR_Pulse() sending 3 pulses seems like overkill, and can result in dead time in gameplay where two people can shoot each other and not get any hits.  This is reduced to a single pulse with IrSender.sendNEC()
2. in markHit() the servos are moved back when 80% of the time is up, but this results in dead time in gameplay while your glasses are clear, but you cannot shoot.  I've reduced it from 20% to 2%.

You can see a 'diff' to the original with [commit 7469543](https://github.com/Ayyyyybeeeee/laser-tag-with-sound-effects/commit/7469543)

To implement this hack:
- [ ] Assemble Hack Pack Build #5 [Laser Tag](https://www.crunchlabs.com/products/laser-tag) kit.
- [ ] One [DFR1173](https://www.dfrobot.com/product-2862.html) formatted and loaded with [contents of ZIP](../../raw/refs/heads/main/DFR1173-with-die-sounds.zip) - ensure the arrow on the cable is pointing to the 5V pin.
- [ ] 3 long (40cm) dupont wires
- [ ] Arduino IDE with the original sketch, compiling and uploading (note: you need to remove the Arduino Nano from the gun)
- [ ] Copy the [new sketch](../main/sketch_mar28a.ino) into the Ardunio IDE, compile and upload
- [ ] Connect dupont wires to the 5v, gnd and Rx pins on the DFR1173
- [ ] The other end of the dupont wires go to the crunchlabs breadboard pin 2, and anywhere on the red (5v) and black gnd) rails
- [ ] Turn it on and test it!

Here are photos of a simple way (electric tape) and a more advanced way (heat shrink) to attach [DFR1173](https://www.dfrobot.com/product-2862.html) to the battery pack:

![Image-15](https://github.com/user-attachments/assets/92859fb6-1595-40d2-860d-9b48af3c0e74) ![Image-3](https://github.com/user-attachments/assets/3ce2ace9-7fd6-4f4e-b927-36f48b569c1c)

Full Size [1](https://github.com/user-attachments/assets/3f237a16-3060-4298-a4bb-19fde553ac99)  [2](https://github.com/user-attachments/assets/3e3c9100-9e02-4c43-a0ca-acf3bd1e04cd)
   

Here is a photo of the wiring on the Crunchlabs breadboard:

![wiring-small](https://github.com/user-attachments/assets/4ff414d1-38cf-4bec-a5a6-82c75dee381e)

Full Size [3](https://github.com/user-attachments/assets/3b3fa358-2b40-43c1-ba51-b71074b0d531)


If you need any help you can contact me on the [Hack Pack Discord](https://discord.gg/hackpack)

Enjoy!

[^1]: [SD Card Files/00/094 protongun_powerup short.mp3](https://www.gbfans.com/shop/amplified-sound-board/)
[^2]: [Star Wars Blaster](https://www.myinstants.com/en/instant/star-wars-blaster-42067/)
[^3]: [Blaster not violent](https://www.myinstants.com/en/instant/blaster-not-violent-bruh-97990//)
[^4]: [SD Card Files/00/045 overheat beeps 3.mp3](https://www.gbfans.com/shop/amplified-sound-board/)
[^5]: [TIE blaster cannon](https://www.101soundboards.com/sounds/24145051-tie-blaster-cannon)
[^6]: [R2-D2 Sounds: Star Wars 15](https://www.101soundboards.com/sounds/36614-15/)
[^7]: [R2-D2 Sounds: Star Wars 14](https://www.101soundboards.com/sounds/36613-14/)
[^8]: [R2-D2 Sounds: Star Wars 13](https://www.101soundboards.com/sounds/36612-13/)
