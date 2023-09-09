/*
 * Copyright (C) 2020 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Fastboot.h"

#include <android-base/logging.h>
#include "gpt_format.h"
#include "fcntl.h"

namespace android {
namespace hardware {
namespace fastboot {
namespace V1_1 {
namespace implementation {

android::hardware::fastboot::V1_0::FileSystemType get_fs_type(struct gpt_entry *gpt_entry)
{
	if (gpt_entry->attributes.type == FS_TYPE_SPARSE_EXT4) {
		return FileSystemType::EXT4;
	} else if (gpt_entry->attributes.type == FS_TYPE_SPARSE_F2FS) {
		return FileSystemType::F2FS;
	} else {
		return FileSystemType::RAW;
	}
}

// Methods from ::android::hardware::fastboot::V1_1::IFastboot follow.
Return<void> Fastboot::getPartitionType(const hidl_string& partitionName,
                                        getPartitionType_cb _hidl_cb) {
	struct gpt_header *gpt_header;
	struct gpt_entry *gpt_entry;
	int offset = PMBR_LBA * 4096;
	int rc = -1;
	char part_name[UID_STR_LEN];

	const char* data = partitionName.c_str();

	gpt_entry = (struct gpt_entry *)aligned_alloc(sizeof(struct gpt_entry) , sizeof(struct gpt_entry));
	memset(gpt_entry, 0, sizeof(struct gpt_entry));

	gpt_header = (struct gpt_header *)aligned_alloc(sizeof(struct gpt_header) , sizeof(struct gpt_header));
	memset(gpt_header, 0, sizeof(struct gpt_header));
	/* replace /dev/block/XXX with the source block device */
	int fd = open("/dev/block/sda", O_RDONLY);

	if (fd < 0) {
		LOG(ERROR) << "getPartitionType: file open error" ;
		free(gpt_entry);
		free(gpt_header);

		return Void();
	}

	if (lseek(fd, offset, SEEK_SET) < 0) {
		LOG(ERROR) << "getPartitionType: file seek error" ;
		free(gpt_entry);
		free(gpt_header);
		close(fd);

		return Void();
	}

	rc = read(fd, gpt_header, sizeof(struct gpt_header));

	for (int i = 0; i < gpt_header->part_num_entry; i++) {
		rc = read(fd, gpt_entry, sizeof(struct gpt_entry));
		if (rc < 0)
			LOG(ERROR) << "READ Error in pit_entry #" << i;
		for (int j = 0 ; j < UID_STR_LEN ; j++)
			part_name[j] = (char)gpt_entry->part_name[j];
		if (!strcmp(data, (const char*)part_name)) {
			LOG(INFO) << "Partition Name : " << data << "found in part entry";
			break;
		}
	}
	_hidl_cb(get_fs_type(gpt_entry), {Status::SUCCESS, ""});

	close(fd);
	free(gpt_entry);
	free(gpt_header);

	return Void();
}

Return<void> Fastboot::doOemCommand(const hidl_string& /* oemCmd */, doOemCommand_cb _hidl_cb) {
    _hidl_cb({Status::FAILURE_UNKNOWN, "Command not supported in default implementation"});
    return Void();
}

Return<void> Fastboot::getVariant(getVariant_cb _hidl_cb) {
    _hidl_cb("NA", {Status::SUCCESS, ""});
    return Void();
}

Return<void> Fastboot::getOffModeChargeState(getOffModeChargeState_cb _hidl_cb) {
    _hidl_cb(false, {Status::SUCCESS, ""});
    return Void();
}

Return<void> Fastboot::getBatteryVoltageFlashingThreshold(
        getBatteryVoltageFlashingThreshold_cb _hidl_cb) {
    _hidl_cb(0, {Status::SUCCESS, ""});
    return Void();
}

Return<void> Fastboot::doOemSpecificErase(doOemSpecificErase_cb _hidl_cb) {
    _hidl_cb({Status::NOT_SUPPORTED, "Command not supported in default implementation"});
    return Void();
}

extern "C" IFastboot* HIDL_FETCH_IFastboot(const char* /* name */) {
    return new Fastboot();
}

}  // namespace implementation
}  // namespace V1_1
}  // namespace fastboot
}  // namespace hardware
}  // namespace android
