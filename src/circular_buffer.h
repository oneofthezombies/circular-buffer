#ifndef CIRCULAR_BUFFER_CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_CIRCULAR_BUFFER_H

#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>

#ifndef CIRCULAR_BUFFER_SIZE
#define CIRCULAR_BUFFER_SIZE 256
#endif

typedef struct {
  uint8_t buf[CIRCULAR_BUFFER_SIZE];
  _Atomic(uint8_t *) rd_ptr;
  _Atomic(uint8_t *) wr_ptr;
} circular_buffer_t;

typedef enum {
  CB_FALSE = 0,
  CB_TRUE = 1,
} cb_bool_t;

void cb_init(circular_buffer_t *const cb);

cb_bool_t cb_is_empty(circular_buffer_t *const cb);

cb_bool_t cb_is_full(circular_buffer_t *const cb);

int32_t cb_write(circular_buffer_t *const cb, const uint8_t *const src,
                 const size_t src_size);

int32_t cb_read(circular_buffer_t *const cb, uint8_t *const dst,
                const size_t dst_size);

#endif // CIRCULAR_BUFFER_CIRCULAR_BUFFER_H
