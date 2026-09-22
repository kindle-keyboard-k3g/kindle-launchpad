#ifndef _RCE_H_
#define _RCE_H_

void set_rce_defaults() ;
void set_rce_autoenable(char *p) ;
void set_rce_enable_string(char *p) ;
void set_rce_disable_string(char *p) ;
void set_rce_interface(char *p) ;
void set_rce_mcgroup(char *p) ;
void set_rce_port(char *p) ;

int rce_init() ;

void rce_start(int s) ;
void rce_stop(int s) ;

#endif
