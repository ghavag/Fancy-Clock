#ifdef __cplusplus
extern "C" {
#endif

  void time_init(void);
  void get_time(unsigned char *h, unsigned char *m, unsigned char *s);
  void set_time(unsigned char h, unsigned char m, unsigned char s);
  
#ifdef __cplusplus
}
#endif
