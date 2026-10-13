/* inventory_mgr.c — istekleri biriktirir, frame sinirinda payload_bind() ile uygular */
/* [payloadConfig] EKLENDI. Sadece sozlesmeyi (payload.h) bilir; hicbir
 * yuk tipini (tank, pod) tanimaz. */
#include "inventory_mgr.h"

static payload_id_t pending[N_STATIONS] = { PL_UNKNOWN, PL_UNKNOWN, PL_UNKNOWN, PL_UNKNOWN };

void inventory_request(uint32_t s, payload_id_t id)
{
    if (s < N_STATIONS) {
        pending[s] = id;
    }
}

void inventory_frame_boundary(void)
{
    for (uint32_t s = 0u; s < N_STATIONS; s++) {
        payload_bind(s, pending[s]);
    }
}
