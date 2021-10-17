#include    <stdio.h>
#include    <sys/types.h>
#include    <sys/socket.h>
#include    <netinet/in.h>
#include    <netdb.h>
#include    <sys/time.h> 
#include    <sys/types.h>
#include    <sys/ioctl.h>
#include    <arpa/inet.h>
#include    <string.h>
#include    <strings.h>
#include    <unistd.h>
#include    <stdlib.h>
#include    <fcntl.h>

#include    "connect.h"
static inline int intmax(int a, int b) {return a>b?a:b;}
static inline int intmin(int a, int b) {return a<b?a:b;}
    
#define BUFF_SZ 4096
typedef struct {
    int handle;
    int ins, del;
    char wbuf[BUFF_SZ];
} write_pipe_t;

static inline void 
wp_init(write_pipe_t *wp) {
    wp->handle=-1;
    wp->ins=wp->del=0;
}
static inline void
wp_close(write_pipe_t *wp) {
    if (wp->handle>=0) close(wp->handle);
    wp_init(wp);
}
static int
wp_open_dev(write_pipe_t *wp, const char *dev_name) {
    wp_close(wp);
    int h=open(dev_name, O_RDWR|O_NONBLOCK);
    if (h<0) return -1;
    return (wp->handle=h);
}
static inline int
wp_is_established(write_pipe_t *wp) {
    return wp->handle>=0;
}
static inline int
wp_bytes_in_buffer(write_pipe_t *wp) {
    return (wp->ins-wp->del+BUFF_SZ)%BUFF_SZ;
}
static inline int
wp_bytes_free(write_pipe_t *wp) {
    return (wp->del-wp->ins+BUFF_SZ-1)%BUFF_SZ;
}
static inline int
_wp_bytes_to_write(write_pipe_t *wp) {
    return (wp->del>wp->ins)?(wp->del-wp->ins-1):(BUFF_SZ-wp->del);
}
static int
wp_insert(write_pipe_t *wp, char *buf, int size) {
    int s=0, c;
    while (((c=_wp_bytes_to_write(wp))>0)&&(size>0)) {
        c=intmin(size, c);
        memcpy(wp->wbuf+wp->ins, buf, c);
        s+=c;
        size-=c;
        buf+=c;
        wp->ins=(wp->ins+c)%BUFF_SZ;
    }
    return s;
}
static inline int
_wp_bytes_to_read(write_pipe_t *wp) {
    return (wp->ins>=wp->del)?(wp->ins-wp->del):(BUFF_SZ-wp->del);
}
static void
wp_delete(write_pipe_t *wp, int size) {
    wp->del=(wp->del+size)%BUFF_SZ;
}
static int
wp_buf_output(write_pipe_t *wp) {
    int c, d;
    while ((c=_wp_bytes_to_read(wp))>0) {
        d=write(wp->handle, wp->wbuf+wp->del, c);
        if (d<0) return -1;
        wp_delete(wp, d);
        if (d!=c) break;
    }
    return 0;
}
static int
wp_set_fd(write_pipe_t *wp, int max, fd_set *fds) {
    if (wp->handle>=0) {
        FD_SET(wp->handle, fds);
        return intmax(max, wp->handle);
    }
    return max;
}
static inline int
wp_set_wfd(write_pipe_t *wp, int max, fd_set *fds) {
    return (wp_bytes_in_buffer(wp)>0)?
        wp_set_fd(wp, max, fds):max;
}
static inline int
wp_isset(write_pipe_t *wp, fd_set *fds) {
    return (wp->handle>=0) && FD_ISSET(wp->handle, fds);
}
static inline int
wp_wisset(write_pipe_t *wp, fd_set *fds) {
    return (wp_bytes_in_buffer(wp)>0) && wp_isset(wp, fds);
}


int ss=-1;  // passive socket
write_pipe_t to_dev, to_sck;    // the two writeout pipes

static void
show_usage(char *name) {
    fprintf(stderr,"usage: %s <dev-name>:<tcp-port-number>\n", name);
}
int 
main(int argc, char *argv[]) {
    
    char buffer[BUFF_SZ];
    if (argc<2) {
        show_usage(argv[0]);
        return 1;
    }
    char *niddle=strchr(argv[1], ':');
    if (niddle==NULL) {
        show_usage(argv[0]);
        return 1;
    }
    *(niddle++)='\0';
    const char *dev_name=argv[1];
    const char *port_name=niddle;
    wp_init(&to_dev); 
    wp_init(&to_sck); 
        
    if ((ss=passivesocket(port_name, "tcp", 1))<0) {
        fprintf(stderr,"unable to open passive socket, %s\n", port_name);
        return 1;
    }
    if (wp_open_dev(&to_dev, dev_name)<0) {
        fprintf(stderr,"unable to open device, %s\n", dev_name);
        return 1;
    }
    
    fd_set read_fd, write_fd, error_fd;
    while (1) {
        unsigned int max=ss;
        FD_ZERO(&read_fd);
        FD_ZERO(&write_fd);
        FD_ZERO(&error_fd);
        FD_SET(ss, &read_fd);
        int established=(to_sck.handle>=0);
        if (established) {
            max=wp_set_fd(&to_sck, max, &read_fd);
            max=wp_set_fd(&to_dev, max, &read_fd);
            wp_set_wfd(&to_sck, max, &write_fd);
            wp_set_wfd(&to_dev, max, &write_fd);
        }
        memcpy(&error_fd, &read_fd, sizeof(fd_set));
        select(1+max, &read_fd, &write_fd, &error_fd, NULL);

        if (FD_ISSET(ss, &error_fd)) {
            fprintf(stderr, "passive socket, %s, error\n", port_name);
            return 1;
        }
        if (established) {
            // when connection has been established
            if (wp_isset(&to_dev, &error_fd)) {
                fprintf(stderr, "device: %s, error\n", dev_name);
                return 1;
            }
            if (wp_isset(&to_sck, &error_fd)) {
                fprintf(stderr, "port: %s, error\n", port_name);
                wp_close(&to_sck);
            }

            if (wp_isset(&to_dev, &read_fd)) {
                int c=wp_bytes_free(&to_sck);
                c=read(to_dev.handle, buffer, c);
                if (c>0) {
                    wp_insert(&to_sck, buffer, c);
                } else {
                    fprintf(stderr, "dev read fail (may due to closed session)!\n");
                    wp_close(&to_dev);
                    return 0;
                }
            } // if (wp_isset(&to_dev, &read_fd))
            
            if (wp_isset(&to_sck, &read_fd)) {
                int c=wp_bytes_free(&to_dev);
                c=read(to_sck.handle, buffer, c);
                if (c>0) {
                    wp_insert(&to_dev, buffer, c);
                } else {
                    fprintf(stderr, "peer closed\n");
                    wp_close(&to_sck);
                }
            } //if (wp_isset(&to_sck, &read_fd))
            
            if (wp_wisset(&to_dev, &write_fd)) {
                if (wp_buf_output(&to_dev)<0) {
                    fprintf(stderr, "write to device failed, try to open again\n");
                    if (wp_open_dev(&to_dev, dev_name)<0) {
                        fprintf(stderr, "reopen fail\n");
                        return 1;
                    }
                }
            } //if (wp_wisset(&to_dev, &write_fd))
            if (wp_wisset(&to_sck, &write_fd)) {
                if (wp_buf_output(&to_sck)<0) {
                    fprintf(stderr, "write to socket failed, socket close\n");
                    wp_close(&to_sck);
                }
            } //if (wp_wisset(&to_sck, &write_fd))
        } else {
            // when connection has NOT been established
            if (FD_ISSET(ss, &read_fd)) {
                int peer=accept(ss, NULL, NULL);
                if (peer<0) {
                    perror("accept fail\n");
                } else {
                    to_sck.handle=peer;
                }
            }
        }
    }
    
    return 0;
}


