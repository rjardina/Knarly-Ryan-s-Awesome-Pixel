#ifndef KRAP_H
#define KRAP_H

#include "Arduino.h"
#include "ws2812.pio.h"

#define Pi 3.14159
#define Tau 6.28318

class Krap {
public:
  /// @brief Constructor
  Krap(uint8_t PinNumber, uint16_t NumberOfPixels=144, String RGBW_Order="GRB");

  /// @brief Color 1 Pixel, only passing the pixel number kills it
  /// @param PixelNumber, if you shoot over the array it will not do anything, for memory safety
  /// @param r, g, b, w: red, green, blue, white value (0-255)
  void ThisPixel(uint16_t PixelNumber, uint8_t r=0, uint8_t g=0, uint8_t b=0, uint8_t w=0);

  /// @brief Fill all the pixels with same value
  /// @brief Passing no arguments blanks/clears the led array
  /// @param r, g, b, w: red, green, blue, white values (0-255)
  void DittoQueue(uint8_t r=0, uint8_t g=0, uint8_t b=0, uint8_t w=0);

  /// @brief Display all the pixels in buffer
  /// @param Backwards: Display the array backwards
  void Pomp(bool Backwards=0);

  /// @ brief Turns the strip one solid single color but doesn't touch the queue
  /// @param r, g, b, w: red, green, blue, white values (0-255)
  void Flash(uint8_t r=32, uint8_t g=32, uint8_t b=32, uint8_t w=32);

  /// @brief recalculate the clock to match a new CPU clock
  void recalculateClock(void);

  /// @brief Queue that holds the color for each pixel
  uint8_t PixelQueue[512][4];

  /// @brief PixelCount: Total Number of Pixels
  uint16_t PixelCount;

  /// @brief HalfCount: Half Number Of Pixels
  uint16_t HalfCount;

  /// @brief Takes a scaled color value & make a pixel that color
  /// @param PixelNumber, ScaledColor, Wavelength (or Scale)
  void Color(uint16_t PixelNumber, uint16_t ScaledColor, uint16_t Wavelength=255);

  /// @brief Takes a scaled color value & make a pixel that color
  /// @brief If Pixel is already lit turns the LED white
  /// @param PixelNumber, ColorValue, Wavelength (or Scale)
  bool Synergistic(uint16_t PixelNumber, uint16_t ScaledColor, uint16_t Wavelength=255);

  /// @brief Converts a pixel from RGB color wheel to RYB color wheel
  void RGB2RYB(uint16_t PixelNumber);

  /// @brief Takes a number between 0 to 1 & scales it to the strip
  uint16_t ScaleToQueue(float Zero2One);

  /// @brief Is the pixel on in the queue
  bool LivingPixel(uint16_t PixelNumber);

  /// @brief Shifts all pixels in queue up by one
  void ShiftUp(void);

  /// @brief Shifts all pixels in queue down by one
  void ShiftDown(void);

  /// @brief Fade all pixels
  void Fade(uint8_t Percentage);

private:
  // pio - 0 or 1
  PIO pixelPio;

  // calculated program offset in memory
  uint pixelOffset;

  // pio state machine to use
  uint pixelSm;

  // Cause arrays start at zero
  uint16_t ArraysEnd;

  // Contains the position of Red in the addressable LEDs
  uint8_t Red;

  // Contains the position of Green in the addressable LEDs
  uint8_t Green;

  // Contains the position of Blue in the addressable LEDs
  uint8_t Blue;

  // Contains the position & if present of White in the addressable LEDs
  uint8_t White;

};

extern uint8_t Brightness;                                                      // 

extern uint8_t StripCount;                                                      // Number of objects made or strip number of strips

#endif // KRAP_H
