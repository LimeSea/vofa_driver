#include "./vofa.h"
#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <float.h>
#include <stdio.h>

#if (CONFIG_VOFA_CONVERT_FLOAT_TO_LITTLE == 1)
#define VOFA_FLOAT_COVERT(x)                        \
    ( (((*(uint32_t *)&(x)) & 0xff000000) >> 24) |        \
        (((*(uint32_t *)&(x)) & 0x00ff0000) >> 8) |       \
        (((*(uint32_t *)&(x)) & 0x0000ff00) << 8) |       \
        (((*(uint32_t *)&(x)) & 0x000000ff) << 24) )
#endif

#if CONFIG_VOFA_BUF_SIZE < 4
#error "CONFIG_VOFA_BUF_SIZE must should over 4"
#endif

#define CONFIG_VOFA_FIREWATER_FORMAT "%f"


int vofa_init(vofa_t *vofa, int(*send)(const uint8_t *, size_t )){
    if(vofa == NULL || send == NULL) return -1;

    vofa->send = send;
    
    return 0;
}

int vofa_write_firewater(vofa_t *vofa, char *any, ...){
    va_list valist;
    double data = 0;
    char *p;
    char *p_end;
    int count = 0;
    char buf[CONFIG_VOFA_BUF_SIZE];

    if(vofa == NULL || vofa->send == NULL) return -1;

    p = buf;
    p_end = p + CONFIG_VOFA_BUF_SIZE;

    if(any != NULL){
        count = snprintf(p,CONFIG_VOFA_BUF_SIZE,"%s:",any);
    }

    p += count;
    if(p >= p_end || count == -1) return -1;

    va_start(valist, any);
    
    data = va_arg(valist, double);
    while(data != DBL_MAX){
        count = snprintf(p, CONFIG_VOFA_BUF_SIZE - (p - buf),CONFIG_VOFA_FIREWATER_FORMAT",",data);
        p += count;
        if(p >= p_end || count == -1) goto _va_fail;
        data = va_arg(valist, double);
    };
    
    va_end(valist);

    if(p - buf){
        if(*(p-1) == ','){
            p-=1;
        }     
    }
    
    count = snprintf(p, CONFIG_VOFA_BUF_SIZE - (p - buf),"\n");
    p += count;
    if(p >= p_end && count == -1) return -1;

    return vofa->send((uint8_t *)buf,p-buf);

    _va_fail:
    va_end(valist);
    return -1;
}

int vofa_write_justfloat(vofa_t *vofa, ...){
    va_list valist;
    size_t float_buf_size = 0;
    size_t now_float_count = 0;
    uint8_t tail[]={0x00, 0x00, 0x80, 0x7f};
    char buf[CONFIG_VOFA_BUF_SIZE];
    float *p;
    double data;

    if(vofa == NULL || vofa->send == NULL) return -1;

    p = (float *)buf;

    float_buf_size = CONFIG_VOFA_BUF_SIZE/sizeof(float);

    va_start(valist, vofa);
    
    data = va_arg(valist, double);
    while(data != DBL_MAX && now_float_count < float_buf_size){
        *p = data;

        #if (CONFIG_VOFA_CONVERT_FLOAT_TO_LITTLE == 1)
        uint32_t c_d = VOFA_FLOAT_COVERT(*p);
        *p = *(float *)&c_d;
        #endif

        p += 1;
        now_float_count++;
        data = va_arg(valist, double);
    };
    va_end(valist);
    
    if(now_float_count > float_buf_size && data != DBL_MAX){
        return -1;
    }

    if(vofa->send((uint8_t *)buf, now_float_count * sizeof(float)) == 0){
        return vofa->send((uint8_t *)tail,sizeof(tail)/sizeof(uint8_t));
    };

    return -1;

}
