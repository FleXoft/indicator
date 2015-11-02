#include <Adafruit_NeoPixel.h>
#include <DigiCDC.h>


Adafruit_NeoPixel strip = Adafruit_NeoPixel(16,1,NEO_GRB | NEO_KHZ800);
# define POS_NONE (255)
unsigned char pos = POS_NONE;
unsigned char colorIndex = 0;
unsigned char color[3];
bool show = false;
bool posReset = true;

# define MOD_IDLE (0)
# define MOD_POS (1)
# define MOD_DATA (2)
unsigned char mod = MOD_IDLE;

 
void setup() {
  
  SerialUSB.begin();
  strip.begin();
  strip.setBrightness(30);
  
} // setup() 


void loop() {

  if (!SerialUSB.available()) {
    if (!show) return;
    //SerialUSB.write("ok\n");
    strip.show();
    show = false;
    return;
  }
    
  unsigned char c = SerialUSB.read();
  SerialUSB.write(".");
    
  switch (c) {

    case '+': {
      mod = MOD_POS;
      pos = POS_NONE;
      posReset = false;
    } break;

    case ':': {
      mod = MOD_DATA;
      if (posReset) pos = 0;
      colorIndex = 0;
    } break;
    
    case ';': {
      mod = MOD_IDLE;
      if (pos > 0) show = true;
      if (pos == POS_NONE) show = false;
      pos = POS_NONE;
      colorIndex = 0;  
      posReset = true;      
    } break;
    
  } // switch char

  switch (mod) {

    case MOD_DATA: {
      if (pos == POS_NONE) return;
      
      bool isDigit = false;    
      if (('0' <= c) && (c <= '9')) {
        isDigit = true;
        c = c - '0';
      }
      if (('a' <= c) && (c <= 'f')) {
        isDigit = true;
        c = c - ('a' - 10);
      }
      if (('A' <= c) && (c <= 'F')) {
        isDigit = true;
        c = c - ('A' - 10);
      }
      if (!isDigit) return;
    
      color[colorIndex++] = c | (c << 4);
      if (colorIndex < 3) return;
    
      strip.setPixelColor(pos,color[0],color[1],color[2]);
      colorIndex = 0;
      pos++;

    } break;

    case MOD_POS: {
      
      if ((c < '0') || ('9' < c)) return;
      if (pos == POS_NONE) pos = 0;

      pos = 10 * pos;
      pos += (c - '0');
            
    } break;
    
  } // switch mod
  
} // loop()
