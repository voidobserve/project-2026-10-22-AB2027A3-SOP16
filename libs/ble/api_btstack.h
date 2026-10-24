#ifndef _API_BTSTACK_H
#define _API_BTSTACK_H

//BLE WORK MODE
typedef enum {
    MODE_NORMAL     = 0,       //normal mode
    MODE_FCC_TEST   = 2,       //fcc test mode, use uart for communication
    MODE_BQB_RF_BLE = 4,       //bqb test mode，use uart for communication
    MODE_IODM_TEST  = 5,       //iodm test mode，use uart for communication
    MODE_WIRELESS   = 6,       //wireless mode
} BT_WORK_MODE;


typedef enum {
    FCC_TX_TONE = 1,
    FCC_TX_TEST,
    FCC_RX_TEST,
} FCC_RF_MODE;

enum {
    TXPKT_KICK_DISABLE = 0,
    TXPKT_KICK_ENABLE,
    TXPKT_KICK_AUTO,
};

typedef enum {
    BT_ALARM_ERR_CODE_NO_ERR = 0x00,
    BT_ALARM_ERR_CODE_UNINITIALIZE,
    BT_ALARM_ERR_CODE_NOT_FREE_MEM,
    BT_ALARM_ERR_CODE_NOT_EXIST_ID,
    BT_ALARM_ERR_CODE_ALREADY_ACTIVE,
    BT_ALARM_ERR_CODE_ALREADY_STOP,
    BT_ALARM_ERR_CODE_INTV_OUT_RANGE,
} bt_timer_err_code_t;

/// Specify what PHY Host prefers to use for RX or TX HCI:7.8.48 / HCI:7.8.49
enum le_phy_mask
{
    /// The Host prefers to use the LE 1M transmitter/receiver PHY (possibly among others)
    PHY_1MBPS_BIT       = (1<<0),
    /// The Host prefers to use the LE 2M transmitter/receiver PHY (possibly among others)
    PHY_2MBPS_BIT       = (1<<1),
    /// The Host prefers to use the LE Coded transmitter/receiver PHY (possibly among others)
    PHY_CODED_BIT       = (1<<2),
};

enum le_phy_value
{
    PHY_UNDEF_VALUE    = 0,
    PHY_1MBPS_VALUE    = 1,
    PHY_2MBPS_VALUE    = 2,
    PHY_CODED_VALUE    = 3,
    PHY_CODED_S2_VALUE = 4,
};

/// Advertising properties bit field bit positions
enum le_adv_prop_bf
{
    // Indicate that advertising is connectable, reception of CONNECT_REQ or AUX_CONNECT_REQ
    // PDUs is accepted. Not applicable for periodic advertising.
    LE_ADV_PROP_CONNECTABLE_POS             = 0,
    LE_ADV_PROP_CONNECTABLE_BIT             = (1UL << LE_ADV_PROP_CONNECTABLE_POS),

    // Indicate that advertising is scannable, reception of SCAN_REQ or AUX_SCAN_REQ PDUs is
    // accepted
    LE_ADV_PROP_SCANNABLE_POS               = 1,
    LE_ADV_PROP_SCANNABLE_BIT               = (1UL << LE_ADV_PROP_SCANNABLE_POS),

    // Indicate that advertising targets a specific device. Only apply in following cases:
    //   - Legacy advertising: if connectable
    //   - Extended advertising: connectable or (non connectable and non discoverable)
    LE_ADV_PROP_DIRECTED_POS                = 2,
    LE_ADV_PROP_DIRECTED_BIT                = (1UL << LE_ADV_PROP_DIRECTED_POS),

    // Indicate that High Duty Cycle has to be used for advertising on primary channel
    // Apply only if created advertising is not an extended advertising
    LE_ADV_PROP_HDC_POS                     = 3,
    LE_ADV_PROP_HDC_BIT                     = (1UL << LE_ADV_PROP_HDC_POS),

    // Use legacy advertising PDUs
    LE_ADV_PROP_LEGACY_POS                  = 4,
    LE_ADV_PROP_LEGACY_BIT                  = (1UL << LE_ADV_PROP_LEGACY_POS),

    // Omit advertiser's address from all PDUs("anonymous advertising").
    // Valid only if created advertising is an extended advertising
    LE_ADV_PROP_ANONYMOUS_POS               = 5,
    LE_ADV_PROP_ANONYMOUS_BIT               = (1UL << LE_ADV_PROP_ANONYMOUS_POS),

    // Include TxPower in the extented header of at least one advertisint PDU.
    // Valid only if created advertising is not a legacy advertising
    LE_ADV_PROP_TX_PWR_POS                  = 6,
    LE_ADV_PROP_TX_PWR_BIT                  = (1UL << LE_ADV_PROP_TX_PWR_POS),
};

/// Advertising properties configurations for extended advertising
enum le_ext_adv_prop
{
    // Non connectable non scannable extended advertising
    LE_EXT_ADV_PROP_NON_CONN_NON_SCAN_MASK  = 0x0000,
    // Non connectable scannable extended advertising
    LE_EXT_ADV_PROP_NON_CONN_SCAN_MASK      = LE_ADV_PROP_SCANNABLE_BIT,
    // Non connectable scannable directed extended advertising
    LE_EXT_ADV_PROP_NON_CONN_SCAN_DIR_MASK  = LE_ADV_PROP_SCANNABLE_BIT | LE_ADV_PROP_DIRECTED_BIT,
    // Non connectable anonymous directed extended advertising
    LE_EXT_ADV_PROP_ANONYM_DIR_MASK         = LE_ADV_PROP_ANONYMOUS_BIT | LE_ADV_PROP_DIRECTED_BIT,
    // Undirected connectable extended advertising
    LE_EXT_ADV_PROP_UNDIR_CONN_MASK         = LE_ADV_PROP_CONNECTABLE_BIT,
    // Directed connectable extended advertising
    LE_EXT_ADV_PROP_DIR_CONN_MASK           = LE_ADV_PROP_CONNECTABLE_BIT | LE_ADV_PROP_DIRECTED_BIT,
    // Legacy un directed connectable and scannable advertising
    LE_LEGACY_ADV_PROP_MASK                 = LE_ADV_PROP_CONNECTABLE_BIT | LE_ADV_PROP_SCANNABLE_BIT | LE_ADV_PROP_LEGACY_BIT,
};

enum le_initiator_filter_policy
{
    LE_INIT_FILT_IGNORE_WLST = 0x00,
    LE_INIT_FILT_USE_WLST = 0x01,
};

///Advertising filter policy
enum le_adv_filter_policy
{
    ///Allow both scan and connection requests from anyone
    LE_ADV_ALLOW_SCAN_ANY_CON_ANY    = 0x00,
    ///Allow both scan req from White List devices only and connection req from anyone
    LE_ADV_ALLOW_SCAN_WLST_CON_ANY,
    ///Allow both scan req from anyone and connection req from White List devices only
    LE_ADV_ALLOW_SCAN_ANY_CON_WLST,
    ///Allow scan and connection requests from White List devices only
    LE_ADV_ALLOW_SCAN_WLST_CON_WLST,
};

enum adv_channel_map
{
    ///Byte value for advertising channel map for channel 37 enable
    ADV_CHNL_37_EN                = 0x01,
    ///Byte value for advertising channel map for channel 38 enable
    ADV_CHNL_38_EN                = 0x02,
    ///Byte value for advertising channel map for channel 39 enable
    ADV_CHNL_39_EN                = 0x04,
    ///Byte value for advertising channel map for channel 37, 38 and 39 enable
    ADV_ALL_CHNLS_EN              = 0x07,
};

typedef enum {
    LE_CONN_INITIATE_ERR_NO_ERR,
    LE_CONN_INITIATE_ERR_CONN_EXISTS,
    LE_CONN_INITIATE_ERR_MEMORY_CAPA_EXCEED,
    LE_CONN_INITIATE_ERR_INVALID_PARAM,
} le_conn_initiate_err_code_t;

#define BLE_ALARM_TIMER_LEN                                         28


//BLE_EVT_FORMAT
/**
 * @format 21
 * @param handle
 * @param status
 */
#define BLE_EVT_PROFILE_FIND_DONE                                   0xA0

/**
 * @format 2228
 * @param con_handle
 * @param start_group_handle
 * @param end_group_handle
 * @param uuid128
 */
#define BLE_EVT_SERVICE_FIND_RESULT                                 0xA1

/**
 * @format 222228
 * @param con_handle
 * @param start_handle
 * @param value_handle
 * @param end_handle
 * @param properties
 * @param uuid128
 */
#define BLE_EVT_CHARACTERISTIC_FIND_RESULT                          0xA2


/**
 * @format 12222
 * @param status
 * @param handle
 * @param interval
 * @param latency
 * @param timeout
 */
#define BLE_EVT_CONNECT_PARAM_UPDATE_DONE                           0xB2

/**
 * @format 16211
 * @param address_type
 * @param address
 * @param handle
 * @param role
 * @param status
 */
#define BLE_EVT_CONNECT                                             0xB3

/**
 * @format 21
 * @param handle
 * @param disconnect_reason
 */
#define BLE_EVT_DISCONNECT                                          0xB4

/**
 * @format 22
 * @param handle
 * @param MTU
 */
#define BLE_EVT_MTU_EXCHANGE_DONE                                   0xB5

 /**
  * @format 122
  * @param status
  * @param conn_handle
  * @param attribute_handle
  */
#define BLE_EVT_INDICATE_DONE                                       0xB6

/**
 * @format 121
 * @param status
 * @param conn_handle
 * @param encryption_enabled
 */
#define BLE_EVT_ENCRYPTION_CHANGE                                   0xC7

 /**
  * @format 21614
  * @param handle
  * @param addr_type
  * @param address
  * @param secure_connection - set to 1 if LE Secure Connection pairing will be used
  * @param passkey
  */
#define BLE_EVT_PASSKEY_DISPLAY                                     0xC9


 /**
  * @format 2161
  * @param handle
  * @param addr_type
  * @param address
  * @param secure_connection - set to 1 if LE Secure Connection pairing will be used
  */
#define BLE_EVT_PASSKEY_INPUT                                       0xCB

  /**
   * @format H1B14
   * @param handle
   * @param addr_type
   * @param address
   * @param secure_connection - set to 1 if LE Secure Connection pairing will be used
   * @param passkey
   */
#define BLE_EVT_NUMERIC_COMPARISON_REQUEST                          0xCC

 /**
  * @format 216
  * @param handle
  * @param addr_type
  * @param address
  */
#define BLE_EVT_IDENTITY_RESOLVING_FAIL                             0xCE

 /**
  * @brief Identify resolving succeeded
  *
  * @format 216162
  * @param handle
  * @param addr_type
  * @param address
  * @param identity_addr_type
  * @param identity_address
  * @param index
  *
  */
#define BLE_EVT_IDENTITY_RESOLVING_SUCCEED                          0xCF


/**
 * @brief Emitted to inform app that pairing has started.
 * @format 216
 * @param handle
 * @param addr_type
 * @param address
 */
#define BLE_EVT_PAIRING_START                                       0xD4

/**
  * @brief Emitted to inform app that pairing is complete.
  * @format 21611
  * @param handle
  * @param addr_type
  * @param address
  * @param status
  * @param reason
  */
#define BLE_EVT_PAIRING_DONE                                        0xD5

/**
 * @brief Emitted to inform app that ltk is missing.
 * @format 216
 * @param handle
 * @param addr_type
 * @param address
 */
#define BLE_EVT_LTK_MISSING                                         0xD8


/**
 * @format 11611N
 * @param advertising_event_type
 * @param address_type
 * @param address
 * @param rssi
 * @param data_length
 * @param data
 */
#define BLE_EVT_ADV_REPORT                                          0xE2

/**
 * @format 21B1111121BJV
 * @param advertising_event_type
 * @param address_type
 * @param address
 * @param primary_phy
 * @param secondary_phy
 * @param advertising_sid
 * @param tx_power
 * @param rssi
 * @param periodic_advertising_interval
 * @param direct_address_type
 * @param direct_address
 * @param data_length
 * @param data
 */
#define BLE_EVT_EXTEND_ADV_REPORT                                   0xE3u

typedef enum {
    ATT_ERR_NO_ERR                                              = 0x00,
    ATT_ERR_INVALID_HANDLE                                      = 0x01,
    ATT_ERR_READ_NOT_PERMITTED                                  = 0x02,
    ATT_ERR_WRITE_NOT_PERMITTED                                 = 0x03,
    ATT_ERR_INVALID_PDU                                         = 0x04,
    ATT_ERR_INSUFFICIENT_AUTHENTICATION                         = 0x05,
    ATT_ERR_REQUEST_NOT_SUPPORTED                               = 0x06,
    ATT_ERR_INVALID_OFFSET                                      = 0x07,
    ATT_ERR_INSUFFICIENT_AUTHORIZATION                          = 0x08,
    ATT_ERR_PREPARE_QUEUE_FULL                                  = 0x09,
    ATT_ERR_ATTRIBUTE_NOT_FOUND                                 = 0x0a,
    ATT_ERR_ATTRIBUTE_NOT_LONG                                  = 0x0b,
    ATT_ERR_INSUFFICIENT_ENCRYPTION_KEY_SIZE                    = 0x0c,
    ATT_ERR_INVALID_ATTRIBUTE_VALUE_LENGTH                      = 0x0d,
    ATT_ERR_UNLIKELY_ERROR                                      = 0x0e,
    ATT_ERR_INSUFFICIENT_ENCRYPTION                             = 0x0f,
    ATT_ERR_UNSUPPORTED_GROUP_TYPE                              = 0x10,
    ATT_ERR_INSUFFICIENT_RESOURCES                              = 0x11,
    ATT_ERR_VALUE_NOT_ALLOWED                                   = 0x13,

    ATT_ERR_IN_WRONG_STATE                                      = 0x80,
    ATT_ERR_NOTIFICATION_NOT_SUPPORTED                          = 0x81,
    ATT_ERR_INDICATION_NOT_SUPPORTED                            = 0x82,
    ATT_ERR_INDICATION_IN_PORGRESS                              = 0x83,
    ATT_ERR_TIMEOUT                                             = 0x84,
    ATT_ERR_DISCONNECT                                          = 0x85,
    ATT_ERR_DATA_MISMATCH                                       = 0x86,
} ATT_ERR_CODE;

//Attribute Property
#define ATT_PROPERTY_BROADCAST                                      0x01
#define ATT_PROPERTY_READ                                           0x02
#define ATT_PROPERTY_WRITE_WITHOUT_RESPONSE                         0x04
#define ATT_PROPERTY_WRITE                                          0x08
#define ATT_PROPERTY_NOTIFY                                         0x10
#define ATT_PROPERTY_INDICATE                                       0x20
#define ATT_PROPERTY_AUTHENTICATED_SIGNED_WRITE                     0x40
#define ATT_PROPERTY_EXTENDED_PROPERTIES                            0x80


//GATT_CLIENT_CHARACTERISTIC_CONFIGURATION
#define GATT_CCC_NOTIFICATION                                       0x01
#define GATT_CCC_INDICATION                                         0x02


//ATT_TRANSACTION_MODE
#define ATT_TRANSACTION_WRITE                                       0x00
#define ATT_TRANSACTION_PREPARE_WRITE                               0x01
#define ATT_TRANSACTION_WRITE_EXECUTE                               0x02
#define ATT_TRANSACTION_WRITE_CANCEL                                0x03


//SM_AUTHREQ
#define SM_AUTHREQ_NO_BONDING                                       0x00
#define SM_AUTHREQ_BONDING                                          0x01
#define SM_AUTHREQ_MITM_PROTECTION                                  0x04
#define SM_AUTHREQ_SECURE_CONNECTION                                0x08
#define SM_AUTHREQ_KEYPRESS                                         0x10
#define SM_AUTHREQ_CT2                                              0x20

typedef uint8_t sm_key_t[16];

typedef uint16_t ble_con_handle_t;

typedef uint8_t bt_timer_handle_t;

typedef void (*kick_func_t)(uint16_t con_handle, uint16_t config);

typedef uint16_t (*att_read_callback_t)(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size);

typedef int (*att_write_callback_t)(uint16_t con_handle, uint16_t attribute_handle, uint16_t trans_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size);

typedef uint16_t (*att_notify_callback_t)(uint16_t con_handle, uint16_t attribute_handle, uint8_t * buffer, uint16_t buffer_size);

typedef void (*att_event_callback_t) (uint8_t event_type, uint8_t *param, uint16_t size);

typedef void (*ble_init_callback_t)(void);

typedef void (*bb_timer_proc_callback_t)(uint32_t time);

typedef void (*ble_vs_tbox_init_callback_t)(void);

// IO Capability Values
enum {
    IO_CAPABILITY_DISPLAY_ONLY = 0,
    IO_CAPABILITY_DISPLAY_YES_NO,
    IO_CAPABILITY_KEYBOARD_ONLY,
    IO_CAPABILITY_NO_INPUT_NO_OUTPUT,
    IO_CAPABILITY_KEYBOARD_DISPLAY, // not used by secure simple pairing
};

//ADDRESS TYPE
typedef enum {
    GAP_PUBLIC_ADDRESS_TYPE = 0,
    GAP_RANDOM_ADDRESS_TYPE_STATIC,
    GAP_RANDOM_ADDRESS_NON_RESOLVABLE,
    GAP_RANDOM_ADDRESS_RESOLVABLE,
} gap_random_address_type_t;

typedef enum {
    BD_ADDR_TYPE_PUBLIC = 0,
    BD_ADDR_TYPE_RANDOM = 1,
} bd_addr_type_t;


//ADV_TYPE
enum {
    ADV_TYPE_IND = 0,
    ADV_TYPE_DIRECT_HI_IND,
    ADV_TYPE_SCAN_IND,
    ADV_TYPE_NONCONN_IND,
    ADV_TYPE_DIRECT_LO_IND,
};

//SCAN STA
typedef enum {
    LE_SCAN_STA_DISABLE,
    LE_SCAN_STA_ENABLE,
    LE_SCAN_STA_W4_DISABLE,
    LE_SCAN_STA_W4_ENABLE,
} le_scanning_state_t;

typedef struct {
    uint8_t adv_en;
    uint8_t adv_data[31];
    uint8_t adv_len;
    uint8_t scan_rsp[31];
    uint8_t scan_rsp_len;
    uint16_t adv_int_min;           //advertisement interval
    uint16_t adv_int_max;           //advertisement interval
    uint8_t adv_type;
    uint8_t direct_address_typ;     //only use for direct adv
    uint8_t direct_address[6];      //only use for direct adv
    uint8_t channel_map;
    uint8_t filter_policy;
} adv_param_t;

typedef struct __attribute__((packed)){
    uint8_t     event_type;
    uint8_t     addr_type;
    uint8_t     addr[6];
    char        rssi;
    uint8_t     report_len;
    const uint8_t * report;
    uint8_t     offset;
} adv_report_t;

typedef struct __attribute__((packed)){
    uint16_t    evt_type;
    uint8_t     addr_type;
    uint8_t     addr[6];
    uint8_t     phy;
    uint8_t     phy2;
    uint8_t     adv_sid;
    uint8_t     tx_pwr;
    char        rssi;
    uint16_t     intv;
    ///Direct address type
    uint8_t     dir_addr_type;
    ///Direct address value
    uint8_t     dir_addr[6];
    uint8_t     report_len;
    const uint8_t * report;
    uint8_t     offset;
} ext_adv_report_t;

typedef struct {
    uint8_t adv_en;
    uint8_t adv_data[31];
    uint8_t adv_len;
    uint8_t scan_rsp[31];
    uint8_t scan_rsp_len;
    uint8_t peer_addr_type;
    uint8_t peer_addr[6];
    uint8_t channel_map;
    uint8_t filter_policy;

    // Used to identify an advertising set, range:0x00 ~ 0xef
    uint8_t adv_hdl;
    // BIT(0)-Connectable advertising / BIT(1)-Scannable advertising / BIT(2)-Directed advertising
    // BIT(3)-High Duty Directed Connectable advertising(less than or equl to 3.75ms Advertising Interval)
    // BIT(4)-Use legacy advertising PDUs / BIT(5)-Omit advertiser's address from all PDUs("anonymous advertising")
    // BIT(6)-Include TxPower in the extented header of at least one advertisint PDU
    uint8_t adv_evt_prop;
    // Minimum advertising interval for undirected and low duty cycle directed advertising.Range:0x20 ~ 0xffffff.Unit 0.625ms
    uint32_t prim_adv_int_min;
    // Maximum advertising interval for undirected and low duty cycle directed advertising.Range:0x20 ~ 0xffffff.Unit 0.625ms
    uint32_t prim_adv_int_max;
    // Range -127 to +20.Unit:dbm
    int8_t max_tx_pwr;
    // 0x01-Primary advertising PHY is LE 1M / 0x03-Primary advertising PHY is LE Coded
    uint8_t prim_phy;
    // 0x00-AUX_ADV_IND shall be sent prior to the next advertising event
    // 0x01-Maximum advertsing events the Controller can skip before sending the AUX_ADV_IND packets on the secondary advertising physical channel.
    uint8_t max_skip;
    // 0x01-Secondary advertising PHY is LE 1M / 0x02-Secondary advertising PHY is LE 2M
    // 0x03-Secondary advertising PHY is LE Coded:125K / 0x03-Secondary advertising PHY is LE Coded:500K
    uint8_t second_phy;
    // Value of the Advertising SID subfield in the ADI field of the PDU
    uint8_t adv_sid;
    // 0x00-Scan request notifications disabled / 0x01-Scan request notifications enabled
    uint8_t scan_req_notif_en;
} ext_adv_param_t;

typedef struct {
     uint8_t   adv_type;
     uint8_t   data_len;
     uint8_t*  data;
} adv_struct_t;

typedef struct {
    uint8_t scan_type;
    uint16_t scan_interval;
    uint16_t scan_window;
} scan_param_t;

typedef struct {
    // 0x00-Basic unfiltered scanning filter policy / 0x01-Basic filtered scanning filter policy
    // 0x02-Extented unfiltered scanning filter policy / 0x03-Extented filtered scanning filter policy
    uint8_t scan_filter_policy;
    // BIT(0)-Scan advertising on the LE 1M PHY
    // BIT(1)-Scan advertising on the LE 2M PHY
    // BIT(2)-Scan advertising on the LE Coded PHY
    uint8_t scan_phys;
        // 0x00-Passive Scanning.No scan request PDUs shall be sent.
        // 0x01-Active Scanning.Scan request PDUs shall be sent.
    uint8_t scan_type;
    uint16_t scan_interval;
    uint16_t scan_window;
} ext_scan_param_t;

typedef struct __attribute__((packed))
{
    ///Initiator filter policy
    uint8_t     init_filter_policy;
    ///Own address type public/random/rpa
    //uint8_t     own_addr_type;
    ///Peer address type public/random/rpa
    uint8_t     peer_addr_type;
    ///Peer address
    uint8_t     peer_addr[6];
    ///Indicates the PHY(s) on which the advertising packets should be received
    uint8_t     init_phys;
    ///Paramaters for PHY(s), PHY_1M / PHY_2M / PHY_CODED
    ///Scan interval (N * 0.625 ms)
    //uint16_t    scan_interval;
    ///Scan window size (N * 0.625 ms)
    //uint16_t    scan_window;
    ///Minimum of connection interval (N * 1.25 ms)
    uint16_t    con_intv_min;
    ///Maximum of connection interval (N * 1.25 ms)
    uint16_t    con_intv_max;
    ///Connection latency
    uint16_t    con_latency;
    ///Link supervision timeout
    uint16_t    superv_to;
    ///Minimum CE length (N * 0.625 ms)
    uint16_t    ce_len_min;
    ///Maximum CE length (N * 0.625 ms)
    uint16_t    ce_len_max;
} ext_conn_create_param_t;

typedef uint8_t bd_addr_t[6];

typedef struct {
    // Key value MSB -> LSB
    uint8_t key[16];
}gap_sec_key_t;

// Do not modify!!
typedef struct {
    // Address type of device. 0 public/1 random
    uint8_t addr_type;
    // BD Address of device
    bd_addr_t addr;
}gap_bdaddr_t;

/**
 * @brief Identity Resolving Key Information
 */
typedef struct {
    // Identify Resolving Key
    gap_sec_key_t irk;
    // Identity broadcast address of device
    gap_bdaddr_t addr;
}gapc_irk_t;

/**
 * @brief Resolving list information about a device.
 */
typedef struct {
    // Privacy Mode
    uint8_t priv_mode;  //PRIV_TYPE_NETWORK = 0x00, Controller accept identity address only
                        //PRIV_TYPE_DEVICE  = 0x01, Controller accept both identity address and resolvable address.
    // Device Identity address
    gap_bdaddr_t addr;
    // Peer device identity resolve key
    uint8_t peer_irk[16];
}gap_ral_dev_info_t;

typedef struct {
    uint8_t rpa_en;
    uint8_t privacy_mode;
    uint16_t renew_to;
    gap_sec_key_t peer_irk;
    gap_bdaddr_t addr;
}rpa_param_t;

typedef struct {
    // Number of entries to be added in the list. 0 means that list content has to be cleard.
    uint8_t size;
    // List of entries to be added in the list.
    gap_bdaddr_t addr;
}wlist_param_t;

typedef struct {
    uint16_t        address_type;
    uint16_t        real_address_type;
    bd_addr_t       address;
    gap_sec_key_t   peer_irk;
} sm_bonding_info_t;

// Read & Write Callbacks for handle range
typedef struct att_service_handler {
    void * item;
    uint16_t start_handle;
    uint16_t end_handle;
    att_read_callback_t read_callback;
    att_write_callback_t write_callback;
    att_event_callback_t event_handler;
} att_service_handler_t;

typedef struct att_client_handler {
    void * item;
    att_read_callback_t read_callback;
    att_notify_callback_t notify_callback;
    att_event_callback_t event_handler;
} att_client_handler_t;

typedef struct {
    uint16_t start_group_handle;
    uint16_t end_group_handle;
    uint16_t uuid16;
    uint8_t  uuid128[16];
} gatt_client_service_t;

typedef struct {
    uint16_t start_handle;
    uint16_t value_handle;
    uint16_t end_handle;
    uint16_t properties;
    uint16_t uuid16;
    uint8_t  uuid128[16];
} gatt_client_characteristic_t;

struct txbuf_tag {
    uint8_t *ptr;
    uint16_t len;
    uint16_t att_handle;
    uint16_t con_handle;
    uint16_t config;
} __attribute__ ((packed)) ;

struct txpkt_tag {
    struct txbuf_tag *buf;
    uint16_t buf_size;
    uint8_t total;
    volatile uint8_t cnt;
    uint8_t rptr;
    volatile uint8_t wptr;
    kick_func_t send_kick;
};

typedef struct {
    uint8_t rf_mode;
    uint8_t tx_power;
    uint8_t tx_freq;
    uint8_t rx_freq;
    uint8_t phy_type;
    uint8_t todo;
} fcc_cfg_t;

typedef enum
{
    BLE_TXPWR_MINUS_12DB = 0x8c,
    BLE_TXPWR_MINUS_6DB = 0x86,
    BLE_TXPWR_MINUS_4DB = 0x84,
    BLE_TXPWR_0DB = 0x00,
    BLE_TXPWR_4DB = 0x04,
    BLE_TXPWR_8DB = 0x08,
} ble_txpwr_sel_t;

typedef struct le_device_memory_db {
    // Identification
    uint16_t addr_type;
    uint16_t real_addr_type;
    bd_addr_t addr;
    sm_key_t irk;

    // Stored pairing information allows to re-establish an enncrypted connection
    // with a peripheral that doesn't have any persistent memory
    sm_key_t ltk;
    uint16_t ediv;
    uint8_t  rand[8];
    uint8_t  key_size;
    uint8_t  authenticated;
    uint8_t  authorized;
} le_device_memory_db_t;

typedef struct
{
    uint8_t  vers;
    uint16_t compid;
    uint16_t subvers;
} ll_version_t;

///Supported LE Features structure
typedef struct
{
    ///8-byte array for LE features
    uint8_t feats[8];
}ll_features_t;

extern struct txpkt_tag ble_tx;

typedef struct {
    uint8_t mem[28];
} bt_timer_mem_t;

//control
void bt_init(void);

int bt_setup(void);

void bt_off(void);

//sleep
bool bt_is_sleep(void);

uint32_t bt_sleep_proc(void);

void bt_enter_sleep(void);

void bt_exit_sleep(void);

uint32_t txpkt_init(struct txpkt_tag *txpkt, void *mem_pool, uint8_t total, uint16_t buf_size);

bool txpkt_is_full(struct txpkt_tag *txpkt);

uint8_t txpkt_available_num(struct txpkt_tag *txpkt);

void txpkt_flush(struct txpkt_tag *txpkt);

/**
  * @brief  Triggers ble to start notify data, Can be used with ble_notify_for_handle_kick function
  * @param  txpkt
  * @param  con_handle
  */
void txpkt_server_send_tick(struct txpkt_tag *txpkt, uint16_t con_handle);

/**
  * @brief  Triggers ble to start write data, Can be used with ble_write_cmd_for_character_kick function(Todo)
  * @param  txpkt
  * @param  con_handle
  */
void txpkt_client_send_tick(struct txpkt_tag *txpkt, uint16_t con_handle);

/**
  * @brief  Triggers ble to start sending data
  * @param  con_handle
  */
void ble_send_kick(uint16_t con_handle, uint16_t config);

/**
  * @brief  ble fcc test mode config.(Currently, only FCC_TX_TONE mode is supported)
  * @param  rf_mode: FCC_RF_MODE
  * @param  tx_power: [0,7]
  * @param  tx_freq: [0,39],freq = 2402 + tx_freq * 2
  * @param  rx_freq: [0,39],freq = 2402 + rx_freq * 2
  * @param  phy_type: 0:1M  1:2M
  * @param  todo: 1
  */
void ble_fcc_cfg_init(fcc_cfg_t *cfg);

/**
 * @brief ble advertisement ctrl
 * @param none
 */
void ble_adv_dis(void);

void ble_adv_en(void);

bool ble_adv_is_en(void);

void ble_ext_adv_en(void);
void ble_ext_adv_dis(void);

/**
  * @brief  set up ble advertisement interval
  * @param  interval: 0x20 - 0x4000(uint:0.625ms)
  * @return return true if ok, invalid parameters will return false
  */
bool ble_set_adv_interval(uint16_t interval);

/**
  * @brief  get ble advertisement interval
  * @param  interval: 0x20 - 0x4000(uint:0.625ms)
  * @return return true if ok, invalid parameters will return false
  */
bool ble_get_adv_interval(uint16_t *interval);

/**
 * @brief set advertisement data
 * @param advertising_data (max 31 octets)
 * @param advertising_data_length
 * @return return true if ok, invalid parameters will return false
 */
bool ble_set_adv_data(const uint8_t *adv_buf, uint32_t size);

/**
 * @brief set scan response Data
 * @param advertising_data (max 31 octets)
 * @param advertising_data_length
 * @return return true if ok, invalid parameters will return false
 */
bool ble_set_scan_rsp_data(const uint8_t *scan_rsp_buf, uint32_t size);

/**
 * @brief initialize advertisement parameter
 * @param advertisement parameter
 * @return return true if ok, invalid parameters will return false
 */
bool ble_set_adv_param(adv_param_t* param);

adv_param_t* ble_get_adv_param(void);

/**
 * @brief initialize advertisement parameter
 * @param advertisement parameter
 * @return return true if ok, invalid parameters will return false
 */
bool ble_set_ext_adv_param(ext_adv_param_t* param);

/**
 * @brief analyse advertisement report, the results will be store in advertisement struct
 * @param report, can be get by BLE_EVT_ADV_REPORT
 * @param adv_struct
 * @return analysis is complete will return true, else will return false
 */
bool ble_adv_report_info_analyse(adv_report_t* report, adv_struct_t* adv_struct);

/**
 * @brief analyse advertisement report, the results will be store in advertisement struct
 * @param report, can be get by BLE_EVT_EXTEND_ADV_REPORT
 * @param adv_struct
 * @return analysis is complete will return true, else will return false
 */
bool ble_ext_adv_report_info_analyse(ext_adv_report_t* report, adv_struct_t* adv_struct);

/**
  * @brief  disconnect ble connection according to connect handle
  * @param  con_handle
  */
void ble_disconnect(uint16_t con_handle);

/**
 * @brief ble master create connect by ble address
 * @param addr_ptr
 * @param addr type : 0-3(0:public, 1:random, 2:public identity 3:random identity)
 * @param interval: 6 - 3200(unit: 1.25ms)
 * @param latency
 * @param timeout (unit: 10ms)
 * @return return LE_CONN_INITIATE_ERR_NO_ERR if ok, other refer to le_conn_initiate_err_code_t;
 */
le_conn_initiate_err_code_t ble_connect_by_addr(uint8_t *addr_ptr, uint8_t addr_type, uint16_t interval, uint16_t conn_latency, uint16_t supervision_timeout);

/**
 * @brief ble master create connect by ble address
 * @param addr_ptr
 * @param addr type : 0-3(0:public, 1:random, 2:public identity 3:random identity)
 * @param interval: 6 - 3200(unit: 1.25ms)
 * @param latency
 * @param timeout (unit: 10ms)
 * @return return true if ok, invalid parameters will return false
 */
bool ble_ext_connect_by_addr(ext_conn_create_param_t *ext_create_conn);

/**
 * @brief ble master cancel a connect creating.
 * @return none
 */
void ble_connect_cancel(void);

/**
 * @brief update of the connection parameter for a given LE connection
 * @param con_handle
 * @param interval: 6 - 3200(unit: 1.25ms)
 * @param latency
 * @param timeout (unit: 10ms)
 * @return return true if ok, invalid parameters will return false
 */
bool ble_update_conn_param(uint16_t con_handle, uint16_t interval, uint16_t latency, uint16_t timeout);

/**
 * @brief update of the connection parameter for a given LE connection
 * @param con_handle
 * @param interval_min: 6 - 3200(unit: 1.25ms)
 * @param interval_max: 6 - 3200(unit: 1.25ms) interval_min <= interval_max
 * @param latency
 * @param timeout (unit: 10ms)
 * @return return true if ok, invalid parameters will return false
 */
bool ble_update_conn_param_2(uint16_t con_handle, uint16_t interval_min, uint16_t interval_max, uint16_t latency, uint16_t timeout);

/**
 * @brief update of the connection parameter for a given LE connection through the LL layer
 * @param con_handle
 * @param interval_min: 6 - 3200(unit: 1.25ms)
 * @param interval_max: 6 - 3200(unit: 1.25ms) interval_min <= interval_max
 * @param latency
 * @param timeout (unit: 10ms)
 * @return return true if ok, invalid parameters will return false
 */
bool ble_ll_update_conn_param(uint16_t con_handle, uint16_t interval_min, uint16_t interval_max, uint16_t latency, uint16_t timeout);

/**
 * @brief get of the connection parameter for a given LE connection
 * @param con_handle
 * @param interval: 6 - 3200(unit: 1.25ms)
 * @param latency
 * @param timeout (unit: 10ms)
 * @return return true if ok, invalid parameters will return false
 */
bool ble_get_conn_param(uint16_t con_handle, uint16_t *interval, uint16_t *latency, uint16_t *timeout);

/**
 * @brief Disable latency, only valid for the slave role. Even if connection parameters are updated later, latency is disabled also
 * @param con_handle
 * @return return true if ok, invalid parameters will return false
 */
bool ble_disable_latency(uint16_t con_handle);

/**
 * @brief Enable latency, only valid for the slave role.
 * @param con_handle
 * @return return true if ok, invalid parameters will return false
 */
bool ble_enable_latency(uint16_t con_handle);

/**
  * @brief  notify data according to handle
  * @param  connect handle
  * @param  attribute handle
  * @param  buf to be send
  * @param  len(must be less than att mtu)
  * @return return 0 if ok, other refer to ATT_ERR_CODE
  */
ATT_ERR_CODE ble_notify_for_handle(uint16_t con_handle, uint16_t att_handle, uint8_t* buf, uint16_t len);

/**
  * @brief  notify data according to handle
  * @param  connect handle
  * @param  attribute handle
  * @param  buf to be send
  * @param  len(must be less than att mtu)
  * @param  kick_cfg, The value is as follows:
            TXPKT_KICK_ENABLE: the Bluetooth thread is triggered to start sending data,
            TXPKT_KICK_DISABLE: the Bluetooth thread will not be triggered to send data immediately, but the data will be cached
            TXPKT_KICK_AUTO: Automatically managed by the Btstack
            It is suitable for continuous data transmission, and can trigger the Bluetooth thread to send data at the last to reduce the switching of threads
  * @return return 0 if ok, other refer to ATT_ERR_CODE
  */
ATT_ERR_CODE ble_notify_for_handle_kick(uint16_t con_handle, uint16_t att_handle, uint8_t* buf, uint16_t len, uint8_t kick_cfg);

/**
  * @brief  indicate data according to handle
  * @param  connect handle
  * @param  attribute handle
  * @param  buf to be send
  * @param  len(must be less than att mtu)
  * @return return 0 if ok, other refer to ATT_ERR_CODE
  */
ATT_ERR_CODE ble_indicate_for_handle(uint16_t con_handle, uint16_t att_handle, uint8_t* buf, uint16_t len);


/**
  * @brief  Writes the characteristic value using the characteristic's value handle
  * @param  connect handle
  * @param  character
  * @param  buf to be send
  * @param  len(must be less than att mtu)
  * @return return 0 if ok, other refer to ATT_ERR_CODE
  */
ATT_ERR_CODE ble_write_req_for_character(uint16_t con_handle, gatt_client_characteristic_t* character, uint8_t* buf, uint16_t len);

/**
  * @brief  Writes the characteristic value using the characteristic's value handle without an acknowledgment that the write was successfully performed.
  * @param  connect handle
  * @param  character
  * @param  buf to be send
  * @param  len(must be less than att mtu)
  * @return return 0 if ok, other refer to ATT_ERR_CODE
  */
ATT_ERR_CODE ble_write_cmd_for_character(uint16_t con_handle, gatt_client_characteristic_t* character, uint8_t* buf, uint16_t len);

/**
 * @brief reads the characteristic value using the characteristic's value handle.
 * @param  con_handle
 * @param  character
 * @return return 0 if ok, other refer to ATT_ERR_CODE
 */
ATT_ERR_CODE ble_read_req_for_character(uint16_t con_handle, gatt_client_characteristic_t* character);

/**
 * @brief reads the long characteristic value using the characteristic's value handle.
 * @param  con_handle
 * @param  character
 * @param  offset
 * @return return 0 if ok, other refer to ATT_ERR_CODE
 */
ATT_ERR_CODE ble_read_blob_for_character(uint16_t con_handle, gatt_client_characteristic_t* character, uint16_t offset);


/**
 * @brief reads the characteristic report reference using the characteristic's end handle.
 *        It is recommended to use this function.
 * @param  con_handle
 * @param  character
 * @return return 0 if ok, other refer to ATT_ERR_CODE
 */
ATT_ERR_CODE ble_read_report_ref_for_character(uint16_t con_handle, gatt_client_characteristic_t* character);

/**
 * @brief set ble address type
 * @param address type
 */
void gap_random_address_set_mode(gap_random_address_type_t random_address_type);

/**
 * @brief BLE initialize callback function registration,
 * @param callback function, which will be called in bluetooth thread
 */
void ble_init_callback_register(ble_init_callback_t  func);

/**
 * @brief BLE test box initialize callback function registration,
 * @param callback function, which will be called in bluetooth thread
 */
void ble_vs_tbox_init_callback_register(ble_vs_tbox_init_callback_t  func);

/**
 * @brief Register server packet handle
 */
void att_server_register_service_handler(att_service_handler_t * handler);

/**
 * @brief Register client packet handle
 */
void att_client_register_handler(att_client_handler_t * handler);

/**
 * @brief ble sm init
 * @param security_req_en(whether to send an sm request when ble connecting)
 * @param io_capability
 * @param authentication_req_flag
 */
void ble_sm_init(uint8_t security_req_en, uint8_t io_capability, uint8_t authentication_req_flag);

/**
 * @brief Trigger Security Request
 */
void ble_sm_send_security_request(ble_con_handle_t con_handle);

/**
 * @brief setup ATT server
 * @param db attribute database created by profile tool
 */
void att_server_init(uint8_t const * db);

/**
 * @brief Set up GATT client.
 */
void gatt_client_init(void);

/**
 * @brief Get att mtu. If status is equal to ATT_ERROR_NO_ERROR, it returns the real value, otherwise the default value ATT_DEFAULT_MTU
 * @param con_handle
 * @param  mtu
 * @return return 0 if ok, other refer to ATT_ERR_CODE
 */
uint8_t att_server_get_mtu(uint16_t con_handle, uint16_t * mtu);

/**
 * @brief Get att mtu. If status is equal to ATT_ERROR_NO_ERROR, it returns the real value, otherwise the default value ATT_DEFAULT_MTU
 * @param  con_handle
 * @param  mtu
 * @return return 0 if ok, other refer to ATT_ERR_CODE
 */
uint8_t gatt_client_get_mtu(uint16_t con_handle, uint16_t * mtu);

/**
 * @brief ble scan ctrl
 * @param none
 */
void ble_scan_en(void);

void ble_scan_dis(void);

/**
 * @brief  ble get scan status
 * @param  none
 * @return sta(refer to le_scanning_state_t)
 */
le_scanning_state_t ble_get_scan_sta(void);

/**
 * @brief ble set scan param
 * @param type    : 0 or 1，0(Passive scan,no scan_rsp)，1(Active scan,adv+scan_rsp)
 * @param interval: 4 - 16384(unit: 0.625ms)
 * @param window  : 4 - 16384(less than or equal to interval unit: 0.625ms)
 * @return return true if ok, invalid parameters will return false
 */
bool ble_set_scan_param(scan_param_t* param);

/**
 * @brief ble set extented scan param
 * @param type    : 0 or 1，0(Passive scan,no scan_rsp)，1(Active scan,adv+scan_rsp)
 * @param interval: 4 - 16384(unit: 0.625ms)
 * @param window  : 4 - 16384(less than or equal to interval unit: 0.625ms)
 * @return return true if ok, invalid parameters will return false
 */
bool ble_set_ext_scan_param(ext_scan_param_t *param);

/**
 * @brief Discovers all primary services. For each found service, BLE_EVT_SERVICE_FIND_RESULT will be generated and passed to the registered callback. BLE_EVT_PROFILE_FIND_DONE, marks the end of discovery.
 * @param  con_handle
 * @return return 0 if ok, other refer to ATT_ERR_CODE
 */
uint8_t ble_client_discover_primary_services(uint16_t con_handle);

/**
 * @brief Discovers a specific primary service given its UUID. For each found service, BLE_EVT_SERVICE_FIND_RESULT will be generated and passed to the registered callback. BLE_EVT_PROFILE_FIND_DONE, marks the end of discovery.
 * @param con_handle
 * @param uuid16
 * @return return 0 if ok, other refer to ATT_ERR_CODE
 */
uint8_t ble_client_discover_primary_services_by_uuid16(uint16_t con_handle, uint16_t uuid16);

uint8_t ble_client_discover_primary_services_by_uuid128(uint16_t con_handle, const uint8_t * uuid128);


/**
 * @brief Get field service from event BLE_EVT_SERVICE_FIND_RESULT
 * @param event packet
 * @param Pointer to storage for service
 */
void ble_service_query_result_get_service(const uint8_t * event, gatt_client_service_t * service);

/**
 * @brief Discovers all characteristics within the specified service. For each found characteristic, an le_characteristics_event_t with type set to BLE_EVT_CHARACTERISTIC_FIND_RESULT will be generated and passed to the registered callback. The gatt_complete_event_t with type set to BLE_EVT_PROFILE_FIND_DONE, marks the end of discovery.
 * @param  callback
 * @param  con_handle
 * @param  service
 * @return return 0 if ok, other refer to ATT_ERR_CODE
 */
uint8_t ble_client_discover_characteristics_for_service(uint16_t con_handle, gatt_client_service_t * service);

/**
 * @brief Get field characteristic from event BLE_EVT_CHARACTERISTIC_FIND_RESULT
 * @param event packet
 * @param Pointer to storage for characteristic
 */
void ble_characteristic_query_result_get_characteristic(const uint8_t * event, gatt_client_characteristic_t * characteristic);

/**
 * @brief Discover all characteristics within the specified service, and return those that match the given UUID. For each found characteristic, BLE_EVT_CHARACTERISTIC_FIND_RESULT will be generated and passed to the registered callback. BLE_EVT_PROFILE_FIND_DONE, marks the end of discovery.
 * @param  con_handle
 * @param  service
 * @param  uuid
 * @return return 0 if ok, other refer to ATT_ERR_CODE
 */
uint8_t ble_client_discover_characteristics_for_service_by_uuid16(uint16_t con_handle, gatt_client_service_t * service, uint16_t uuid16);

uint8_t ble_client_discover_characteristics_for_service_by_uuid128(uint16_t con_handle, gatt_client_service_t * service, const uint8_t * uuid128);


/**
 * @brief Writes the client characteristic configuration of the specified characteristic.
 * @param  con_handle
 * @param  characteristic
 * @param  configuration : GATT_CCC_NOTIFICATION, GATT_CCC_INDICATION
 * @return return 0 if ok, other refer to ATT_ERR_CODE
 */
uint8_t ble_client_write_client_characteristic_configuration(uint16_t con_handle, gatt_client_characteristic_t * characteristic, uint16_t configuration);

/**
 * @brief Get specific characteristic struct by its uuid.
 * @param  ALL characteristics to be matched
 * @param  The number of characteristics
 * @param  uuid
 * @return return 0 if ok, other refer to ATT_ERR_CODE
 */
gatt_client_characteristic_t* ble_get_characteristic_for_uuid16(gatt_client_characteristic_t *characteristic, uint8_t num, uint16_t uuid16);

gatt_client_characteristic_t* ble_get_characteristic_for_uuid128(gatt_client_characteristic_t *characteristic, uint8_t num, uint8_t *uuid128);


/**
  * @brief  Host renew resolvable private address timeout.
  * @param  rpa params. mainly timeout in seconds and address resolution enable.
  * @return none
  */
void ble_set_rslv_renew_to(rpa_param_t *param);

/**
  * @brief  Host set address resolution enable.
  * @param  1 enable
            0 disable.
  * @return none
  */
void ble_set_rslv_enable(uint8_t enable);

/**
  * @brief  Host add device to resolvable address list.
  * @param  privacy mode.
  * @param  peer device address.
  * @param  peer device irk.
  * @return none
  */
void ble_add_dev_to_ral_list(gap_ral_dev_info_t *ral_dev_info);

/**
  * @brief  Host remove device from resolvable address list.
  * @param  privacy mode.
  * @param  peer device address.
  * @param  peer device irk.
  * @return none
  */
void ble_rmv_dev_from_ral_list(gap_ral_dev_info_t *ral_dev_info);

/**
  * @brief  Host add device to filter accept list.
  * @param  size of wlist.
  * @param  peer device identity address type.
  * @param  peer device identity address.
  * @return none
  */
void ble_add_dev_to_white_list(wlist_param_t *param);

/**
  * @brief  Host remove device from filter accept list.
  * @param  peer device identity address type.
  * @param  peer device identity address.
  * @return none
  */
void ble_rmv_dev_from_white_list(wlist_param_t *param);

/**
  * @brief  Host read filter accept list size.
  * @return none
  */
void ble_rd_dev_white_list(void);

/**
  * @brief  Host clear filter accept list.
  * @return none
  */
void ble_clr_dev_white_list(void);

/**
  * @brief  Used by the user to enter passkey when BLE_EVT_PASSKEY_INPUT event is triggered.
  * @return none
  */
bool ble_sm_passkey_input(uint16_t con_handle, uint32_t passkey);

/**
  * @brief  Used by the user to enter yes or no when BLE_EVT_NUMERIC_COMPARISON_REQUEST event is triggered.
  * @return none
  */
bool ble_sm_nc_exch(uint16_t con_handle, bool yesno);

/**
 * @brief Delete the bonding information of a specified address
 * @param address_type
 * @param address
 * @return return true if ok, invalid parameters will return false
 */
bool ble_delete_bonding_for_addr(bd_addr_type_t address_type, bd_addr_t address);

/**
 * @brief Delete the bonding information of a specified index
 * @param index: [0,N), N can be get form ble_get_bonding_cnt() function
 * @return return true if ok, invalid parameters will return false
 */
bool ble_delete_bonding_for_index(uint8_t index);

/**
 * @brief Delete all bonding information
 */
void ble_delete_bonding_all(void);

/**
 * @brief Get the number of bonding information
 * @return The number of binding information
 */
uint8_t ble_get_bonding_cnt(void);


/**
 * @brief Gets the bonding information for the specified index
 * @param index: [0,N), N can be get form ble_get_bonding_cnt() function
 * @return specified bonding information
 */
sm_bonding_info_t* ble_get_bonding_info_for_index(uint8_t index);

/**
 * @brief Get the bonding index for connect handle
 * @return index. -1 is unknown
 */
int16_t ble_get_bonding_index_for_conn_handle(uint16_t conn_handle);

/**
 * @brief  Initialize the timer memery pool.
 * @note   This function and a series of related functions are invoked after bt init.
 * @param  pool: memery pool head address.
 * @param  size: byte size of memery pool, one alarm timer need 28Bytes memery.
 * @retval None.
 */
void bt_alarm_timer_init(void *pool, uint32_t size);

/**
 * @brief  Start specified id alarm timer.
 * @param  id: alarm timer id.
 * @retval result code.
 */
bt_timer_err_code_t bt_alarm_timer_start(uint8_t id);

/**
 * @brief  Stop specified id alarm timer.
 * @param  id: alarm timer id.
 * @retval result code.
 */
bt_timer_err_code_t bt_alarm_timer_stop(uint8_t id);

/**
 * @brief  Acquire a alarm timer.
 * @param  id: alarm timer id.
 * @param  interval: alarm timer trigger interval, unit [1, 0xffffff) ms.
 * @param  is_repeat: trigger mode. 0-once 1-repeat
 * @param  callback: callback function.
 * @retval result code.
 */
bt_timer_err_code_t bt_alarm_timer_acquire(uint8_t *id, uint32_t interval, uint8_t is_repeat, void (*callback)(uint8_t id));

/**
 * @brief  Release specified id alarm timer.
 * @param  id: alarm timer id.
 * @retval result code.
 */
bt_timer_err_code_t bt_alarm_timer_release(uint8_t id);

/**
 * @brief  Query specified id alarm timer remain time.
 * @param  id: alarm timer id.
 * @param  remain: the remain time for specified id alarm timer.
 * @retval result code.
 */
bt_timer_err_code_t bt_alarm_timer_query_time(uint8_t id, uint32_t *remain);

/**
 * @brief  Modify specified id alarm timer interval, if the timer is start, it
 *         will be re-start after update interval param.
 * @param  id: alarm timer id.
 * @param  interval: new interval, unit [1, 0xffffff) ms.
 * @param  is_repeat: new is_repeat mode state.
 * @retval result code.
 */
bt_timer_err_code_t bt_alarm_timer_modify_interval(uint8_t id, uint32_t interval, uint8_t is_repeat);

/**
 * @brief  Get connection rssi .
 * @param  conhdl: connect handle.
 * @retval -128 ~ 127dBm.
 */
int8_t ble_get_conn_rssi(uint16_t conhdl);

/**
 * @brief Set device random static address.
 * @param device random static address.
 * @return none
 */
void ble_set_static_addr(uint8_t *addr);

/**
 * @brief Update device random address.
 * @return none
 */
void ble_update_random_address(void);

/**
 * @brief Get device random address.
 * @param addr: used for storing device random address
 * @return none
 */
void ble_get_random_address(uint8_t *addr);

/**
 * @brief Set ble max mtu.
 * @param max_mtu, the current maximum value is 512.
 * @return return true if ok, invalid parameters will return false
 */
bool ble_max_mtu_set(uint16_t max_mtu);

/**
 * @brief Set ble tx power.
 * @param  tx_pwr_sel:  BLE_TXPWR_MINUS_12DB(-12dbm)
 *                      BLE_TXPWR_MINUS_6DB(-6dbm)
 *                      BLE_TXPWR_MINUS_4DB(-4dbm)
 *                      BLE_TXPWR_0DB(0dbm)
 *                      BLE_TXPWR_4DB(4dbm)
 *                      BLE_TXPWR_8DB(8dbm)
 *         txpwr_dec:   from 0 to 24, each gear will result in a 0.5db reduction in TX power.
 * @return return true if ok, invalid parameters will return false
 */
bool ble_rf_set_tx_pwr(uint8_t tx_pwr_sel);

bool ble_rf_set_tx_pwr_2(uint8_t tx_pwr_sel, uint8_t txpwr_dec);

/**
 * @brief Initialize external Power Amplifier gpio.
 * @param  gpio_pin_txen, only support PA0 to PA7, which means formal parameter need to between GPIO_PIN_0 and GPIO_PIN_7
 * @param  gpio_pin_rxen, only support PA0 to PA7, which means formal parameter need to between GPIO_PIN_0 and GPIO_PIN_7
 * @return return true if ok, invalid parameters will return false
 */
bool ble_rf_external_pa_init(uint32_t gpio_pin_txen, uint32_t gpio_pin_rxen);

/**
 * @brief Get link layer version info
 * @param  con_handle
 * @param  vers
 * @return return true if ok, invalid parameters will return false
 */
bool ble_ll_version_get(uint16_t con_handle, ll_version_t *vers);

/**
 * @brief Get link layer feature exchage
 * @param  con_handle
 * @param  feature_data
 * @return return true if ok, invalid parameters will return false
 */
bool ble_ll_feature_exchange_get(uint16_t con_handle, ll_features_t *feature_data);

#endif //_API_BTSTACK_H
