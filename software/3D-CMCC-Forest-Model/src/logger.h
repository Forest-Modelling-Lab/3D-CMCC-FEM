/* logger.h */
#ifndef LOGGER_H_
#define LOGGER_H_

typedef struct {
	FILE *f;
	int file_output;
	int std_output;
} logger_t;

logger_t* logger_new(const char* const filename);
#define logger_enable_file(p)	((p)->file_output=1)
#define logger_disable_file(p)	((p)->file_output=0)
#define logger_enable_std(p)	((p)->std_output=1)
#define logger_disable_std(p)	((p)->std_output=0)
void logger(logger_t *p, const char *text, ...);
void logger_close(logger_t* p);

#endif /* LOGGER_H_ */