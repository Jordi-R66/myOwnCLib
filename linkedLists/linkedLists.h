#pragma once

#if defined(DECLARE_FORWARD) || defined(DECLARE_BACKWARDS) || defined(DECLARE_DOUBLE)
	#include "../targetDetection/macros.h"
	#include "../targetDetection/types.h"

	#pragma pack(1)

	typedef enum {
		FORWARD = 0,
		BACKWARDS = 1,
		DOUBLE = 2
	} LinkedListTypes;

	#ifdef DECLARE_FORWARD
		typedef struct ForwardNode {
			void* value;
			ForwardNode* next;
		} ForwardNode;

		typedef ForwardNode ForwardLinkedList;

		void addForwardNode(ForwardLinkedList* list, void* value, SizeT valSize);
		void removeForwardNode(ForwardLinkedList* list, void* value, SizeT valSize);
	#endif

	#ifdef DECLARE_BACKWARDS
		typedef struct BackwardsNode {
			BackwardsNode* previous;
			void* value;
		} BackwardsNode;

		typedef BackwardsNode BackwardsLinkedList;

		void addBackwardsNode(BackwardsLinkedList* list, void* value, SizeT valSize);
		void removeBackwardsNode(BackwardsLinkedList* list, void* value, SizeT valSize);
	#endif

	#ifdef DECLARE_DOUBLE
		typedef struct DoubleNode {
			DoubleNode* previous;
			void* value;
			DoubleNode* next;
		} DoubleNode;

		typedef DoubleNode DoubleLinkedList;

		void addDoubleNode(DoubleLinkedList* list, void* value, SizeT valSize);
		void removeDoubleNode(DoubleLinkedList* list, void* value, SizeT valSize);
	#endif

	#pragma pack()

	typedef struct LinkedList {
		void* node;
		LinkedListTypes listType;
	} LinkedList;

	void addNode(LinkedList* list, void* value, SizeT valSize);
	void removeNode(LinkedList* list, void* value, SizeT valSize);

#endif
