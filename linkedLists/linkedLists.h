#pragma once

#if defined(DECLARE_FORWARD) || defined(DECLARE_BACKWARDS) || defined(DECLARE_DOUBLE)
	#include "../targetDetection/macros.h"
	#include "../targetDetection/types.h"

	#pragma pack(1)

	#ifdef DECLARE_FORWARD
		typedef struct ForwardNode {
			void* value;
			ForwardNode* next;
		} ForwardNode;

		typedef ForwardNode ForwardLinkedList;
	#endif

	#ifdef DECLARE_BACKWARDS
		typedef struct BackwardsNode {
			BackwardsNode* previous;
			void* value;
		} BackwardsNode;

		typedef BackwardsNode BackwardsLinkedList;
	#endif

	#ifdef DECLARE_DOUBLE
		typedef struct DoubleNode {
			DoubleNode* previous;
			void* value;
			DoubleNode* next;
		} DoubleNode;

		typedef DoubleNode DoubleLinkedList;
	#endif

	#pragma pack()

	#ifdef DECLARE_FORWARD
		typedef ForwardLinkedList LinkedList;
	#elif defined(DECLARE_BACKWARDS)
		typedef BackwardsLinkedList LinkedList;
	#else
		typedef DoubleLinkedList LinkedList;
	#endif



#endif
