/* inventory_mgr.h — istasyonlara hangi yukun takili oldugunu yoneten bilesen */
/* [payloadConfig] EKLENDI (orijinalde sadece yorumda geciyordu):
 * Yuk degisikligi istekleri her an alinir, payload_bind() ise SADECE
 * frame sinirinda cagrilir. Boylece bir frame boyunca type_of[] sabit kalir. */
#ifndef INVENTORY_MGR_H
#define INVENTORY_MGR_H

#include "payload.h"

void inventory_request(uint32_t s, payload_id_t id);   /* istek: her an      */
void inventory_frame_boundary(void);                   /* uygula: frame sinirinda */

#endif /* INVENTORY_MGR_H */
