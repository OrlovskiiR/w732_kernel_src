/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

/*
 * MUSB OTG driver peripheral defines
 *
 * Copyright 2005 Mentor Graphics Corporation
 * Copyright (C) 2005-2006 by Texas Instruments
 * Copyright (C) 2006-2007 Nokia Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 * NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef __MUSB_GADGET_H
#define __MUSB_GADGET_H

#include <linux/list.h>
enum buffer_map_state {
	UN_MAPPED = 0,
	PRE_MAPPED,
	MUSB_MAPPED
};

struct musb_request {
	struct usb_request	request;
	struct list_head	list;
	struct musb_ep		*ep;
	struct musb		*musb;
	u8 tx;			/* endpoint direction */
	u8 epnum;
	//u8 mapped;
	enum buffer_map_state map_state;
	u8 queue;
};

static inline struct musb_request *to_musb_request(struct usb_request *req)
{
	return req ? container_of(req, struct musb_request, request) : NULL;
}

extern struct usb_request *
musb_alloc_request(struct usb_ep *ep, gfp_t gfp_flags);
extern void musb_free_request(struct usb_ep *ep, struct usb_request *req);


/*
 * struct musb_ep - peripheral side view of endpoint rx or tx side
 */
struct musb_ep {
	/* stuff towards the head is basically write-once. */
	struct usb_ep			end_point;
	char				name[12];
	struct musb_hw_ep		*hw_ep;
	struct musb			*musb;
	u16 fifo_size ;
	enum musb_buf_mode	fifo_mode;
	u8				current_epnum;

	/* ... when enabled/disabled ... */
	u8				type;
	u8				is_in;
	u16				packet_sz;
	const struct usb_endpoint_descriptor	*desc;
	struct dma_channel		*dma;

	/* later things are modified based on usage */
	struct list_head		req_list;

	u8				wedged;

	/* true if lock must be dropped but req_list may not be advanced */
	u8				busy;
	bool            dma_configed;

	u8				hb_mult;
};

static inline struct musb_ep *to_musb_ep(struct usb_ep *ep)
{
	return ep ? container_of(ep, struct musb_ep, end_point) : NULL;
}

static inline struct musb_request *next_request(struct musb_ep *ep)
{
	struct list_head	*queue = &ep->req_list;

	if (list_empty(queue))
		return NULL;
	return container_of(queue->next, struct musb_request, list);
}

extern void musb_g_tx(struct musb *musb, u8 epnum);
extern void musb_g_rx(struct musb *musb, u8 epnum);

extern const struct usb_ep_ops musb_g_ep0_ops;

extern int musb_gadget_setup(struct musb *);
extern void musb_gadget_cleanup(struct musb *);

extern void musb_g_giveback(struct musb_ep *, struct usb_request *, int);

extern void musb_ep_restart(struct musb *, struct musb_request *);

#endif		/* __MUSB_GADGET_H */
