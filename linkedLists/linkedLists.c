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

	void removeForwardNode(ForwardLinkedList** list, SizeT position, bool cascade) {
		ForwardLinkedList* prev = NULL;
		ForwardLinkedList* current = *list;
		ForwardLinkedList* next = current->next;

		for (SizeT pos = 0; pos < position; pos++) {
			prev = current;
			current = next;
			next = current->next;

			if (current == NULL) {
				return;
			}
		}

		if (prev != NULL) {
			prev->next = next;
		} else {
			*list = next;
		}

		free(current->value);
		current->next = NULL;
		free(current);

		if (cascade) {
			removeForwardNode(list, position, true);
		}
	}
#endif

#ifdef DECLARE_BACKWARDS
	void addBackwardsNode(BackwardsLinkedList* list, void* value, SizeT valSize) {
		BackwardsNode* node = list;

		while (node->previous != NULL) {
			node = node->previous;
		}

		node->previous = (BackwardsNode*)calloc(1, sizeof(BackwardsNode));
		node = node->previous;

		node->value = calloc(1, valSize);
		copyMemory(value, node->value, valSize);
		node->previous = NULL;
	}

	void removeBackwardsNode(BackwardsLinkedList** list, SizeT position, bool cascade) {
		BackwardsLinkedList* next = NULL;
		BackwardsLinkedList* current = *list;
		BackwardsLinkedList* previous = current->previous;

		for (SizeT pos = 0; pos < position; pos++) {
			next = current;
			current = previous;
			previous = current->previous;

			if (current == NULL) {
				return;
			}
		}

		if (next != NULL) {
			next->previous = previous;
		} else {
			*list = previous;
		}

		free(current->value);
		current->previous = NULL;
		free(current);

		if (cascade) {
			removeBackwardsNode(list, position, true);
		}
	}
#endif

#ifdef DECLARE_DOUBLE
	void addDoubleNode(DoubleLinkedList* list, void* value, SizeT valSize) {
		DoubleNode* previous = NULL;
		DoubleNode* current = list;
		DoubleNode* next = list->next;

		while (next != NULL) {
			current = next;
			previous = current->previous;
			next = current->next;
		}

		next = (DoubleNode*)calloc(1, sizeof(DoubleNode));
		next->previous = current;

		copyMemory(value, next->value, valSize);
		next->next = NULL;
	}

	void removeDoubleNode(DoubleLinkedList** list, SizeT position, bool cascade);
#endif

void addNode(LinkedList* list, void* value, SizeT valSize);
void removeNode(LinkedList** list, SizeT position, bool cascade);

#endif