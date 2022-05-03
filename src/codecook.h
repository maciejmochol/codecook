/**
 * @libdoc Codecook documentation
 *
 * This is source code documentation for the
 * GNU SCADA project
 *
 * @defgroup Core
 * @defgroup CD_Tree
 *
 */

#ifndef __CODECOOK_H__
#define __CODECOOK_H__

class Codecook_System_Table;

enum ChannelsNumber { ZERO_CH, PROBE_CH, STANDARD_SET_CH } ; 
enum LockOrNot { LOCK, NO_LOCK };
extern Codecook_System_Table *cstable;

// From libio.h:

#ifndef NULL
# if defined __GNUG__ && \
    (__GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 8))
#  define NULL (__null)
# else
#  if !defined(__cplusplus)
#   define NULL ((void*)0)
#  else
#   define NULL (0)
#  endif
# endif
#endif

#ifndef __codecook_version__
#define __codecook_version__ "Codecook, version 0.60 (26.04.2000)"
#endif

#endif
