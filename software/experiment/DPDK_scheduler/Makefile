#	BSD LICENSE
#
#	Copyright(c) 2010-2015 Intel Corporation. All rights reserved.
#	All rights reserved.
#
#	Redistribution and use in source and binary forms, with or without
#	modification, are permitted provided that the following conditions
#	are met:
#
#	  * Redistributions of source code must retain the above copyright
#		notice, this list of conditions and the following disclaimer.
#	  * Redistributions in binary form must reproduce the above copyright
#		notice, this list of conditions and the following disclaimer in
#		the documentation and/or other materials provided with the
#		distribution.
#	  * Neither the name of Intel Corporation nor the names of its
#		contributors may be used to endorse or promote products derived
#		from this software without specific prior written permission.
#
#	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
#	OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

include $(RTE_SDK)/mk/rte.vars.mk

ifeq ($(CONFIG_RTE_LIBRTE_PIPELINE),y)

#
# library name
#
APP = main

CFLAGS += -O3
CFLAGS += $(WERROR_FLAGS)
CFLAGS += -g

LDLIBS += -lconfuse

#
# all source are stored in SRCS-y
#
SRCS-y := main.c
SRCS-y += config.c
SRCS-y += init.c
SRCS-y += receive.c
SRCS-y += forwarding.c
SRCS-y += transmit.c
SRCS-y += output_queue.c
SRCS-y += flows2nodes.c
SRCS-y += rx2flows.c
SRCS-y += test.c

include $(RTE_SDK)/mk/rte.extapp.mk

endif

# # binary name
# APP = main

# # all source are stored in SRCS-y
# SRCS-y := main.c
# SRCS-y += config.c
# SRCS-y += init.c
# SRCS-y += receive.c
# SRCS-y += forwarding.c
# SRCS-y += transmit.c
# SRCS-y += output_queue.c
# SRCS-y += flows2nodes.c
# SRCS-y += rx2flows.c
# SRCS-y += test.c

# PKGCONF ?= pkg-config

# # Build using pkg-config variables if possible
# ifneq ($(shell $(PKGCONF) --exists libdpdk && echo 0),0)
# $(error "no installation of DPDK found")
# endif

# all: shared
# .PHONY: shared static
# shared: build/$(APP)-shared
# 	ln -sf $(APP)-shared build/$(APP)
# static: build/$(APP)-static
# 	ln -sf $(APP)-static build/$(APP)

# PC_FILE := $(shell $(PKGCONF) --path libdpdk 2>/dev/null)
# CFLAGS += -O3 $(shell $(PKGCONF) --cflags libdpdk)
# LDFLAGS_SHARED = $(shell $(PKGCONF) --libs libdpdk)
# LDFLAGS_SHARED += -lconfuse
# LDFLAGS_STATIC = $(shell $(PKGCONF) --static --libs libdpdk)
# LDFLAGS_STATIC += -lconfuse

# CFLAGS += -DALLOW_EXPERIMENTAL_API

# build/$(APP)-shared: $(SRCS-y) Makefile $(PC_FILE) | build
# 	$(CC) $(CFLAGS) $(SRCS-y) -o $@ $(LDFLAGS) $(LDFLAGS_SHARED)

# build/$(APP)-static: $(SRCS-y) Makefile $(PC_FILE) | build
# 	$(CC) $(CFLAGS) $(SRCS-y) -o $@ $(LDFLAGS) $(LDFLAGS_STATIC)

# build:
# 	@mkdir -p $@

# .PHONY: clean
# clean:
# 	rm -f build/$(APP) build/$(APP)-static build/$(APP)-shared
# 	test -d build && rmdir -p build || true
