#
# Copyright@ SAMSUNG Electronics Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Makefile for exynos_fusion

EXYNOS_FUSION_BUILD := REL

FLAGS += -Ios/drivers/exynos_fusion/inc

FLAGS += -DTARGET_BOARD_$(shell echo $(VARIANT) | tr a-z A-Z)

ifeq ($(EXYNOS_FUSION_BUILD), DEV)
include os/drivers/exynos_fusion/src/build.mk
else
FLAGS += -lexynos_fusion -lm -Los/drivers/exynos_fusion
endif

SRCS_os += os/drivers/exynos_fusion/exynos_fusion_interface.c
