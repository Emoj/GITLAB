#include <built_in.h>
#define filename "TIMELOG.txt"


sbit Mmc_Chip_Select at PORTG.B1;
sbit Mmc_Chip_Select_Direction at DDRG.B1;

char hour, minute, second;

char file_contents[10] = "XX:XX:XX\n";

unsigned long i;

void i2c_write(unsigned char data2) {

    TWDR = data2;
    TWCR = 1<<(TWINT)|1<<(TWEN);
    
    while(!(TWCR&1<<(TWINT)));
}

unsigned int i2c_readwithoutack(void) {

    TWCR = 1<<(TWINT) | 1<<(TWEN);
    
    while(!(TWCR&1<<(TWINT)));
    
    return TWDR;
}
unsigned int i2c_readwithack(void) {

    TWCR = 1<<(TWINT) | 1<<(TWEN)|1<<(TWEA);
    
    while(!(TWCR&1<<(TWINT)));
        
    return TWDR;
}

void i2c_init(void) {

    TWSR = 0x00;
    TWBR = 0x47;
    TWCR = 0x04;
}

void i2c_start(void) {

    TWCR = 1<<(TWINT) | 1<<(TWSTA)|1<<(TWEN);
    while((TWCR &1<<(TWINT))==0);
}

void i2c_stop(void) {

    TWCR = 1<<(TWINT)|1<<(TWEN)|1<<(TWSTO);
}

void interrupt_ISR(void) org IVT_ADDR_INT4 {

    i2c_start();
    i2c_write(0xA0);
    i2c_write(0x02);
    i2c_start();
    i2c_write(0xA1);
    second = i2c_readwithack();
    minute = i2c_readwithack();
    hour = (0x3f&i2c_readwithoutack());
    i2c_stop();

    Mmc_Fat_Assign(filename, 0x80);
    Mmc_Fat_Append();

    file_contents[0] = hour / 10 + '0';
    file_contents[1] = hour % 10 + '0';
    file_contents[3] = minute / 16 + '0';
    file_contents[4] = minute % 16 + '0';
    file_contents[6] = second / 16 + '0';
    file_contents[7] = second % 16 + '0';
    Mmc_Fat_Write(file_contents, 10);
    
    UART1_write('x');
}

void main() {

    DDRD = 0xFF;
    
    UART1_Init(9600);
     UART1_Write_Text("Init");
    Delay_100ms();
    SPI1_Init_Advanced(_SPI_MASTER, _SPI_FCY_DIV128, _SPI_CLK_LO_LEADING);
    Spi_Rd_Ptr = SPI1_Read;

    EIMSK = (1 << INT4);
    SREG_I_bit = 1;
    i2c_init();

    if (!Mmc_Fat_Init()) {

        //SPI1_Init_Advanced(_SPI_MASTER, _SPI_FCY_DIV2,_SPI_CLK_LO_LEADING);
        while(1);
    }
}