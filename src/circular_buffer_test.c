#include <assert.h>

#define CIRCULAR_BUFFER_SIZE 128
#include "circular_buffer.h"

int main() {
  circular_buffer_t cb;
  cb_init(&cb);
  //   assert(cb_is_empty(&cb));
  //   assert(!cb_is_full(&cb));
  //   uint8_t src[] = {1, 2, 3};
  //   assert(cb_write(&cb, src, sizeof(src)) == 3);
  //   assert(!cb_is_empty(&cb));

  return 0;
}
