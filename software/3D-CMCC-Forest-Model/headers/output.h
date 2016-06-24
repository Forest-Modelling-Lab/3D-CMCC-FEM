/* output.h */
#ifndef OUTPUT_H_
#define OUTPUT_H_

enum {
	AR_DAILY_OUT
	, AR_MONTHLY_OUT
	, AR_YEARLY_OUT

	, GPP_DAILY_OUT
	, GPP_MONTHLY_OUT
	, GPP_YEARLY_OUT

	, NPP_DAILY_OUT
	, NPP_MONTHLY_OUT
	, NPP_YEARLY_OUT

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
void output_free(output_t *ov);
int output_write(const output_t* const vars, const char *const path
					, const int year_start, const int years_count
					, const int x_cells_count
					, const int y_cells_count, const int type);


#endif /* OUTPUT_H_ */