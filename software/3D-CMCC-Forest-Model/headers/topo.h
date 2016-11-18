/* topo.h */
#ifndef TOPO_H_
#define TOPO_H_

enum
{
	TOPO_X = 0
	, TOPO_Y
	, TOPO_ELEV

	, TOPO_VARS_COUNT
};

typedef struct
{
	float values[TOPO_VARS_COUNT];
} topo_t;

topo_t* topo_import(const char *const filename, int*const topos_count);

#endif /* TOPO_H_ */
