/* topo.h */
#ifndef TOPO_H_
#define TOPO_H_

enum {
	TOPO_ELEV = 0
	, TOPO_VARS_COUNT
};

typedef struct {
	float values[TOPO_VARS_COUNT];
} topo_t;

topo_t *topo_new(void);
int topo_import(topo_t *const t, const char *const filename, const int x, const int y);

#endif /* TOPO_H_ */
