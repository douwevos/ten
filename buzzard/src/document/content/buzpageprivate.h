#include "buzpage.h"

struct _BuzPageContext {
	AAltObjectContext parent;
	BuzEnrichmentData *enrichment_data;
	int enriched_count;
	ALock *lock;
};

typedef struct _BuzPageContext BuzPageContext;
