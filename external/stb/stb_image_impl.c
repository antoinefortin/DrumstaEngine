/* Single translation unit that compiles the stb_image implementation.
   Everywhere else, just #include <stb_image.h> (declarations only). */
#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"
