#ifndef _INTEL_DRIVER_H_
#define _INTEL_DRIVER_H_

#include <stddef.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>

#include <drm.h>
#include <i915_drm.h>
#include <intel_bufmgr.h>

#include <va/va_backend.h>
#include "va_backend_compat.h"

#include "intel_compiler.h"

#define BATCH_SIZE      0x80000
#define BATCH_RESERVED  0x10

#define CMD_MI                                  (0x0 << 29)
#define CMD_2D                                  (0x2 << 29)
#define CMD_3D                                  (0x3 << 29)

#define MI_NOOP                                 (CMD_MI | 0)

#define MI_BATCH_BUFFER_END                     (CMD_MI | (0xA << 23))
#define MI_BATCH_BUFFER_START                   (CMD_MI | (0x31 << 23))

#define MI_FLUSH                                (CMD_MI | (0x4 << 23))
#define   MI_FLUSH_STATE_INSTRUCTION_CACHE_INVALIDATE   (0x1 << 0)

#define MI_FLUSH_DW                             (CMD_MI | (0x26 << 23) | 0x2)
#define MI_FLUSH_DW2                            (CMD_MI | (0x26 << 23) | 0x3)
#define   MI_FLUSH_DW_VIDEO_PIPELINE_CACHE_INVALIDATE   (0x1 << 7)
#define   MI_FLUSH_DW_NOWRITE                           (0 << 14)
#define   MI_FLUSH_DW_WRITE_QWORD                       (1 << 14)
#define   MI_FLUSH_DW_WRITE_TIME                        (3 << 14)

#define MI_STORE_DATA_IMM                       (CMD_MI | (0x20 << 23))

#define MI_STORE_REGISTER_MEM                   (CMD_MI | (0x24 << 23))

#define MI_LOAD_REGISTER_IMM                    (CMD_MI | (0x22 << 23))

#define MI_LOAD_REGISTER_MEM                    (CMD_MI | (0x29 << 23))

#define MI_LOAD_REGISTER_REG                    (CMD_MI | (0x2A << 23))

#define MI_MATH                                 (CMD_MI | (0x1A << 23))

#define MI_CONDITIONAL_BATCH_BUFFER_END         (CMD_MI | (0x36 << 23))
#define   MI_COMPARE_MASK_MODE_ENANBLED                 (1 << 19)

#define MI_COPY_MEM_MEM                         (CMD_MI | (0x2E << 23))

#define XY_COLOR_BLT_CMD                        (CMD_2D | (0x50 << 22) | 0x04)
#define XY_COLOR_BLT_WRITE_ALPHA                (1 << 21)
#define XY_COLOR_BLT_WRITE_RGB                  (1 << 20)
#define XY_COLOR_BLT_DST_TILED                  (1 << 11)

#define GEN8_XY_COLOR_BLT_CMD                   (CMD_2D | (0x50 << 22) | 0x05)

/* BR13 */
#define BR13_8                                  (0x0 << 24)
#define BR13_565                                (0x1 << 24)
#define BR13_1555                               (0x2 << 24)
#define BR13_8888                               (0x3 << 24)

#define CMD_PIPE_CONTROL                        (CMD_3D | (3 << 27) | (2 << 24) | (0 << 16))
#define CMD_PIPE_CONTROL_CS_STALL               (1 << 20)
#define CMD_PIPE_CONTROL_NOWRITE                (0 << 14)
#define CMD_PIPE_CONTROL_WRITE_QWORD            (1 << 14)
#define CMD_PIPE_CONTROL_WRITE_DEPTH            (2 << 14)
#define CMD_PIPE_CONTROL_WRITE_TIME             (3 << 14)
#define CMD_PIPE_CONTROL_DEPTH_STALL            (1 << 13)
#define CMD_PIPE_CONTROL_WC_FLUSH               (1 << 12)
#define CMD_PIPE_CONTROL_IS_FLUSH               (1 << 11)
#define CMD_PIPE_CONTROL_TC_FLUSH               (1 << 10)
#define CMD_PIPE_CONTROL_NOTIFY_ENABLE          (1 << 8)
#define CMD_PIPE_CONTROL_FLUSH_ENABLE           (1 << 7)
#define CMD_PIPE_CONTROL_DC_FLUSH               (1 << 5)
#define CMD_PIPE_CONTROL_GLOBAL_GTT             (1 << 2)
#define CMD_PIPE_CONTROL_LOCAL_PGTT             (0 << 2)
#define CMD_PIPE_CONTROL_STALL_AT_SCOREBOARD    (1 << 1)
#define CMD_PIPE_CONTROL_DEPTH_CACHE_FLUSH      (1 << 0)

#define CMD_PIPE_CONTROL_GLOBAL_GTT_GEN8        (1 << 24)
#define CMD_PIPE_CONTROL_LOCAL_PGTT_GEN8        (0 << 24)
#define CMD_PIPE_CONTROL_VFC_INVALIDATION_GEN8  (1 << 4)
#define CMD_PIPE_CONTROL_CC_INVALIDATION_GEN8   (1 << 3)
#define CMD_PIPE_CONTROL_SC_INVALIDATION_GEN8   (1 << 2)

struct intel_batchbuffer;

#define ALIGN(i, n)    (((i) + (n) - 1) & ~((n) - 1))
#define IS_ALIGNED(i, n) (((i) & ((n)-1)) == 0)
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define ARRAY_ELEMS(a) (sizeof(a) / sizeof((a)[0]))
#define CLAMP(min, max, a) ((a) < (min) ? (min) : ((a) > (max) ? (max) : (a)))

#define ALIGN_FLOOR(i, n) ((i) & ~((n) - 1))

#define Bool int
#define True 1
#define False 0

extern uint32_t g_intel_debug_option_flags;
#define VA_INTEL_DEBUG_OPTION_ASSERT    (1 << 0)
#define VA_INTEL_DEBUG_OPTION_BENCH     (1 << 1)
#define VA_INTEL_DEBUG_OPTION_DUMP_AUB  (1 << 2)
#define VA_INTEL_DEBUG_VERBOSE          (1 << 3)

#define ASSERT_RET(value, fail_ret) do {    \
		if (!(value)) {                     \
			if (g_intel_debug_option_flags & VA_INTEL_DEBUG_OPTION_ASSERT)       \
				assert(value);              \
			return fail_ret;                \
		}                                   \
	} while (0)

#define ASSERT_RET_MUTEX(value, mutex, fail_ret) do {    \
		if (!(value)) {                     \
			if (g_intel_debug_option_flags & VA_INTEL_DEBUG_OPTION_ASSERT)       \
				assert(value);              \
			_i965UnlockMutex(mutex);        \
			return fail_ret;                \
		}                                   \
	} while (0)

#define SET_BLOCKED_SIGSET()   do {     \
		sigset_t bl_mask;               \
		sigfillset(&bl_mask);           \
		sigdelset(&bl_mask, SIGFPE);    \
		sigdelset(&bl_mask, SIGILL);    \
		sigdelset(&bl_mask, SIGSEGV);   \
		sigdelset(&bl_mask, SIGBUS);    \
		sigdelset(&bl_mask, SIGKILL);   \
		pthread_sigmask(SIG_SETMASK, &bl_mask, &intel->sa_mask); \
	} while (0)

#define RESTORE_BLOCKED_SIGSET() do {    \
		pthread_sigmask(SIG_SETMASK, &intel->sa_mask, NULL); \
	} while (0)

#define PPTHREAD_MUTEX_LOCK() do {             \
		SET_BLOCKED_SIGSET();                  \
		pthread_mutex_lock(&intel->ctxmutex);       \
	} while (0)

#define PPTHREAD_MUTEX_UNLOCK() do {           \
		pthread_mutex_unlock(&intel->ctxmutex);     \
		RESTORE_BLOCKED_SIGSET();              \
	} while (0)

#define WARN_ONCE(...) do {                     \
		static int g_once = 1;                  \
		if (g_once) {                           \
			g_once = 0;                         \
			fprintf(stderr, "WARNING: " __VA_ARGS__);    \
		}                                       \
	} while (0)

struct intel_device_info {
	int gen;
	int gt;

	unsigned int urb_size;
	unsigned int max_wm_threads;

/**
 * Low-Power Intel hardware has a known defect on this driver
 * where low-resolution encoding may produce duplicate frames
 * at a specified quality and bitrate.
 * 
 * Use an alternate lookup table for H.264 to avoid such issue.
 * 
 * See https://github.com/intel/intel-vaapi-driver/issues/541
 * for more information.
 */
#define HW_WORKAROUND_USE_ALTERNATE_SEARCH_TABLE				(1 << 1)
/**
 * ILK and SNB only has a RGBX capable VPP shader for converting NV12
 * into RGBA.
 * 
 * The driver for some reason tries to reuse this shader for BGRA and other
 * formats with alpha in them, causing them to explode horribly.
 * 
 * To be fair this is my fault since I'm the one that exposed BGRA to Chromium.
 * 
 * Currently provides no functionality.
 */
#define HW_WORKAROUND_INVALID_RGBX_SHADER_USE					(1 << 2)
	uint32_t driver_workarounds;

	unsigned int is_g4x         : 1; /* gen4 */
	unsigned int is_ivybridge   : 1; /* gen7 */
	unsigned int is_baytrail    : 1; /* gen7 */
	unsigned int is_haswell     : 1; /* gen7 */
	unsigned int is_cherryview  : 1; /* gen8 */
	unsigned int is_skylake     : 1; /* gen9 */
	unsigned int is_broxton     : 1; /* gen9 */
	unsigned int is_kabylake    : 1; /* gen9p5 */
	unsigned int is_glklake     : 1; /* gen9p5 lp*/
	unsigned int is_cfllake     : 1;
};

struct intel_driver_data {
	int fd;
	int device_id;
	int revision;

	int dri2Enabled;

	sigset_t sa_mask;
	pthread_mutex_t ctxmutex;
	int locked;

	dri_bufmgr *bufmgr;
	const struct intel_device_info *device_info;

	/* We will always have a positive number of EUs. */
	unsigned int eu_total;

	unsigned int mocs_state;

	unsigned int has_exec2  : 1; /* Flag: has execbuffer2? */
	unsigned int has_bsd    : 1; /* Flag: has bitstream decoder for H.264? */
	unsigned int has_blt    : 1; /* Flag: has BLT unit? */
	unsigned int has_vebox  : 1; /* Flag: has VEBOX unit */
	unsigned int has_bsd2   : 1; /* Flag: has the second BSD video ring unit */
	unsigned int has_huc    : 1; /* Flag: has a fully loaded HuC firmware? */
	unsigned int hybrid_vp8 : 1; /* Flag: User has enrolled in experimental VP8 encoding support. */
	unsigned int rc_hw_mode : 1; /* Flag: User has enrolled in RateControlCounter */
	unsigned int dec_base	: 1; /* Flag: User has enrolled in experimental VA_DEC_SLICE_MODE_BASE support  */
};

bool intel_driver_init(VADriverContextP ctx);
void intel_driver_terminate(VADriverContextP ctx);

static INLINE struct intel_driver_data *
intel_driver_data(VADriverContextP ctx)
{
	return (struct intel_driver_data *)ctx->pDriverData;
}

struct intel_region {
	int x;
	int y;
	unsigned int width;
	unsigned int height;
	unsigned int cpp;
	unsigned int pitch;
	unsigned int tiling;
	unsigned int swizzle;
	dri_bo *bo;
};

#define IS_G4X(device_info)             (device_info->is_g4x)

#define IS_IRONLAKE(device_info)        (device_info->gen == 5)

#define IS_GEN6(device_info)            (device_info->gen == 6)

#define IS_HASWELL(device_info)         (device_info->is_haswell)
#define IS_GEN7(device_info)            (device_info->gen == 7)

#define IS_CHERRYVIEW(device_info)      (device_info->is_cherryview)
#define IS_GEN8(device_info)            (device_info->gen == 8)

#define IS_GEN9(device_info)            (device_info->gen == 9)

#define IS_SKL(device_info)             (device_info->is_skylake)

#define IS_BXT(device_info)             (device_info->is_broxton)

#define IS_KBL(device_info)             (device_info->is_kabylake ||\
										 device_info->is_cfllake)

#define IS_GLK(device_info)             (device_info->is_glklake)

#define IS_GEN10(device_info)           (device_info->gen == 10)

#endif /* _INTEL_DRIVER_H_ */
