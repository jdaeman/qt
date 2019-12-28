#ifndef __ROUTE_H_
#define __ROUTE_H_

#ifdef __cplusplus
extern "C"
{
#endif

int route_init(void);
void route_exit(void);

int default_route_lookup(int ifindex, unsigned int * route, unsigned char * neigh);


#ifdef __cplusplus
}
#endif

#endif
