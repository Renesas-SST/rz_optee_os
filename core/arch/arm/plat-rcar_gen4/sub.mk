global-incdirs-y += .
global-incdirs-y += include
srcs-y += main.c
srcs-$(CFG_OTP_SUPPORT) += tee_common_otp.c
srcs-y += rcar_log_func.c
srcs-y += rcar_mutex.c
srcs-$(CFG_DYNAMIC_TA_AUTH_BY_HWENGINE) += rcar_ta_auth.c
srcs-$(CFG_ARM64_core) += rcar_common_a64.S
srcs-$(CFG_ICUM_FW_SERVICE) += rcar_fw_security_service.c
srcs-y += rcar_suspend_to_ram.c

# trace_ext.c: upstream 4.10 provides core/kernel/trace_ext.c — no override needed

subdirs-y += drivers
subdirs-y += tee
subdirs-$(CFG_ICUM_FW_SERVICE) += secure
