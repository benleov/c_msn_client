c_msn_client
================================

Dependancies 
--------------------------------

 * [ssl](http://www.rtfm.com/openssl-examples/)

On Ubuntu this is the libssl-dev package.

Resources
--------------------------------

* [MSN error codes](http://www.hypothetic.org/docs/msn/reference/error_list.php)

Compilation
--------------------------------

Required Flags:

(Linker)

-lpthread 		
-lssl 
-lcrypto
-lm 

(gcc)

-D_GNU_SOURCE	


Optional:

-O0 
-g3 
-Wall 
-c 
-fmessage
-length=0 
-v
