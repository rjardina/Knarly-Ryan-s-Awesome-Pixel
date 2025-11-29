KRAP = Knaryl Ryan's Awesome Pixels<p>

by: Ryan Jardina ~ KF5EKB<p>

Released under the GNU GPL v2<p>

This library drives neopixel/WS2812 leds for the Raspberry Pico.<br>

Can handle 512 LEDs per each object made. & can do 8 strips.<p>

<hr> <!------------------------------------------------------------------------>

Version .1:<br>
This is just a prerelease version for my binary clock. more will follow!<p>

<hr> <!------------------------------------------------------------------------>

Public:<p>

<tt>Krap ObjectsName(PinNumber, NumberOfPixels, ColorOrder) ~ Constructor:</tt><br>
GPIO pin number to used<br>
Number of Pixels in strip<br>
Color order using capital letters of each color exsample: "RGBW"<p>

<tt>ObjectName.ThisPixel(PixelNumber, Red, Green, Blue, White)</tt><br>
Default arguments for colors is 0 / blanks pixel<br>
Blanks a single pixel or adds color value<p>

<tt>ObjectsName.DittoQueue(Red, Green, Blue, White)</tt></br>
Default arguments blanks whole memory for strip OR make it one color<p>

<tt>ObjectsName.Pomp(Backwards)</tt><br>
Output momory to strip.<br>
Backwards argument of 1 will flip the strips output<p>

<tt>ObjectsName.Flash(Red, Green, Blue, White)</tt><br>
Flashes the strip one solid color<br>
Default arguments is a soft white<p>

<tt>ObjectsName.Color(PixelNumber, ColorOnWheel, SizeOfWheel)</tt><br>
<p>

<tt>ObjectsName.RGB2RYB(PixelNumber)</tt><br>
When using the "Color" Function this take the values in memeory & makes those
RGB values from the color to Red, Yellow, Blue.<p>

<tt>ObjectsName.LivingPixel(PixelNumber)</tt><br>
Returns a 0 or 1 for Off of On<p>

<tt>ObjectsName.PixelCount</tt><br>
Returns the total number of pixels<p>

<tt>ObjectsName.PixelQueue[NumberOfPixels][?]</tt><br>
0 = RedPixel, 1 = GreenPixel, 2 = BluePixel, 3 = WhitePixel<p>
