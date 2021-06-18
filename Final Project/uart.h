extern void USART1_Init (void);
extern uint8_t SendChar (uint8_t ch);
extern uint8_t GetKey (void);
extern void USART1_IRQHandler (void); 
volatile extern uint8_t inKey;
