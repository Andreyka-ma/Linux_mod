#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/fcntl.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <linux/kthread.h>

#define PATH "/etc/Hello_from_kernel_dir/"

char* message = "Hello from kernel module\n";
char* write_file_path = "~/Hello/defaultfile123";
int sleep_time = 1000;
struct task_struct *task1;

int rtw_atoi(char *s) {
	int num = 0, flag = 0;
	int i;
	for (i = 0; i <= strlen(s); i++) {
		if (s[i] >= '0' && s[i] <= '9')
			num = num * 10 + s[i] - '0';
		else if (s[0] == '-' && i == 0)
			flag = 1;
		else
			break;
	}
	if (flag == 1)
		num = num * -1;
	return num;
}

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
	writeFile(fp, message, strlen(message)); 				
	filp_close(fp, NULL);
}

static int write_thread(void* data) {
	while(!kthread_should_stop()) {
		char * path;
		struct file *fp;
		char bigbuff[100];
		int i;
		
		path = PATH "time.conf";
		fp = filp_open(path, O_RDONLY, 0);
		for (i = 0; i < 100; i++) { bigbuff[i] = '\0'; }
		readFile(fp, bigbuff, 99);
		filp_close(fp, NULL);
		
		sleep_time = rtw_atoi(bigbuff);
		
		path = PATH "filename.conf";
		fp = filp_open(path, O_RDONLY, 0);
		for (i = 0; i < 100; i++) { bigbuff[i] = '\0'; }
		readFile(fp, bigbuff, 99);
		filp_close(fp, NULL);
		
		for (i = 0; i < 100; i++) { 
			if (bigbuff[i] == '\n')  {
				bigbuff[i] = '\0';
			}
		}
		
		write_file_path = bigbuff;
			
		//printk(bigbuff);
		myWrite(write_file_path);
		msleep(sleep_time);
	}
	return 0;
}	

static int __init mdl_init(void) {
	pr_info("Hello from kernel - init\n");
	task1 = kthread_run(&write_thread, NULL, "Write_thread");
	return 0;
}

static void __exit mdl_exit(void) {
	int ret = kthread_stop(task1);
	pr_info("Hello from kernel - cleanup, waiting for task 1...\n"); 
	pr_info("Hello from kernel - task 1 returned with code %d", ret);
  	pr_info("Hello from kernel - exit\n"); 
}

MODULE_LICENSE("GPL");
module_init(mdl_init);
module_exit(mdl_exit);
