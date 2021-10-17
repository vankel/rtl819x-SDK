#include <stdio.h>  
#include <assert.h>  
#include <errno.h>  
  
#ifdef WIN32  
#include <winsock2.h>  
#include <stdlib.h>  
#include <io.h>  
#else  
  
#include <stdlib.h>  
#include <unistd.h>  
#include <string.h>  
#include <sys/ioctl.h>  
#include <sys/socket.h>  
#include <sys/time.h>  
#include <sys/types.h>   
#include <arpa/inet.h>  
#include <fcntl.h>  
#include <netdb.h>  
#include <netinet/in.h>  
#include <arpa/nameser.h>  
#include <resolv.h>  
#include <net/if.h>  
  
#endif  
  
#include "udp.h"  
#include "stun.h"  

  
#ifdef HAS_MESSAGE_INTEGRITY  
void  
computeHmac(char* hmac, const char* input, int length, const char* key, int keySize);  
#endif  
  
static bool   
stunParseAtrAddress( char* body, unsigned int hdrLen,  StunAtrAddress4* result )  
{  
   if ( hdrLen != 8 )  
   {  
      printf("hdrLen wrong for Address\n");  
      return false;  
   }  
   result->pad = *body++;  
   result->family = *body++;  
   if (result->family == IPv4Family)  
   {  
      UInt16 nport;  
      UInt32 naddr;  
      memcpy(&nport, body, 2); body+=2;  
      result->ipv4.port = ntohs(nport);  
        
//      UInt32 naddr;  
      memcpy(&naddr, body, 4); body+=4;  
      result->ipv4.addr = ntohl(naddr);  
      return true;  
   }  
   else if (result->family == IPv6Family)  
   {  
      printf("ipv6 not supported\n");  
   }  
   else  
   {  
      printf("bad address family: %d\n", result->family);  
   }  
     
   return false;  
}  
  
static bool   
stunParseAtrChangeRequest( char* body, unsigned int hdrLen,  StunAtrChangeRequest* result )  
{  
   printf("hdr length = %d expecting %d\n", hdrLen, sizeof(result));  
     
   if ( hdrLen != 4 )  
   {  
      printf("Incorrect size for ChangeRequest\n");  
      return false;  
   }  
   else  
   {  
      memcpy(&result->value, body, 4);  
      result->value = ntohl(result->value);  
      return true;  
   }  
}  
  
static bool   
stunParseAtrError( char* body, unsigned int hdrLen,  StunAtrError* result )  
{  
   if ( hdrLen >= sizeof(result) )  
   {  
      printf("head on Error too large\n");  
      return false;  
   }  
   else  
   {  
      memcpy(&result->pad, body, 2); body+=2;  
      result->pad = ntohs(result->pad);  
      result->errorClass = *body++;  
      result->number = *body++;  
        
      result->sizeReason = hdrLen - 4;  
      memcpy(&result->reason, body, result->sizeReason);  
      result->reason[result->sizeReason] = 0;  
      return true;  
   }  
}  
  
static bool   
stunParseAtrUnknown( char* body, unsigned int hdrLen,  StunAtrUnknown* result )  
{  
   if ( hdrLen >= sizeof(result) )  
   {  
      return false;  
   }  
   else  
   {  
      int i;  
      if (hdrLen % 4 != 0) return false;  
      result->numAttributes = hdrLen / 4;  
//      int i;  
//      for (int i=0; i<result.numAttributes; i++)  
      for (i=0; i<result->numAttributes; i++)  
      {  
         memcpy(&result->attrType[i], body, 2); body+=2;  
         result->attrType[i] = ntohs(result->attrType[i]);  
      }  
      return true;  
   }  
}  
  
  
static bool   
stunParseAtrString( char* body, unsigned int hdrLen,  StunAtrString* result )  
{  
   if ( hdrLen >= STUN_MAX_STRING )  
   {  
      printf("String is too large\n");  
      return false;  
   }  
   else  
   {  
      if (hdrLen % 4 != 0)  
      {  
         printf("Bad length string %d\n", hdrLen);  
         return false;  
      }  
        
      result->sizeValue = hdrLen;  
      memcpy(&result->value, body, hdrLen);  
      result->value[hdrLen] = 0;  
      return true;  
   }  
}  
  
  
static bool   
stunParseAtrIntegrity( char* body, unsigned int hdrLen,  StunAtrIntegrity* result )  
{  
   if ( hdrLen != 20)  
   {  
      printf("MessageIntegrity must be 20 bytes\n");  
      return false;  
   }  
   else  
   {  
      memcpy(&result->hash, body, hdrLen);  
      return true;  
   }  
}  
  
bool  
stunParseMessage( char* buf, unsigned int bufLen, StunMessage* msg)  
{  
   char* body;  
   unsigned int size;  
  
   printf("Received stun message: %d bytes\n", bufLen);  
   memset(msg, 0, sizeof(*msg));  
     
   if(sizeof(StunMsgHdr) > bufLen)  
   {  
      printf("Bad message\n");  
      return false;  
   }  
     
   memcpy(&msg->msgHdr, buf, sizeof(StunMsgHdr));  
   msg->msgHdr.msgType = ntohs(msg->msgHdr.msgType);  
   msg->msgHdr.msgLength = ntohs(msg->msgHdr.msgLength);  
  
   if (msg->msgHdr.msgLength + sizeof(StunMsgHdr) != bufLen)  
   {  
      printf("Message header length doesn't match message size: %d - %d\n", msg->msgHdr.msgLength, bufLen);  
      return false;  
   }  
  
   body = buf + sizeof(StunMsgHdr);  
   size = msg->msgHdr.msgLength;  
  
   //clog << "bytes after header = " << size << endl;  
     
   while ( size > 0 )  
   {  
      // !jf! should check that there are enough bytes left in the buffer  
      StunAtrHdr* attr = (StunAtrHdr*)body;  
        
      unsigned int attrLen = ntohs(attr->length);  
      int atrType = ntohs(attr->type);  
        
      //clog << "Found attribute type=" << AttrNames[atrType] << " length=" << attrLen << endl;  
      if ( attrLen+4 > size )   
      {  
         printf("claims attribute is larger than size of message (attribute type=%d)\n", atrType);  
         return false;  
      }  
  
      body += 4; // skip the length and type in attribute header  
      size -= 4;  
  
      switch ( atrType )  
      {  
         case MappedAddress:  
            msg->hasMappedAddress = true;  
            if ( stunParseAtrAddress(  body,  attrLen,  &msg->mappedAddress )== false )  
            {  
               printf("problem parsing MappedAddress\n");  
               return false;  
            }  
            else  
            {  
               printf("MappedAddress = %08X:%d\n", msg->mappedAddress.ipv4.addr, msg->mappedAddress.ipv4.port);  
            }  
              
            break;    
  
         case ResponseAddress:  
            msg->hasResponseAddress = true;  
            if ( stunParseAtrAddress(  body,  attrLen,  &msg->responseAddress )== false )  
            {  
               printf("problem parsing ResponseAddress\n");  
               return false;  
            }  
            else  
            {  
               printf("ResponseAddress = %08X:%d\n", msg->responseAddress.ipv4.addr, msg->responseAddress.ipv4.port);  
            }  
            break;    
  
         case ChangeRequest:  
            msg->hasChangeRequest = true;  
            if (stunParseAtrChangeRequest( body, attrLen, &msg->changeRequest) == false)  
            {  
               printf("problem parsing ChangeRequest\n");  
               return false;  
            }  
            else  
            {  
               printf("ChangeRequest = %d\n", msg->changeRequest.value);  
            }  
            break;  
  
         case SourceAddress:  
            msg->hasSourceAddress = true;  
            if ( stunParseAtrAddress(  body,  attrLen,  &msg->sourceAddress )== false )  
            {  
               printf("problem parsing SourceAddress\n");  
               return false;  
            }  
            else  
            {  
               printf("SourceAddress = %08X:%d\n", msg->sourceAddress.ipv4.addr, msg->sourceAddress.ipv4.port);  
            }  
            break;    
  
         case ChangedAddress:  
            msg->hasChangedAddress = true;  
            if ( stunParseAtrAddress(  body,  attrLen,  &msg->changedAddress )== false )  
            {  
               printf("problem parsing ChangedAddress\n");  
               return false;  
            }  
            else  
            {  
               printf("ChangedAddress = %08X:%d\n", msg->changedAddress.ipv4.addr, msg->changedAddress.ipv4.port);  
            }  
            break;    
              
         case Username:   
            msg->hasUsername = true;  
            if (stunParseAtrString( body, attrLen, &msg->username) == false)  
            {  
               printf("problem parsing Username\n");  
               return false;  
            }  
            else  
            {  
               printf("Username = %s\n", msg->username.value);  
            }  
  
            break;  
  
         case Password:   
            msg->hasPassword = true;  
            if (stunParseAtrString( body, attrLen, &msg->password) == false)  
            {  
               printf("problem parsing Password\n");  
               return false;  
            }  
            else  
            {  
               printf("Password = %s\n", msg->password.value);  
            }  
            break;  
  
         case MessageIntegrity:  
            msg->hasMessageIntegrity = true;  
            if (stunParseAtrIntegrity( body, attrLen, &msg->messageIntegrity) == false)  
            {  
               printf("problem parsing MessageIntegrity\n");  
               return false;  
            }  
            else  
            {  
               //clog << "MessageIntegrity = " << msg.messageIntegrity.hash << endl;  
            }  
  
            // read the current HMAC  
            // look up the password given the user of given the transaction id   
            // compute the HMAC on the buffer  
            // decide if they match or not  
            break;  
  
         case ErrorCode:  
            msg->hasErrorCode = true;  
            if (stunParseAtrError(body, attrLen, &msg->errorCode) == false)  
            {  
               printf("problem parsing ErrorCode\n");  
               return false;  
            }  
            else  
            {  
               printf("ErrorCode = %d %d %s\n", (int)(msg->errorCode.errorClass), (int)(msg->errorCode.number), msg->errorCode.reason);  
            }  
              
            break;  
  
         case UnknownAttribute:  
            msg->hasUnknownAttributes = true;  
            if (stunParseAtrUnknown(body, attrLen, &msg->unknownAttributes) == false)  
            {  
               printf("problem parsing UnknownAttribute\n");  
               return false;  
            }  
            break;  
  
         case ReflectedFrom:  
            msg->hasReflectedFrom = true;  
            if ( stunParseAtrAddress(  body,  attrLen,  &msg->reflectedFrom )== false )  
            {  
               printf("problem parsing ReflectedFrom\n");  
               return false;  
            }  
            break;    
  
         default:  
            printf("Unknown attribute: %d\n", atrType);  
            if ( atrType <= 0x7FFF ) return false;  
      }  
      
      body += attrLen;  
      size -= attrLen;  
   }  
      
   return true;  
}  
  
static char*   
encode16(char* buf, UInt16 data)  
{  
   UInt16 ndata = htons(data);  
   memcpy(buf, (void*)(&ndata), sizeof(UInt16));  
   return buf + sizeof(UInt16);  
}  
  
static char*   
encode32(char* buf, UInt32 data)  
{  
   UInt32 ndata = htonl(data);  
//   memcpy(buf, reinterpret_cast<void*>(&ndata), sizeof(UInt32));  
   memcpy(buf, (void*)(&ndata), sizeof(UInt32));  
   return buf + sizeof(UInt32);  
}  
  
  
static char*   
encode(char* buf, const char* data, unsigned int length)  
{  
   memcpy(buf, data, length);  
   return buf + length;  
}  
  
  
static char*   
encodeAtrAddress4(char* ptr, UInt16 type, const StunAtrAddress4* atr)  
{  
   ptr = encode16(ptr, type);  
   ptr = encode16(ptr, 8);  
   *ptr++ = atr->pad;  
   *ptr++ = IPv4Family;  
   ptr = encode16(ptr, atr->ipv4.port);  
   ptr = encode32(ptr, atr->ipv4.addr);  
  
   return ptr;  
}  
  
static char*   
encodeAtrChangeRequest(char* ptr, const StunAtrChangeRequest* atr)  
{  
   ptr = encode16(ptr, ChangeRequest);  
   ptr = encode16(ptr, 4);  
   ptr = encode32(ptr, atr->value);  
   return ptr;  
}  
  
static char*   
encodeAtrError(char* ptr, const StunAtrError* atr)  
{  
   ptr = encode16(ptr, ErrorCode);  
   ptr = encode16(ptr, 6 + atr->sizeReason);  
   ptr = encode16(ptr, atr->pad);  
   *ptr++ = atr->errorClass;  
   *ptr++ = atr->number;  
   ptr = encode(ptr, atr->reason, atr->sizeReason);  
   return ptr;  
}  
  
static char*   
encodeAtrUnknown(char* ptr, const StunAtrUnknown* atr)  
{  
   int i;  
   ptr = encode16(ptr, UnknownAttribute);  
   ptr = encode16(ptr, 2+2*atr->numAttributes);  
//   for (int i=0; i<atr->numAttributes; i++)  
   for (i=0; i<atr->numAttributes; i++)  
   {  
      ptr = encode16(ptr, atr->attrType[i]);  
   }  
   return ptr;  
}  
  
static char*   
encodeAtrString(char* ptr, UInt16 type, const StunAtrString* atr)  
{  
   assert(atr->sizeValue % 4 == 0);  
     
   ptr = encode16(ptr, type);  
   ptr = encode16(ptr, atr->sizeValue);  
   ptr = encode(ptr, atr->value, atr->sizeValue);  
   return ptr;  
}  
  
  
#ifdef HAS_MESSAGE_INTEGRITY     
static char*   
encodeAtrIntegrity(char* ptr, const StunAtrIntegrity* atr)  
{  
   ptr = encode16(ptr, MessageIntegrity);  
   ptr = encode16(ptr, 20);  
   ptr = encode(ptr, atr->hash, sizeof(atr->hash));  
   return ptr;  
}  
#endif  
  
unsigned int  
stunEncodeMessage( const StunMessage* msg, char* buf, unsigned int bufLen, const StunAtrString* password)  
{  
   char *ptr, *lengthp;  
   assert(bufLen >= sizeof(StunMsgHdr));  
   ptr = buf;  
     
   ptr = encode16(ptr, msg->msgHdr.msgType);  
   lengthp = ptr;  
   ptr = encode16(ptr, 0);  
   ptr = encode(ptr, (const char*)(msg->msgHdr.id.octet), sizeof(msg->msgHdr.id));  
     
   printf("Encoding stun message: \n");  
   if (msg->hasMappedAddress)  
   {  
      printf("Encoding MappedAddress: %08X:%d\n", msg->mappedAddress.ipv4.addr, msg->mappedAddress.ipv4.port);  
      ptr = encodeAtrAddress4 (ptr, MappedAddress, &msg->mappedAddress);  
   }  
   if (msg->hasResponseAddress)  
   {  
      printf("Encoding ResponseAddress: %08X:%d\n", msg->responseAddress.ipv4.addr, msg->responseAddress.ipv4.port);  
      ptr = encodeAtrAddress4(ptr, ResponseAddress, &msg->responseAddress);  
   }  
   if (msg->hasChangeRequest)  
   {  
      printf("Encoding ChangeRequest: %d\n", msg->changeRequest.value);  
      ptr = encodeAtrChangeRequest(ptr, &msg->changeRequest);  
   }  
   if (msg->hasSourceAddress)  
   {  
      printf("Encoding SourceAddress: %08X:%d\n", msg->sourceAddress.ipv4.addr, msg->sourceAddress.ipv4.port);  
      ptr = encodeAtrAddress4(ptr, SourceAddress, &msg->sourceAddress);  
   }  
   if (msg->hasChangedAddress)  
   {  
      printf("Encoding ChangedAddress: %08X:%d\n", msg->changedAddress.ipv4.addr, msg->changedAddress.ipv4.port);  
      ptr = encodeAtrAddress4(ptr, ChangedAddress, &msg->changedAddress);  
   }  
   if (msg->hasUsername)  
   {  
      printf("Encoding Username: %s\n", msg->username.value);  
      ptr = encodeAtrString(ptr, Username, &msg->username);  
   }  
   if (msg->hasPassword)  
   {  
      printf("Encoding Password: %s\n", msg->password.value);  
      ptr = encodeAtrString(ptr, Password, &msg->password);  
   }  
   if (msg->hasErrorCode)  
   {  
      printf("Encoding ErrorCode: class=%d number= %d reason=%s\n",   
            (int)(msg->errorCode.errorClass),  
            (int)(msg->errorCode.number),   
            msg->errorCode.reason);  
        
      ptr = encodeAtrError(ptr, &msg->errorCode);  
   }  
   if (msg->hasUnknownAttributes)  
   {  
      printf("Encoding UnknownAttribute: ???\n");  
      ptr = encodeAtrUnknown(ptr, &msg->unknownAttributes);  
   }  
   if (msg->hasReflectedFrom)  
   {  
      printf("Encoding ReflectedFrom: %08X:%d\n", msg->reflectedFrom.ipv4.addr, msg->reflectedFrom.ipv4.port);  
      ptr = encodeAtrAddress4(ptr, ReflectedFrom, &msg->reflectedFrom);  
   }  
#ifdef HAS_MESSAGE_INTEGRITY     
   if (password->sizeValue > 0)  
   {  
      StunAtrIntegrity integrity;     
      printf("HMAC with password: %d\n", password->value);  
  
      computeHmac(integrity.hash, buf, (int)(ptr-buf) , password->value, password->sizeValue);  
      ptr = encodeAtrIntegrity(ptr, &integrity);  
   }  
#endif     
   if (msg->hasConnReqBinding)  
   {  
      printf("Encoding ConnReqBinding: %s\n", msg->connReqBinding.value);  
      ptr = encodeAtrString(ptr, ConnReqBinding, &msg->connReqBinding);  
   }  
   if (msg->hasBindingChange)  
   {  
      printf("Encoding BindingChange: %s\n", msg->bindingChange.value);  
      ptr = encodeAtrString(ptr, BindingChange, &msg->bindingChange);  
   } 
   printf("\n");  
  
   encode16(lengthp, (UInt16)(ptr - buf - sizeof(StunMsgHdr)));  
   return (int)(ptr - buf);  
}  
  
int   
stunRand()  
{  
   static bool init;  
   // return 32 bits of random stuff  
   assert( sizeof(int) == 4 );  
   init=false;  
   if ( !init )  
   {   
      UInt64 tick;     
      int seed;  
      init = true;  
              
#if defined(WIN32)   
      volatile unsigned int lowtick=0,hightick=0;  
      __asm  
         {  
            rdtsc   
               mov lowtick, eax  
               mov hightick, edx  
               }  
      tick = hightick;  
      tick <<= 32;  
      tick |= lowtick;  
#elif defined(__GNUC__) && ( defined(__i686__) || defined(__i386__) )  
      asm("rdtsc" : "=A" (tick));  
#elif defined (__SUNPRO_CC)   
      tick = gethrtime();//This is Not expensive Under solaris 8 & above but systemcall in solaris7  
#elif defined(__MACH__)  
      int fd=open("/dev/random",O_RDONLY);  
      read(fd,&tick,sizeof(tick));  
      close(fd);  
#elif 1
      int fd=open("/dev/urandom",O_RDONLY);  
      read(fd,&tick,sizeof(tick));  
      close(fd); 
#else  
#     error Need some way to seed the random number generator   
#endif   
      seed = (int)(tick);  
#ifdef WIN32  
      srand(seed);  
#else  
      srandom(seed);  
#endif  
   }  
  
#ifdef WIN32  
   assert( RAND_MAX == 0x7fff );  
   int r1 = rand();  
   int r2 = rand();  
  
   int ret = (r1<<16) + r2;  
  
   return ret;  
#else  
   return random();   
#endif  
}  
  
#ifdef HAS_MESSAGE_INTEGRITY  
#ifdef WIN32  
static void  
computeHmac(char* hmac, const char* input, int length, const char* key, int sizeKey)  
{  
  strncpy(hmac,"hmac-not-implemented",20);  
}  
#else  
#include <openssl/hmac.h>  
  
#if 1 // previous implementation from STUN open source.  
void  
computeHmac(char* hmac, const char* input, int length, const char* key, int sizeKey)  
{  
   unsigned int resultSize=0;  
   HMAC(EVP_sha1(),   
        key, sizeKey,   
        (const unsigned char*)(input), length,   
        (unsigned char*)(hmac),  
        &resultSize);  
   assert(resultSize == 20);  
}  
#else // fit to DLINK  
#define computeHmac(hmac, input, length, key, sizeKey) \  
hmac_md5(key, input, length, hmac)  
#endif  
#endif  
  
static void  
toHex(const char* buffer, int bufferSize, char* output)   
{  
   static char hexmap[] = "0123456789abcdef";  
  
   const char* p = buffer;  
   char* r = output;  
   int i;  
//   for (int i=0; i < bufferSize; i++)  
   for (i=0; i < bufferSize; i++)  
   {  
      unsigned char temp = *p++;  
         
      int hi = (temp & 0xf0)>>4;  
      int low = (temp & 0xf);  
        
      *r++ = hexmap[hi];  
      *r++ = hexmap[low];  
   }  
   *r = 0;  
}  
  
void  
stunCreateUserName(const StunAddress4* source, StunAtrString* username)  
{  
   UInt64 hitime;  
   UInt64 lotime;  
   char buffer[1024];     
   char hmac[20];     
   char hmacHex[41];     
   int l;  
   char key[] = "Jason";     
   UInt64 time = stunGetSystemTimeSecs();  
   time -= (time % 20*60);  
   hitime = time >> 32;  
   lotime = time & 0xFFFFFFFF;  
     
   sprintf(buffer,  
       "%08lx:%08lx:%08lx:",   
           (UInt32)(source->addr),  
           (UInt32)(stunRand()),  
           (UInt32)(lotime));  
   assert( strlen(buffer) < 1024 );  
  
   assert(strlen(buffer) + 41 < STUN_MAX_STRING);  
     
   computeHmac(hmac, buffer, strlen(buffer), key, strlen(key) );  
   toHex(hmac, 20, hmacHex );  
   hmacHex[40] =0;  
  
   strcat(buffer,hmacHex);  
     
   l = strlen(buffer);  
   assert( l+1 < STUN_MAX_STRING );  
   username->sizeValue = l;  
   memcpy(username->value,buffer,l);  
   username->value[l]=0;  
  
   //clog << "computed username=" << username.value << endl;  
}  
  
void  
stunCreatePassword(const StunAtrString* username, StunAtrString* password)  
{  
   char hmac[20];  
   char key[] = "Fluffy";  
   //char buffer[STUN_MAX_STRING];  
   computeHmac(hmac, username->value, strlen(username->value), key, strlen(key));  
   toHex(hmac, 20, password->value);  
   password->sizeValue = 40;  
   password->value[40]=0;  
  
   //clog << "password=" << password->value << endl;  
}  
#endif  
  
UInt64  
stunGetSystemTimeSecs()  
{  
   UInt64 time=0;  
#if defined(WIN32)    
   SYSTEMTIME t;  
   GetSystemTime( &t );  
   time = (t.wHour*60+t.wMinute)*60+t.wSecond;   
#else  
   struct timeval now;  
   gettimeofday( &now , NULL );  
   //assert( now );  
   time = now.tv_sec;  
#endif  
   return time;  
}  
  
  
//static // cwni mark  
void  
parseHostName( char* peerName,  
                   UInt32* ip,  
                   UInt16* portVal,  
                   UInt16 defaultPort )  
{  
   struct in_addr sin_addr;  
      
   char host[512];  
   char* port = NULL;  
   int portNum = defaultPort;     
   char* sep;     
   struct hostent* h;     
   strncpy(host,peerName,512);  
   host[512-1]='\0';  
     
   // pull out the port part if present.  
   sep = strchr(host,':');  
     
   if ( sep == NULL )  
   {  
      portNum = defaultPort;  
   }  
   else  
   {  
      char* endPtr=NULL;     
      *sep = '\0';  
      port = sep + 1;  
      // set port part  
              
      portNum = strtol(port,&endPtr,0);  
        
      if ( endPtr != NULL )  
      {  
         if ( *endPtr != '\0' )  
         {  
            portNum = defaultPort;  
         }  
      }  
   }  
      
   assert( portNum >= 1024 );  
   assert( portNum <= 65000 );  
  
   // figure out the host part   
     
#ifdef WIN32  
   assert( strlen(host) >= 1 );  
   if ( isdigit( host[0] ) )  
   {  
       // assume it is a ip address   
       unsigned long a = inet_addr(host);  
       //cerr << "a=" << hex << a << endl;  
          
       ip = ntohl( a );  
   }  
   else  
   {  
       // assume it is a host name   
       h = gethostbyname( host );  
   
   if ( h == NULL )  
   {  
       int err = errno;  
       printf("error was %d\n", err);  
       assert( err != WSANOTINITIALISED );  
       assert(0);  
       ip = ntohl( 0x7F000001L );  
   }  
   else  
   {  
       sin_addr = *(struct in_addr*)h->h_addr;  
       ip = ntohl( sin_addr.s_addr );  
   }  
  }  
  
#else  
   h = gethostbyname( host );  
   if ( h == NULL )  
   {  
       int err = errno;  
       printf("error was %d\n", err);        
       assert(0);  
           *ip = ntohl( 0x7F000001L );  
   }  
   else  
   {  
      sin_addr = *(struct in_addr*)h->h_addr;  
      *ip = ntohl( sin_addr.s_addr );  
   }  
#endif  
  
   *portVal = portNum;  
}  
  
  
void  
stunParseServerName( char* name, StunAddress4* addr)  
{  
   assert(name);  
  
   // TODO - put in DNS SRV stuff.  
  
   parseHostName( name, &addr->addr, &addr->port, 3478);   
}  
  
#define STUN_MAX_MESSAGE_SIZE 2048  
  
static void  
stunCreateErrorResponse(StunMessage* response, int cl, int number, const char* msg)  
{  
   response->msgHdr.msgType = BindErrorResponseMsg;  
   response->hasErrorCode = true;  
   response->errorCode.errorClass = cl;  
   response->errorCode.number = number;  
   strcpy(response->errorCode.reason, msg);  
}  
  
#ifdef HAS_MESSAGE_INTEGRITY  
static void  
stunCreateSharedSecretErrorResponse(StunMessage* response, int cl, int number, const char* msg)  
{  
   response->msgHdr.msgType = SharedSecretErrorResponseMsg;  
   response->hasErrorCode = true;  
   response->errorCode.errorClass = cl;  
   response->errorCode.number = number;  
   strcpy(response->errorCode.reason, msg);  
}  
  
static void  
stunCreateSharedSecretResponse(const StunMessage* request, const StunAddress4* source, StunMessage* response)  
{  
   response->msgHdr.msgType = SharedSecretResponseMsg;  
   response->msgHdr.id = request->msgHdr.id;  
  
   response->hasUsername = true;  
   stunCreateUserName( source, &response->username);  
  
   response->hasPassword = true;  
   stunCreatePassword( &response->username, &response->password);  
}  
#endif  
  
// This funtion takes a single message sent to a stun server, parses  
// and constructs an apropriate repsonse - returns true if message is  
// valid  
bool  
stunServerProcessMsg( char* buf,  
                      unsigned int bufLen,  
                      StunAddress4* from,   
                      StunAddress4* myAddr,  
                      StunAddress4* altAddr,   
                      StunMessage* resp,  
                      StunAddress4* destination,  
                      StunAtrString* hmacPassword,  
                      bool* changePort,  
                      bool* changeIp,  
                      bool verbose)  
{  
   StunMessage req;      
   bool ok;  
   StunAddress4 mapped;     
   StunAddress4 respondTo;  
   UInt32 flags;  
   int i;     
   // set up information for default response   
  
   memset( resp, 0 , sizeof(*resp) );  
     
   *changeIp = false;  
   *changePort = false;  
     
   ok = stunParseMessage( buf,bufLen, &req);  
     
   if (!ok)      // Complete garbage, drop it on the floor  
   {  
      if (verbose) printf("Request did not parse\n");  
      return false;  
   }  
   printf("Request parsed ok\n");  
     
   mapped = req.mappedAddress.ipv4;  
   respondTo = req.responseAddress.ipv4;  
   flags = req.changeRequest.value;  
  
   switch (req.msgHdr.msgType)  
   {  
#ifdef HAS_MESSAGE_INTEGRITY     
      case SharedSecretRequestMsg:  
         if(verbose) printf("Received SharedSecretRequestMsg on udp. send error 433.\n");  
// !cj! - should fix so you know if this came over TLS or UDP  
         stunCreateSharedSecretResponse(&req, from, resp);  
         //stunCreateSharedSecretErrorResponse(*resp, 4, 33, "this request must be over TLS");  
         return true;  
#endif           
           
      case BindRequestMsg:  
         if (!req.hasMessageIntegrity)  
         {  
            if (verbose) printf("BindRequest does not contain MessageIntegrity\n");  
              
            if (0) // !jf! mustAuthenticate  
            {  
               if(verbose) printf("Received BindRequest with no MessageIntegrity. Sending 401.\n");  
               stunCreateErrorResponse(resp, 4, 1, "Missing MessageIntegrity");  
               return true;  
            }  
         }  
#ifdef HAS_MESSAGE_INTEGRITY           
         else  
         {  
            if (!req.hasUsername)  
            {  
               if (verbose) printf("No UserName. Send 432.\n");  
               stunCreateErrorResponse(resp, 4, 32, "No UserName and contains MessageIntegrity");  
               return true;  
            }  
            else  
            {  
               if (verbose) printf("Validating username: %s\n", req.username.value);  
               // !jf! could retrieve associated password from provisioning here  
               if (strcmp(req.username.value, "test") == 0)  
               {  
                  if (0)  
                  {  
                     // !jf! if the credentials are stale   
                     stunCreateErrorResponse(resp, 4, 30, "Stale credentials on BindRequest");  
                     return true;  
                  }  
                  else  
                  {  
                     unsigned char hmac[20];  
                     unsigned int hmacSize=20;  
                    
                     if (verbose) printf("Validating MessageIntegrity\n");  
                     // need access to shared secret  
  
#ifndef WIN32  
                     HMAC(EVP_sha1(),   
                          "1234", 4,   
//                          reinterpret_cast<const unsigned char*>(buf), bufLen-20-4,   
                          (const unsigned char*)(buf), bufLen-20-4,   
                          hmac, &hmacSize);  
                     assert(hmacSize == 20);  
#endif  
  
                     if (memcmp(buf, hmac, 20) != 0)  
                     {  
                        if (verbose) printf("MessageIntegrity is bad. Sending \n");  
                        stunCreateErrorResponse(resp, 4, 3, "Unknown username. Try test with password 1234");  
                        return true;  
                     }  
  
                     // need to compute this later after message is filled in  
                     resp->hasMessageIntegrity = true;  
                     assert(req.hasUsername);  
                     resp->hasUsername = true;  
                     resp->username = req.username; // copy username in  
                  }  
               }  
               else  
               {  
                  if (verbose) printf("Invalid username: %s\n", req.username.value);   
               }  
            }  
         }  
#endif  
           
         // !jf! should check for unknown attributes here and send 420 listing the  
         // unknown attributes.   
           
         if ( respondTo.port == 0 ) respondTo = *from;  
         if ( mapped.port == 0 ) mapped = *from;  
      
         *changeIp   = ( flags & ChangeIpFlag )?true:false;  
         *changePort = ( flags & ChangePortFlag )?true:false;  
  
         if (verbose)  
         {  
            printf("Request is valid:\n");  
            printf("\t flags=%d\n", flags);  
            printf("\t changeIp=%d\n", *changeIp);  
            printf("\t changePort=%d\n", *changePort);  
            printf("\t from = %08X:%d\n", from->addr, from->port);  
            printf("\t respond to = %08X:%d\n", respondTo.addr, respondTo.port);  
            printf("\t mapped = %08X:%d\n", mapped.addr, mapped.port);  
         }  
      
         // form the outgoing message  
         resp->msgHdr.msgType = BindResponseMsg;  
         for ( i=0; i<16; i++ )  
         {  
            resp->msgHdr.id.octet[i] = req.msgHdr.id.octet[i];  
         }  
  
         resp->hasMappedAddress = true;  
         resp->mappedAddress.ipv4.port = mapped.port;  
         resp->mappedAddress.ipv4.addr = mapped.addr;  
  
         resp->hasSourceAddress = true;  
         resp->sourceAddress.ipv4.port = (*changePort) ? altAddr->port : myAddr->port;  
         resp->sourceAddress.ipv4.addr = (*changeIp) ? altAddr->addr : myAddr->addr ;  
      
         resp->hasChangedAddress = true;  
         resp->changedAddress.ipv4.port = altAddr->port;  
         resp->changedAddress.ipv4.addr = altAddr->addr;  
  
#ifdef HAS_MESSAGE_INTEGRITY  
         if ( req.hasUsername && req.username.sizeValue > 0 )   
         {  
            // copy username in  
            resp->hasUsername = true;  
            memcpy( resp->username.value, req.username.value, req.username.sizeValue );  
         }  
  
         if ( req.hasMessageIntegrity & req.hasUsername )    
         {  
            // this creates the password that will be used in the HMAC when then  
            // messages is sent  
            stunCreatePassword( &req.username, hmacPassword );  
         }  
           
         if (req.hasUsername && (req.username.sizeValue > 64 ) )  
         {  
            UInt32 source;  
            sscanf(req.username.value, "%lx", &source);  
            resp->hasReflectedFrom = true;  
            resp->reflectedFrom.ipv4.port = 0;  
            resp->reflectedFrom.ipv4.addr = source;  
         }  
#endif           
  
         destination->port = respondTo.port;  
         destination->addr = respondTo.addr;  
  
         return true;  
           
      default:  
         if (verbose) printf("Unknown or unsupported request\n");  
         return false;  
   }  
  
   assert(0);  
   return false;  
}  
     
bool  
stunInitServer(StunServerInfo* info, const StunAddress4* myAddr, const StunAddress4* altAddr)  
{  
   assert( myAddr->port != 0 );  
   assert( altAddr->port!= 0 );  
   assert( myAddr->addr  != 0 );  
   assert( altAddr->addr != 0 );  
  
   info->myAddr = *myAddr;  
   info->altAddr = *altAddr;  
  
   info->myFd = -1;  
   info->altPortFd = -1;  
   info->altIpFd = -1;  
   info->altIpPortFd = -1;  
     
   if ((info->myFd = openPort(myAddr->port, myAddr->addr)) == INVALID_SOCKET)  
   {  
      printf("Can't open %08X:%d\n", myAddr->addr, myAddr->port);  
      stunStopServer(info);  
      return false;  
   }  
   printf("Opened %08X:%d --> %d\n", myAddr->addr, myAddr->port, info->myFd);  
  
   if ((info->altPortFd = openPort(altAddr->port, myAddr->addr)) == INVALID_SOCKET)  
   {  
      printf("Can't open %08X:%d\n", myAddr->addr, myAddr->port);  
      stunStopServer(info);  
      return false;  
   }  
   printf("Opened %08X:%d --> %d\n", myAddr->addr, altAddr->port, info->altPortFd);  
     
   if ((info->altIpFd = openPort( myAddr->port, altAddr->addr)) == INVALID_SOCKET)  
   {  
      printf("Can't open %08X:%d\n", altAddr->addr, altAddr->port);  
      stunStopServer(info);  
      return false;  
   }  
   printf("Opened %08X:%d --> %d\n", altAddr->addr, myAddr->port, info->altIpFd);  
     
   if ((info->altIpPortFd = openPort(altAddr->port, altAddr->addr)) == INVALID_SOCKET)  
   {  
      printf("Can't open %08X:%d\n", altAddr->addr, altAddr->port);  
      stunStopServer(info);  
      return false;  
   }  
   printf("Opened %08X:%d --> %d\n", altAddr->addr, altAddr->port, info->altIpPortFd);  
     
   return true;  
}  
  
void  
stunStopServer(StunServerInfo* info)  
{  
   if (info->myFd > 0) close(info->myFd);  
   if (info->altPortFd > 0) close(info->altPortFd);  
   if (info->altIpFd > 0) close(info->altIpFd);  
   if (info->altIpPortFd > 0) close(info->altIpPortFd);  
}  
  
  
bool  
stunServerProcess(StunServerInfo* info, bool verbose)  
{  
   char msg[udpMaxMessageLength];  
   int msgLen = udpMaxMessageLength;  
   bool ok = false;  
   bool recvAlt =false;  
   fd_set fdSet;   
   unsigned int maxFd=0;  
   struct timeval tv;     
   int e;  
        
   msgLen = sizeof(msg);  
        
   FD_ZERO(&fdSet);   
   FD_SET(info->myFd,&fdSet);   
   if ( info->myFd >= maxFd ) maxFd=info->myFd+1;  
   FD_SET(info->altPortFd,&fdSet);   
   if ( info->altPortFd >= maxFd ) maxFd=info->altPortFd+1;  
   FD_SET(info->altIpFd,&fdSet);  
   if (info->altIpFd>=maxFd) maxFd=info->altIpFd+1;  
   FD_SET(info->altIpPortFd,&fdSet);  
   if (info->altIpPortFd>=maxFd) maxFd=info->altIpPortFd+1;  
     
   tv.tv_sec = 0;  
   tv.tv_usec = 100000;  
  
   e = select( maxFd, &fdSet, NULL,NULL, &tv );  
   if (e < 0)  
   {  
      printf("Error on select\n");  
   }  
   else if (e >= 0)  
   {  
      StunAddress4 from;  
      bool changePort = false;  
      bool changeIp = false;  
  
      StunMessage resp;  
      StunAddress4 dest;  
      StunAtrString hmacPassword;    
        
      char buf[STUN_MAX_MESSAGE_SIZE];  
      int len = STUN_MAX_MESSAGE_SIZE;  
       
      if (FD_ISSET(info->myFd,&fdSet))  
      {  
         if (verbose) printf("received on A1:P1\n");  
         recvAlt = false;  
         ok = getMessage( info->myFd, msg, &msgLen, &from.addr, &from.port );  
      }  
      else if (FD_ISSET(info->altPortFd, &fdSet))  
      {  
         if (verbose) printf("received on A1:P2\n");  
         recvAlt = false;  
         ok = getMessage( info->altPortFd, msg, &msgLen, &from.addr, &from.port );  
      }  
      else if (FD_ISSET(info->altIpFd,&fdSet))  
      {  
         if (verbose) printf("received on A2:P1\n");  
         recvAlt = true;  
         ok = getMessage( info->altIpFd, msg, &msgLen, &from.addr, &from.port );  
      }  
      else if (FD_ISSET(info->altIpPortFd, &fdSet))  
      {  
         if (verbose) printf("received on A2:P2\n");  
         recvAlt = true;  
         ok = getMessage( info->altIpPortFd, msg, &msgLen, &from.addr, &from.port );  
      }  
      else  
      {  
         return true;  
      }  
      
      if ( !ok )   
      {  
         if ( verbose ) printf("Get message did not return a valid message\n");  
         return true;  
      }  
        
      if ( verbose ) printf("Got a request len=%d from %08X:%d\n", msgLen, from.addr, from.port);  
        
      if ( msgLen <= 0 )  
      {  
         return true;  
      }  
        
      hmacPassword.sizeValue = 0;  
    
      ok = stunServerProcessMsg( msg, msgLen, &from,   
                                 recvAlt ? &info->altAddr : &info->myAddr,  
                                 recvAlt ? &info->myAddr : &info->altAddr,   
                                 &resp,  
                                 &dest,  
                                 &hmacPassword,  
                                 &changePort,  
                                 &changeIp,  
                                 verbose );  
        
      if ( !ok )  
      {  
         if ( verbose ) printf("Failed to parse message\n");  
         return true;  
      }  
  
      len = stunEncodeMessage( &resp, buf, len, &hmacPassword );  
        
      if ( dest.addr == 0 )  ok=false;  
      if ( dest.port == 0 ) ok=false;  
        
      if ( ok )  
      {  
         bool sendAlt = false; // send on the default IP address         
         assert( dest.addr != 0 );  
         assert( dest.port != 0 );  
  
         if ( recvAlt )  sendAlt = !sendAlt; // if received on alt, then flip logic  
         if ( changeIp ) sendAlt = !sendAlt; // if need to change IP, then flip logic   
  
         if ( !changePort )  
         {  
            if ( !sendAlt )  
            {  
               sendMessage( info->myFd, buf, len, dest.addr, dest.port );  
            }  
            else  
            {  
               sendMessage( info->altIpFd, buf, len,dest.addr, dest.port );  
            }  
         }  
         else  
         {  
            if ( !sendAlt )  
            {  
               sendMessage( info->altPortFd, buf, len,dest.addr, dest.port );  
            }  
            else  
            {  
               sendMessage( info->altIpPortFd, buf, len, dest.addr, dest.port );  
            }  
         }  
      }  
   }  
  
   return true;  
}  
  
int   
stunFindLocalInterfaces(UInt32* addresses,int maxRet)  
{  
#ifdef WIN32  
    return 0;  
#else  
   struct ifconf ifc;  
     
   int s = socket( AF_INET, SOCK_DGRAM, 0 );  
   int len = 100 * sizeof(struct ifreq);  
  
   char buf[ len ];  
   int e;  
   char *ptr;  
   int tl;  
   int count;  
     
     
   ifc.ifc_len = len;  
   ifc.ifc_buf = buf;  
     
   e = ioctl(s,SIOCGIFCONF,&ifc);  
   ptr = buf;  
   tl = ifc.ifc_len;  
   count=0;  
    
   while ( (tl > 0) && ( count < maxRet) )  
   {  
      struct ifreq* ifr = (struct ifreq *)ptr;  
      struct ifreq ifr2;        
      struct sockaddr a;  
      struct sockaddr_in* addr;  
      UInt32 ai;  
              
      int si = sizeof(ifr->ifr_name) + sizeof(struct sockaddr);  
      tl -= si;  
      ptr += si;  
      //char* name = ifr->ifr_ifrn.ifrn_name;  
      //cerr << "name = " << name << endl;  
   
      ifr2 = *ifr;  
        
      e = ioctl(s,SIOCGIFADDR,&ifr2);  
      if ( e == -1 )  
      {  
         break;  
      }  
        
      //cerr << "ioctl addr e = " << e << endl;  
  
      a = ifr2.ifr_addr;  
      addr = (struct sockaddr_in*) &a;  
  
      ai = ntohl( addr->sin_addr.s_addr );  
      if ((int)((ai>>24)&0xFF) != 127)  
      {  
         addresses[count++] = ai;  
      }  
  
#if 1  
      printf("Detected interface %2x.%2x.%2x.%2x\n", (int)((ai>>24)&0xFF), (int)((ai>>16)&0xFF), (int)((ai>> 8)&0xFF), (int)((ai    )&0xFF) );  
#endif  
   }  
  
    close(s);  
  
   return count;  
#endif  
}  
  
  
static void  
stunBuildReqSimple( StunMessage* msg,  
                    const StunAtrString* username,  
            bool changePort, bool changeIp, unsigned int id)  
{  
   int i;  
   assert( msg );  
   memset( msg , 0 , sizeof(*msg) );  
     
   msg->msgHdr.msgType = BindRequestMsg;  
  
   for ( i=0; i<16; i=i+4 )  
   {  
      int r;  
      assert(i+3<16);  
      r = stunRand();  
      msg->msgHdr.id.octet[i+0]= r>>0;  
      msg->msgHdr.id.octet[i+1]= r>>8;  
      msg->msgHdr.id.octet[i+2]= r>>16;  
      msg->msgHdr.id.octet[i+3]= r>>24;  
   }  
  
   if ( id != 0 )  
   {  
      msg->msgHdr.id.octet[0] = id;   
   }  
     
   msg->hasChangeRequest = true;  
   msg->changeRequest.value =(changeIp?ChangeIpFlag:0) |   
                             (changePort?ChangePortFlag:0);  


   if ( username->sizeValue > 0 )  
   {  
      msg->hasUsername = true;  
      msg->username = *username;  
   }  
}  
  
  
static void   
stunSendTest( Socket myFd, StunAddress4* dest, const StunAtrString* username, const StunAtrString* password, int testNum, bool verbose )  
{   
   bool changePort=false;  
   bool changeIP=false;  
   bool discard=false;  
   StunMessage req;     
   char buf[STUN_MAX_MESSAGE_SIZE];  
   int len = STUN_MAX_MESSAGE_SIZE;  
     
   assert( dest->addr != 0 );  
   assert( dest->port != 0 );  
   
   switch (testNum)  
   {  
      case 1:  
      case 10:  
         break;  
      case 2:  
         changePort=true;  
         changeIP=true;  
         break;  
      case 3:  
         changePort=true;  
         break;  
      case 4:  
         changeIP=true;  
         break;  
      case 5:  
         discard=true;  
         break;  
      default:  
         printf("Test %d is unkown\n", testNum);  
         assert(0);  
   }  
     
   memset(&req, 0, sizeof(StunMessage));  
     
   stunBuildReqSimple( &req, username,   
               changePort , changeIP ,
               testNum );  
  
   len = stunEncodeMessage( &req, buf, len, password );  
  
   if ( verbose )  
   {  
      printf("About to send msg of len %d to %08X:%d\n", len,  dest->addr, dest->port);  
   }  
     
   sendMessage( myFd,  
                buf, len,   
                dest->addr,   
                dest->port );  
}  

static void   
stunSendTest_tr111( Socket myFd, StunAddress4* dest, const StunAtrString* username,
                    const StunAtrString* password, int testNum, bool verbose,
                    bool connReqBinding, bool bindingChange,
                    bool hasRespAddr, StunAddress4* respAddr )  
{   
   bool changePort=false;  
   bool changeIP=false;  
   bool discard=false;  
   StunMessage req;     
   char buf[STUN_MAX_MESSAGE_SIZE];  
   int len = STUN_MAX_MESSAGE_SIZE;  
     
   assert( dest->addr != 0 );  
   assert( dest->port != 0 );  
   
   switch (testNum)  
   {  
      case 1:  
      case 10:  
         break;  
      case 2:  
         changePort=true;  
         changeIP=true;  
         break;  
      case 3:  
         changePort=true;  
         break;  
      case 4:  
         changeIP=true;  
         break;  
      case 5:  
         discard=true;  
         break;  
      default:  
         printf("Test %d is unkown\n", testNum);  
         assert(0);  
   }  
     
   memset(&req, 0, sizeof(StunMessage));  
     
   stunBuildReqSimple( &req, username,   
               changePort , changeIP ,
               testNum );

   req.hasConnReqBinding = connReqBinding;
   req.connReqBinding.sizeValue = 0x0014;
   sprintf( req.connReqBinding.value, "%s", "dslforum.org/TR-111 " );

   req.hasBindingChange = bindingChange;
   req.bindingChange.sizeValue = 0;

   req.hasResponseAddress = hasRespAddr;
   if (respAddr)
      req.responseAddress.ipv4 = *respAddr;
  
   len = stunEncodeMessage( &req, buf, len, password );  
  
   if ( verbose )  
   {  
      printf("About to send msg of len %d to %08X:%d\n", len,  dest->addr, dest->port);  
   }  
     
   sendMessage( myFd,  
                buf, len,   
                dest->addr,   
                dest->port );  
}  
  
#ifdef HAS_MESSAGE_INTEGRITY  
void   
stunGetUserNameAndPassword(  const StunAddress4* dest,   
                             StunAtrString* username,  
                             StunAtrString* password)  
{   
// !cj! This is totally bogus - need to make TLS connection to dest and get a  
// username and password to use   
   stunCreateUserName(dest, username);  
   stunCreatePassword(username, password);  
}  
#endif  
  
void   
stunTest( StunAddress4* dest, int testNum, bool verbose )  
{   
   Socket myFd;  
   StunAtrString username;  
   StunAtrString password;  
   char msg[STUN_MAX_MESSAGE_SIZE];  
   int msgLen = STUN_MAX_MESSAGE_SIZE;  
   StunAddress4 from;     
   StunMessage resp;  
   bool ok;  
     
   assert( dest->addr != 0 );  
   assert( dest->port != 0 );  
  
   myFd = openPort(0, 0);  
     
   username.sizeValue = 0;  
   password.sizeValue = 0;  
     
#ifdef USE_TLS  
   stunGetUserNameAndPassword( dest, &username, &password );  
#endif  
  
   stunSendTest( myFd, dest, &username, &password, testNum, verbose );  
      
   getMessage( myFd,  
               msg,  
               &msgLen,  
               &from.addr,  
               &from.port );  
     
   memset(&resp, 0, sizeof(StunMessage));  
  
   if ( verbose ) printf("Got a response\n");  
   ok = stunParseMessage( msg,msgLen, &resp );  
     
   if ( verbose )  
   {  
      printf("\t ok=%d\n", ok);  
      printf("\t id=%d\n", resp.msgHdr.id);  
      printf("\t mappedAddr=%08X:%d\n", resp.mappedAddress.ipv4.addr, resp.mappedAddress.ipv4.port);  
      printf("\t changedAddr=%08X:%d\n", resp.changedAddress.ipv4.addr, resp.changedAddress.ipv4.port);  
      printf("\n");  
   }  
}  


void   
stunTest_tr111( Socket myFd, StunAddress4* dest, int testNum, bool verbose,
                StunMessage* resp, bool connReqBinding, bool bindingChange,
                bool hasRespAddr, StunAddress4* respAddr,
                char *un, char *pw, void (*udpConnReqCb)(char *buf, unsigned int bufLen) )  
{   
   //Socket myFd;  
   StunAtrString username;  
   StunAtrString password;  
   char msg[STUN_MAX_MESSAGE_SIZE];  
   int msgLen = STUN_MAX_MESSAGE_SIZE;  
   StunAddress4 from;     
   //StunMessage resp;  
   bool ok;  

   struct timeval tv;  
   fd_set fdSet; int fdSetSize;  
   int err, e;  
     
   assert( dest->addr != 0 );  
   assert( dest->port != 0 );  
  
   //myFd = openPort(0, 0);  

   if ( un )
   {
      username.sizeValue = strlen(un);
	  strcpy(username.value, un);
   }
   else
   {
      username.sizeValue = 0;  
   }

   if ( pw )
   {
      password.sizeValue = strlen(pw);
	  strcpy(password.value, pw);
   }
   else
   {
      password.sizeValue = 0; 
   }
     
#ifdef USE_TLS  
   stunGetUserNameAndPassword( dest, &username, &password );  
#endif  
  
   stunSendTest_tr111( myFd, dest, &username, &password, testNum, verbose,
                       connReqBinding, bindingChange, hasRespAddr, respAddr );  

   FD_ZERO(&fdSet); fdSetSize=0;  
   FD_SET(myFd,&fdSet);   
   fdSetSize = myFd+1;  
   tv.tv_sec=0;  
   tv.tv_usec=1000*1000; // 1000 ms  

   err = select(fdSetSize, &fdSet, NULL, NULL, &tv);  
   e = errno;  
   if ( err == SOCKET_ERROR )  
   {  
      // error occured  
      if ( verbose ) printf("Error in select\n");  
   }  
   else if ( err == 0 )  
   {  
      // timeout occured   
      //stunSendTest( myFd, dest, &username, &password, 1 ,verbose );  
      if ( verbose ) printf("Timeout\n");
   }  
   else  
   {  
      getMessage( myFd,  
                  msg,  
                  &msgLen,  
                  &from.addr,  
                  &from.port );  

	  memset(resp, 0, sizeof(StunMessage)); 

#if 0 // for debug only
      msgLen = sprintf(msg, "GET http://10.1.1.1:8080?ts=1120673700&id=1234&un=CPE57689"
                "&cn=XTGRWIPC6D3IPXS3&sig=3545F7B5820D76A3DF45A3A509DA8D8C38F13512 HTTP/1.1");
#endif

	  if ( msg[0] == 0 || msg[0] == 1 ) // stun msg
	  {
     
         if ( verbose ) printf("Got a response\n");  
         ok = stunParseMessage( msg,msgLen, resp );  
     
         if ( verbose )  
         {  
            printf("\t ok=%d\n", ok);  
            printf("\t id=%d\n", resp->msgHdr.id);  
            printf("\t mappedAddr=%08X:%d\n", resp->mappedAddress.ipv4.addr, resp->mappedAddress.ipv4.port);  
            printf("\t changedAddr=%08X:%d\n", resp->changedAddress.ipv4.addr, resp->changedAddress.ipv4.port);  
            printf("\n");  
         }  
	  }
	  else // UDP Connection Req.
	  {
	     if ( udpConnReqCb )
	     {
	        if ( verbose ) printf("Got a UDP Connection Req\n");  
		 	udpConnReqCb( msg, msgLen );
	     }
	  }
   }
} 


void   
stunTest_tr111_send( Socket myFd, StunAddress4* dest, int testNum, bool verbose,
                bool connReqBinding, bool bindingChange,
                bool hasRespAddr, StunAddress4* respAddr )  
{   
   //Socket myFd;  
   StunAtrString username;  
   StunAtrString password;  
   char msg[STUN_MAX_MESSAGE_SIZE];  
   int msgLen = STUN_MAX_MESSAGE_SIZE;  
   StunAddress4 from;     
   //StunMessage resp;  
   bool ok;  

   struct timeval tv;  
   fd_set fdSet; int fdSetSize;  
   int err, e;  
     
   assert( dest->addr != 0 );  
   assert( dest->port != 0 );  
  
   //myFd = openPort(0, 0);  
     
   username.sizeValue = 0;  
   password.sizeValue = 0;  
     
#ifdef USE_TLS  
   stunGetUserNameAndPassword( dest, &username, &password );  
#endif  
  
   stunSendTest_tr111( myFd, dest, &username, &password, testNum, verbose,
                       connReqBinding, bindingChange, hasRespAddr, respAddr );  
}


void   
stunTest_tr111_recv( Socket myFd, bool verbose, StunMessage* resp )  
{   
   //Socket myFd;  
   StunAtrString username;  
   StunAtrString password;  
   char msg[STUN_MAX_MESSAGE_SIZE];  
   int msgLen = STUN_MAX_MESSAGE_SIZE;  
   StunAddress4 from;     
   //StunMessage resp;  
   bool ok;  

   struct timeval tv;  
   fd_set fdSet; int fdSetSize;  
   int err, e;  
     
   //assert( dest->addr != 0 );  
   //assert( dest->port != 0 );  
  
   //myFd = openPort(0, 0);  

   FD_ZERO(&fdSet); fdSetSize=0;  
   FD_SET(myFd,&fdSet);   
   fdSetSize = myFd+1;  
   tv.tv_sec=0;  
   tv.tv_usec=1000*1000; // 1000 ms  

   err = select(fdSetSize, &fdSet, NULL, NULL, &tv);  
   e = errno;  
   if ( err == SOCKET_ERROR )  
   {  
      // error occured  
      if ( verbose ) printf("Error in select\n");  
   }  
   else if ( err == 0 )  
   {  
      // timeout occured   
      //stunSendTest( myFd, dest, &username, &password, 1 ,verbose );  
      if ( verbose ) printf("Timeout\n");
   }  
   else  
   {  
      getMessage( myFd,  
                  msg,  
                  &msgLen,  
                  &from.addr,  
                  &from.port );  
     
      memset(resp, 0, sizeof(StunMessage));  
  
      if ( verbose ) printf("Got a response\n");  
      ok = stunParseMessage( msg,msgLen, resp );  
     
      if ( verbose )  
      {  
         printf("\t ok=%d\n", ok);  
         printf("\t id=%d\n", resp->msgHdr.id);  
         printf("\t mappedAddr=%08X:%d\n", resp->mappedAddress.ipv4.addr, resp->mappedAddress.ipv4.port);  
         printf("\t changedAddr=%08X:%d\n", resp->changedAddress.ipv4.addr, resp->changedAddress.ipv4.port);  
         printf("\n");  
      }  
   }
}

  
  
NatType  
stunNatType( StunAddress4* dest, bool verbose )  
{   
   bool respTestI=false;  
   bool isNat=true;  
   StunAddress4 testIchangedAddr;  
   StunAddress4 testImappedAddr;  
   bool respTestI2=false;   
   bool mappedIpSame = true;  
   StunAddress4 testI2mappedAddr;  
   StunAddress4 testI2dest=*dest;  
   bool respTestII=false;  
   bool respTestIII=false;  
   StunAtrString username;  
   StunAtrString password;  
   int count;  
   Socket myFd;  
   Socket s;  
  
   assert( dest->addr != 0 );  
   assert( dest->port != 0 );  
     
   myFd = openPort(0, 0);  
         
   memset(&testImappedAddr,0,sizeof(testImappedAddr));  
    
   username.sizeValue = 0;  
   password.sizeValue = 0;  
  
#ifdef USE_TLS   
   stunGetUserNameAndPassword( dest, &username, &password );  
#endif  
  
   stunSendTest( myFd, dest, &username, &password, 1, verbose );  
   count=0;  
   while ( count++ < 10 )  
   {  
      struct timeval tv;  
      fd_set fdSet; int fdSetSize;  
      int err, e;  
      FD_ZERO(&fdSet); fdSetSize=0;  
      FD_SET(myFd,&fdSet);   
      fdSetSize = myFd+1;  
      tv.tv_sec=0;  
      tv.tv_usec=1000*1000; // 1000 ms   
  
      err = select(fdSetSize, &fdSet, NULL, NULL, &tv);  
      e = errno;  
      if ( err == SOCKET_ERROR )  
      {  
         // error occured  
         printf("Error in select\n");  
      }  
      else if ( err == 0 )  
      {  
         // timeout occured   
         if (!respTestI )   
         {  
            stunSendTest( myFd, dest, &username, &password, 1 ,verbose );  
         }           
         if ((!respTestI2) && respTestI )   
         {  
            // check the address to send to if valid   
            if (  ( testI2dest.addr != 0 ) &&  
                  ( testI2dest.port != 0 ) )  
            {  
               stunSendTest( myFd, &testI2dest, &username, &password, 10  ,verbose);  
            }  
         }  
         if (!respTestII )  
         {  
            stunSendTest( myFd, dest, &username, &password, 2 ,verbose );  
         }     
         if (!respTestIII )  
         {  
            stunSendTest( myFd, dest, &username, &password, 3 ,verbose );  
         }  
      }  
      else  
      {  
         //clog << "-----------------------------------------" << endl;  
         assert( err>0 );  
         // data is avialbe on some fd   
         if ( myFd!=INVALID_SOCKET ) if ( FD_ISSET(myFd,&fdSet) )  
         {  
            char msg[udpMaxMessageLength];  
            int msgLen = udpMaxMessageLength;  
              
            StunAddress4 from;  
            StunMessage resp;                          
  
            getMessage( myFd,  
                        msg,  
                        &msgLen,  
                        &from.addr,  
                        &from.port );  
              
            memset(&resp, 0, sizeof(StunMessage));  
              
            stunParseMessage( msg,msgLen, &resp );  
                          
            if ( verbose )  
            {  
               printf("Received message of type %d\n", resp.msgHdr.msgType);  
            }  
              
            switch( resp.msgHdr.id.octet[0] )  
            {  
               case 1:  
                  testIchangedAddr.addr = resp.changedAddress.ipv4.addr;  
                  testIchangedAddr.port = resp.changedAddress.ipv4.port;  
                  testImappedAddr.addr = resp.mappedAddress.ipv4.addr;  
                  testImappedAddr.port = resp.mappedAddress.ipv4.port;  
                  respTestI=true;    
                  testI2dest.addr = resp.changedAddress.ipv4.addr;  
                  //testI2dest.port = resp.changedAddress.ipv4.port;  
                  break;  
               case 2:  
                  respTestII=true;  
                  break;  
               case 3:  
                  respTestIII=true;  
                  break;  
               case 10:  
                  testI2mappedAddr.addr = resp.mappedAddress.ipv4.addr;  
                  testI2mappedAddr.port = resp.mappedAddress.ipv4.port;  
                  respTestI2=true;  
                  mappedIpSame = false;  
                  if ( (testI2mappedAddr.addr  == testImappedAddr.addr ) &&  
                       (testI2mappedAddr.port == testImappedAddr.port ))  
                  {   
                     mappedIpSame = true;  
                  }  
                
                  break;  
            }  
         }  
      }  
   }  
   // see if we can bind to this address   
   //cerr << "try binding to " << testImappedAddr << endl;  
   s = openPort( 11000, testImappedAddr.addr );  
   if ( s != INVALID_SOCKET )  
   {  
      close(s);  
      isNat = false;  
      //cerr << "binding worked" << endl;  
   }  
   else  
   {  
      isNat = true;  
      //cerr << "binding failed" << endl;  
   }  
  
   if (verbose)  
   {  
      printf("test I = %d\n", respTestI);  
      printf("test II = %d\n", respTestII);  
      printf("test III = %d\n", respTestIII);  
      printf("test I(2) = %d\n", respTestI2);  
      printf("is nat  = %d\n", isNat);  
      printf("mapped IP same = %d\n", mappedIpSame);  
   }  
  
   // implement logic flow chart from draft RFC  
   if ( respTestI )  
   {  
      if ( isNat )  
      {  
         if (respTestII)  
         {  
            return StunTypeConeNat;  
         }  
         else  
         {  
            if ( mappedIpSame )  
            {  
               if ( respTestIII )  
               {  
                  return StunTypeRestrictedNat;  
               }  
               else  
               {  
                  return StunTypePortRestrictedNat;  
               }  
            }  
            else  
            {  
               return StunTypeSymNat;  
            }  
         }  
      }  
      else  
      {  
         if (respTestII)  
         {  
            return StunTypeOpen;  
         }  
         else  
         {  
            return StunTypeSymFirewall;  
         }  
      }  
   }  
   else  
   {  
      return StunTypeBlocked;  
   }  
  
   assert(0);  
   return StunTypeUnknown;  
}  
  
  
/* ==================================================================== 
 * The Vovida Software License, Version 1.0  
 *  
 * Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved. 
 *  
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 *  
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 *  
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in 
 *    the documentation and/or other materials provided with the 
 *    distribution. 
 *  
 * 3. The names "VOCAL", "Vovida Open Communication Application Library", 
 *    and "Vovida Open Communication Application Library (VOCAL)" must 
 *    not be used to endorse or promote products derived from this 
 *    software without prior written permission. For written 
 *    permission, please contact vocal@vovida.org. 
 * 
 * 4. Products derived from this software may not be called "VOCAL", nor 
 *    may "VOCAL" appear in their name, without prior written 
 *    permission of Vovida Networks, Inc. 
 *  
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND 
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA 
 * NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES 
 * IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
 * DAMAGE. 
 *  
 * ==================================================================== 
 *  
 * This software consists of voluntary contributions made by Vovida 
 * Networks, Inc. and many individuals on behalf of Vovida Networks, 
 * Inc.  For more information on Vovida Networks, Inc., please see 
 * <http://www.vovida.org/>. 
 * 
 */  
  
// Local Variables:  
// mode:c++  
// c-file-style:"ellemtel"  
// c-file-offsets:((case-label . +))  
// indent-tabs-mode:nil  
// End:  