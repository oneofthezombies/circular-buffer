#include "circular_buffer.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

const uint8_t *_cb_buf_limit(const circular_buffer_t *const cb) {
  assert(cb && "cb must not be null");
  return cb->buf + sizeof(cb->buf);
}

const uint8_t *_cb_ptr_advance(const circular_buffer_t *const cb,
                               const uint8_t *const ptr,
                               const int32_t advance) {
  assert(cb && "cb must not be null");
  assert(ptr && "ptr must not be null");
  assert(abs(advance) < sizeof(cb->buf) &&
         "size of advance must be less than size of buffer");

  const uint8_t *const buf_limit = _cb_buf_limit(cb);
  const uint8_t *advanced_ptr = ptr + advance;
  if (advanced_ptr >= buf_limit) {
    size_t overflow = advanced_ptr - buf_limit;
    advanced_ptr = cb->buf + overflow;
  } else if (advanced_ptr < cb->buf) {
    size_t underflow = cb->buf - advanced_ptr;
    advanced_ptr = buf_limit - 1 - underflow;
  }

  assert(cb->buf <= advanced_ptr && advanced_ptr < buf_limit &&
         "advanced ptr must be in range");
  return advanced_ptr;
}

void cb_init(circular_buffer_t *const cb) {
  memset(cb->buf, 0, sizeof(cb->buf));
  atomic_store(&cb->rd_ptr, cb->buf);
  atomic_store(&cb->wr_ptr, cb->buf);
}

cb_bool_t cb_is_empty(circular_buffer_t *const cb) {
  return cb->rd_ptr == cb->wr_ptr;
}

cb_bool_t cb_is_full(circular_buffer_t *const cb) {
  return _cb_ptr_advance(cb, atomic_load(&cb->wr_ptr), 1) == cb->rd_ptr;
}

int32_t cb_write(circular_buffer_t *const cb, const uint8_t *const src,
                 const size_t src_size) {
  uint8_t *wr_ptr = atomic_load(&cb->wr_ptr);
  const uint8_t *const rd_ptr = atomic_load(&cb->rd_ptr);
  const uint8_t *const buf_limit = _cb_buf_limit(cb);

  size_t total_write_size = 0;
  if (rd_ptr <= wr_ptr) {
    const size_t space_size = buf_limit - wr_ptr;
    if (space_size == 0) {
      return 0;
    }

    const size_t write_size = src_size < space_size ? src_size : space_size;
    memcpy(wr_ptr, src, write_size);
    wr_ptr = (uint8_t *)_cb_ptr_advance(cb, wr_ptr, write_size);
    total_write_size += write_size;
  }

  const size_t remain_src_size = src_size - total_write_size;
  if (remain_src_size > 0) {
    const size_t space_size = _cb_ptr_advance(cb, rd_ptr, -1) - wr_ptr;
    if (space_size > 0) {
      const size_t write_size =
          remain_src_size < space_size ? remain_src_size : space_size;
      memcpy(wr_ptr, src + total_write_size, write_size);
      wr_ptr = (uint8_t *)_cb_ptr_advance(cb, wr_ptr, write_size);
      total_write_size += write_size;
    }
  }

  atomic_store(&cb->wr_ptr, wr_ptr);
  return total_write_size;
}

int32_t cb_read(circular_buffer_t *const cb, uint8_t *const dst,
                const size_t dst_size) {
  const uint8_t *const wr_ptr = atomic_load(&cb->wr_ptr);
  uint8_t *rd_ptr = atomic_load(&cb->rd_ptr);
  const uint8_t *const buf_limit = _cb_buf_limit(cb);

  size_t total_read_size = 0;
  if (wr_ptr < rd_ptr) {
    const size_t data_size = buf_limit - rd_ptr;
    const size_t read_size = dst_size < data_size ? dst_size : data_size;
    memcpy(dst, rd_ptr, read_size);
    rd_ptr = (uint8_t *)_cb_ptr_advance(cb, rd_ptr, read_size);
    total_read_size += read_size;
  }

  const size_t remain_dst_size = dst_size - total_read_size;
  if (remain_dst_size > 0) {
    const size_t data_size = wr_ptr - rd_ptr;
    if (data_size > 0) {
      const size_t read_size =
          remain_dst_size < data_size ? remain_dst_size : data_size;
      memcpy(dst, rd_ptr + total_read_size, read_size);
      rd_ptr = (uint8_t *)_cb_ptr_advance(cb, rd_ptr, read_size);
      total_read_size += read_size;
    }
  }

  atomic_store(&cb->rd_ptr, rd_ptr);
  return total_read_size;
}
