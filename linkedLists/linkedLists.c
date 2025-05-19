#define DECLARE_FORWARD
#define DECLARE_BACKWARDS
#define DECLARE_DOUBLE

#include "linkedLists.h"

#if defined(DECLARE_FORWARD) || defined(DECLARE_BACKWARDS) || defined(DECLARE_DOUBLE)

#include "../memory/memfuncs.h"
#include <stdlib.h>

#ifdef DECLARE_FORWARD
	void addForwardNode(ForwardLinkedList* list, void* value, SizeT valSize);
	void removeForwardNode(ForwardLinkedList* list, void* value, SizeT valSize);
#endif

#ifdef DECLARE_BACKWARDS
	void addBackwardsNode(BackwardsLinkedList* list, void* value, SizeT valSize);
	void removeBackwardsNode(BackwardsLinkedList* list, void* value, SizeT valSize);
#endif

#ifdef DECLARE_DOUBLE
	void addDoubleNode(DoubleLinkedList* list, void* value, SizeT valSize);
	void removeDoubleNode(DoubleLinkedList* list, void* value, SizeT valSize);
#endif

void addNode(LinkedList* list, void* value, SizeT valSize);
void removeNode(LinkedList* list, void* value, SizeT valSize);

#endif