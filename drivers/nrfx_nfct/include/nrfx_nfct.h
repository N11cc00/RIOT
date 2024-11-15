#ifndef NRFX_NFCT_H
#define NRFX_NFCT_H

// TODO: check for nrfx package in use here

typedef struct {
    // TODO
} nrfx_nfct_t;   

int init_nrfx_nfct(nrfx_nfct_t *nfct, const nrfx_nfct_params_t *params);

int enable_nrfx_nfct(nrfx_nfct_t *nfct);

int disable_nrfx_nfct(nrfx_nfct_t *nfct);


#endif