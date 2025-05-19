#pragma once

#if defined(DECLARE_FORWARD) || defined(DECLARE_BACKWARDS) || defined(DECALRE_DOUBLE)
	#pragma pack(1)

	#ifdef DECLARE_FORWARD
		typedef struct ForwardNode {
			void* value;
			ForwardNode* next;
		} ForwardNode;
	#endif

	#ifdef DECLARE_BACKWARDS
		typedef struct BackwardsNode {
			BackwardsNode* previous;
			void* value;
		} BackwardsNode;
	#endif

	#ifdef DECLARE_DOUBLE
		typedef struct DoubleNode {
			DoubleNode* previous;
			void* value;
			DoubleNode* next;
		} DoubleNode;
	#endif

	#pragma pack()
#endif