#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifdef _NDEDUG
#define ph_debug(M, ...)
#else
#define ph_debug(M, ...) fprintf(stderr, "DEBUG %s:%d: " M "\n", \
  __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#define ph_clean_errno() (errno == 0 ? "None" :strerror(errno))

#define ph_log_err(M, ...) fprintf(stderr, "[ERROR] (%s:%d: errno:%s) " M "\n", \
  __FILE__, __LINE__, ph_clean_errno(), ##__VA_ARGS__)

#define ph_log_warn(M, ...) fprintf(stderr, "[WARN] (%s:%d: errno:%s) " M "\n", \
  __FILE__, __LINE__, ph_clean_errno(), ##__VA_ARGS__)

#define ph_log_info(M, ...) fprintf(stderr, "[INFO] (%s:%d) " M "\n", \
  __FILE__, __LINE__, ##__VA_ARGS__)

#define ph_check(A, M, ...) if (!(A)) { \
  ph_log_err(M , ##__VA_ARGS__); \
  errno = 0; \
  goto error;\
}

#define ph_sentinel(M, ...) { \
  ph_log_err(M, ##__VA_ARGS__); \
  errno = 0; \
  goto error; \
}

#define ph_check_mem(A) ph_check((A), "Out of memory.")

#define ph_check_debug(A, M, ...) if (!(A)) { \
  ph_debug(M, ##__VA_ARGS__); \
  errno = 0; \
  goto error; \
}


#endif