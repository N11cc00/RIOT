#ifndef NFCT_H
#define NFCT_H-

// struct that requires the nfct config
typedef struct {} nfct_config_t;

// pointer to an enum
typedef enum {
    NFCT_EVENT_FIELD_DETECTED,
    NFCT_EVENT_FIELD_LOST,
    NFCT_EVENT_SELECTED,
    NFCT_EVENT_END_OF_TX,
    NFCT_EVENT_START_OF_TX,
    NFCT_EVENT_START_OF_RX,
    NFCT_EVENT_END_OF_RX
} nfct_events_t;

// struct with function pointers and enums that need to be implemented by an nfct device
typedef struct {
    void *(start_tx) (void);
    void *(start_rx) (void);
    int *(init) (const nfct_config_t *nfct_config);
    void *(uninit) (void);
    void *(enable) (void);
    void *(disable) (void);
    bool supports_t2t;
    bool supports_t4t;
} nfct_t;

#endif