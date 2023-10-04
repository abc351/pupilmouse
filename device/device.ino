#include "Arduino.h"
char buf[1600];
char* buf_end=buf+1600;
#define resol 10
void setup() {
  Serial.begin(115200);
  DDRC=0xEF;
}

void loop() {
  while(!Serial.available());
  char a=Serial.read();
  if(a=='r'){
    for(char* bufp=buf;bufp!=buf_end;*(bufp++)=PINC) delayMicroseconds(78);
    for(char* bufp=buf;bufp!=buf_end;bufp++) *bufp=(*bufp&0x10?'A':' ');
     for(char* bufp=buf;bufp!=buf_end;bufp++) Serial.write(*bufp);
     Serial.write("AAAAAAAAAAAAA"); delay(500); Serial.flush();
  }
  else{
    buf[0]=a;
    for(char* bufp=buf+1;bufp!=buf_end;bufp++) {while(!Serial.available());*bufp=Serial.read();}
    for(char* bufp=buf;bufp!=buf_end;bufp++) *bufp=((*bufp)!=' '?0x00:0x2F);
    for(char* bufp=buf;bufp!=buf_end;bufp++) {
      PORTC=(*bufp); delayMicroseconds(27);
      PORTC=0; delayMicroseconds(27);
      PORTC=(*bufp); delayMicroseconds(27);
      PORTC=0; delayMicroseconds(27);
    }
    delay(500);
    Serial.flush();
  }
}
