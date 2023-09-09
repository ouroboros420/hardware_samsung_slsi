/*
 * Copyright (C) 2018 The Android Open Source Project
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

#define LOG_TAG "chub_test"

#include <sys/types.h>
#include <sys/wait.h>
#include <fstream>
#include <iostream>
#include <cstring>

#include <log/log.h>

#define SCRIPT_REBOOT "/vendor/bin/chub_test_reboot.sh"
#define SCRIPT_PR "/vendor/bin/chub_test_pr.sh 2> /dev/null"
#define SENSORTEST_RAND "/system/bin/sensortest_rand"

/*
// read and execute line by line
	std::string str;
	std::ifstream in(ALIVE);

	while (std::getline(in, str)) {
		ALOGI("%s", str.c_str());
		system(str.c_str());
	}
*/

int main(int argc, char** argv) {

	ALOGI("Welcome! cmd: %d %d", argc, atoi(argv[argc - 1]));
	switch (atoi(argv[argc - 1]))
	{
		case 1:
			system(SCRIPT_REBOOT);
			break;

		case 2:
			system(SCRIPT_PR);
			break;

		case 3:
			system(SENSORTEST_RAND);
			break;

		default:
			ALOGE("invalid argument %d", atoi(argv[argc - 1]));
	}

	return 0;
}
