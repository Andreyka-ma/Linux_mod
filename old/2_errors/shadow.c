#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/fcntl.h>
#include <linux/delay.h>
#include <asm/uaccess.h>

#define FILE_PATH "/home/andrey/Module/shadow/filest3.txt"
#define MSG_SIZE 25
char message[MSG_SIZE] = "Writing from Kernel yeah\n";

static int readFile(struct file *fp, char *buf, int len) {
	int rlen = 0, sum = 0;
	if (!(fp->f_mode & FMODE_CAN_READ)) { return -EPERM; }

	while (sum < len) {
		rlen = kernel_read(fp, buf + sum, len - sum, &fp->f_pos);
		if (rlen > 0)
			sum += rlen;
		else if (0 != rlen)
			return rlen;
		else
			break;
	}
	return  sum;
}

static int writeFile(struct file *fp, char *buf, int len) {
	int wlen = 0, sum = 0;
	//if (!fp->f_op || !fp->f_op->write) { return -EPERM; }
	
	while (sum < len) {
		wlen = kernel_write(fp, buf + sum, len - sum, &fp->f_pos);
		if (wlen > 0)
			sum += wlen;
		else if (0 != wlen)
			return wlen;
		else
			break;
	}
	return sum;
}

static int openFile(struct file **fpp, const char *path, int flag, int mode) {
	struct file *fp;
	fp = filp_open(path, flag, mode);
	if (IS_ERR(fp)) {
		*fpp = NULL;
		return PTR_ERR(fp);
	} else {
		*fpp = fp;
		printk("Opened");
		return 0;
	}
}

static void myWrite(const char* path) {
	struct file *fp;
	int ret = openFile(&fp, path, O_CREAT | O_WRONLY | O_APPEND, 0666);
	if (0 == ret) {
		writeFile(fp, message, MSG_SIZE); 				
		filp_close(fp, NULL);
		printk("Write success");
	}
}

static void myRead(const char* path) {
	struct file *fp = filp_open(path, O_RDONLY, 0);
	char bigbuff[100];
	int i = 0;
	for (; i < 100; i++) { bigbuff[i] = '\0'; }
	readFile(fp, bigbuff, 99);
	printk(bigbuff); 		
	filp_close(fp, NULL);
}

static void write_thread() {
	int i = 0;
	for (; i < 10; ++i) {
		msleep(1000);
		myWrite(FILE_PATH);
		myRead(FILE_PATH);
	}
}	

static int __init mdl_init(void) { 		
	pr_info("Shadow init\n");
	write_thread();
	return 0;
}

static void __exit mdl_exit(void) {
  	pr_info("Shadow EXIT\n"); 
}

MODULE_LICENSE("GPL");
module_init(mdl_init);
module_exit(mdl_exit);
