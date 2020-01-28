/*
 * Copyright (c) 2020, NVIDIA CORPORATION.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <nvgpu/pmu/volt.h>
#include <nvgpu/gk20a.h>
#include <nvgpu/pmu/cmd.h>

#include "volt_rail.h"
#include "volt_dev.h"
#include "volt_policy.h"


static int volt_send_load_cmd_to_pmu(struct gk20a *g)
{
	struct nvgpu_pmu *pmu = g->pmu;
	struct nv_pmu_rpc_struct_volt_load rpc;
	int status = 0;

	(void) memset(&rpc, 0, sizeof(struct nv_pmu_rpc_struct_volt_load));
	PMU_RPC_EXECUTE(status, pmu, VOLT, LOAD, &rpc, 0);
	if (status != 0) {
		nvgpu_err(g, "Failed to execute RPC status=0x%x",
			status);
	}

	return status;
}

void nvgpu_pmu_volt_rpc_handler(struct gk20a *g, struct nv_pmu_rpc_header *rpc)
{
	switch (rpc->function) {
	case NV_PMU_RPC_ID_VOLT_BOARD_OBJ_GRP_CMD:
		nvgpu_pmu_dbg(g,
			"reply NV_PMU_RPC_ID_VOLT_BOARD_OBJ_GRP_CMD");
		break;
	case NV_PMU_RPC_ID_VOLT_LOAD:
		nvgpu_pmu_dbg(g,
			"reply NV_PMU_RPC_ID_VOLT_LOAD");
		break;
	default:
		nvgpu_pmu_dbg(g, "invalid reply");
		break;
	}
}

int nvgpu_pmu_volt_sw_setup(struct gk20a *g)
{
	int err;
	nvgpu_log_fn(g, " ");

	err = volt_rail_sw_setup(g);
	if (err != 0) {
		return err;
	}

	err = volt_dev_sw_setup(g);
	if (err != 0) {
		return err;
	}

	err = volt_policy_sw_setup(g);
	if (err != 0) {
		return err;
	}

	return 0;
}

int nvgpu_pmu_volt_pmu_setup(struct gk20a *g)
{
	int err;
	nvgpu_log_fn(g, " ");

	err = volt_rail_pmu_setup(g);
	if (err != 0) {
		return err;
	}

	err = volt_dev_pmu_setup(g);
	if (err != 0) {
		return err;
	}

	err = volt_policy_pmu_setup(g);
	if (err != 0) {
		return err;
	}

	err = volt_send_load_cmd_to_pmu(g);
	if (err != 0) {
		nvgpu_err(g,
			"Failed to send VOLT LOAD CMD to PMU: status = 0x%08x.",
			err);
		return err;
	}

	return 0;
}