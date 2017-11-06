#include "stdafx.h"
#include "MaskRect.h"

void Mask::openWindow (int nHorPos, int nVerPos, int nWidth, int nHeight) {
	MaskRect *rect;
	MaskRect *next = NULL;
	for (rect = root->next; rect != NULL; rect = next) {
		next = rect->next;
		
		if ((rect->y >= nVerPos) && (rect->y+rect->h <= nVerPos + nHeight)
			&& (rect->x >= nHorPos) && (rect->x+rect->w <= nHorPos + nWidth)) {
			// rectがすっぽり含まれるので、何も残らない。
			remove(rect);
			delete rect;
		} else if ( (rect->y >= nVerPos+nHeight) || (rect->y+rect->h < nVerPos)
			|| (rect->x >= nHorPos+nWidth) || (rect->x+rect->w < nHorPos) ) {
			// rect との重なりが何もない。
		} else {
			if (rect->y < nVerPos) {
				add( new MaskRect(rect->x, rect->y, rect->w, nVerPos-rect->y));
				rect->h = rect->h - (nVerPos-rect->y);
				rect->y = nVerPos;
			}
			if (rect->y+rect->h > nVerPos+nHeight) {
				add( new MaskRect(rect->x, nVerPos+nHeight, rect->w, (rect->y+rect->h)-(nVerPos+nHeight)));
				rect->h = nVerPos+nHeight-rect->y;
			}
			if ((rect->x < nHorPos) && (rect->h > 0)) {
				add( new MaskRect(rect->x, rect->y, nHorPos-rect->x, rect->h));
			}
			if ((rect->x+rect->w > nHorPos+nWidth) && (rect->h > 0)) {
				add( new MaskRect(nHorPos+nWidth, rect->y, (rect->x+rect->w)-(nHorPos+nWidth), rect->h) );
			}
			remove(rect);
			delete rect;
		}
	}
}

void Mask::shakeout(int limit) {
	MaskRect *rect;
	MaskRect *next = NULL;
	for (rect = root->next; rect != NULL; rect = next) { // 上端または下端に細い帯になった場合は、マスクをページの外側に広げる。
		next = rect->next;
		if (rect->h < limit && rect->y == 0) {
			rect->ExpandUp(limit);
		} else if (rect->h < limit && (rect->y + rect->h)==height) {
			rect->ExpandDown(limit);
		}
	}
	for (rect = root->next; rect != NULL; rect = next) { // 途中で細い帯になった場合は、上下に隣接するマスクの中へ広げる。
		next = rect->next;
		if (rect->h < limit) {
			MaskRect *adjoin;
			for (adjoin=root->next; adjoin!=NULL; adjoin=adjoin->next) {
				if ((adjoin->y == (rect->y + rect->h))
					&& (adjoin->x <= rect->x) 
					&& (adjoin->x + adjoin->w >= rect->x + rect->w)) {
					rect->ExpandDown(adjoin->h);
					break;
				} else if ((rect->y == (adjoin->y + adjoin->h))
					&& (adjoin->x <= rect->x) 
					&& (adjoin->x + adjoin->w >= rect->x + rect->w)) {
					rect->ExpandUp(adjoin->h);
					break;
				}
			}

		}
	}
	for (rect = root->next; rect != NULL; rect = next) { // 右端または下左端に細い帯になった場合は、マスクをページの外側に広げる。
		next = rect->next;
		if (rect->w < limit && rect->x == 0) {
			rect->ExpandLeft(limit);
		} else if (rect->w < limit && (rect->x + rect->w)==width) {
			rect->ExpandRight(limit);
		}
	}
}

void Mask::remove(MaskRect *node) {
	if (node->prev != NULL) {
		node->prev->next = node->next;
	}
	if (node->next  != NULL) {
		node->next->prev = node->prev;
	}
}

void Mask::add(MaskRect *node) {
	if (root->next != NULL) {
		root->next->prev = node;
	}
	node->next = root->next;
	node->prev = root;
	root->next = node;
}
