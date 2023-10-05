/*
 * Copyright (c) 2012 Linux Box Corporation.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR `AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef TIRPC_SVC_RQST_H
#define TIRPC_SVC_RQST_H

#include <rpc/svc.h>

/**
 ** Maintains a tree of all extant transports by event
 **/

#define SVC_RQST_FLAG_NONE		SVC_XPRT_FLAG_NONE
/* uint16_t actually used */
#define SVC_RQST_FLAG_CHAN_AFFINITY	0x1000 /* bind conn to parent chan */
#define SVC_RQST_FLAG_MASK (SVC_RQST_FLAG_CHAN_AFFINITY)

/* uint32_t instructions */
#define SVC_RQST_FLAG_LOCKED		SVC_XPRT_FLAG_LOCKED
#define SVC_RQST_FLAG_UNLOCK		SVC_XPRT_FLAG_UNLOCK
#define SVC_RQST_FLAG_EPOLL		0x00080000

#define SVC_RQST_FLAG_PART_LOCKED	0x00100000
#define SVC_RQST_FLAG_PART_UNLOCK	0x00200000

#define SVC_RQST_FLAG_SREC_LOCKED	0x01000000
#define SVC_RQST_FLAG_SREC_UNLOCK	0x02000000
#define SVC_RQST_FLAG_XPRT_UREG		0x04000000
#define SR_REQ_RELEASE_KEEP_LOCKED	0x08000000

/*
 * exported interface:
 *
 *  svc_rqst_init -- init module (optional)
 *  svc_rqst_init_xprt -- init svc_rqst part of xprt handle
 *  svc_rqst_new_evchan -- create event channel
 *  svc_rqst_evchan_reg -- set {xprt, dispatcher} mapping
 *  svc_rqst_foreach_xprt -- scan registered xprts at id (or 0 for all)
 *  svc_rqst_thrd_run -- enter dispatch loop at id
 *  svc_rqst_thrd_signal --request thread to run a callout function which
 *   can cause the thread to return
 *  svc_rqst_shutdown -- cause all threads to return
 *
 * callback function interface:
 *
 * svc_xprt_rendezvous -- called when a new transport connection is accepted,
 *  can be used by the application to chose a correct request handler, or do
 *  other adaptation
 */
void svc_rqst_init();
void svc_rqst_init_xprt(SVCXPRT *xprt);
int svc_rqst_new_evchan(uint32_t *chan_id /* OUT */ , void *u_data,
			uint32_t flags);
int svc_rqst_evchan_reg(uint32_t chan_id, SVCXPRT *xprt, uint32_t flags);
int svc_rqst_rearm_events(SVCXPRT *xprt, uint32_t flags);

int svc_rqst_xprt_register(SVCXPRT *xprt, SVCXPRT *newxprt);
int svc_rqst_thrd_run(uint32_t chan_id, uint32_t flags);
int svc_rqst_thrd_signal(uint32_t chan_id, uint32_t flags);

/* xprt/connection rendezvous callout */
typedef int (*svc_rqst_rendezvous_t)
	(SVCXPRT *oxprt, SVCXPRT *nxprt, uint32_t flags);

/* iterator callback prototype */
typedef void (*svc_rqst_xprt_each_func_t) (uint32_t chan_id, SVCXPRT *xprt,
					   void *arg);
int svc_rqst_foreach_xprt(uint32_t chan_id, svc_rqst_xprt_each_func_t each_f,
			  void *arg);

#define SVC_RQST_STATE_NONE           0x00000
#define SVC_RQST_STATE_ACTIVE         0x00001	/* thrd in event loop */
#define SVC_RQST_STATE_BLOCKED        0x00002	/* channel blocked */
#define SVC_RQST_STATE_DESTROYED      0x00004

#define SVC_RQST_SIGNAL_SHUTDOWN      0x00008	/* chan shutdown */
#define SVC_RQST_SIGNAL_MASK (SVC_RQST_SIGNAL_SHUTDOWN)

#endif				/* TIRPC_SVC_RQST_H */
