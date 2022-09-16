#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/fcntl.h>
#include <asm/uaccess.h>

//#define FILE_PATH "/etc/shadow"
#define FILE_PATH "/home/andrey/Module/shadow/filest"


static int readFile(struct file *fp, char *buf, int len)
{
	int rlen = 0, sum = 0;
    pr_info("Read_file called\n"); 		
	if (!(fp->f_mode & FMODE_CAN_READ)) {
	    pr_info("Read err\n"); 		
		return -EPERM;
	}

	while (sum < len) {
		rlen = kernel_read(fp, buf + sum, len - sum, &fp->f_pos);
		if (rlen > 0)
			sum += rlen;
		else if (0 != rlen)
			return rlen;
		else
			break;
	}
	pr_info("Read_file finished\n"); 		
	return  sum;
}

static int read_file(const char *path, u32 *sz)
{
	int ret = 0;
	struct file *fp = filp_open(path, O_RDONLY, 0);
	//char buf;

	char bigbuff[100];
	int i = 0;
	for (; i < 100; i++) {
		bigbuff[i] = '\0';
	}
	pr_info("Read_file called\n"); 
	
	if (IS_ERR(fp)) {
		ret = PTR_ERR(fp);
        pr_info("Not readable\n"); 		
    }
	else {
		if (1 != readFile(fp, bigbuff, 1)) {
			ret = PTR_ERR(fp);
            pr_info("Opened but not readable?\n"); 			
        }
		if (ret == 0 && sz) {
			*sz = i_size_read(fp->f_path.dentry->d_inode);
	        pr_info("Readable\n"); 	
		}
		readFile(fp, bigbuff + 1, 99);
	    printk(bigbuff); 		
		filp_close(fp, NULL);
	}
	return ret;
}

static int __init mdl_init(void) { 		
  u32 u;
  pr_info("Shadow init\n");
  read_file(FILE_PATH, &u);
  return 0;
}

static void __exit mdl_exit(void) {
  	pr_info("Shadow EXIT\n"); 
}

MODULE_LICENSE("GPL");
module_init(mdl_init);
module_exit(mdl_exit);
