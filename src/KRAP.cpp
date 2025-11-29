#include <KRAP.h>

/// @brief Constructor
Krap::Krap(uint8_t PinNumber, uint16_t NumberOfPixels, String RGBW_Order) {
  for (uint8_t i = 0; i < RGBW_Order.length(); i++) {                           // Cycles throw all the colors
    switch (RGBW_Order.charAt(i)) {                                             // Go to color case
      case 'R': Red =   8 * (RGBW_Order.length() - i); break;                   //
      case 'G': Green = 8 * (RGBW_Order.length() - i); break;                   // These calculates is for how many bits to shift to output to strip
      case 'B': Blue =  8 * (RGBW_Order.length() - i); break;                   // Each colors is 8-bits
      case 'W': White = 8 * (RGBW_Order.length() - i); break; } }               //
  pixelSm = (StripCount < 4 ? StripCount : StripCount - 4);                     //
  pixelPio = (StripCount < 4 ? pio0 : pio1);                                    //
  StripCount++;                                                                 // Add to Strip count
  uint offset = pio_add_program(pixelPio, &ws2812_program);                     //
  ws2812_program_init(pixelPio, pixelSm, offset, PinNumber, 800000, RGBW_Order.length() * 8); //
  PixelCount = NumberOfPixels;                                                  // Number of pixels on the strip
  HalfCount = PixelCount / 2;                                                   // Half number of pixels
  ArraysEnd = PixelCount - 1; }                                                 // Cause arrays start at zero

/// @brief Color 1 Pixel, only passing the number kills it
/// @param r, g, b, w: red, green, blue, white value (0-255)
void Krap::ThisPixel(uint16_t PixelNumber, uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  if (PixelNumber > ArraysEnd) { return; }                                      // Pixel number bigger than array? exit function
  PixelQueue[PixelNumber][0] = r;                                               // Red
  PixelQueue[PixelNumber][1] = g;                                               // Green
  PixelQueue[PixelNumber][2] = b;                                               // Blue
  PixelQueue[PixelNumber][3] = w; }                                             // White

/// @brief Fill all the pixels with same value
/// @brief Passing no arguments blanks/clears the led array
/// @param r, g, b, w: red, green, blue, white values (0-255)
void Krap::DittoQueue(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  for (uint16_t i = 0; i < PixelCount; i++) {                                   // Cycles through all pixels
    PixelQueue[i][0] = r;                                                       // Red is now argument 'r' for retard
    PixelQueue[i][1] = g;                                                       // Green is now argument 'g' for gay
    PixelQueue[i][2] = b;                                                       // Blue is now argument 'b' for bastard
    PixelQueue[i][3] = w; } }                                                   // White is now argument 'w' for whore

/// @brief Display all the pixels in queue
void Krap::Pomp(bool Backwards) {
  float Intensity = ((Brightness ? constrain(Brightness, 1, 100) : map(analogRead(A1), 0, 1023, 1, 100)) * .01); // if undefine use the POT else use the value for precentage, between .01 to 1
  for (uint16_t i = 0; i < PixelCount; i++) {                                   // Cycles through array of Pixels
    uint16_t PresentPixel = (Backwards ? ArraysEnd - i : i);                    // Current pixel starting from which side of array, top or bottom?
    pio_sm_put_blocking(pixelPio, pixelSm, uint32_t(                            // Output pixel to strip
      uint8_t(PixelQueue[PresentPixel][0] * Intensity) << Red |                 // This & next 3 lines shifts the color from the array into a 32 bit integer 
      uint8_t(PixelQueue[PresentPixel][1] * Intensity) << Green |               // read the last comment
      uint8_t(PixelQueue[PresentPixel][2] * Intensity) << Blue |                // read the last comment
      uint8_t(PixelQueue[PresentPixel][3] * Intensity) << White ) ); } }        // read the last comment, dummy!

/// @brief Turns the strip one solid color but doesn't touch the queue
void Krap::Flash(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  for (uint16_t i = 0; i < PixelCount; i++) {                                   // Cycles through array of Pixels
    pio_sm_put_blocking(pixelPio, pixelSm, uint32_t(r << Red | g << Green | b << Blue | w << White)); } } //

/// @brief recalculate the clock to match a new CPU clock
void Krap::recalculateClock(void) {                                             //
  int cycles_per_bit = ws2812_T1 + ws2812_T2 + ws2812_T3;                       //
  float div = clock_get_hz(clk_sys) / (800000.0f * cycles_per_bit);             //
  pio_sm_set_clkdiv(pixelPio, pixelSm, div); }                                  //

void Krap::Color(uint16_t PixelNumber, uint16_t ScaledColor, uint16_t Wavelength) {
  if (PixelNumber > ArraysEnd) { return; }                                      // Pixel number bigger than array? exit function
  for (uint8_t Phase = 0; Phase < 3; Phase++) {                                 // Cycles through all 3 colors
    uint8_t RGB = min(255, max(64 + 122 * asin(cos(2 * (Pi / Wavelength) * ScaledColor + (Phase * 1.3333) * Pi)), 0)); // The magic I did years ago
    PixelQueue[PixelNumber][Phase] |= RGB; } }                                  //

/// @brief If two things take the same pixel & are !NOT! the same color turn pixel white
bool Krap::Synergistic(uint16_t PixelNumber, uint16_t ScaledColor, uint16_t Wavelength) {
  if (PixelNumber > ArraysEnd) { return 0; }                                    // Pixel number bigger than array? exit function
  for (uint8_t Phase = 0; Phase < 3; Phase++) {                                 // Cycle through all 3 colors
    uint8_t RGB = min(255, max(64 + 122 * asin(cos(2 * (Pi / Wavelength) * ScaledColor + (Phase * 1.3333) * Pi)), 0)); //
    if (PixelQueue[PixelNumber][Phase] == 0 || PixelQueue[PixelNumber][Phase] == RGB) { PixelQueue[PixelNumber][Phase] = RGB; } // The magic I did years ago
    else { for (uint8_t i = 0; i < 4; i++) { PixelQueue[PixelNumber][i] = 255; } return 1; } } //
  return 0; }

void Krap::RGB2RYB(uint16_t PixelNumber) {
  if (PixelNumber > ArraysEnd) { return; }                                      // Pixel number bigger than array? exit function
  uint16_t RYB[3];                                                              // Start an array
  uint16_t Mixing = min(PixelQueue[PixelNumber][1], PixelQueue[PixelNumber][2]); // ????
  RYB[0] = PixelQueue[PixelNumber][0] + PixelQueue[PixelNumber][1] - Mixing;    // Took this math from a paper I read years ago
  RYB[1] = PixelQueue[PixelNumber][1] + (2 * Mixing);                           // it still works / does it's job
  RYB[2] = 2 * (PixelQueue[PixelNumber][2] - Mixing);                           // this is a random sentence, doubt anyone will ever read this
  float Blend = 255 / float(RYB[0] + RYB[1] + RYB[2]);                          // blah blah blah
  for (uint8_t i = 0; i < 3; i++) { PixelQueue[PixelNumber][i] = Blend * RYB[i]; } } // takes RYB that was converted & places it in pixel

uint16_t Krap::ScaleToQueue(float Zero2One) {
  return constrain(Zero2One * PixelCount, 0, ArraysEnd); }                      // Scales to strip lenght & keeps it on it too.

bool Krap::LivingPixel(uint16_t PixelNumber) {
  for (uint8_t i = 0; i < 4; i++) {                                             // Cycle through all sub pixels
    if (PixelQueue[PixelNumber][i]) { return 1; } }                             // Is sub pixel on then return true
  return 0; }                                                                   // Returns false if not on

void Krap::ShiftUp(void) {
  for (uint16_t i = PixelCount; i > 0; i--) {                                   // Shifts the array up by one
    for (uint8_t c = 0; c < 4; c++) { PixelQueue[i][c] = PixelQueue[i - 1][c]; } }  // Cycles through each color
  ThisPixel(0); }                                                               // Blanks first pixel

void Krap::ShiftDown(void) {
  for (uint16_t i = 0; i < PixelCount; i++) {                                   // Shifts the array down by one
    for (uint8_t c = 0; c < 4; c++) { PixelQueue[i][c] = PixelQueue[i + 1][c]; } }  // Cycles through each color
  ThisPixel(ArraysEnd); }                                                       // Blanks last pixel

void Krap::Fade(uint8_t Percentage) {
  for (uint16_t i = 0; i < PixelCount; i++) {                                   // Cycles through array
    for (uint8_t c = 0; c < 4; c++) {                                           // Cycles through each color
      PixelQueue[i][c] *= (100 - float(Percentage)) / 100; } } }                // Takes the current sub pixels & multples it by a fraction of 1


// --- Non class ---------------------------------------------------------------

uint8_t Brightness = 0;                                                         //
uint8_t StripCount = 0;                                                         // The number of object that have been defined or strips
