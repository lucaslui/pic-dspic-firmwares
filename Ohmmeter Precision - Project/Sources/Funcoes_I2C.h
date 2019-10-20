#ifndef FUNCOES_I2C_H
#define	FUNCOES_I2C_H

void Inicializando_I2C();
void i2c_waitForIdle();
void i2c_start();
void i2c_repStart();
void i2c_stop();
int i2c_read(unsigned char ack);
unsigned char i2c_write(unsigned char i2cWriteData);
void write_msp(unsigned char data);
unsigned int read_msp();

#endif	/* FUNCOES_I2C_H */

