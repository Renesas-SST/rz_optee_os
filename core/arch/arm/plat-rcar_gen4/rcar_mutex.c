// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2020, Renesas Electronics Corporation
 */

#include <kernel/mutex.h>
#include <kernel/panic.h>
#include <kernel/spinlock.h>
#include <kernel/tee_time.h>
#include <kernel/thread.h>
#include <optee_rpc_cmd.h>

#include "../kernel/mutex_lockdep.h"
#include "rcar_common.h"
#include "rcar_mutex.h"

void rcar_nex_mutex_lock(struct mutex *m)
{
#ifdef CFG_VIRTUALIZATION
	bool can_lock;
	TEE_Result res = TEE_SUCCESS;
	struct thread_param params = THREAD_PARAM_VALUE(IN,
					CFG_RCAR_MUTEX_DELAY, 0, 0);

	can_lock = mutex_trylock(m);

	while (!can_lock) {
		res = thread_rpc_cmd(OPTEE_RPC_CMD_SUSPEND, 1, &params);

		if (res != TEE_SUCCESS) {
			panic("rcar_nex_mutex_lock failed");
		}
		can_lock = mutex_trylock(m);
	}
#else
	mutex_lock(m);
#endif
}

TEE_Result thread_hw_wait_cmd(const TEE_Time *base_time, uint32_t timeout,
			      uint32_t wait, uint32_t delay)
{
	TEE_Result res;
	TEE_Time current_time;
	uint32_t mytime;
	struct thread_param params;

	res = tee_time_get_sys_time(&current_time);
	if (res != TEE_SUCCESS)
		return TEE_ERROR_BAD_STATE;

	mytime = (current_time.seconds - base_time->seconds) * 1000 +
		 (int32_t)current_time.millis - (int32_t)base_time->millis;

	if (mytime >= timeout)
		return TEE_ERROR_OVERFLOW;

	if (mytime > delay) {
		params = THREAD_PARAM_VALUE(IN, wait, 0, 0);
		res = thread_rpc_cmd(OPTEE_RPC_CMD_SUSPEND, 1, &params);
		if (res != TEE_SUCCESS)
			return TEE_ERROR_OUT_OF_MEMORY;
	}

	return TEE_SUCCESS;
}

void rcar_nex_mutex_unlock(struct mutex *m)
{
#ifdef CFG_VIRTUALIZATION
	uint32_t old_itr_status;

	old_itr_status = cpu_spin_lock_xsave(&m->spin_lock);

	if (m->state == 0U) {
		panic("rcar_nex_mutex_unlock failed");
	}

	m->state = 0;

	cpu_spin_unlock_xrestore(&m->spin_lock, old_itr_status);
#else
	mutex_unlock(m);
#endif
}
