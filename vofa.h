#ifndef _VOFA_H_
#define _VOFA_H_

#include <stddef.h>
#include <stdint.h>
#include <float.h>

/* sample:
int vofa_send(const uint8_t * buf, size_t size){
    send(buf,size) // Your sending function
    return 0;
}

int main(void){
    vofa_t vofa;
    vofa_init(&vofa,vofa_send);
    vofa_write_firewater_data(&vofa, "test",1.1,1.2);
    vofa_write_firewater_data(&vofa, "test",1.1,1.2,1.3);
    vofa_write_justfloat_data(&vofa, 1.1,1.2);
    vofa_write_justfloat_data(&vofa, 1.1,1.2,1.3);
}
*/

#define CONFIG_VOFA_CONVERT_FLOAT_TO_LITTLE  0 
#define CONFIG_VOFA_BUF_SIZE 50

#define vofa_write_firewater_data(vofa, any, ...)               \
    vofa_write_firewater(vofa, any, ## __VA_ARGS__, DBL_MAX)  


#define vofa_write_justfloat_data(vofa, ...)                    \
    vofa_write_justfloat(vofa, ## __VA_ARGS__, DBL_MAX)

typedef struct{
    int(*send)(const uint8_t *buf, size_t size);
}vofa_t;


int vofa_init(vofa_t *vofa, int(*send)(const uint8_t *, size_t ));


int vofa_write_firewater(vofa_t *vofa, char *any, ...);
int vofa_write_justfloat(vofa_t *vofa, ...);


#endif
