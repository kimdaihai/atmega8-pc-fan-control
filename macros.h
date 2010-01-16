// macros
#define SETBIT(ADDRESS,BIT) (ADDRESS |= (1<<BIT))
#define CLEARBIT(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT))
#define FLIPBIT(ADDRESS,BIT) (ADDRESS ^= (1<<BIT))
#define CHECKBIT(ADDRESS,BIT) (ADDRESS & (1<<BIT))

#define SETBITMASK(x,y) (x |= (y))
#define CLEARBITMASK(x,y) (x &= (~y))
#define FLIPBITMASK(x,y) (x ^= (y))
#define CHECKBITMASK(x,y) (x & (y))

#define VARFROMCOMB(x, y) x
#define BITFROMCOMB(x, y) y

#define C_SETBIT(comb) SETBIT(VARFROMCOMB(comb), BITFROMCOMB(comb))
#define C_CLEARBIT(comb) CLEARBIT(VARFROMCOMB(comb), BITFROMCOMB(comb))
#define C_FLIPBIT(comb) FLIPBIT(VARFROMCOMB(comb), BITFROMCOMB(comb))
#define C_CHECKBIT(comb) CHECKBIT(VARFROMCOMB(comb), BITFROMCOMB(comb))

// lights
#define Lifeline	PORTC, 5
#define Red_LED		PORTC, 4
#define Yellow_LED	PORTC, 3
#define Green_LED	PORTC, 2

// buttons
#define Minus_REF	PINB, 5
#define Plus_REF	PINB, 4

//delays
#define F_CPU	16000000
#define Delay_ms(__ms) \
    if((unsigned long) (F_CPU/1000.0 * __ms) != F_CPU/1000.0 * __ms)\
          __builtin_avr_delay_cycles((unsigned long) ( F_CPU/1000.0 * __ms)+1);\
    else __builtin_avr_delay_cycles((unsigned long) ( F_CPU/1000.0 * __ms))
#define Delay_s(__s) \
    if((unsigned long) (F_CPU/1.0 * __s) != F_CPU/1.0 * __s)\
          __builtin_avr_delay_cycles((unsigned long) ( F_CPU/1.0 * __s)+1);\
    else __builtin_avr_delay_cycles((unsigned long) ( F_CPU/1.0 * __s))
