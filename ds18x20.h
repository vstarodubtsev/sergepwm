#ifndef DS18X20_H_
#define DS18X20_H_

int  temp_18b20(void);
void convert (unsigned int td, char* int_part, unsigned int* frac_part);

#endif
