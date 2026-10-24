/**
******************************************************************************************************************************
*
*@file ble_app.c
*
*@brief Source file for ble application.User can handle custom ble data in this model.
*
*@Create date: 2023-05-27
*
*
*Copyright (c) 2023, BLUETRUM
******************************************************************************************************************************
**/
#include "ble_app.h"
#include "ble_init.h"

app_env_t app_env;

void ble_app_env_init(void)
{
    memset(&app_env, 0, sizeof(app_env_t));

    memset(app_env.roles, ROLE_UNKNOWN, MAX_BLE_CONNECTIONS);
}

uint8_t ble_get_conn_cnt(uint8_t role)
{
    uint8_t conn_cnt = 0;

    uint8_t loop_cnt = MAX_BLE_CONNECTIONS - 1;

    do {
        //printf("app_env.roles[%d] %x, role %x\n", loop_cnt, app_env.roles[loop_cnt], role);
        if (app_env.roles[loop_cnt] == role) {
            conn_cnt++;
        }
    } while (loop_cnt--);

    return conn_cnt;
}

void ble_adv_scan_state_correct(void)
{
    uint8_t conn_cnt_master = ble_get_conn_cnt(ROLE_MASTER);
    uint8_t conn_cnt_slave = ble_get_conn_cnt(ROLE_SLAVE);
    printf("conn_cnt_master %d, conn_cnt_slave %d\n", conn_cnt_master,
           conn_cnt_slave);

    if (conn_cnt_master < MAX_MASTER_NR) {
        ble_scan_en();
    }

    if (conn_cnt_slave < MAX_SLAVE_NR) {
        ble_adv_en();
    }
}

void ble_conn_state_set(uint8_t state, uint8_t conidx, uint8_t role)
{
    app_env.roles[conidx] = role;

    ble_adv_scan_state_correct();
}

void ble_advertising_report_callback(adv_report_t adv_report)
{
    // if (adv_report.rssi >= -40) {
    //     printf("evt_type %x, addr_type %x, Adv Addr:", adv_report.event_type,
    //            adv_report.addr_type);
    //     print_r(adv_report.addr, 6);
    //     printf("Adv Data: %d###", adv_report.report_len);
    //     print_r(adv_report.report, adv_report.report_len);
    //     printf("adv_report.rssi == %d\n", (int16_t)adv_report.rssi);
    // }

    // 客户的遥控器：
    if (adv_report.report_len >= 10 && adv_report.report[0] == 0x10 &&
        adv_report.report[1] == 0xFF) {
        printf("evt_type %x, addr_type %x, Adv Addr:", adv_report.event_type,
               adv_report.addr_type);
        print_r(adv_report.addr, 6);
        printf("Adv Data: %d###", adv_report.report_len);
        print_r(adv_report.report, adv_report.report_len);
        printf("adv_report.rssi == %d\n", (int)(int8_t)adv_report.rssi);
    }

    uint8_t con_dev_name1[] = "LE_AB2020";
    uint8_t find1 = 0;

    for (uint8_t offset = 0; offset < adv_report.report_len - 1;) {
        ble_ltv_t *beacon_ltv = (ble_ltv_t *)&adv_report.report[offset];

        if ((beacon_ltv->type == 0x09) && beacon_ltv->len &&
            (beacon_ltv->len < MAX_ADV_DATA_LEN)) {
            if (!strncmp((char *)beacon_ltv->data, (const char *)con_dev_name1,
                         beacon_ltv->len - 1)) {
                find1 = 1;
                printf("find device %s\n", con_dev_name1);
            }
        }

        if (find1) {
            if (ble_get_conn_cnt(ROLE_MASTER) < MAX_MASTER_NR) {
                ble_scan_dis();

                bool adv_is_en = ble_adv_is_en();

                // Prevent connected by other devices when initiating a connection.
                if (adv_is_en) {
                    delay_ms(2);
                    ble_adv_dis();
                }

                printf("ble_connect_by_addr: ");
                print_r(adv_report.addr, 6);

                le_conn_initiate_err_code_t ret;
                ret = ble_connect_by_addr(adv_report.addr, adv_report.addr_type,
                                          24, 0, 400);

                if (ret) {
                    printf(
                        "Central initiate a connecetion failed!, reason = %x\n",
                        ret);

                    if (adv_is_en) {
                        delay_5ms(1);
                        ble_adv_en();
                    }
                }

            } else {
                printf("Master device(%d) reaches the upper limit(%d)\n",
                       ble_get_conn_cnt(ROLE_MASTER), MAX_MASTER_NR);
            }

            break;
        }

        offset += beacon_ltv->len + 1;
    }
}
