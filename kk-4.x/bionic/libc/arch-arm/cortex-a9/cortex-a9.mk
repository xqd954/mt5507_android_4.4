ifeq "$(TARGET_ARCH_VARIANT)" "armv7-a-neon"
$(warning "TARGET_ARCH_VARIANT is $(TARGET_ARCH_VARIANT), neon is supported")
$(call libc-add-cpu-variant-src,MEMCPY,arch-arm/cortex-a9/bionic/memcpy.S)
$(call libc-add-cpu-variant-src,MEMSET,arch-arm/cortex-a9/bionic/memset.S)
else
$(warning "TARGET_ARCH_VARIANT is $(TARGET_ARCH_VARIANT), neon is not supported")
$(call libc-add-cpu-variant-src,MEMCPY,arch-arm/cortex-a9/bionic/no_neon_memcpy.S)
$(call libc-add-cpu-variant-src,MEMSET,arch-arm/cortex-a9/bionic/no_neon_memset.S)
endif
$(call libc-add-cpu-variant-src,STRCAT,arch-arm/cortex-a9/bionic/strcat.S)
$(call libc-add-cpu-variant-src,STRCMP,arch-arm/cortex-a9/bionic/strcmp.S)
$(call libc-add-cpu-variant-src,STRCPY,arch-arm/cortex-a9/bionic/strcpy.S)
$(call libc-add-cpu-variant-src,STRLEN,arch-arm/cortex-a9/bionic/strlen.S)
ifeq "$(TARGET_ARCH_VARIANT)" "armv7-a-neon"
$(warning "TARGET_ARCH_VARIANT is $(TARGET_ARCH_VARIANT), neon is supported")
$(call libc-add-cpu-variant-src,__STRCAT_CHK,arch-arm/cortex-a9/bionic/__strcat_chk.S)
$(call libc-add-cpu-variant-src,__STRCPY_CHK,arch-arm/cortex-a9/bionic/__strcpy_chk.S)
else
$(warning "TARGET_ARCH_VARIANT is $(TARGET_ARCH_VARIANT), neon is not supported")
endif

include bionic/libc/arch-arm/generic/generic.mk
