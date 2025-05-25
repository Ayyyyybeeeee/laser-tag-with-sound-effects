# Crunchlabs Hack Pack Laser Tag

This is a hack (or a mod) for the [Crunchlabs](https://www.crunchlabs.com/) Hack Pack Build #5 [Laser Tag](https://www.crunchlabs.com/products/laser-tag) kit.

This hack adds sound effects using [DFR1173](https://www.dfrobot.com/product-2862.html) - a startup sound, a shoot sound (one for each team), a sound when you are 'hit' (one for each team) and an alarm sound if you try and shoot when in timeout.  I’m using the proton pack startup sound from ghostbusters, and various other MP3 files.

I used the [DFR1173](https://www.dfrobot.com/product-2862.html) sound module from DFROBOT, possibly the least documented arduino module ever.  Most of the time on this project was dedicated to making this thing work.

The sound files I used are in the ZIP file.  I also found it helpful to format the DFR1173 first using windows format command from DOS prompt.  It's really important that the bit rate does not exceed 192, and the filenames are in a numbered (two digit) sub-folder, and the filenames begin with a three digit number and the total filename length does not exceed 12 bytes.  You load the sound files using a special USB cable that comes with the DFR1173 (make sure the arrow on the cable is pointing to the 5V pin).

I simply taped the DFR1173 on to the front of the battery with electrical tape.  An upgrade would be to 3D print a proper holder for this, or even use heatshrink tubing.  I've used 3 long (40cm) dupont wires to take the 5v/gnd and signal from the crunchlabs breadboard to the DFR1173 5v, gnd and Rx pins.

My code includes a couple of 'fixes' to the original code:
1. in sendIR_Pulse() sending 3 pulses seems like overkill, and can result in dead time in gameplay where two people can shoot each other and not get any hits.  This is reduced to a single pulse with IrSender.sendNEC()
2. in markHit() the servos are moved back when 80% of the time is up, but this results in dead time in gameplay while your glasses are clear, but you cannot shoot.  I've reduced it from 20% to 2%.

You can see a 'diff' to the original with [commit 7469543](https://github.com/Ayyyyybeeeee/laser-tag-with-sound-effects/commit/7469543)

To implement this hack:
- [ ] Assemble Hack Pack Build #5 [Laser Tag](https://www.crunchlabs.com/products/laser-tag) kit.
- [ ] One [DFR1173](https://www.dfrobot.com/product-2862.html) formatted and loaded with [contents of ZIP](../../raw/refs/heads/main/DFR1173-with-die-sounds.zip)
- [ ] 3 long (40cm) dupont wires
- [ ] Arduino IDE with the original sketch, compiling and uploading (note: you need to remove the Arduino Nano from the gun)
- [ ] Copy the [new sketch](../main/sketch_mar28a.ino) into the Ardunio IDE, compile and upload
- [ ] Connect dupont wires to the 5v, gnd and Rx pins on the DFR1173
- [ ] The other end of the dupont wires go to the crunchlabs breadboard pin 2, and anywhere on the red (5v) and black gnd) rails
- [ ] Turn it on and test it!

If you need any help you can contact me on the [Hack Pack Discord](https://discord.gg/hackpack)

Enjoy!

