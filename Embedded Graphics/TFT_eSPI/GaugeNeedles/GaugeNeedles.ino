// Created by Bodmer 17/3/20 as an example to the TFT_eSPI library:
// https://github.com/Bodmer/TFT_eSPI

#define NEEDLE_LENGTH 25  // Visible length
#define NEEDLE_RADIUS 80  // Radius at tip

#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite needle = TFT_eSprite(&tft); // Sprite object for needle

// =======================================================================================
// Setup
// =======================================================================================
void setup()   {
  Serial.begin(115200); // Debug only

  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_NAVY);

  // Create the needle Sprite
  createNeedle();

  // Define where the needle pivot point is on the TFT
  tft.setPivot(tft.width()/2, tft.height()/2);  // Set the TFT pivot point

  // Full scale to sweep area (angle, delay_per_degree)
  plotNeedle(240, 1);

  // Reset needle position to 0
  plotNeedle(0, 1);
}

// =======================================================================================
// Loop
// =======================================================================================
void loop() {
  // Plot needle at random angle in range 0 to 240, speed 5ms per degree
  plotNeedle(random(241), 5);

  // Pause at new position
  delay(250);
}

// =======================================================================================
// Create the needle Sprite
// =======================================================================================
void createNeedle(void)
{
  needle.setColorDepth(8);
  needle.createSprite(11, NEEDLE_LENGTH);  // create the needle Sprite 11 pixels wide by 30 high

  needle.fillSprite(TFT_BLACK); // Fill with black

  // Define needle pivot point
  uint16_t piv_x = needle.width() / 2;   // pivot x in Sprite (middle)
  uint16_t piv_y = NEEDLE_RADIUS;        // pivot y in Sprite
  needle.setPivot(piv_x, piv_y);         // Set pivot point in this Sprite

  // Draw the red needle with a yellow tip
  // Keep needle tip 1 pixel inside dial circle to avoid leaving stray pixels
  needle.fillRect(piv_x - 1, 2, 3, NEEDLE_LENGTH, TFT_RED);
  needle.fillRect(piv_x - 1, 2, 3, 5, TFT_YELLOW);
}

// =======================================================================================
// Move the needle to a new position
// =======================================================================================
void plotNeedle(int angle, byte ms_delay)
{
  static int16_t old_angle = -120; // Starts at -120 degrees

  if (angle < 0) angle = 0; // Limit angle to emulate needle end stops
  if (angle > 240) angle = 240;

  angle -= 120; // Starts at -120 degrees

  // Move the needle until new angle reached
  while (angle != old_angle) {

    if (old_angle < angle) old_angle++;
    else old_angle--;

    // Draw the needle in the new postion
    needle.pushRotated(old_angle);

    // Slow needle down slightly as it approaches new postion
    if (abs(old_angle - angle) < 10) ms_delay += ms_delay / 5;

    // Wait before next update
    delay(ms_delay);
  }
}

// =======================================================================================