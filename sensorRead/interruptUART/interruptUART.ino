//Reference: https://stackoverflow.com/questions/53104440/how-to-implement-interrupt-for-serial-usart-communication-for-atmega328p-arduino

#define USART_BAUDRATE 115200
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

void setup()
{

    UCSR0B = (1 << RXEN0) | (1 << TXEN0);   // Turn on the transmission and reception circuitry
    UCSR0C = (1 << UCSZ00) | (1 << UCSZ01); // Use 8-bit character sizes

    UBRR0H = (BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register
    UBRR0L = BAUD_PRESCALE;        // Load lower 8-bits of the baud rate value into the low byte of the UBRR register

    UCSR0B |= (1 << RXCIE0); // Enable the USART Recieve Complete interrupt (USART_RXC)

    sei(); // Enable the Global Interrupt Enable flag so that interrupts can be processed
}

//intrerupt on receive
ISR(USART_RXC_vect)
{
    char ReceivedByte;
    ReceivedByte = UDR0; // Fetch the received byte value into the variable "ByteReceived"
    UDR0 = ReceivedByte; // Echo back the received byte back to the computer
}

//use eeprom function to read/write to EEPROM
void loop()
{
    Serial.write(UDR0);

}