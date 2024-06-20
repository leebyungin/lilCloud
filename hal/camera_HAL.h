#ifndef _CAMERA_HAL_H_
#define _CAMERA_HAL_H_

#ifdef __cplusplus
extern "C"{
#endif

int camera_open(void);
int camera_take_picture(void);
int camera_close(void);
int camera_dump(void);

#ifdef __cplusplus
}
#endif

#endif
