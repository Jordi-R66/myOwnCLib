MOCL_DIR ?= libs/myOwnCLib
MOCL_INCLUDES = -I$(MOCL_DIR)
MOCL_SRCS =

# ==========================================
# RÉSOLUTION DES DÉPENDANCES EN CASCADE
# ==========================================

# --- Vectors dépend de Matrices ---
ifdef MOCL_USE_VECTORS
    MOCL_USE_MATRICES = 1
    MOCL_SRCS += $(MOCL_DIR)/maths/vectors/vectors.c
endif

# --- Les collections avancées dépendent de Lists ---
ifdef MOCL_USE_LINKED_LISTS
    MOCL_USE_LISTS = 1
    MOCL_SRCS += $(MOCL_DIR)/collections/linkedLists/linkedLists.c
endif
ifdef MOCL_USE_DICTS
    MOCL_USE_LISTS = 1
    MOCL_SRCS += $(MOCL_DIR)/collections/dicts/dict.c
endif
ifdef MOCL_USE_QUEUES
    MOCL_USE_LISTS = 1
    MOCL_SRCS += $(MOCL_DIR)/collections/queues/queue.c
endif
ifdef MOCL_USE_STACKS
    MOCL_USE_LISTS = 1
    MOCL_SRCS += $(MOCL_DIR)/collections/stacks/stack.c
endif

# --- Résolution de Lists -> Memory ---
ifdef MOCL_USE_LISTS
    MOCL_USE_MEMORY = 1
    MOCL_SRCS += $(MOCL_DIR)/collections/lists/list.c
endif

# --- Matrices -> Memory ---
ifdef MOCL_USE_MATRICES
    MOCL_USE_MEMORY = 1
    MOCL_SRCS += $(MOCL_DIR)/maths/matrices/matrix.c
    
    # Optionnel: Tout charger si on veut les matrices avancées
    ifdef MOCL_USE_ADV_MATRICES
        MOCL_SRCS += $(MOCL_DIR)/maths/matrices/advMatrix.c \
                     $(MOCL_DIR)/maths/matrices/gauss.c \
                     $(MOCL_DIR)/maths/matrices/mlMatrix.c \
                     $(MOCL_DIR)/maths/matrices/tracking.c
    endif
endif

# --- Variable Size Integers -> Strings ---
ifdef MOCL_USE_VAR_INT
    MOCL_USE_STRINGS = 1
    MOCL_SRCS += $(MOCL_DIR)/variableSizeInt/customInteger.c
endif

# --- Strings -> Memory ---
ifdef MOCL_USE_STRINGS
    MOCL_USE_MEMORY = 1
    MOCL_SRCS += $(MOCL_DIR)/strings/customStrings.c
endif

# --- Base (Memory & Endianness) ---
# Ce bloc est toujours évalué en dernier. 
# Si n'importe quel module ci-dessus a mis MOCL_USE_MEMORY à 1, la base est incluse.
ifdef MOCL_USE_MEMORY
    MOCL_SRCS += $(MOCL_DIR)/memory/memfuncs.c
    MOCL_SRCS += $(MOCL_DIR)/endianness/endianness.c
endif