#include "CommonTypes.h"
#include "GPIOConfig.h"
#include "Display.h"

#ifdef TFT_DISPLAY
namespace DisplayInterface
{
    Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS_PIN, TFT_DC_PIN, TFT_MOSI_PIN, TFT_CLK_PIN, TFT_RST_PIN, TFT_MISO_PIN);
    void Display::init()
    {
        tft.begin();
        tft.drawRGBBitmap(0, 0, (uint16_t *)StartUp_Logo.data, StartUp_Logo.width, StartUp_Logo.height); //ISAAYU logo
        delay(1000);
        tft.drawRGBBitmap(0, 0, (uint16_t *)StartUp_entry.data, StartUp_entry.width, StartUp_entry.height); //ISAAYU entry
        delay(1000);
        tft.drawRGBBitmap(0, 0, (uint16_t *)StartUp_sensorBG.data, StartUp_sensorBG.width, StartUp_sensorBG.height); //ISAAYU sensor BG
    }

    void Display::process( void )
    {
        tft.setRotation(4);
        tft.setTextSize(2);
        tft.drawRGBBitmap(0, 0, (uint16_t *)StartUp_sensorBG.data, StartUp_sensorBG.width, StartUp_sensorBG.height); //ISAAYU sensor BG
                                                                                                                     //PH dsiplay
        tft.setCursor(10, 30);
        tft.setCursor(90, 30);
        tft.setTextColor(ILI9341_BLUE);
        tft.println(measuredParams.f_PHValue);
        tft.setCursor(40, 60);
        tft.setTextColor(ILI9341_BLACK);
        tft.print("L-H :");
        tft.setTextColor(ILI9341_BLUE);
        tft.println("5.5-6.5");

        //EC display
        tft.setCursor(10, 120);
        tft.setCursor(70, 140);
        tft.setTextColor(ILI9341_BLUE);
        tft.print(measuredParams.f_ECValue);
        tft.println(" PPM");
        tft.setCursor(40, 170);
        tft.setTextColor(ILI9341_BLACK);
        tft.print("L-H :");
        tft.setTextColor(ILI9341_BLUE);
        tft.println("900-1200");

        //TEMP display
        tft.setCursor(10, 220);
        tft.setCursor(60, 250);
        tft.setTextColor(ILI9341_BLUE);
        tft.print(measuredParams.f_TempValue);
        tft.println(" C");
    }
}
#endif