/*
* @file sgpu_trace.c
* @copyright 2020 Samsung Electronics
*/

#include <errno.h>
#include "amdgpu.h"
#include "sgpu_drm.h"
#include "xf86drm.h"
#include "amdgpu_internal.h"

drm_public int sgpu_trace_op(struct amdgpu_trace_info *info,
			     uint32_t flags)
{
	union drm_sgpu_trace trace;
	struct sgpu_trace_buffer *next, *prev;
	int r;

	if (!info || !info->buf)
		return -EINVAL;

	info->idx = (info->idx + 1) % info->num_buf;
	next = &info->buf[(info->idx + 1) % info->num_buf];
	prev = &info->buf[info->idx];

	trace.in.flags = flags;
	trace.in.handle = next->bo->handle;
	trace.in.size = next->size;

	r = drmCommandWriteRead(info->dev->fd, DRM_SGPU_TRACE,
				&trace, sizeof(trace));
	if (r)
		return r;

	if (!info->pas_id) {
		info->pas_id = trace.out.pas_id;
	} else if (info->pas_id != trace.out.pas_id) {
		r = -ESRCH;
		return r;
	}

	if (trace.out.overflow) {
		r = -EOVERFLOW;
	}

	prev->rptr = trace.out.rptr;
	prev->wptr = trace.out.wptr;

	return r;
}
