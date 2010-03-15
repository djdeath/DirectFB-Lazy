#ifndef __LAZY_PASSTHROUGH_H__
#define __LAZY_PASSTHROUGH_H__

void lazy_operation_init (void);
void lazy_operation_fini (void);

int lazy_operation_addlayer (unsigned int layer_id, unsigned int buffer_id,
                             int width, int height,
                             int sx, int sy, int sw, int sh,
                             int dx, int dy, int dw, int dh);
int lazy_operation_dellayer (unsigned int layer_id);
int lazy_operation_fliplayer (unsigned int layer_id, unsigned int buffer_id);

int lazy_operation_addbuffer (int width, int height, int bpp, unsigned int *buffer_id);
int lazy_operation_delbuffer (unsigned int buffer_id);

#endif /* __LAZY_PASSTHROUGH_H__ */
