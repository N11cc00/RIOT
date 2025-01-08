#ifndef CFG_NFCT_DEFAULT_H
#define CFG_NFCT_DEFAULT_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#if IS_USED(MODULE_NFCT) && IS_USED(MODULE_NRFX_NFCT_T2T_EMULATOR)
    #if IS_USED(MODULE_NFCT_T2T_EMULATOR)
        #include "nfct_t2t_emulator.h"
        #define DEFAULT_T2T_EMULATOR_DEV  (nfct_t2t_emulator_t) {   \
            .init = initialize_t2t,                 \
            .enable = enable_t2t,                   \
            .disable = disable_t2t,                 \
            .uninit = uninitialize_t2t              \
        }
    #endif
#endif

#ifndef DEFAULT_T2T_EMULATOR_DEV
#include "nfct_t2t_emulator.h"
#include "nrfx_nfct_t2t_emulator.h"
#define DEFAULT_T2T_EMULATOR_DEV (nfct_t2t_emulator_t) {   \
    .init = initialize_t2t,                 \
    .enable = enable_t2t,                   \
    .disable = disable_t2t,                 \
    .uninit = uninitialize_t2t              \
}
#endif


#ifdef __cplusplus
}
#endif

#endif /* CFG_NFCT_DEFAULT_H */