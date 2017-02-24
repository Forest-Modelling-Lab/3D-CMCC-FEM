/* output.h */
#ifndef OUTPUT_H_
#define OUTPUT_H_

#include "matrix.h"

typedef enum {
	OUTPUT_TYPE_DAILY
	, OUTPUT_TYPE_MONTHLY
	, OUTPUT_TYPE_YEARLY

	, OUTPUT_TYPES_COUNT
} e_output_types;

enum {
	AR_OUT
	, GPP_OUT
	, NPP_OUT

	, OUTPUT_VARS_COUNT
};

typedef struct {
	int *daily_vars;
	int daily_vars_count;
	int *monthly_vars;
	int monthly_vars_count;
	int *yearly_vars;
	int yearly_vars_count;
	double *daily_vars_value;
	double *monthly_vars_value;
	double *yearly_vars_value;
} output_t;

output_t* output_import(const char *const filename);
void output_push_values(const output_t* const o
							, const cell_t* const c
							, const int month
							, const int day
							, const int year_index
							, const int years_count
							, const int x_cells_count
							, const int y_cells_count
							, const e_output_types type);
void output_free(output_t *ov);
int output_write(const output_t* const vars
					, const char *const path
					, const int year_start
					, const int years_count
					, const int x_cells_count
					, const int y_cells_count, const e_output_types type);

#endif /* OUTPUT_H_ */
