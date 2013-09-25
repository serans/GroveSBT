/** 
 * Circular buffer implemented for the GroveSBT interface for Arduino
 * @author M.H.Serans
 */
#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#define CIRCULARBUFFER_SIZE 40

typedef struct circularBuffer {
    char data[CIRCULARBUFFER_SIZE];
    byte head;
    byte tail;
    byte len;
};

void c_buffer_init(circularBuffer *buffer) {
    buffer->head=0;
    buffer->tail=0;
    buffer->len = 0;
}

int c_buffer_push(char c, circularBuffer *buffer) {
    if( buffer->len <  CIRCULARBUFFER_SIZE ) {
        buffer->data[buffer->tail] = c;
        buffer->tail = (buffer->tail+1) % CIRCULARBUFFER_SIZE;
        buffer->len++;
        return 0;
    }
    return 1;
}

int c_buffer_pull(char *c, circularBuffer *buffer) {
    if(buffer->len > 0) {
        *c = buffer->data[buffer->head];
        buffer->head = (buffer->head +1) % CIRCULARBUFFER_SIZE;
        buffer->len--;
        return 0;
    }
    return 1;
}

byte c_buffer_len(circularBuffer *buffer) {
    return buffer->len;
}

boolean c_buffer_equals_string(circularBuffer *buffer, char *txt) {
    byte str_size = 0;
    byte i;
    
    for(i=0; txt[i]!='\0'; i++) {
        str_size++;
        
        if(buffer->len < str_size) 
            return false;
        
        if(txt[i] != buffer->data[(buffer->head+i) % CIRCULARBUFFER_SIZE])
            return false;
    }

    if(str_size < buffer->len) return false;
    
    return true;
}

/* Shifts the buffer so that head is in position 0, making it possible to 
 * read buffer->data just like a regular string (provided that it is correctly 
 * null-terminated)
void c_buffer_shift_to_zero(circularBuffer *buffer) {
    char tmp;

    array_reverse(buffer->data, CIRCULARBUFFER_SIZE);
    array_reverse(buffer->data, buffer->head);
    array_reverse(&buffer->data[bufer->head], CIRCULARBUFFER_SIZE-buffer->head);
    
    buffer->head = 0;
    buffer->tail = buffer->len;
}

static void array_reverse(char *A, byte len) {
    byte i;
    char tmp;
    for(i=0; i<len/2; i++) {
        tmp = A[i];
        A[i] = A[len-i];
        A[len-i] = tmp;
    }
}
 */
 
#endif
