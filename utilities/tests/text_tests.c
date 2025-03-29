#include <unity.h>
#include "utilities/text.h"

TextContents text;

void setUp() {
  text = (TextContents){ 0 };
}

void tearDown() {
  DestroyTextContents(&text);
}
