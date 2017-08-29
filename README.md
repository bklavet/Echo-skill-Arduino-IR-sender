# Echo-skill-Arduino-IR-sender
alexa skill to have arduino yun send ir signal to a device

Using the IRLIB2 arduino library, I was able to capture raw ir commands for my Pyle PUCRC95 
robotic vacuum

Using nearbus on an arduino yun I was able to make an alexa skill that tells nearbus to excute the 
ir command on the arduino yun.

I can now have Alexa send the IR command to my robotic vacuum to turn it on or have it return to its base station
no port forwarding required!

This could be expanded for other Infared devices like TV's DVD players, anything with a remote
the IRLIB2 has alot of popular protocols, but if it isn't recongized you can save your own,
but with the limited space on the arduino yun, you will be limited to only a few ir commands
