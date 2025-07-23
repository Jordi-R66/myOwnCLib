#pragma once

#if defined(DECLARE_FORWARD) || defined(DECLARE_BACKWARDS) || defined(DECLARE_DOUBLE)

	#include "../targetSpecific/macros.h"
	#include "../targetSpecific/types.h"

	#pragma pack(1)

	typedef struct {
		SizeT size;
		ptr value;
	} ValueField;

	typedef enum {
		FORWARD = 0,
		BACKWARDS = 1,
		DOUBLE = 2
	} LinkedListTypes;

	#ifdef DECLARE_FORWARD
		typedef struct ForwardNode {
			ValueField value;
			ForwardNode* next;
		} ForwardNode;

		typedef ForwardNode ForwardLinkedList;

		void addForwardNode(ForwardLinkedList* list, ValueField value);
		void removeForwardNode(ForwardLinkedList** list, SizeT position, bool cascade);
	#endif

	#ifdef DECLARE_BACKWARDS
		typedef struct BackwardsNode {
			BackwardsNode* previous;
			ValueField value;
		} BackwardsNode;

		typedef BackwardsNode BackwardsLinkedList;

		void addBackwardsNode(BackwardsLinkedList* list, ValueField value);
		void removeBackwardsNode(BackwardsLinkedList** list, SizeT position, bool cascade);
	#endif

	#ifdef DECLARE_DOUBLE
		typedef struct DoubleNode {
			DoubleNode* previous;
			ValueField value;
			DoubleNode* next;
		} DoubleNode;

		typedef DoubleNode DoubleLinkedList;

		void addDoubleNode(DoubleLinkedList* list, ValueField value);
		void removeDoubleNode(DoubleLinkedList** list, SizeT position, bool cascade);
	#endif

	typedef struct LinkedList {
		ptr node;
		LinkedListTypes listType;
	} LinkedList;

	#pragma pack()

	void addNode(LinkedList* list, ValueField value);
	void removeNode(LinkedList** list, SizeT position, bool cascade);

#endif