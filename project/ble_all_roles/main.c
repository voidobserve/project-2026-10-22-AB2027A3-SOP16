#include "include.h"
#include "driver_lowpwr.h"


int main(void)
{
    sys_cb.rst_reason = sys_rst_init(WK0_10S_RESET);
    printf("Hello AB202X: %08x\n", sys_cb.rst_reason);
    sys_rst_dump(sys_cb.rst_reason);

    sys_cb.wakeup_reason  = lowpwr_get_wakeup_source();

    printf("SW_VERSION: %s\n",SW_VERSION);

    sys_ram_info_dump();

    bsp_sys_init();

    prod_test_init();

    func_run();

    return 0;
}
