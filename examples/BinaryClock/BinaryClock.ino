/** 
Binary Clock for the Pico using WS2812 LED strip & DS1307 real time clock.
Clock should auto resize to any size strip
Note: untested on the DS3231 might work?

by Ryan Jardina ~ KF5EKB
Released under the GNU GPL v2
**/

#include <KRAP.h>                                                               // Library for driving the pixels
Krap Strip(21, 144, "GRB");                                                     // Creates Object: PinNumber, NumberOfPixels, "Color Order"
#include <Jab.h>                                                                // Library for buttons
Jab Button(INPUT_PULLUP, 2, 3, 4, 5);                                           // Creates object: Type of Input, PinNumber1, PinNumber2, PinNumber3, PinNumber4
#include <RTK.h>                                                                // Library for RTC
RTK Clock;                                                                      // Creates Clock Object
#include <EEPROM.h>                                                             // Library to store variables after power cycle

const uint8_t DefaultOption[13] = {0, 24, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 3};     // Default option values
uint8_t Option[13] = {0, 24, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 3};                  // Option values
uint8_t & BCD = Option[0];                                                      // Use Binary Coded Decimal
uint8_t & BitArea = Option[1];                                                  // How many pixels in each bit
uint8_t & BitSpace = Option[2];                                                 // How many pixels spaces between bits
uint8_t & ChromaticTime = Option[3];                                            // Change color based on time
uint8_t & EndCaps = Option[4];                                                  // Want spacing on each end of strip
uint8_t & FixedNibbles = Option[5];                                             // Give each number a fix amount of nubbles or 0 for dynamic
uint8_t & FlipBits = Option[6];                                                 // Flips bits order
uint8_t & FlipStrip = Option[7];                                                // Flip all pixels on strip ?
uint8_t & Invert = Option[8];                                                   // Invert the bits from color to white to indicate on
uint8_t & NoBinary = Option[9];                                                 // Only Chromatic time
uint8_t & Rotation = Option[10];                                                // Reverse the color wheel
uint8_t & RYB = Option[11];                                                     // Covert RGB to RYB
uint8_t & TimeDepth = Option[12];                                               // How much time to show (Sec/Min/Hr/Wd/Da/Mo/Yr)
uint8_t ColorWheel[] = {60, 60, 12, 7, 31, 12, 10};                             // Adjust the Chromatic time

uint8_t NeededBits(uint8_t TheNumber) {
  for (int8_t i = 7; i >= 0; i--) {                                             // Cycles through bits starting at MSB
    if (TheNumber >> i & 1) { return FixedNibbles ? (i < 4 ? 4 : 8) : ++i; } }  // Finds MSB that is on returns that or whole nibble(s)
  return 0; }                                                                   // Shouldn't need this, also will not compile without it

uint16_t NumberSize(uint8_t Total) {
  uint8_t BitSpace = 0;                                                         // Place holder
  for (uint8_t i = 0; i < Total; i++) {                                         // Cycles throw array to find
    BitSpace += NeededBits(Clock.MaxDaTi[!BCD][i]); }                           // How many bits will be used
  return BitSpace * BitArea; }                                                  // Return the pixel space of the bits

void StoreOptions(bool Defaults=0) {
  for (uint8_t i = 0; i < sizeof(Option); i++) {                                // Cycles through all options
    EEPROM.write(i, (Defaults ? Option[i] = DefaultOption[i] : Option[i])); }   // Write Defaults or Current
  EEPROM.commit(); }                                                            // Stores them

void setup(void) {
  pinMode(LED_BUILTIN, OUTPUT);                                                 // Output for Heartbeat
  Wire.setSDA(0); Wire.setSCL(1); Wire.begin();                                 // Set Data & Clock Pins then start I2C/TWI
  EEPROM.begin(sizeof(Option));                                                 // Starts EEPROM + sets size
  if (!Clock.TimeCycling()) { StoreOptions(1); }                                // Checks to see if clock is running & if not starts it then set default options
  else {                                                                        // Was the clock running
    for (uint8_t i = 0; i < sizeof(Option); i++) {                              // Cycle through options
      Option[i] = EEPROM.read(i); } } }                                         // Restores them from memory backup

void loop(void) {
  digitalWrite(LED_BUILTIN, millis() >> 9 & 1);                                 // Heartbeat to see if code is cycling
  Clock.GetTime();                                                              // Puts time in pico's memory
  //                         >------ Remove space used by time ------<   >--- Divid by needed spaces ---<
  uint8_t Interspace = float(Strip.PixelCount - NumberSize(TimeDepth)) / (TimeDepth + (EndCaps ? 1 : -1)); // Space between digits
  if (Interspace < BitArea && BitArea >= 1) { BitArea--; }                      // Adjust bits to be smaller than space between digits (for cosmetics) also gives cool effect!
  for (uint8_t Digit = 0; Digit < TimeDepth; Digit++) {                         // Cycles through all numbers
    for (uint8_t CurrentBit = 0; CurrentBit < NeededBits(Clock.MaxDaTi[!BCD][Digit]); CurrentBit++) { // Loops through number of bits in each number
      for (uint8_t BitPart = 0; BitPart < BitArea; BitPart++) {                 // Builds bit one pixel at a time
        //                     >--empty space between digits---<   >--digit space--<   >---construct present bits---<
        uint16_t CurrentPixel = Interspace * (Digit + EndCaps) + NumberSize(Digit) + CurrentBit * BitArea + BitPart;
        if (BitPart < BitSpace) { }                                             // Makes a space between bits
        else if (((Clock.DaTi[!BCD][Digit] >> (FlipBits ? (NeededBits(Clock.MaxDaTi[!BCD][Digit]) - CurrentBit - 1) : CurrentBit) & 1) ^ Invert) || NoBinary) { // Color Bit if on & not inverted
          if (ChromaticTime) { Strip.Color(CurrentPixel, abs((Rotation ? Clock.MaxDaTi[1][Digit] : 0) - Clock.DaTi[1][Digit]), ColorWheel[Digit]); } // Use Chromatic time in bits
            else { Strip.Color(CurrentPixel, Digit, TimeDepth); }               // Or each digit is one color
          if (RYB) { Strip.RGB2RYB(CurrentPixel); } }                           // Convert RGB to RYB
        else { Strip.ThisPixel(CurrentPixel, 32, 32, 32, 32); } } } }           // Bit is white if off or Inverted
  Strip.Pomp(FlipStrip);                                                        // Displays LED / time
  Strip.DittoQueue();                                                           // Clears led memory

  if (Button.UserInput()) {                                                     // Checks to see if buttons were pressed
    bool TimeUpdated = 0;                                                       // Marker to set if time was updated
    switch (Button.LastInput) {                                                 // Check user's input from buttons than goes to that case
      default: case 0x0:                                                break;  // Do nothing
      case 0x1 ... 0x7: // 1 = Seconds, 2 = Minutes, 3 = Hours, 4 = Weeks, 5 = Date, 6 = Month, 7 = Year
        if (++Clock.DaTi[1][Button.LastInput - 1] >= Clock.MaxDaTi[1][Button.LastInput - 1]) { // Add one to time slot & compare it to the max
          Clock.DaTi[1][Button.LastInput - 1] = Clock.MinDaTi[Button.LastInput - 1]; }         // if it was bigger than Max make it the Min value
          TimeUpdated = 1;                                              break;  // Update marker
      case 0x8: if (++TimeDepth > 7) { TimeDepth = 3; } BitArea = 24; BitSpace = 1; break; // Show how much time
      case 0x9: BitArea++;                                              break;  // Make bits bigger
      case 0xA: ChromaticTime = !ChromaticTime;                         break;  // Chromatic time
      case 0xB: FixedNibbles = !FixedNibbles;                           break;  // Make each digit use a full nibble (4-bits) or dynamic
      case 0xC: EndCaps = !EndCaps; BitArea = 24; BitSpace = 1;         break;  // Add or remove end spacing
      case 0xD: FlipStrip = !FlipStrip;                                 break;  // Flip the whole strip
      case 0xE: Rotation = !Rotation;                                   break;  // Reverse the color wheel R?B to RB? ~ ? Y or G
      case 0xF: Brightness < 100 ? Brightness += 25 : Brightness = 0;   break;  // Brightness is 0 than use POT or use number
      case 0x101 ... 0x107: // 101 = Seconds, 102 = Minutes, 103 = Hours, 104 = Weeks, 105 = Date, 106 = Month, 107 = Year
        if (Clock.DaTi[1][Button.LastInput - 0x101]-- <= Clock.MinDaTi[Button.LastInput - 0x101]) { // Subtracts 1 from current time slot & compares it to smaller than min
          Clock.DaTi[1][Button.LastInput - 0x101] = Clock.MaxDaTi[1][Button.LastInput - 0x101]; }   // if smaller make it max value
          TimeUpdated = 1;                                              break;  // Update marker
      case 0x108: BCD = !BCD;                                           break;  // Use real binary or binary coded decimal
      case 0x109: BitArea--; if (!BitArea) { BitArea++; BitSpace = 1; } break;  // Make bits smaller
      case 0x10A: RYB = !RYB;                                           break;  // Change color wheel from RGB to RYB
      case 0x10B: Invert = !Invert;                                     break;  // Invert bits
      case 0x10C: if (++BitSpace >= BitArea) { BitSpace = 0; }          break;  // Space between bits
      case 0x10D: FlipBits = !FlipBits;                                 break;  // Flips bit order
      case 0x10E: NoBinary = !NoBinary;                                 break;  // This is a stupid option
      case 0x10F: StoreOptions(1); Clock.SetTime(1); goto SKIP; }               // Resets everything! & goto is to skip writing to memory twice in a row.
    TimeUpdated ? Clock.SetTime() : StoreOptions();                             // Writes changes
SKIP: Strip.Flash(); } }                                                        // Flash strip on input press
