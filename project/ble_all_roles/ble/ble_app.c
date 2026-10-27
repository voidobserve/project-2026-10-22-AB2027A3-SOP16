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

/**
 * @brief 初始化 BLE 应用环境
 *        清零 app_env，并把所有连接槽位的角色标记为 ROLE_UNKNOWN（空闲/未连接）。
 *        每条 BLE 连接占用一个槽位（conidx），app_env.roles[conidx] 记录该连接是主机还是从机。
 */
void ble_app_env_init(void)
{
    memset(&app_env, 0, sizeof(app_env_t));

    memset(app_env.roles, ROLE_UNKNOWN, MAX_BLE_CONNECTIONS);
}

/**
 * @brief 统计当前处于指定角色的连接数量
 * @param role  ROLE_MASTER：统计作为主机（Central，主动连接别人）的连接数
 *              ROLE_SLAVE ：统计作为从机（Peripheral，被手机等连入）的连接数
 * @return 该角色当前的连接数量
 */
uint8_t ble_get_conn_cnt(uint8_t role)
{
    uint8_t conn_cnt = 0;

    uint8_t loop_cnt = MAX_BLE_CONNECTIONS - 1;

    // 倒序遍历所有连接槽位（ loop_cnt 从 MAX_BLE_CONNECTIONS - 1 递减到 0）
    do {
        //printf("app_env.roles[%d] %x, role %x\n", loop_cnt, app_env.roles[loop_cnt], role);
        if (app_env.roles[loop_cnt] == role) {
            conn_cnt++;
        }
    } while (loop_cnt--);

    return conn_cnt;
}

/**
 * @brief 根据当前连接数自动纠正广播/扫描状态（连接建立或断开后调用）
 *        本工程是主从一体（All Roles）：
 *        - 主机连接数未满 MAX_MASTER_NR 时重新开启扫描，继续发现并连接目标设备；
 *        - 从机连接数未满 MAX_SLAVE_NR 时重新开启广播，允许手机等设备连入。
 *        连接数达到上限的角色不再开启对应功能。
 */
void ble_adv_scan_state_correct(void)
{
    uint8_t conn_cnt_master = ble_get_conn_cnt(ROLE_MASTER);
    uint8_t conn_cnt_slave = ble_get_conn_cnt(ROLE_SLAVE);
    printf("conn_cnt_master %d, conn_cnt_slave %d\n", conn_cnt_master,
           conn_cnt_slave);

    // 主机连接未满：开启扫描，用于搜索目标广播并主动发起连接
    if (conn_cnt_master < MAX_MASTER_NR) {
        ble_scan_en();
    }

#if LE_ADV_EN
    // 从机连接未满：开启广播，等待手机等中心设备连入（LE_ADV_EN=0 时不广播）
    if (conn_cnt_slave < MAX_SLAVE_NR) {
        ble_adv_en();
    }
#endif
}

/**
 * @brief 记录某条连接的角色，并纠正广播/扫描状态
 * @param state   连接状态（预留参数，当前未使用）
 * @param conidx  连接索引，对应 app_env.roles[] 的槽位
 * @param role    该连接的角色：ROLE_MASTER（我方是主机）/ ROLE_SLAVE（我方是从机）/ ROLE_UNKNOWN（已断开/空闲）
 */
void ble_conn_state_set(uint8_t state, uint8_t conidx, uint8_t role)
{
    app_env.roles[conidx] = role;

    ble_adv_scan_state_correct();
}

#if LE_MASTER_CONNECT_EN
/**
 * @brief 扫描到广播包时的回调（本工程的核心逻辑：搜索目标设备并主动发起连接）
 *
 * 处理流程：
 *  1. 逐条解析广播数据里的 AD 结构（LTV：len + type + data），查找“完整本地名称”
 *     （AD Type = 0x09）为 "LE_AB2020" 的目标设备（客户遥控器广播包的打印由
 *     rf24g_remoter_data_process() 统一处理，这里不重复打印）；
 *  2. 找到目标设备后，本机作为主机（Central）按其地址主动发起 BLE 连接。
 *
 * @param adv_report 一条扫描到的广播报告（含广播者地址、地址类型、广播数据、RSSI 等）
 */
void ble_advertising_report_callback(adv_report_t adv_report)
{
    // 客户遥控器广播包(0x10 0xFF开头)的打印统一在 rf24g_remoter_data_process() 里处理,这里不重复打印
    // if (adv_report.rssi >= -40) {
    //     printf("evt_type %x, addr_type %x, Adv Addr:", adv_report.event_type,
    //            adv_report.addr_type);
    //     print_r(adv_report.addr, 6);
    //     printf("Adv Data: %d###", adv_report.report_len);
    //     print_r(adv_report.report, adv_report.report_len);
    //     printf("adv_report.rssi == %d\n", (int16_t)adv_report.rssi);
    // }

    // 目标设备名：只连接广播中名字为 "LE_AB2020" 的设备
    uint8_t con_dev_name1[] = "LE_AB2020";
    uint8_t find1 = 0; // 找到目标设备的标志：0-未找到，1-已找到

    // 逐条解析广播数据中的 AD 结构（LTV 格式：1 字节 len + 1 字节 type + (len-1) 字节 data）。
    // 注意 len 的含义是 type+data 的总长度，所以：
    //   - 真实数据长度 = len - 1
    //   - 下一个 AD 结构的偏移 = 当前偏移 + len + 1（+1 是 len 字节本身）
    // 循环条件 report_len - 1 保证至少还剩 len 和 type 两字节可读。
    for (uint8_t offset = 0; offset < adv_report.report_len - 1;) {
        ble_ltv_t *beacon_ltv = (ble_ltv_t *)&adv_report.report[offset];

        // type == 0x09：AD Type = Complete Local Name（完整本地名称）
        // len 非 0 且小于 MAX_ADV_DATA_LEN，防止长度非法导致越界访问
        if ((beacon_ltv->type == 0x09) && beacon_ltv->len &&
            (beacon_ltv->len < MAX_ADV_DATA_LEN)) {
            // 与目标名比较前 len-1 个字符（len-1 是名称真实长度，不含结尾 '\0'）
            if (!strncmp((char *)beacon_ltv->data, (const char *)con_dev_name1,
                         beacon_ltv->len - 1)) {
                find1 = 1;
                printf("find device %s\n", con_dev_name1);
            }
        }

        // 找到目标设备：本机作为主机（Central）主动发起连接
        if (find1) {
            // 主机连接数未达上限才允许发起新连接（MAX_MASTER_NR = 2）
            if (ble_get_conn_cnt(ROLE_MASTER) < MAX_MASTER_NR) {
                ble_scan_dis(); // 先停止扫描，避免干扰即将发起的连接

#if LE_ADV_EN
                bool adv_is_en = ble_adv_is_en();

                // Prevent connected by other devices when initiating a connection.
                // 发起连接期间暂时关闭本机广播，防止在此期间被其他设备（手机）连入，
                // 本机同时支持从机角色，不关广播可能被抢占一条连接槽位。
                if (adv_is_en) {
                    delay_ms(2); // 等待广播真正停止
                    ble_adv_dis();
                }
#endif

                printf("ble_connect_by_addr: ");
                print_r(adv_report.addr, 6);

                // 按广播里的地址发起连接（本机作为主机）：
                //   interval = 24 * 1.25ms = 30ms（连接间隔）
                //   conn_latency = 0（从机延迟）
                //   supervision_timeout = 400 * 10ms = 4s（连接超时）
                le_conn_initiate_err_code_t ret;
                ret = ble_connect_by_addr(adv_report.addr, adv_report.addr_type,
                                          24, 0, 400);

                if (ret) { // 非 0 表示发起连接失败
                    printf(
                        "Central initiate a connecetion failed!, reason = %x\n",
                        ret);

#if LE_ADV_EN
                    // 发起失败时恢复之前关闭的广播，保证从机功能继续可用
                    if (adv_is_en) {
                        delay_5ms(1);
                        ble_adv_en();
                    }
#endif
                }

            } else {
                // 主机连接数已达上限，放弃本次连接
                printf("Master device(%d) reaches the upper limit(%d)\n",
                       ble_get_conn_cnt(ROLE_MASTER), MAX_MASTER_NR);
            }

            break; // 本条广播已处理完毕（找到目标就不再解析后续 AD 结构）
        }

        offset += beacon_ltv->len + 1; // 跳到下一个 AD 结构
    }
}
#endif /* LE_MASTER_CONNECT_EN */
