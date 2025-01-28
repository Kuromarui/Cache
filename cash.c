#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/smp.h> 
#include <asm/processor.h> 
#include <asm/msr.h> 
#include <asm/cpu.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andr");
MODULE_DESCRIPTION("A kernel module to disable CPU cache and MTRR across all CPUs.");
MODULE_VERSION("0.4");

#define MSR_MTRRdefType 0x2FF
#define MTRR_TYPE_WRBACK 0x06

static void enable_mtrr(void) {
    wrmsrl(MSR_MTRRdefType, MTRR_TYPE_WRBACK);
    printk(KERN_INFO "Enabling MTRR on CPU %d.\n", smp_processor_id());
}

static void update_cr0_cache_state(bool enable) {
    unsigned long cr0;


    cr0 = read_cr0();
    if (enable) {
        cr0 &= ~(1UL << 30); 
        printk(KERN_INFO "Enabling cache on CPU %d.\n", smp_processor_id());
    } else {
        cr0 |= (1UL << 30); 
        printk(KERN_INFO "Disabling cache on CPU %d.\n", smp_processor_id());
    }

    write_cr0(cr0);
    wbinvd();

}


static void disable_mtrr(void) {

    wrmsrl(MSR_MTRRdefType, 0x00);
    printk(KERN_INFO "Disabling MTRR on CPU %d.\n", smp_processor_id());
}

static void disable_cache_on_all_cpus(void *info) {
    update_cr0_cache_state(false);
    //disable_mtrr();
}

static void enable_cache_on_all_cpus(void *info) {
    update_cr0_cache_state(true);
    //enable_mtrr();
}

static int __init cache_control_init(void) {
    printk(KERN_INFO "Loading cache control module...\n");
    
    smp_call_function_single(12,disable_cache_on_all_cpus, NULL, 1);
    return 0;
}

static void __exit cache_control_exit(void) {
    printk(KERN_INFO "Unloading cache control module...\n");
    smp_call_function(enable_cache_on_all_cpus, NULL, 1);
}

module_init(cache_control_init);
module_exit(cache_control_exit);