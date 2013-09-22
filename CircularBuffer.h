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

#endif
