#ifndef PDEBUG_H
#define PDEBUG_H

#undef PDEBUG
#undef PDEBUGA
#ifdef AWLD_DEBUG
#  ifdef __KERNEL__
#    define PDEBUG(fmt, args...) printk( KERN_INFO DEVICE_NAME ": %s " fmt, __func__, ## args)
#    define PDEBUGA(fmt, args...) printk(fmt, ## args)
#  else
#    define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#    define PDEBUGA(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif
#else
#  define PDEBUG(fmt, args...)
#  define PDEBUGA(fmt, args...)
#endif

#undef PERROR
#  ifdef __KERNEL__
#    define PERROR(fmt, args...) printk( KERN_ERR DEVICE_NAME ": %s " fmt, __func__, ## args)
#  else
#    define PERROR(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif

#undef PINFO
#  ifdef __KERNEL__
#    define PINFO(fmt, args...) printk( KERN_INFO DEVICE_NAME ": %s " fmt, __func__, ## args)
#  else
#    define PINFO(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif

#endif
