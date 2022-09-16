#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/fcntl.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <linux/kthread.h>

#define MSG_SIZE 20
char message[MSG_SIZE] = "Writing from Kernel\n";
char* write_file_path = "/etc/Andreyka/filest.txt";
int sleep_time = 1000;
struct task_struct *task1;

static int readFile(struct file *fp, char *buf, int len) {
	int rlen = 0, sum = 0;
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

static void myRead(const char* path) {
	struct file *fp = filp_open(path, O_RDONLY, 0);
	char bigbuff[100];
	int i = 0;
	for (; i < 100; i++) { bigbuff[i] = '\0'; }
	readFile(fp, bigbuff, 99);
	printk(bigbuff); 		
	filp_close(fp, NULL);
}

static int writeFile(struct file *fp, char *buf, int len) {
	int wlen = 0, sum = 0;
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

static void myWrite(const char* path) {
	struct file *fp = filp_open(path, O_CREAT | O_WRONLY | O_APPEND, 0666);
	writeFile(fp, message, MSG_SIZE); 				
	filp_close(fp, NULL);
}

static int write_thread(void* data) {
	int i = 40;
	while(--i) {
		msleep(sleep_time);
		myWrite(write_file_path);
	}
	return 0;
}	

static int __init mdl_init(void) {
	pr_info("Shadow init\n");
	task1 = kthread_run(&write_thread, NULL, "Write_thread");
	return 0;
}

static void __exit mdl_exit(void) {
  	pr_info("Shadow EXIT\n"); 
}

MODULE_LICENSE("GPL");
module_init(mdl_init);
module_exit(mdl_exit);
