#Skill to have the Amazon Echo control a robotic vacuum that recognizes infared signals.


Physical things that are needed to duplicate this project:
* Amazon Echo
* Arduino Yun https://www.amazon.com/Arduino-A000008-ATmega32u4-Microcontroller-Board/dp/B00MBCGWQQ/ref=sr_1_1?ie=UTF8&qid=1473784222&sr=8-1&keywords=arduino+yun
* IR LED, 470ohm resistor, 2n2222 npn transitor, if you need to capture the ir signal from a working remote you will also need an ir receiver.
	I found adafruits learning site helpful for capturing ir https://learn.adafruit.com/using-an-infrared-library/sending-ir-codes

	Ok... So how does this work?
This will help you build an alexa skill that will control an arduino yun using a rest service through Nearbus.
Nearbus is free and allows for the arduino to work without the need for port forwarding, and providing some security.
http://www.nearbus.net/ 

The flow of the skill is as follows...
# ALEXA---->AWS Lambda--->Nearbus--->Arduino Yun--->ir device

In this example we are going to have the Arduino Yun send an IR command to a robotic Vacuum PUCRC95, but this could be modified for multiple infared devices,
or a number of arduino projects.  It should be noted that Nearbus has several different downloads available 
for multiple arduino boards, but I have a yun... so that is what this documentation is going to reflect.

Something to note:
I had to modify the NearBusYun_v15.h it wouldn't compile so I changed WProgram.h to Arduino.h
My changes have been saved to this github, but if you want to go directly through nearbus.  be prepared to make the same change.

# For the Arduino Yun

If this is your first rodeo with the Arduino Yun, or Arduino in general, this should be a good project to do
But you will want the Arduino IDE loaded on a computer 
Here's a link to get the Arduino IDE https://www.arduino.cc/en/Main/Software
Once you have that loaded you will need to add the libraries from this github to the Arduino Libraries used by the Arduino IDE,
they have been provided on this github and can be found in the Arduino Yun Stuff folder.
FlexiTimer2
NearBusYun_v15
IRLib2
IRLibFreq
IRLibProtocols
IRLibRecv
IRLibRecvPCI

After all this has been done, hook your arduino yun to your computer with micro usb cord and get familiar with how to 
connect to your board with the IDE.  
If you need help: https://www.arduino.cc/en/Guide/ArduinoYun 

Open the pureclean_arduino_on-off sketch located in the Arduino Stuff/arduino yun folder.  It should load and be ready for you to edit with your information that you will soon have.

# NearBus setup
http://www.nearbus.net/ 
you will want to sign up and setup a login.
Select the New Device tab
you will see a number of parameters and values that need entry.  Not all of them are necessary here are the important ones
* Device Name = What ever you want i used "Arduino"

* Shared Secret= 8 digit char/number example "12345678" This will be used in the Arduino template sketch

* PIN = 4 digit pin I put one in, but I am not sure if it is needed or not

* Device Identifier = this will be provided and will be needed in the arduino sketch

* Unclick Configured as VCMU

click "Setup"

# Back to the Arduino Yun

So open the pureclean_arduino_on-off sketch
towards the top you will see a section titled Agent Configuration Parameters
 char deviceId []  = ""; enter in your Device Identifier you got from NearBus between the quotes with no spaces.
 char sharedSecret[] ="" enter in your shared secret you got from NearBus between the quotes with no spaces.
 
 Scroll down to the section that says Example 2 - Digital Output
 you will see a line that says client.get("http://nearbus.net/v1/api_transp......)
 edit the http request with you Device Identifier, your username you use to log into nearbus, and password you use to log on to nearbus.
 
 What is happening in the sketch is the Yun is waiting to receive a command to send the ir command, when it recieves this,
 it fires off the command. The Arduino Yun sends a reset command back to Nearbus
 so everything goes back to the way it was. 
 
 Load this sketch to the arduino and set up your led to digital pin 9
 connect the arduino Yun to your internet connect via Wifi or ethernet
 log into Nearbus and check your device list the state should be Green and say up, if it does try this.
 open a browser and enter http://nearbus.net/v1/api_transp/NB10xxxx?user=xxxxxxx&pass=xxxxxxx&method=POST&reg00=1
 Make sure you update it with your information and hit enter.  
 
 the led on the arduino should send the ir command.
 If it does, Great!  now we can build an alexa skill to control this with our voice!
 
# Alexa Skill Setup
I would download notepad++ to edit the index file
https://notepad-plus-plus.org/
the part will be using the contents of the src folder from this github
Open the index file with notpad++ and edit the path (line 53) to match your Device Identifier, users name and password.
Save!


Login to your developer.amazon.com account and go to the Alexa page.
https://developer.amazon.com/alexa
    Click "Get Started" under the "Alexa Skills Kit"
	Click "Add a new Skill" at the top right.
	Make sure "Custom Interaction Model" is selected
	Give it a Name
	Put in the "Invocation Name".  This is what you will use to "tell" Alexa. I used "pure clean"
	Click SAVE.  This will generate your ApplicationID and will be used when creating the Lambda Function

Next log into aws lambda https://aws.amazon.com/lambda/	
under the Compute section you will see Lambda, select it

Login to your "aws" Management console for Lambda and create a new Function
Once you create a new Lambda Function, do not choose a Blueprint (click Skip). 
The next thing we need to do is configure a trigger for our skill
* make sure that at the top left by your logon name it shows N. Virginia
* beside the lambda icon click on the box and select Alexa Skills Kit
* slect next
* Type in a function name
* for code entry type select upload zip
Place the index and AlexaSkill files in a zip file and choose to upload.
	Role should be: "lambda_basic_execution"
	Once you select the role, a new browser window will open. Click the "Allow" button in this new window.
	Click "Next" in the  original window to Review the new function.
	Click "Create Function"
	Select the "Alexa Skills Kit" item and Click "Submit"
	
Take note of the ARN value at the top right of the screen.  You will need this in the next step.

Go BACK to your developer.amazon.com account and go to the Alexa page.
    Click "Get Started" under the "Alexa Skills Kit"
    Click "Edit" next to the Skill you already created.
	Click Next
	Copy the Contents of the "IntentSchema" file from the "Custom Slot skill" folder above into the "IntentSchema" text area
	Click on "Add Slot Type" by the section named Custom Slot Type.  Enter Control_List for the "Enter Type" section.  Cut and paste the custom slot values.txt file in the section labled "EnterValues".  Click OK
	Copy the Contents of the "SampleUtterance" file from the "Custom Slot skill" folder above into the "SampleUtterance" text area
	Click "Next"
	If everything is good, you will now be able to put the ARN value you noted from the Lambda Function you previously created.
	Click on the "Lambda ARN..." bullet and paste that value in.
	Click "No" on the Account Linking for now.
	Click Next.
	If all is good, you will be able be done for now. There is no need to go to the "Publishing Information" 
	
	you should now be able to test the skill, if you used "garage door" for your invocation name, you could say:
	Alexa, tell pure clean to start vacuuming!

