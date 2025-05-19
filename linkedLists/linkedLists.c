#define DECLARE_FORWARD
#define DECLARE_BACKWARDS
#define DECLARE_DOUBLE

#include "linkedLists.h"

#if defined(DECLARE_FORWARD) || defined(DECLARE_BACKWARDS) || defined(DECLARE_DOUBLE)

#include "../memory/memfuncs.h"
#include <stdlib.h>
#include <string.h>

#ifdef DECLARE_FORWARD
	void addForwardNode(ForwardLinkedList* list, void* value, SizeT valSize) {
		ForwardNode* node = list;

		while (node->next != NULL) {
			node = node->next;
		}

		node->next = (ForwardNode*)calloc(1, sizeof(ForwardNode));
		node = node->next;

		node->value = calloc(1, valSize);
		copyMemory(value, node->value, valSize);
		node->next = NULL;
	}

	void removeForwardNode(ForwardLinkedList* list, SizeT position, bool cascade) {
		
	}
#endif

#ifdef DECLARE_BACKWARDS
	void addBackwardsNode(BackwardsLinkedList* list, void* value, SizeT valSize);
	void removeBackwardsNode(BackwardsLinkedList* list, SizeT position, bool cascade);
#endif

#ifdef DECLARE_DOUBLE
	void addDoubleNode(DoubleLinkedList* list, void* value, SizeT valSize);
	void removeDoubleNode(DoubleLinkedList* list, SizeT position, bool cascade);
#endif

void addNode(LinkedList* list, void* value, SizeT valSize);
void removeNode(LinkedList* list, SizeT position, bool cascade);

#endif