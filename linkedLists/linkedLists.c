#define DECLARE_FORWARD
#define DECLARE_BACKWARDS
#define DECLARE_DOUBLE

#include "linkedLists.h"

#if defined(DECLARE_FORWARD) || defined(DECLARE_BACKWARDS) || defined(DECLARE_DOUBLE)

#include "../memory/memfuncs.h"
#include <stdlib.h>
#include <string.h>

#ifdef DECLARE_FORWARD
	void addForwardNode(ForwardLinkedList* list, ValueField value) {
		ForwardNode* node = list;

		while (node->next != NULL) {
			node = node->next;
		}

		node->next = (ForwardNode*)calloc(1, sizeof(ForwardNode));
		node = node->next;

		node->value = value;
		node->next = NULL;
	}

	void removeForwardNode(ForwardLinkedList** list, SizeT position, bool cascade) {
		ForwardNode* prev = NULL;
		ForwardNode* current = *list;
		ForwardNode* next = current->next;

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

		free(current->value.value);
		current->next = NULL;
		free(current);

		if (cascade) {
			removeForwardNode(list, position, true);
		}
	}
#endif

#ifdef DECLARE_BACKWARDS
	void addBackwardsNode(BackwardsLinkedList* list, ValueField value) {
		BackwardsNode* node = list;

		while (node->previous != NULL) {
			node = node->previous;
		}

		node->previous = (BackwardsNode*)calloc(1, sizeof(BackwardsNode));
		node = node->previous;

		node->value = value;
		node->previous = NULL;
	}

	void removeBackwardsNode(BackwardsLinkedList** list, SizeT position, bool cascade) {
		BackwardsNode* next = NULL;
		BackwardsNode* current = *list;
		BackwardsNode* previous = current->previous;

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

		free(current->value.value);
		current->previous = NULL;
		free(current);

		if (cascade) {
			return;//removeBackwardsNode(list, position, true);
		}
	}
#endif

#ifdef DECLARE_DOUBLE
	void addDoubleNode(DoubleLinkedList* list, ValueField value) {
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

		next->value = value;
		next->next = NULL;
	}

	void removeDoubleNode(DoubleLinkedList** list, SizeT position, bool cascade) {
		DoubleNode* next = NULL;
		DoubleNode* current = *list;
		DoubleNode* previous = current->previous;

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

		free(current->value.value);
		current->previous = NULL;
		free(current);

		if (cascade) {
			return;//removeBackwardsNode(list, position, true);
		}
	}
#endif

void addNode(LinkedList* list, ValueField value) {
	switch (list->listType) {
		#ifdef DECLARE_FORWARD
		case FORWARD:
			addForwardNode((ForwardLinkedList*)list->node, value);
			break;
		#endif
		#ifdef DECLARE_BACKWARDS
		case BACKWARDS:
			addBackwardsNode((BackwardsLinkedList*)list->node, value);
			break;
		#endif
		#ifdef DECLARE_DOUBLE
		case DOUBLE:
			addDoubleNode((DoubleLinkedList*)list->node, value);
			break;
		#endif
		default:
			break;
	}
}

void removeNode(LinkedList** list, SizeT position, bool cascade) {
	LinkedList* usedList = *list;

	switch (usedList->listType) {
		#ifdef DECLARE_FORWARD
		case FORWARD:
			removeForwardNode((ForwardLinkedList**)&usedList->node, position, cascade);
			break;
		#endif

		#ifdef DECLARE_BACKWARDS
		case BACKWARDS:
			removeBackwardsNode((BackwardsLinkedList**)&usedList->node, position, cascade);
			break;
		#endif

		#ifdef DECLARE_DOUBLE
		case DOUBLE:
			removeDoubleNode((DoubleLinkedList**)&usedList->node, position, cascade);
			break;
		#endif

		default:
			break;
	}
}
#endif