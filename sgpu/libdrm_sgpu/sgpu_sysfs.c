/*
************************************************************************************************************************
*
*  Copyright (c) 2022 Samsung Electronics Co, Ltd. All rights reserved.
*
***********************************************************************************************************************/


#include "amdgpu.h"
#include "sgpu_drm.h"
#include "amdgpu_internal.h"
#include "xf86drm.h"
#include <string.h>
#include <errno.h>

/**
 * Creates an sgpu_instance_data and returns a handle
 *
 * \param   dev    - \c [in] Device handle. See #amdgpu_device_initialize()
 * \param   handle - \c [out] Pointer to the returned handle
 *
 * A handle of 0 is treated as invalid and will never be assigned.
 * On success, (*handle) will be assigned a non-negative value,
 * otherwise it is left unmodified.
 *
 * \return   0 on success\n
 *          <0 - Negative POSIX Error code
 */
drm_public int sgpu_instance_data_create(amdgpu_device_handle dev, uint32_t *handle)
{
	union drm_sgpu_instance_data args;
	int r;

	if (dev == NULL || handle == NULL)
		return -EINVAL;

	memset(&args, 0, sizeof(args));
	args.in.op = SGPU_INSTANCE_DATA_OP_CREATE;

	r = drmCommandWriteRead(dev->fd, DRM_SGPU_INSTANCE_DATA,
				&args, sizeof(args));
	if (!r)
		*handle = args.out.handle;
	return r;
}

/**
 * Destroys an sgpu_instance_data given its handle
 *
 * \param   dev    - \c [in] Device handle. See #amdgpu_device_initialize()
 * \param   handle - \c [in] Handle to an sgpu_instance_data
 *
 * A handle of 0 is treated as invalid and will be ignored in
 * this function.
 *
 * \return   0 on success\n
 *          <0 - Negative POSIX Error code
 */
drm_public int sgpu_instance_data_destroy(amdgpu_device_handle dev, uint32_t handle)
{
	union drm_sgpu_instance_data args;

	if (dev == NULL || handle == 0)
		return -EINVAL;

	memset(&args, 0, sizeof(args));
	args.in.op = SGPU_INSTANCE_DATA_OP_DESTROY;
	args.in.handle = handle;

	return drmCommandWriteRead(dev->fd, DRM_SGPU_INSTANCE_DATA,
				   &args, sizeof(args));
}

/**
 * Updates mem_profile sysfs node
 *
 * \param   dev    - \c [in] Device handle. See #amdgpu_device_initialize()
 * \param   buffer - \c [in] Buffer containing memory profile
 * \param   len    - \c [in] Length of buffer
 * \param   instance_data_handle - \c [in] Handle to an sgpu_instance_data
 *
 * An instance_data_handle of 0 is treated as invalid and will be
 * ignored in this function.
 *
 * \return   0 on success\n
 *          <0 - Negative POSIX Error code
 */
drm_public int sgpu_mem_profile_update(amdgpu_device_handle dev, uint64_t buf, uint32_t length,
				       uint32_t instance_data_handle)
{
	struct drm_sgpu_mem_profile_add mem_profile;

	if (instance_data_handle == 0)
		return -EINVAL;

	memset(&mem_profile, 0, sizeof(mem_profile));
	mem_profile.buffer = buf;
	mem_profile.len = length;
	mem_profile.instance_data_handle = instance_data_handle;

	return drmCommandWrite(dev->fd, DRM_SGPU_MEM_PROFILE_ADD, &mem_profile,
			       sizeof(struct drm_sgpu_mem_profile_add));
}

