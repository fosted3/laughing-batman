#include <OctoWS2811.h>
#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>

#define LIGHTING 0
#define COLOR 1
#define RANDOM 2
#define PLASMA 3

//Color Temp to RGB lookup table generated from data available at http://www.tannerhelland.com/4435/convert-temperature-rgb-algorithm-code/
const unsigned char r_k[] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 254, 252, 249, 247, 245, 243, 240, 239, 237, 235, 233, 231, 230, 228, 227, 225, 224, 222, 221, 220, 218, 217, 216, 215, 214, 212, 211, 210, 209, 208, 207, 207, 206, 205, 204, 203, 202, 201, 201, 200, 199, 199, 198, 197, 196, 196, 195, 195, 194, 193, 193, 192, 192, 191, 191, 190, 190, 189, 189, 188, 188, 187, 187, 186, 186, 185, 185, 185, 184, 184, 183, 183, 183, 182, 182, 182, 181, 181, 181, 180, 180, 180, 179, 179, 179, 178, 178, 178, 178, 177, 177, 177, 177, 176, 176, 176, 175, 175, 175, 175, 175, 174, 174, 174, 174, 173, 173, 173, 173, 173, 172, 172, 172, 172, 172, 171, 171, 171, 171, 171, 170, 170, 170, 170, 170, 170, 169, 169, 169, 169, 169, 169, 169, 168, 168, 168, 168, 168, 168, 168, 167, 167, 167, 167, 167, 167, 167, 166, 166, 166, 166, 166, 166, 166, 166, 165, 165, 165, 165, 165, 165, 165, 165, 165, 164, 164, 164, 164, 164, 164, 164, 164, 164, 164, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 163, 162, 162, 162, 162, 162, 162, 162, 162, 162, 162, 162, 162, 162, 161, 161, 161, 161, 161, 161, 161, 161, 161, 161, 161, 161, 161, 161, 160, 160, 160, 160, 160, 160, 160, 160, 160, 160, 160, 160, 160, 160, 160, 159, 159, 159, 159, 159, 159, 159, 159, 159, 159, 159, 159, 159, 159, 159, 159, 159, 159, 158, 158, 158, 158, 158, 158, 158, 158, 158, 158, 158, 158, 158, 158, 158, 158, 158, 158, 158, 158, 158, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 157, 156, 156, 156, 156, 156, 156, 156, 156, 156, 156, 156, 156, 156, 156, 156, 156, 156, 156, 156, 156, 156, 156, 156, 156, 156, 156, 156, 156, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155, 155};
const unsigned char g_k[] = {56, 71, 83, 93, 101, 109, 115, 121, 126, 131, 137, 142, 147, 152, 157, 161, 165, 169, 173, 177, 180, 184, 187, 190, 193, 196, 199, 201, 204, 206, 209, 211, 213, 215, 217, 219, 221, 223, 225, 227, 228, 230, 232, 233, 235, 236, 238, 239, 240, 242, 243, 244, 245, 246, 248, 249, 249, 247, 246, 245, 243, 242, 241, 240, 239, 238, 237, 236, 235, 234, 233, 232, 231, 230, 230, 229, 228, 227, 227, 226, 225, 225, 224, 223, 223, 222, 221, 221, 220, 220, 219, 219, 218, 218, 217, 217, 216, 216, 216, 215, 215, 214, 214, 214, 213, 213, 212, 212, 212, 211, 211, 211, 210, 210, 210, 210, 209, 209, 209, 208, 208, 208, 208, 207, 207, 207, 207, 206, 206, 206, 206, 205, 205, 205, 205, 205, 204, 204, 204, 204, 204, 203, 203, 203, 203, 203, 202, 202, 202, 202, 202, 202, 201, 201, 201, 201, 201, 201, 201, 200, 200, 200, 200, 200, 200, 200, 199, 199, 199, 199, 199, 199, 199, 199, 198, 198, 198, 198, 198, 198, 198, 198, 198, 197, 197, 197, 197, 197, 197, 197, 197, 197, 197, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 195, 195, 195, 195, 195, 195, 195, 195, 195, 195, 195, 195, 195, 194, 194, 194, 194, 194, 194, 194, 194, 194, 194, 194, 194, 194, 194, 194, 193, 193, 193, 193, 193, 193, 193, 193, 193, 193, 193, 193, 193, 193, 193, 193, 193, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 191, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 190, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 189, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188, 188};
const unsigned char b_k[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 33, 44, 54, 63, 72, 79, 87, 94, 101, 107, 114, 120, 126, 132, 137, 143, 148, 153, 159, 163, 168, 173, 177, 182, 186, 190, 194, 198, 202, 206, 210, 213, 217, 220, 224, 227, 230, 233, 236, 239, 242, 245, 248, 251, 253, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
unsigned char gamma_lookup[256];

const int leds_per_pin = 120;
DMAMEM int display_memory[leds_per_pin * 6];
int drawing_memory[leds_per_pin * 6];
const int config = WS2811_GRB | WS2811_800kHz;
OctoWS2811 leds(leds_per_pin, display_memory, drawing_memory, config);
String serial_input = "";
boolean new_command = false;
unsigned int color_temp = 10000;
unsigned int rgb_r = 0;
unsigned int rgb_g = 0;
unsigned int rgb_b = 0;
unsigned int bright = 0;
float gam = 2.5;
char buf[10];
unsigned int mode = LIGHTING;

void setup()
{
  set_gamma(gam);
  leds.begin();
  Serial.begin(9600);
  serial_input.reserve(50);
}

void loop()
{
  while(Serial.available())
  {
    char input = (char) Serial.read();
    if (input == '\n' || input == '\r')
    {
      new_command = true;
    }
    else
    {
      serial_input += input;
    }
  }
  if (new_command)
  {
    Serial.print("Got: ");
    Serial.println(serial_input);
    new_command = false;
    if (serial_input.startsWith("lighting"))
    {
      unsigned int new_color_temp = serial_input.substring(9, serial_input.indexOf(' ', 9)).toInt();
      unsigned int new_brightness = serial_input.substring(serial_input.indexOf(' ', 9)).toInt();
      if (new_color_temp > 40000 || new_color_temp < 1000)
      {
        Serial.println("Invalid color temperature value");
      }
      else
      {
        color_temp = new_color_temp;
      }
      if (new_brightness > 100)
      {
        Serial.println("Invalid brightness value");
      }
      else
      {
        bright = new_brightness;
      }
      mode = LIGHTING;
    }
    else if (serial_input.startsWith("gamma"))
    {
      serial_input.substring(6).toCharArray(buf, 10);
      float new_gam = atof(buf);
      if (new_gam < 1.0f || new_gam > 5.0f)
      {
        Serial.println("Invalid gamma value");
      }
      else
      {
        gam = new_gam;
        set_gamma(gam);
      }
    }
    else if (serial_input.startsWith("random"))
    {
      unsigned int new_brightness = serial_input.substring(7).toInt();
      if (new_brightness > 100)
      {
        Serial.println("Invalid brightness value");
      }
      else
      {
        bright = new_brightness;
      }
      mode = RANDOM;
    }
    else if (serial_input.startsWith("plasma"))
    {
      unsigned int new_brightness = serial_input.substring(7).toInt();
      if (new_brightness > 100)
      {
        Serial.println("Invalid brightness value");
      }
      else
      {
        bright = new_brightness;
      }
      mode = PLASMA;
    }
    else if (serial_input.startsWith("rgb"))
    {
      unsigned int i1 = serial_input.indexOf(' ', 4);
      unsigned int i2 = serial_input.indexOf(' ', i1+1);
      unsigned int i3 = serial_input.indexOf(' ', i2+1);
      unsigned int new_r = serial_input.substring(4, i1).toInt();
      unsigned int new_g = serial_input.substring(i1, i2).toInt();
      unsigned int new_b = serial_input.substring(i2, i3).toInt();
      unsigned int new_brightness = serial_input.substring(i3).toInt();
      if (new_brightness > 100)
      {
        Serial.println("Invalid brightness value");
      }
      else
      {
        bright = new_brightness;
      }
      if (new_r > 255)
      {
        Serial.println("Invalid R value");
      }
      else
      {
        rgb_r = new_r;
      }
      if (new_g > 255)
      {
        Serial.println("Invalid G value");
      }
      else
      {
        rgb_g = new_g;
      }
      if (new_b > 255)
      {
        Serial.println("Invalid B value");
      }
      else
      {
        rgb_b = new_b;
      }
      mode = COLOR;
    }
   serial_input = "";
  }
  if (mode == LIGHTING)
  {
    set_color_temp(color_temp, bright);
  }
  else if (mode == COLOR)
  {
    set_color(rgb_r, rgb_g, rgb_b, bright);
  }
  else if (mode == RANDOM)
  {
    set_random(bright);
  }
  else if (mode == PLASMA)
  {
    plasma(bright);
  }
  if (mode != PLASMA) { delay(50); }
  while(leds.busy());
}

void set_color_temp(unsigned int k, unsigned int brightness)
{
  if (k > 999 && k < 40001 && brightness < 101)
  {
      k -= 1000;
      k /= 100;
      unsigned int corr = ((unsigned int) r_k[k]) * brightness;
      unsigned int corg = ((unsigned int) g_k[k]) * brightness;
      unsigned int corb = ((unsigned int) b_k[k]) * brightness;
      corr /= 100;
      corg /= 100;
      corb /= 100;
      corr = min(corr, 255);
      corg = min(corg, 255);
      corb = min(corb, 255);
      corr = gamma_lookup[corr];
      corg = gamma_lookup[corg];
      corb = gamma_lookup[corb];
      for (unsigned int i = 0; i < 8 * leds_per_pin; i++)
      {
        leds.setPixel(i, corr, corg, corb);
      }
  }
  leds.show();
}

void set_gamma(float g) //Generate a gamma correction lookup table (quanitization sucks)
{
  for (unsigned int i = 0; i < 256; i++)
  {
      gamma_lookup[i] = (unsigned char) (pow((float)i / (float)255, g) * 255 + 0.5);
  }
}

void set_color(unsigned int r, unsigned int g, unsigned int b, unsigned int brightness)
{
  r *= brightness;
  g *= brightness;
  b *= brightness;
  r /= 100;
  g /= 100;
  b /= 100;
  r = min(r, 255);
  g = min(g, 255);
  b = min(b, 255);
  r = gamma_lookup[r];
  g = gamma_lookup[g];
  b = gamma_lookup[b];
  for (unsigned int i = 0; i < 8 * leds_per_pin; i++)
  {
    leds.setPixel(i, r, g, b);
  }
  leds.show();
}

void set_random(unsigned int brightness)
{
  brightness = (unsigned int) (((float) brightness) * 2.55f);
  for (unsigned int i = 0; i < 8 * leds_per_pin; i++)
  {
    leds.setPixel(i, gamma_lookup[random(0, brightness)], gamma_lookup[random(0, brightness)], gamma_lookup[random(0, brightness)]);
    //leds.setPixel(i, random(0, brightness), random(0, brightness), random(0, brightness));
  }
  leds.show();
}

float dist(unsigned int x, unsigned int y)
{
  return sqrt((x-6)*(x-6)+(y-5)*(y-5));
}

void plasma(unsigned int brightness)
{
  int rp = 0;
  int gp = 0;
  int bp = 0;
  unsigned int i;
  for (float t = 0; t < 6.25; t+= 0.25)
  {
    for (unsigned int x = 0; x < 12; x++)
    {
      for (unsigned int y = 0; y < 10; y++)
      {
        rp = 64 * (-sin(x + t) + sin(y - t) - sin(x + y + t) + sin(dist(x, y) - t)) + 255;
        gp = 64 * (sin(x - t) - sin(y + t) + sin(x + y - t) - sin(dist(x, y) + t)) + 255;
        bp = 64 * (sin(x + t) + sin(y + t) + sin(x + y + t) + sin(dist(x, y) + t)) + 255;
        rp *= brightness;
        rp /= 100;
        gp *= brightness;
        gp /= 100;
        bp *= brightness;
        bp /= 100;
        rp = max(min(rp, 255), 0);
        gp = max(min(gp, 255), 0);
        bp = max(min(bp, 255), 0);
        i = x + (12 * y) + 240;
        leds.setPixel(i, gamma_lookup[rp], gamma_lookup[gp], gamma_lookup[bp]);
      }
    }
    while(leds.busy());
    leds.show();
  }
}
