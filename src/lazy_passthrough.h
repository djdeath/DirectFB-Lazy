#ifndef __LAZY_PASSTHROUGH_H__
#define __LAZY_PASSTHROUGH_H__

void lazy_operation_init (void);
void lazy_operation_fini (void);

int lazy_operation_addlayer (int layer_num, int buffer_num,
                             int width, int height,
                             int sx, int sy, int sw, int sh,
                             int dx, int dy, int dw, int dh);
int lazy_operation_dellayer (int layer_num);
int lazy_operation_fliplayer (int layer_num, int buffer_num);

#endif /* __LAZY_PASSTHROUGH_H__ */
