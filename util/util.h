
#define DEBUG_LEVEL_ 3
#ifdef DEBUG_LEVEL_
#define dp(n, fmt, args...) \
    if (DEBUG_LEVEL_ <= n)  \
    fprintf(stderr, "%s:%d," fmt, __FILE__, __LINE__, ##args)
#define dp0(n, fmt)        \
    if (DEBUG_LEVEL_ <= n) \
    fprintf(stderr, "%s:%d," fmt, __FILE__, __LINE__)
#define _dp(n, fmt, args...) \
    if (DEBUG_LEVEL_ <= n)   \
    fprintf(stderr, " " fmt, ##args)
#else /* DEBUG_LEVEL_ */
#define dp(n, fmt, args...)
#define dp0(n, fmt)
#define _dp(n, fmt, args...)
#endif /* DEBUG_LEVEL_ */

#define ONE_LINE 80
#define PAST 0
#define PRESENT 1
#define JIFFIES_NUM 4

void get_user_key(char *mac);
int get_network_status(char* network);

void convrt_mac(const char *data, char *cvrt_str, int sz);
void convert_license(char* mac);
void get_cpu_usage();
int get_send_byte();
float get_cpu_temp();
float get_gpu_temp();

 
enum jiffy{USER, USER_NICE, SYSTEM, IDLE} jiffy_enum;