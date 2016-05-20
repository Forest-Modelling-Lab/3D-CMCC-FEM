/* topo.h */
#ifndef TOPO_H_
#define TOPO_H_

enum {
	TOPO_ELEV = 0, /* fuck standard! */

	TOPO_VALUES_COUNT
};

typedef struct {
	float values[TOPO_VALUES_COUNT];
} topo_t;

/*
	err is 0 if ok
	err is 1 if out of memory
	err is 2 if file do not exist
	err is 3 if file is not imported correctly
*/
topo_t *topo_import(const char *const filename, int *const err);

#endif
