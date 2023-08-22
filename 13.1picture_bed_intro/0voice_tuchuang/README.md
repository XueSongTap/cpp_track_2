编译报错，缺fcgi_config.h

yxc@VM-4-5-ubuntu:~/code/cpp_track/13.1picture_bed/0voice_tuchuang$ make
gcc -c src_cgi/login_cgi.c -o src_cgi/login_cgi.o -I./include -I/usr/include/fastdfs -I/usr/include/fastcommon -I/usr/local/include/hiredis/ -I/usr/include/mysql/ -Wall
src_cgi/login_cgi.c:8:10: fatal error: fcgi_config.h: No such file or directory
 #include "fcgi_config.h"
          ^~~~~~~~~~~~~~~
compilation terminated.
Makefile:159: recipe for target 'src_cgi/login_cgi.o' failed
make: *** [src_cgi/login_cgi.o] Error 1
yxc@VM-4-5-ubuntu:~/code/cpp_track/13.1picture_bed/0voice_tuchuang$ 