#include <nrfx_nfct.h>
#include <assert.h>

static void test_init(void) {
    nrfx_nfct_config_t config = {
        .rxtx_int_mask = 0,
        .cb = NULL,
        .irq_priority = 0
    };
    nrfx_err_t error = nrfx_nfct_init(&config);
    assert(error == NRFX_SUCCESS);
}

int main(void) {
    test_init();
    return 0;
}