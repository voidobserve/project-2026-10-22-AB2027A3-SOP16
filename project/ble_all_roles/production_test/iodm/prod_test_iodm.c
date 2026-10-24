#include "include.h"
#include "prod_test_iodm.h"
#include "bsp_huart_iodm.h"

#if FUNC_IODM_EN

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif

static const uint8_t iodm_crc8_tbl[256] = {
    0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83, 0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
    0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e, 0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,
    0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0, 0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,
    0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d, 0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
    0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5, 0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,
    0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58, 0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
    0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6, 0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24,
    0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b, 0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,
    0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, 0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
    0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92, 0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,
    0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c, 0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee,
    0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1, 0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,
    0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49, 0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
    0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4, 0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,
    0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a, 0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
    0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7, 0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35,
};

static iodm_var_t iodm_var;
static iodm_packet_t iodm_rx_packet AT(.prod_test.iodm.rx);
static iodm_packet_t iodm_tx_packet AT(.prod_test.iodm.tx);

AT(.com_text.iodm.set_do_flag)
void prod_test_iodm_set_do_flag(uint8_t flag)
{
    iodm_var.do_flag |= flag;
}

AT(.com_text.iodm.packet_receive)
void prod_test_iodm_packet_receive(uint8_t *buf, uint8_t buf_len)
{
    memcpy(&iodm_rx_packet, buf, buf_len);
}

static uint8_t prod_test_iodm_crc8_maxim(uint8_t *buf, uint8_t length)
{
    uint8_t i;
    uint8_t crc = 0;
    for (i = 0; i < length; i++) {
        crc = iodm_crc8_tbl[(crc ^ buf[i])];
    }
    return crc;
}

static void prod_test_iodm_cmd_rsp(uint8_t cmd, uint8_t data_len, uint8_t result, uint8_t *data)
{
    iodm_packet_t *tx_packet = &iodm_tx_packet;
    uint8_t tx_crc_offset;

    tx_packet->header = IODM_HEADER;
    tx_packet->distinguish = IODM_DISTINGUISH;
    tx_packet->cmd = cmd;
    tx_packet->length = data_len + 1;
    tx_packet->data[0] = result;
    if (data_len != 0) {
        memcpy(&tx_packet->data[1], data, data_len);
    }

    //crc
    tx_crc_offset = IODM_PACKET_HEADER_LEN + tx_packet->length;
    tx_packet->checksum = prod_test_iodm_crc8_maxim((uint8_t *)tx_packet, tx_crc_offset);
    *((uint8_t *)tx_packet + tx_crc_offset) =  tx_packet->checksum;

    TRACE("test iodm tx packet:");
    TRACE_R(tx_packet, tx_crc_offset + 1);

    bsp_huart_iodm_dma_tx((uint8_t *)tx_packet, tx_crc_offset + 1);
}

static uint8_t prod_test_iodm_check_rx_crc(iodm_packet_t *rx_packet)
{
    uint8_t res = 0;
    uint8_t rx_crc_offset = IODM_PACKET_HEADER_LEN + rx_packet->length;
    uint8_t crc = prod_test_iodm_crc8_maxim((uint8_t *)rx_packet, rx_crc_offset);
    res = (crc == rx_packet->checksum) ? 1 : 0;
    return res;
}

static void prod_test_iodm_reveice_data_deal(void)
{
    iodm_packet_t *rx_packet = &iodm_rx_packet;
    uint8_t rsp_cmd = rx_packet->cmd;
    uint8_t rsp_param_len = 0;
    uint8_t rsp_result = IODM_RES_SUCCESS;
    uint8_t rsp_data[IODM_DATA_LEN] = {0};
    uint8_t rx_crc_offset = IODM_PACKET_HEADER_LEN + rx_packet->length;
    rx_packet->checksum = *((uint8_t *)rx_packet + rx_crc_offset);

    TRACE("test iodm rx packet:");
    TRACE_R(rx_packet, rx_crc_offset+1);

    if (prod_test_iodm_check_rx_crc(rx_packet) == 1) {
        switch(rx_packet->cmd){
            case IODM_CMD_DEV_RST:
                TRACE("IODM_CMD_DEV_RST\n");
                break;

            case IODM_CMD_SET_LE_ADDR:
                TRACE("IODM_CMD_SET_LE_ADDR\n");
                break;
                
            case IODM_CMD_GET_LE_ADDR:
                TRACE("IODM_CMD_GET_LE_ADDR\n");
                break;
                
            case IODM_CMD_SET_LE_NAME:
                TRACE("IODM_CMD_SET_LE_NAME\n");
                break;
                
            case IODM_CMD_GET_LE_NAME:
                TRACE("IODM_CMD_GET_LE_NAME\n");
                break;
                
            case IODM_CMD_LE_FCC_TEST_ENTER:
                TRACE("IODM_CMD_LE_FCC_TEST_ENTER\n");
                break;
                
            case IODM_CMD_LE_FCC_SET_PARAM:
                TRACE("IODM_CMD_LE_FCC_SET_PARAM\n");
                break;
                
            case IODM_CMD_LE_FCC_TEST_EXIT:
                TRACE("IODM_CMD_LE_FCC_TEST_EXIT\n");
                break;
                
            case IODM_CMD_LE_BQB_RF_ENTER:
                TRACE("IODM_CMD_LE_BQB_RF_ENTER\n");
                break;
                
            case IODM_CMD_LE_BQB_RF_TX_V2_PRAM:
                TRACE("IODM_CMD_LE_BQB_RF_TX_V2_PRAM\n");
                break;
                
            case IODM_CMD_LE_BQB_RF_RX_V2_PRAM:
                TRACE("IODM_CMD_LE_BQB_RF_RX_V2_PRAM\n");
                break;
                
            case IODM_CMD_LE_BQB_RF_STOP:
                TRACE("IODM_CMD_LE_BQB_RF_STOP\n");
                break;
                
            case IODM_CMD_LE_BQB_RF_EXIT:
                TRACE("IODM_CMD_LE_BQB_RF_EXIT\n");
                break;
                
            case IODM_CMD_SET_XOSC_CAP:
                TRACE("IODM_CMD_SET_XOSC_CAP\n");
                break;
                
            case IODM_CMD_GET_XOSC_CAP:
                TRACE("IODM_CMD_GET_XOSC_CAP\n");
                break;
                                
            case IODM_CMD_GET_VER_INFO:
                TRACE("IODM_CMD_GET_VER_INFO\n");
                break;
                                
            case IODM_CMD_GET_INFO:
                TRACE("IODM_CMD_GET_INFO\n");
                break;
                                
            case IODM_CMD_MIC_LOOKBACK_ENTER:
                TRACE("IODM_CMD_MIC_LOOKBACK_ENTER\n");
                break;
                                
            case IODM_CMD_MIC_LOOKBACK_EXIT:
                TRACE("IODM_CMD_MIC_LOOKBACK_EXIT\n");
                break;
                                
            case IODM_CMD_PROTOCOL_VER:
                TRACE("IODM_CMD_PROTOCOL_VER\n");
                break;

            default:
                TRACE("IODM CMD UNKNOWN!!!\n");
                rsp_result = IODM_RES_FAIL;
                break;
        }
    } else {
        TRACE("test iodm rcr err!\n");
        rsp_result = IODM_RES_FAIL;
    }
    prod_test_iodm_cmd_rsp(rsp_cmd, rsp_param_len, rsp_result, rsp_data);
}

void prod_test_iodm_init(void)
{
    memset(&iodm_rx_packet, 0, sizeof(iodm_packet_t));
    memset(&iodm_tx_packet, 0, sizeof(iodm_packet_t));
    memset(&iodm_var, 0, sizeof(iodm_var_t));
    bsp_huart_iodm_init();
}

AT(.text.app.proc.iodm)
void prod_test_iodm_process(void)
{
    if (iodm_var.do_flag & FLAG_IODM_RECEIVE_EVENT){
        iodm_var.do_flag &= ~FLAG_IODM_RECEIVE_EVENT;
        prod_test_iodm_reveice_data_deal();
    }
}

#endif
