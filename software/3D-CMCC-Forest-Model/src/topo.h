/* topo.h */
#ifndef TOPO_H_
#define TOPO_H_

enum {
	TOPO_ELEV = 0 /* fuck standard! */
	, TOPO_VALUES_COUNT
};

typedef struct {
	float values[TOPO_VALUES_COUNT];
} topo_t;

topo_t* topo_new(void);
void topo_clear(topo_t* const t);
int topo_import(topo_t *const t, const char *const filename, const int x, const int y);

#endif
