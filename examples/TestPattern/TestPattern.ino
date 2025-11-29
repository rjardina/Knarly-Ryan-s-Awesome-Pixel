/**
Test pattern to see if order is set correctly, should be showing:
Red, Green, Blue, White then blank (or two blanks if strip has no white) 

by Ryan Jackass ~ KF5EKB
Released under the GNU GPL v2
Nov 28th 2025
**/

#include <KRAP.h>                                                               // Library for driving pixels
Krap Strip(21, 144, "GRB");                                                     // Makes Object: using Pin number, Number of LEDs and kind

uint32_t EffectCounter[2];                                                      // Place holders for effect

void setup() {
  pinMode(LED_BUILTIN, OUTPUT); }                                               // Output for Heartbeat

void TestPattern(void) {
  if (millis() > EffectCounter[0] + 100) {                                      // Checks to see when the last update delays the next for a 10th of a second
    if (EffectCounter[1] % 5 != 4) { Strip.PixelQueue[0][EffectCounter[1] % 5] = 255; } // Finds the remaider in counter when diving by 5 than turn on remaining pixel, or something
    Strip.Pomp();                                                               // Display pixels
    Strip.ShiftUp();                                                            // Moves all the pixels up by one
    EffectCounter[1]++;                                                         // Add one
    EffectCounter[0] = millis(); } }                                            // Mark for last updated

void loop() {
  digitalWrite(LED_BUILTIN, millis() >> 9 & 1);                                 // Heartbeat to see if code is cycling
  TestPattern(); }                                                              // Goto the test pattern
