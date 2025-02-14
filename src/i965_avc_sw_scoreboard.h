#include <va/va_backend.h>

#include "i965_defines.h"
#include "i965_media_h264.h"

static void i965_avc_sw_scoreboard_objects(VADriverContextP ctx, struct i965_h264_context *i965_h264_context)
{
	struct intel_batchbuffer *batch = i965_h264_context->batch;
	int width_in_mb_minus_1 = i965_h264_context->picture.width_in_mbs - 1;
	int height_in_mb_minus_1 = i965_h264_context->avc_it_command_mb_info.mbs / (width_in_mb_minus_1 + 1) / (1 + !!i965_h264_context->picture.mbaff_frame_flag) - 1;
	int total_mb = i965_h264_context->avc_it_command_mb_info.mbs;
	int kernel_index = i965_h264_context->picture.mbaff_frame_flag ? 17 : 16;

	BEGIN_BATCH(batch, 16);
	OUT_BATCH(batch, CMD_MEDIA_OBJECT_PRT | (16 - 2));
	OUT_BATCH(batch, 0);
	OUT_BATCH(batch,
			  ((kernel_index << 24) | /* Interface Descriptor Offset. */
			   (1 << 23)));           /* PRT_Fence Needed */
	OUT_BATCH(batch, 0);
	OUT_BATCH(batch,
			  ((height_in_mb_minus_1 << 16) |
			   (width_in_mb_minus_1)));
	OUT_BATCH(batch, total_mb);
	OUT_BATCH(batch, 0);
	OUT_BATCH(batch, 0);
	OUT_BATCH(batch, 0);
	OUT_BATCH(batch, 0);
	OUT_BATCH(batch, 0);
	OUT_BATCH(batch, 0);
	OUT_BATCH(batch, 0);
	OUT_BATCH(batch, 0);
	OUT_BATCH(batch, 0);
	OUT_BATCH(batch, 0);
	ADVANCE_BATCH(batch);
}