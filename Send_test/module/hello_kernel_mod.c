#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/fcntl.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <linux/kthread.h>

#define PATH "/etc/Hello_dir/"
#define BUFF_SIZE 512

char* message = "Hello from kernel module\n";
char* write_file_path = "/home/default_hello";
long sleep_time = 10000;
struct task_struct *task1;

// Функция чтения файла
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

// Функция записи в файл
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

// Основная функция
static int write_thread(void* data) {
	while(!kthread_should_stop()) {
		char * path;
		struct file *fp1; 
		struct file *fp2;
		struct file *fpw;
		char bigbuff[BUFF_SIZE];
		int i;
		
		// Чтение значения таймера из conf файла
		path = PATH "time.conf";
		fp1 = filp_open(path, O_RDONLY, 0);
		if (!IS_ERR(fp1)) {
			//printk("Hello from kernel - Read timer...\n");
			for (i = 0; i < BUFF_SIZE; i++) { bigbuff[i] = '\0'; }
			readFile(fp1, bigbuff, BUFF_SIZE - 1);
			filp_close(fp1, NULL);
			
			// Перевод таймера из строки в число, при ошибке
			// задаем значение по умолчанию (10 с)
			if (kstrtol(bigbuff, 0, &sleep_time)) {
				sleep_time = 10000;
			}
		}
	
		// Чтение значения имени файла из conf файла
		path = PATH "filename.conf";
		fp2 = filp_open(path, O_RDONLY, 0);
		if (!IS_ERR(fp2)) {
			//printk("Hello from kernel - Read name...\n");
			for (i = 0; i < BUFF_SIZE; i++) { bigbuff[i] = '\0'; }
			readFile(fp2, bigbuff, BUFF_SIZE - 1);
			filp_close(fp2, NULL);
			write_file_path = bigbuff;
		}
		
		// Запись сообщения в заданный файл	
		fpw = filp_open(write_file_path, O_CREAT | O_WRONLY | O_APPEND, 0666);
		if (!IS_ERR(fpw)) {
			//printk("Hello from kernel - Write...\n");
			writeFile(fpw, message, strlen(message)); 				
			filp_close(fpw, NULL);
		}
		
		// Таймер
		msleep(sleep_time);
		//printk("Hello from kernel - ITER\n");
		//printk("Hello from kernel - %s\n", write_file_path);
		//printk("Hello from kernel - %ld\n", sleep_time);
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
