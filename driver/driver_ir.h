/**
******************************************************************************************************************************
*
*@file driver_ir.h
*
*@brief Header file for IR driver
*
*Copyright (c) 2023, BLUETRUM
******************************************************************************************************************************
**/
#ifndef _IR_H_
#define _IR_H_

#include "driver_com.h"

//IRTX

// IR One Code Time Register
#define ADDR_REG_IRONETIME                          *(volatile unsigned long*) (SFR7_BASE + 0x20*4)

// IR Zero Code Time Register
#define ADDR_REG_IRZEROTIME                         *(volatile unsigned long*) (SFR7_BASE + 0x21*4)

// IR Start Code Time Register
#define ADDR_REG_IRSTARTTIME                        *(volatile unsigned long*) (SFR7_BASE + 0x22*4)

// IR Repeat Code Time Register
#define ADDR_REG_IRREPEATTIME                       *(volatile unsigned long*) (SFR7_BASE + 0x23*4)

// IR Repeat Code Control Register
#define ADDR_REG_IRREPEATCON                        *(volatile unsigned long*) (SFR7_BASE + 0x24*4)
// IR TX Interval Between Repeat Time = (IRREPTIME + 1) * (1M Or 32K)
#define POS_REG_IRREPEATCON_IRREPTIME               13
#define BIT_REG_IRREPEATCON_IRREPTIME               0xFFFFE000
#define SET_REG_IRREPEATCON_IRREPTIME(val)          ADDR_REG_IRREPEATCON = ((ADDR_REG_IRREPEATCON & (~BIT_REG_IRREPEATCON_IRREPTIME)) | ((val) << POS_REG_IRREPEATCON_IRREPTIME))
#define GET_REG_IRREPEATCON_IRREPTIME()             ((ADDR_REG_IRREPEATCON & BIT_REG_IRREPEATCON_IRREPTIME) >> POS_REG_IRREPEATCON_IRREPTIME)
// IR TX End Pulse Time(NEC Or TC9012) = (IRENDTIME  + 1) * (1M Or 32K)
#define POS_REG_IRREPEATCON_IRENDTIME               3
#define BIT_REG_IRREPEATCON_IRENDTIME               0x8FF8
#define SET_REG_IRREPEATCON_IRENDTIME(val)          ADDR_REG_IRREPEATCON = ((ADDR_REG_IRREPEATCON & (~BIT_REG_IRREPEATCON_IRENDTIME)) | ((val) << POS_REG_IRREPEATCON_IRENDTIME))
#define GET_REG_IRREPEATCON_IRENDTIME()             ((ADDR_REG_IRREPEATCON & BIT_REG_IRREPEATCON_IRENDTIME) >> POS_REG_IRREPEATCON_IRENDTIME)
// IR TX If tc9012 Indicated As Sync Code
#define POS_REG_IRREPEATCON_IRTX_SYC                2
#define BIT_REG_IRREPEATCON_IRTX_SYC                0x4
#define SETF_REG_IRREPEATCON_IRTX_SYC()             ADDR_REG_IRREPEATCON |= BIT_REG_IRREPEATCON_IRTX_SYC
#define CLRF_REG_IRREPEATCON_IRTX_SYC()             ADDR_REG_IRREPEATCON &= ~BIT_REG_IRREPEATCON_IRTX_SYC
#define GETF_REG_IRREPEATCON_IRTX_SYC()             ((ADDR_REG_IRREPEATCON & BIT_REG_IRREPEATCON_IRTX_SYC) >> POS_REG_IRREPEATCON_IRTX_SYC)
// IR TX Repeat Mode
#define POS_REG_IRREPEATCON_IRREP_SEL               1
#define BIT_REG_IRREPEATCON_IRREP_SEL               0x2
#define SETF_REG_IRREPEATCON_IRREP_SEL()            ADDR_REG_IRREPEATCON |= BIT_REG_IRREPEATCON_IRREP_SEL
#define CLRF_REG_IRREPEATCON_IRREP_SEL()            ADDR_REG_IRREPEATCON &= ~BIT_REG_IRREPEATCON_IRREP_SEL
#define GETF_REG_IRREPEATCON_IRREP_SEL()            ((ADDR_REG_IRREPEATCON & BIT_REG_IRREPEATCON_IRREP_SEL) >> POS_REG_IRREPEATCON_IRREP_SEL)
// IR TX Whether Key Is Release
#define POS_REG_IRREPEATCON_IRTX_KEY                0
#define BIT_REG_IRREPEATCON_IRTX_KEY                0x1
#define SETF_REG_IRREPEATCON_IRTX_KEY()             ADDR_REG_IRREPEATCON |= BIT_REG_IRREPEATCON_IRTX_KEY
#define CLRF_REG_IRREPEATCON_IRTX_KEY()             ADDR_REG_IRREPEATCON &= ~BIT_REG_IRREPEATCON_IRTX_KEY
#define GETF_REG_IRREPEATCON_IRTX_KEY()             ((ADDR_REG_IRREPEATCON & BIT_REG_IRREPEATCON_IRTX_KEY) >> POS_REG_IRREPEATCON_IRTX_KEY)

// IR TX Control Register
#define ADDR_REG_IRTXCON                            *(volatile unsigned long*) (SFR7_BASE + 0x25*4)
// IR TX Carrier Duty = (IRCW_DUTY / IRCW_LENGTH + 1)
#define POS_REG_IRTXCON_IRCW_DUTY                   20
#define BIT_REG_IRTXCON_IRCW_DUTY                   0xFF00000
#define SET_REG_IRTXCON_IRCW_DUTY(val)              ADDR_REG_IRTXCON = ((ADDR_REG_IRTXCON & (~BIT_REG_IRTXCON_IRCW_DUTY)) | ((val) << POS_REG_IRTXCON_IRCW_DUTY))
#define GET_REG_IRTXCON_IRCW_DUTY()                 ((ADDR_REG_IRTXCON & BIT_REG_IRTXCON_IRCW_DUTY) >> POS_REG_IRTXCON_IRCW_DUTY)
// IR TX Carrier Freq = (3M(or 4M) / IRCW_LENGTH + 1)
#define POS_REG_IRTXCON_IRCW_LENGTH                 12
#define BIT_REG_IRTXCON_IRCW_LENGTH                 0xFF000
#define SET_REG_IRTXCON_IRCW_LENGTH(val)            ADDR_REG_IRTXCON = ((ADDR_REG_IRTXCON & (~BIT_REG_IRTXCON_IRCW_LENGTH)) | ((val) << POS_REG_IRTXCON_IRCW_LENGTH))
#define GET_REG_IRTXCON_IRCW_LENGTH()               ((ADDR_REG_IRTXCON & BIT_REG_IRTXCON_IRCW_LENGTH) >> POS_REG_IRTXCON_IRCW_LENGTH)
// IR TX Carrier Enable
#define POS_REG_IRTXCON_IRTX_CARRIER_EN             11
#define BIT_REG_IRTXCON_IRTX_CARRIER_EN             0x800
#define SETF_REG_IRTXCON_IRTX_CARRIER_EN()          ADDR_REG_IRTXCON |= BIT_REG_IRTXCON_IRTX_CARRIER_EN
#define CLRF_REG_IRTXCON_IRTX_CARRIER_EN()          ADDR_REG_IRTXCON &= ~BIT_REG_IRTXCON_IRTX_CARRIER_EN
#define GETF_REG_IRTXCON_IRTX_CARRIER_EN()          ((ADDR_REG_IRTXCON & BIT_REG_IRTXCON_IRTX_CARRIER_EN) >> POS_REG_IRTXCON_IRTX_CARRIER_EN)
// IR TX Select Fall Or Rise Trigger In Capture Mode
#define POS_REG_IRTXCON_IREDGE_SEL                  10
#define BIT_REG_IRTXCON_IREDGE_SEL                  0x400
#define SETF_REG_IRTXCON_IREDGE_SEL()               ADDR_REG_IRTXCON |= BIT_REG_IRTXCON_IREDGE_SEL
#define CLRF_REG_IRTXCON_IREDGE_SEL()               ADDR_REG_IRTXCON &= ~BIT_REG_IRTXCON_IREDGE_SEL
#define GETF_REG_IRTXCON_IREDGE_SEL()               ((ADDR_REG_IRTXCON & BIT_REG_IRTXCON_IREDGE_SEL) >> POS_REG_IRTXCON_IREDGE_SEL)
// IR TX Select Encoding Format In Modulation Mode
#define POS_REG_IRTXCON_IRTX_SEL                    8
#define BIT_REG_IRTXCON_IRTX_SEL                    0x300
#define SET_REG_IRTXCON_IRTX_SEL(val)               ADDR_REG_IRTXCON = ((ADDR_REG_IRTXCON & (~BIT_REG_IRTXCON_IRTX_SEL)) | ((val) << POS_REG_IRTXCON_IRTX_SEL))
#define GET_REG_IRTXCON_IRTX_SEL()                  ((ADDR_REG_IRTXCON & BIT_REG_IRTXCON_IRTX_SEL) >> POS_REG_IRTXCON_IRTX_SEL)
// IR TX Interrup Enable In Modualtion Mode
#define POS_REG_IRTXCON_IRTX_IE                     7
#define BIT_REG_IRTXCON_IRTX_IE                     0x80
#define SETF_REG_IRTXCON_IRTX_IE()                  ADDR_REG_IRTXCON |= BIT_REG_IRTXCON_IRTX_IE
#define CLRF_REG_IRTXCON_IRTX_IE()                  ADDR_REG_IRTXCON &= ~BIT_REG_IRTXCON_IRTX_IE
#define GETF_REG_IRTXCON_IRTX_IE()                  ((ADDR_REG_IRTXCON & BIT_REG_IRTXCON_IRTX_IE) >> POS_REG_IRTXCON_IRTX_IE)
// IR Capture Enable
#define POS_REG_IRTXCON_IR_CAPEN                    6
#define BIT_REG_IRTXCON_IR_CAPEN                    0x40
#define SETF_REG_IRTXCON_IR_CAPEN()                 ADDR_REG_IRTXCON |= BIT_REG_IRTXCON_IR_CAPEN
#define CLRF_REG_IRTXCON_IR_CAPEN()                 ADDR_REG_IRTXCON &= ~BIT_REG_IRTXCON_IR_CAPEN
#define GETF_REG_IRTXCON_IR_CAPEN()                 ((ADDR_REG_IRTXCON & BIT_REG_IRTXCON_IR_CAPEN) >> POS_REG_IRTXCON_IR_CAPEN)
// IR TX Code "1" Mode
#define POS_REG_IRTXCON_IRTX_1MODE                  5
#define BIT_REG_IRTXCON_IRTX_1MODE                  0x20
#define SETF_REG_IRTXCON_IRTX_1MODE()               ADDR_REG_IRTXCON |= BIT_REG_IRTXCON_IRTX_1MODE
#define CLRF_REG_IRTXCON_IRTX_1MODE()               ADDR_REG_IRTXCON &= ~BIT_REG_IRTXCON_IRTX_1MODE
#define GETF_REG_IRTXCON_IRTX_1MODE()               ((ADDR_REG_IRTXCON & BIT_REG_IRTXCON_IRTX_1MODE) >> POS_REG_IRTXCON_IRTX_1MODE)
// IR TX Code "0" Mode
#define POS_REG_IRTXCON_IRTX_0MODE                  4
#define BIT_REG_IRTXCON_IRTX_0MODE                  0x10
#define SETF_REG_IRTXCON_IRTX_0MODE()               ADDR_REG_IRTXCON |= BIT_REG_IRTXCON_IRTX_0MODE
#define CLRF_REG_IRTXCON_IRTX_0MODE()               ADDR_REG_IRTXCON &= ~BIT_REG_IRTXCON_IRTX_0MODE
#define GETF_REG_IRTXCON_IRTX_0MODE()               ((ADDR_REG_IRTXCON & BIT_REG_IRTXCON_IRTX_0MODE) >> POS_REG_IRTXCON_IRTX_0MODE)
// IR TX Invert
#define POS_REG_IRTXCON_IRTX_INV                    3
#define BIT_REG_IRTXCON_IRTX_INV                    0x8
#define SETF_REG_IRTXCON_IRTX_INV()                 ADDR_REG_IRTXCON |= BIT_REG_IRTXCON_IRTX_INV
#define CLRF_REG_IRTXCON_IRTX_INV()                 ADDR_REG_IRTXCON &= ~BIT_REG_IRTXCON_IRTX_INV
#define GETF_REG_IRTXCON_IRTX_INV()                 ((ADDR_REG_IRTXCON & BIT_REG_IRTXCON_IRTX_INV) >> POS_REG_IRTXCON_IRTX_INV)
// IR TX Kick Start In Modulation Mode Or Learn Mode
#define POS_REG_IRTXCON_IRTX_KST                    2
#define BIT_REG_IRTXCON_IRTX_KST                    0x4
#define SETF_REG_IRTXCON_IRTX_KST()                 ADDR_REG_IRTXCON |= BIT_REG_IRTXCON_IRTX_KST
#define CLRF_REG_IRTXCON_IRTX_KST()                 ADDR_REG_IRTXCON &= ~BIT_REG_IRTXCON_IRTX_KST
// IR TX Learn Mode Enable
#define POS_REG_IRTXCON_IRTXLEN_EN                  1
#define BIT_REG_IRTXCON_IRTXLEN_EN                  0x2
#define SETF_REG_IRTXCON_IRTXLEN_EN()               ADDR_REG_IRTXCON |= BIT_REG_IRTXCON_IRTXLEN_EN
#define CLRF_REG_IRTXCON_IRTXLEN_EN()               ADDR_REG_IRTXCON &= ~BIT_REG_IRTXCON_IRTXLEN_EN
#define GETF_REG_IRTXCON_IRTXLEN_EN()               ((ADDR_REG_IRTXCON & BIT_REG_IRTXCON_IRTXLEN_EN) >> POS_REG_IRTXCON_IRTXLEN_EN)
// IR TX Modulation Mode Enable
#define POS_REG_IRTXCON_IRTXMOD_EN                  0
#define BIT_REG_IRTXCON_IRTXMOD_EN                  0x1
#define SETF_REG_IRTXCON_IRTXMOD_EN()               ADDR_REG_IRTXCON |= BIT_REG_IRTXCON_IRTXMOD_EN
#define CLRF_REG_IRTXCON_IRTXMOD_EN()               ADDR_REG_IRTXCON &= ~BIT_REG_IRTXCON_IRTXMOD_EN
#define GETF_REG_IRTXCON_IRTXMOD_EN()               ((ADDR_REG_IRTXCON & BIT_REG_IRTXCON_IRTXMOD_EN) >> POS_REG_IRTXCON_IRTXMOD_EN)

// IR TX Data Register
#define ADDR_REG_IRTXDAT                            *(volatile unsigned long*) (SFR7_BASE + 0x26*4)
// IR TX Data
#define POS_REG_IRTXDAT                             0
#define BIT_REG_IRTXDAT                             0xFFFFFFFF
#define SET_REG_IRTXDAT(val)                        ADDR_REG_IRTXDAT = ((ADDR_REG_IRTXDAT & (~BIT_REG_IRTXDAT)) | ((val) << POS_REG_IRTXDAT))
#define GET_REG_IRTXDAT()                           ((ADDR_REG_IRTXDAT & BIT_REG_IRTXDAT) >> POS_REG_IRTXDAT)

// IR TX Data Length Register
#define ADDR_REG_IRTXLEN                            *(volatile unsigned long*) (SFR7_BASE + 0x27*4)
// IR TX Length
#define POS_REG_IRTXLEN                             0
#define BIT_REG_IRTXLEN                             0xFFFFFFFF
#define SET_REG_IRTXLEN(val)                        ADDR_REG_IRTXLEN = ((ADDR_REG_IRTXLEN & (~BIT_REG_IRTXLEN)) | ((val) << POS_REG_IRTXLEN))
#define GET_REG_IRTXLEN()                           ((ADDR_REG_IRTXLEN & BIT_REG_IRTXLEN) >> POS_REG_IRTXLEN)

// IR TX Clear Pending Register
#define ADDR_REG_IRTXPEND                           *(volatile unsigned long*) (SFR7_BASE + 0x28*4)
// IR TX Modulation Mode Enable
#define POS_REG_IRTXPEND_IRTX_PND                   0
#define BIT_REG_IRTXPEND_IRTX_PND                   0x1
#define SETF_REG_IRTXPEND_IRTX_PND()                ADDR_REG_IRTXPEND |= BIT_REG_IRTXPEND_IRTX_PND
#define CLRF_REG_IRTXPEND_IRTX_PND()                ADDR_REG_IRTXPEND &= ~BIT_REG_IRTXPEND_IRTX_PND
#define GETF_REG_IRTXPEND_IRTX_PND()                ((ADDR_REG_IRTXPEND & BIT_REG_IRTXPEND_IRTX_PND) >> POS_REG_IRTXPEND_IRTX_PND)

// IR DMA Control Register
#define ADDR_REG_IRDMACON                           *(volatile unsigned long*) (SFR7_BASE + 0x29*4)
// IR Write DMA Mode
#define POS_REG_IRDMACON_IR_WDMA_MODE               7
#define BIT_REG_IRDMACON_IR_WDMA_MODE               0x80
#define SETF_REG_IRDMACON_IR_WDMA_MODE()            ADDR_REG_IRDMACON |= BIT_REG_IRDMACON_IR_WDMA_MODE
#define CLRF_REG_IRDMACON_IR_WDMA_MODE()            ADDR_REG_IRDMACON &= ~BIT_REG_IRDMACON_IR_WDMA_MODE
#define GETF_REG_IRDMACON_IR_WDMA_MODE()            ((ADDR_REG_IRDMACON & BIT_REG_IRDMACON_IR_WDMA_MODE) >> POS_REG_IRDMACON_IR_WDMA_MODE)
// IR Write DMA Half Interrup Enable
#define POS_REG_IRDMACON_IR_WDMA_HPND_IE            6
#define BIT_REG_IRDMACON_IR_WDMA_HPND_IE            0x40
#define SETF_REG_IRDMACON_IR_WDMA_HPND_IE()         ADDR_REG_IRDMACON |= BIT_REG_IRDMACON_IR_WDMA_HPND_IE
#define CLRF_REG_IRDMACON_IR_WDMA_HPND_IE()         ADDR_REG_IRDMACON &= ~BIT_REG_IRDMACON_IR_WDMA_HPND_IE
#define GETF_REG_IRDMACON_IR_WDMA_HPND_IE()         ((ADDR_REG_IRDMACON & BIT_REG_IRDMACON_IR_WDMA_HPND_IE) >> POS_REG_IRDMACON_IR_WDMA_HPND_IE)
// IR Write DMA All Interrup Enable
#define POS_REG_IRDMACON_IR_WDMA_APND_IE            5
#define BIT_REG_IRDMACON_IR_WDMA_APND_IE            0x20
#define SETF_REG_IRDMACON_IR_WDMA_APND_IE()         ADDR_REG_IRDMACON |= BIT_REG_IRDMACON_IR_WDMA_APND_IE
#define CLRF_REG_IRDMACON_IR_WDMA_APND_IE()         ADDR_REG_IRDMACON &= ~BIT_REG_IRDMACON_IR_WDMA_APND_IE
#define GETF_REG_IRDMACON_IR_WDMA_APND_IE()         ((ADDR_REG_IRDMACON & BIT_REG_IRDMACON_IR_WDMA_APND_IE) >> POS_REG_IRDMACON_IR_WDMA_APND_IE)
// IR Write DMA Enable
#define POS_REG_IRDMACON_IR_WDMA_EN                 4
#define BIT_REG_IRDMACON_IR_WDMA_EN                 0x10
#define SETF_REG_IRDMACON_IR_WDMA_EN()              ADDR_REG_IRDMACON |= BIT_REG_IRDMACON_IR_WDMA_EN
#define CLRF_REG_IRDMACON_IR_WDMA_EN()              ADDR_REG_IRDMACON &= ~BIT_REG_IRDMACON_IR_WDMA_EN
#define GETF_REG_IRDMACON_IR_WDMA_EN()              ((ADDR_REG_IRDMACON & BIT_REG_IRDMACON_IR_WDMA_EN) >> POS_REG_IRDMACON_IR_WDMA_EN)

// IR Read DMA Mode
#define POS_REG_IRDMACON_IR_RDMA_MODE               3
#define BIT_REG_IRDMACON_IR_RDMA_MODE               0x8
#define SETF_REG_IRDMACON_IR_RDMA_MODE()            ADDR_REG_IRDMACON |= BIT_REG_IRDMACON_IR_RDMA_MODE
#define CLRF_REG_IRDMACON_IR_RDMA_MODE()            ADDR_REG_IRDMACON &= ~BIT_REG_IRDMACON_IR_RDMA_MODE
#define GETF_REG_IRDMACON_IR_RDMA_MODE()            ((ADDR_REG_IRDMACON & BIT_REG_IRDMACON_IR_RDMA_MODE) >> POS_REG_IRDMACON_IR_RDMA_MODE)
// IR Read DMA Half Interrup Enable
#define POS_REG_IRDMACON_IR_RDMA_HPND_IE            2
#define BIT_REG_IRDMACON_IR_RDMA_HPND_IE            0x4
#define SETF_REG_IRDMACON_IR_RDMA_HPND_IE()         ADDR_REG_IRDMACON |= BIT_REG_IRDMACON_IR_RDMA_HPND_IE
#define CLRF_REG_IRDMACON_IR_RDMA_HPND_IE()         ADDR_REG_IRDMACON &= ~BIT_REG_IRDMACON_IR_RDMA_HPND_IE
#define GETF_REG_IRDMACON_IR_RDMA_HPND_IE()         ((ADDR_REG_IRDMACON & BIT_REG_IRDMACON_IR_RDMA_HPND_IE) >> POS_REG_IRDMACON_IR_RDMA_HPND_IE)
// IR Read DMA All Interrup Enable
#define POS_REG_IRDMACON_IR_RDMA_APND_IE            1
#define BIT_REG_IRDMACON_IR_RDMA_APND_IE            0x2
#define SETF_REG_IRDMACON_IR_RDMA_APND_IE()         ADDR_REG_IRDMACON |= BIT_REG_IRDMACON_IR_RDMA_APND_IE
#define CLRF_REG_IRDMACON_IR_RDMA_APND_IE()         ADDR_REG_IRDMACON &= ~BIT_REG_IRDMACON_IR_RDMA_APND_IE
#define GETF_REG_IRDMACON_IR_RDMA_APND_IE()         ((ADDR_REG_IRDMACON & BIT_REG_IRDMACON_IR_RDMA_APND_IE) >> POS_REG_IRDMACON_IR_RDMA_APND_IE)
// IR Read DMA Enable
#define POS_REG_IRDMACON_IR_RDMA_EN                 0
#define BIT_REG_IRDMACON_IR_RDMA_EN                 0x1
#define SETF_REG_IRDMACON_IR_RDMA_EN()              ADDR_REG_IRDMACON |= BIT_REG_IRDMACON_IR_RDMA_EN
#define CLRF_REG_IRDMACON_IR_RDMA_EN()              ADDR_REG_IRDMACON &= ~BIT_REG_IRDMACON_IR_RDMA_EN
#define GETF_REG_IRDMACON_IR_RDMA_EN()              ((ADDR_REG_IRDMACON & BIT_REG_IRDMACON_IR_RDMA_EN) >> POS_REG_IRDMACON_IR_RDMA_EN)

// IR DMA Input Address Register
#define ADDR_REG_IRDMAIADR                          *(volatile unsigned long*) (SFR7_BASE + 0x2A*4)

// IR DMA Input Size Register
#define ADDR_REG_IRDMAISIZE                         *(volatile unsigned long*) (SFR7_BASE + 0x2B*4)

// IR DMA Output Address Register
#define ADDR_REG_IRDMAOADR                          *(volatile unsigned long*) (SFR7_BASE + 0x2C*4)

// IR DMA Output Size Register
#define ADDR_REG_IRDMAOSIZE                         *(volatile unsigned long*) (SFR7_BASE + 0x2D*4)

// IR DMA Pending Register
#define ADDR_REG_IRDMAPEND                          *(volatile unsigned long*) (SFR7_BASE + 0x2E*4)
// IR Write DMA Half Size Pending Bit
#define POS_REG_IRDMAPEND_WDMA_HPND                 17
#define BIT_REG_IRDMAPEND_WDMA_HPND                 0x20000
#define SETF_REG_IRDMAPEND_WDMA_HPND()              ADDR_REG_IRDMAPEND |= BIT_REG_IRDMAPEND_WDMA_HPND
#define CLRF_REG_IRDMAPEND_WDMA_HPND()              ADDR_REG_IRDMAPEND &= ~BIT_REG_IRDMAPEND_WDMA_HPND
#define GETF_REG_IRDMAPEND_WDMA_HPND()              ((ADDR_REG_IRDMAPEND & BIT_REG_IRDMAPEND_WDMA_HPND) >> POS_REG_IRDMAPEND_WDMA_HPND)
// IR Write DMA All Size Pending Bit
#define POS_REG_IRDMAPEND_WDMA_APND                 16
#define BIT_REG_IRDMAPEND_WDMA_APND                 0x10000
#define SETF_REG_IRDMAPEND_WDMA_APND()              ADDR_REG_IRDMAPEND |= BIT_REG_IRDMAPEND_WDMA_APND
#define CLRF_REG_IRDMAPEND_WDMA_APND()              ADDR_REG_IRDMAPEND &= ~BIT_REG_IRDMAPEND_WDMA_APND
#define GETF_REG_IRDMAPEND_WDMA_APND()              ((ADDR_REG_IRDMAPEND & BIT_REG_IRDMAPEND_WDMA_APND) >> POS_REG_IRDMAPEND_WDMA_APND)
// IR Read DMA Half Size Pending Bit
#define POS_REG_IRDMAPEND_RDMA_HPND                 1
#define BIT_REG_IRDMAPEND_RDMA_HPND                 0x2
#define SETF_REG_IRDMAPEND_RDMA_HPND()              ADDR_REG_IRDMAPEND |= BIT_REG_IRDMAPEND_RDMA_HPND
#define CLRF_REG_IRDMAPEND_RDMA_HPND()              ADDR_REG_IRDMAPEND &= ~BIT_REG_IRDMAPEND_RDMA_HPND
#define GETF_REG_IRDMAPEND_RDMA_HPND()              ((ADDR_REG_IRDMAPEND & BIT_REG_IRDMAPEND_RDMA_HPND) >> POS_REG_IRDMAPEND_RDMA_HPND)
// IR Read DMA All Size Pending Bit
#define POS_REG_IRDMAPEND_RDMA_APND                 0
#define BIT_REG_IRDMAPEND_RDMA_APND                 0x1
#define SETF_REG_IRDMAPEND_RDMA_APND()              ADDR_REG_IRDMAPEND |= BIT_REG_IRDMAPEND_RDMA_APND
#define CLRF_REG_IRDMAPEND_RDMA_APND()              ADDR_REG_IRDMAPEND &= ~BIT_REG_IRDMAPEND_RDMA_APND
#define GETF_REG_IRDMAPEND_RDMA_APND()              ((ADDR_REG_IRDMAPEND & BIT_REG_IRDMAPEND_RDMA_APND) >> POS_REG_IRDMAPEND_RDMA_APND)

// IRRX

// IR RX Control Register
#define ADDR_REG_IRRXCON                            *(volatile unsigned long*) (SFR7_BASE + 0x2F*4)
// IR Key Release Pending
#define POS_REG_IRRXCON_KEYRELS                     17
#define BIT_REG_IRRXCON_KEYRELS                     0x20000
#define SETF_REG_IRRXCON_KEYRELS()                  ADDR_REG_IRRXCON |= BIT_REG_IRRXCON_KEYRELS
#define CLRF_REG_IRRXCON_KEYRELS()                  ADDR_REG_IRRXCON &= ~BIT_REG_IRRXCON_KEYRELS
#define GETF_REG_IRRXCON_KEYRELS()                  ((ADDR_REG_IRRXCON & BIT_REG_IRRXCON_KEYRELS) >> POS_REG_IRRXCON_KEYRELS)
// IR Rx Data Finish Pending
#define POS_REG_IRRXCON_RXPND                       16
#define BIT_REG_IRRXCON_RXPND                       0x10000
#define SETF_REG_IRRXCON_RXPND()                    ADDR_REG_IRRXCON |= BIT_REG_IRRXCON_RXPND
#define CLRF_REG_IRRXCON_RXPND()                    ADDR_REG_IRRXCON &= ~BIT_REG_IRRXCON_RXPND
#define GETF_REG_IRRXCON_RXPND()                    ((ADDR_REG_IRRXCON & BIT_REG_IRRXCON_RXPND) >> POS_REG_IRRXCON_RXPND)
// IR Rx Wake Up Sleep Mode Enable
#define POS_REG_IRRXCON_IRWKEN                      4
#define BIT_REG_IRRXCON_IRWKEN                      0x10
#define SETF_REG_IRRXCON_IRWKEN()                   ADDR_REG_IRRXCON |= BIT_REG_IRRXCON_IRWKEN
#define CLRF_REG_IRRXCON_IRWKEN()                   ADDR_REG_IRRXCON &= ~BIT_REG_IRRXCON_IRWKEN
#define GETF_REG_IRRXCON_IRWKEN()                   ((ADDR_REG_IRRXCON & BIT_REG_IRRXCON_IRWKEN) >> POS_REG_IRRXCON_IRWKEN)
// IR Rx 32K Configure Select
#define POS_REG_IRRXCON_IR32KSEL                    3
#define BIT_REG_IRRXCON_IR32KSEL                    0x8
#define SETF_REG_IRRXCON_IR32KSEL()                 ADDR_REG_IRRXCON |= BIT_REG_IRRXCON_IR32KSEL
#define CLRF_REG_IRRXCON_IR32KSEL()                 ADDR_REG_IRRXCON &= ~BIT_REG_IRRXCON_IR32KSEL
#define GETF_REG_IRRXCON_IR32KSEL()                 ((ADDR_REG_IRRXCON & BIT_REG_IRRXCON_IR32KSEL) >> POS_REG_IRRXCON_IR32KSEL)
// IR Rx Data Select
#define POS_REG_IRRXCON_IRRXSEL                     2
#define BIT_REG_IRRXCON_IRRXSEL                     0x4
#define SETF_REG_IRRXCON_IRRXSEL()                  ADDR_REG_IRRXCON |= BIT_REG_IRRXCON_IRRXSEL
#define CLRF_REG_IRRXCON_IRRXSEL()                  ADDR_REG_IRRXCON &= ~BIT_REG_IRRXCON_IRRXSEL
#define GETF_REG_IRRXCON_IRRXSEL()                  ((ADDR_REG_IRRXCON & BIT_REG_IRRXCON_IRRXSEL) >> POS_REG_IRRXCON_IRRXSEL)
// IR Rx Interrupt Enable
#define POS_REG_IRRXCON_IRIE                        1
#define BIT_REG_IRRXCON_IRIE                        0x2
#define SETF_REG_IRRXCON_IRIE()                     ADDR_REG_IRRXCON |= BIT_REG_IRRXCON_IRIE
#define CLRF_REG_IRRXCON_IRIE()                     ADDR_REG_IRRXCON &= ~BIT_REG_IRRXCON_IRIE
#define GETF_REG_IRRXCON_IRIE()                     ((ADDR_REG_IRRXCON & BIT_REG_IRRXCON_IRIE) >> POS_REG_IRRXCON_IRIE)
// IR Rx Enable
#define POS_REG_IRRXCON_IREN                        0
#define BIT_REG_IRRXCON_IREN                        0x1
#define SETF_REG_IRRXCON_IREN()                     ADDR_REG_IRRXCON |= BIT_REG_IRRXCON_IREN
#define CLRF_REG_IRRXCON_IREN()                     ADDR_REG_IRRXCON &= ~BIT_REG_IRRXCON_IREN
#define GETF_REG_IRRXCON_IREN()                     ((ADDR_REG_IRRXCON & BIT_REG_IRRXCON_IREN) >> POS_REG_IRRXCON_IREN)

// IR RX Data Register
#define ADDR_REG_IRRXDAT                            *(volatile unsigned long*) (SFR7_BASE + 0x30*4)
// IR RX Data
#define POS_REG_IRRXDAT                             0
#define BIT_REG_IRRXDAT                             0xFFFFFFFF
#define GET_REG_IRRXDAT()                            ((ADDR_REG_IRRXDAT & BIT_REG_IRRXDAT) >> POS_REG_IRRXDAT)

// IR RX Clear Pending Register
#define ADDR_REG_IRRXCPND                           *(volatile unsigned long*) (SFR7_BASE + 0x31*4)
// IR Clear Key Release Pending
#define POS_REG_IRRXCPND_CRELSPND                   17
#define BIT_REG_IRRXCPND_CRELSPND                   0x20000
#define SETF_REG_IRRXCPND_CRELSPND()                ADDR_REG_IRRXCPND |= BIT_REG_IRRXCPND_CRELSPND
#define CLRF_REG_IRRXCPND_CRELSPND()                ADDR_REG_IRRXCPND &= ~BIT_REG_IRRXCPND_CRELSPND
#define GETF_REG_IRRXCPND_CRELSPND()                ((ADDR_REG_IRRXCPND & BIT_REG_IRRXCPND_CRELSPND) >> POS_REG_IRRXCPND_CRELSPND)
// IR Clear RX Finish Pending
#define POS_REG_IRRXCPND_CRXSPND                    16
#define BIT_REG_IRRXCPND_CRXSPND                    0x10000
#define SETF_REG_IRRXCPND_CRXSPND()                 ADDR_REG_IRRXCPND |= BIT_REG_IRRXCPND_CRXSPND
#define CLRF_REG_IRRXCPND_CRXSPND()                 ADDR_REG_IRRXCPND &= ~BIT_REG_IRRXCPND_CRXSPND
#define GETF_REG_IRRXCPND_CRXSPND()                 ((ADDR_REG_IRRXCPND & BIT_REG_IRRXCPND_CRXSPND) >> POS_REG_IRRXCPND_CRXSPND)

// IR RX Error Configure Register 0
#define ADDR_REG_IRRXERR0                           *(volatile unsigned long*) (SFR7_BASE + 0x32*4)
// IR Repeat Time Error = (RPTERR + 1 ) * (1M or 32K)
#define POS_REG_IRRXERR0_RPTERR                     16
#define BIT_REG_IRRXERR0_RPTERR                     0xFFF0000
#define SET_REG_IRRXERR0_RPTERR(val)                ADDR_REG_IRRXERR0 = ((ADDR_REG_IRRXERR0 & (~BIT_REG_IRRXERR0_RPTERR)) | ((val) << POS_REG_IRRXERR0_RPTERR))
#define GET_REG_IRRXERR0_RPTERR()                   ((ADDR_REG_IRRXERR0 & BIT_REG_IRRXERR0_RPTERR) >> POS_REG_IRRXERR0_RPTERR)
// IR Data Time Error = (DATERR + 1 ) * (1M or 32K)
#define POS_REG_IRRXERR0_DATERR                     0
#define BIT_REG_IRRXERR0_DATERR                     0xFFF
#define SET_REG_IRRXERR0_DATERR(val)                ADDR_REG_IRRXERR0 = ((ADDR_REG_IRRXERR0 & (~BIT_REG_IRRXERR0_DATERR)) | ((val) << POS_REG_IRRXERR0_DATERR))
#define GET_REG_IRRXERR0_DATERR()                   ((ADDR_REG_IRRXERR0 & BIT_REG_IRRXERR0_DATERR) >> POS_REG_IRRXERR0_DATERR)

// IR RX Error Configure Register 1
#define ADDR_REG_IRRXERR1                           *(volatile unsigned long*) (SFR7_BASE + 0x33*4)
// IR Time Out Length = (TOPR + 1 ) * (1M or 32K)
#define POS_REG_IRRXERR1_TOPR                       20
#define BIT_REG_IRRXERR1_TOPR                       0xFFF00000
#define SET_REG_IRRXERR1_TOPR(val)                  ADDR_REG_IRRXERR1 = ((ADDR_REG_IRRXERR1 & (~BIT_REG_IRRXERR1_TOPR)) | ((val) << POS_REG_IRRXERR1_TOPR))
#define GET_REG_IRRXERR1_TOPR()                     ((ADDR_REG_IRRXERR1 & BIT_REG_IRRXERR1_TOPR) >> POS_REG_IRRXERR1_TOPR)
// IR Data One Time Error = (ONEERR + 1 ) * (1M or 32K)
#define POS_REG_IRRXERR1_ONEERR                     10
#define BIT_REG_IRRXERR1_ONEERR                     0xFFC00
#define SET_REG_IRRXERR1_ONEERR(val)                ADDR_REG_IRRXERR1 = ((ADDR_REG_IRRXERR1 & (~BIT_REG_IRRXERR1_ONEERR)) | ((val) << POS_REG_IRRXERR1_ONEERR))
#define GET_REG_IRRXERR1_ONEERR()                   ((ADDR_REG_IRRXERR1 & BIT_REG_IRRXERR1_ONEERR) >> POS_REG_IRRXERR1_ONEERR)
// IR Data Zero Time Error = (ZEROERR + 1 ) * (1M or 32K)
#define POS_REG_IRRXERR1_ZEROERR                    0
#define BIT_REG_IRRXERR1_ZEROERR                    0x3FF
#define SET_REG_IRRXERR1_ZEROERR(val)               ADDR_REG_IRRXERR1 = ((ADDR_REG_IRRXERR1 & (~BIT_REG_IRRXERR1_ZEROERR)) | ((val) << POS_REG_IRRXERR1_ZEROERR))
#define GET_REG_IRRXERR1_ZEROERR()                  ((ADDR_REG_IRRXERR1 & BIT_REG_IRRXERR1_ZEROERR) >> POS_REG_IRRXERR1_ZEROERR)

// IR RX Period Configure Register 0
#define ADDR_REG_IRRXPR0                            *(volatile unsigned long*) (SFR7_BASE + 0x34*4)
// IR Repeat Time Period = (IRRXPR + 1 ) * (1M or 32K)
#define POS_REG_IRRXPR0_RPTPR                       16
#define BIT_REG_IRRXPR0_RPTPR                       0x7FFF0000
#define SET_REG_IRRXPR0_RPTPR(val)                  ADDR_REG_IRRXPR0 = ((ADDR_REG_IRRXPR0 & (~BIT_REG_IRRXPR0_RPTPR)) | ((val) << POS_REG_IRRXPR0_RPTPR))
#define GET_REG_IRRXPR0_RPTPR()                     ((ADDR_REG_IRRXPR0 & BIT_REG_IRRXPR0_RPTPR) >> POS_REG_IRRXPR0_RPTPR)
// IR Data Time Period = (DATPR + 1 ) * (1M or 32K)
#define POS_REG_IRRXPR0_DATPR                       0
#define BIT_REG_IRRXPR0_DATPR                       0x7FFF
#define SET_REG_IRRXPR0_DATPR(val)                  ADDR_REG_IRRXPR0 = ((ADDR_REG_IRRXPR0 & (~BIT_REG_IRRXPR0_DATPR)) | ((val) << POS_REG_IRRXPR0_DATPR))
#define GET_REG_IRRXPR0_DATPR()                     ((ADDR_REG_IRRXPR0 & BIT_REG_IRRXPR0_DATPR) >> POS_REG_IRRXPR0_DATPR)

// IR RX Period Configure Register 1
#define ADDR_REG_IRRXPR1                            *(volatile unsigned long*) (SFR7_BASE + 0x35*4)
// IR Data One Time Period = (IRRXPR + 1 ) * (1M or 32K)
#define POS_REG_IRRXPR1_ONEPR                       16
#define BIT_REG_IRRXPR1_ONEPR                       0x7FFF0000
#define SET_REG_IRRXPR1_ONEPR(val)                  ADDR_REG_IRRXPR1 = ((ADDR_REG_IRRXPR1 & (~BIT_REG_IRRXPR1_ONEPR)) | ((val) << POS_REG_IRRXPR1_ONEPR))
#define GET_REG_IRRXPR1_ONEPR()                     ((ADDR_REG_IRRXPR1 & BIT_REG_IRRXPR1_ONEPR) >> POS_REG_IRRXPR1_ONEPR)
// IR Data Zero Time Period = (ZEROPR + 1 ) * (1M or 32K)
#define POS_REG_IRRXPR1_ZEROPR                       0
#define BIT_REG_IRRXPR1_ZEROPR                       0x7FFF
#define SET_REG_IRRXPR1_ZEROPR(val)                  ADDR_REG_IRRXPR1 = ((ADDR_REG_IRRXPR1 & (~BIT_REG_IRRXPR1_ZEROPR)) | ((val) << POS_REG_IRRXPR1_ZEROPR))
#define GET_REG_IRRXPR1_ZEROPR()                     ((ADDR_REG_IRRXPR1 & BIT_REG_IRRXPR1_ZEROPR) >> POS_REG_IRRXPR1_ZEROPR)

// IR TX Filter Control Register
#define ADDR_REG_IRFLTCON                            *(volatile unsigned long*) (SFR7_BASE + 0x36*4)
// IR Filter Length
#define POS_REG_IRFLTCON_IRFLT_LEN                   8
#define BIT_REG_IRFLTCON_IRFLT_LEN                   0xFF00
#define SET_REG_IRFLTCON_IRFLT_LEN(val)              ADDR_REG_IRFLTCON = ((ADDR_REG_IRFLTCON & (~BIT_REG_IRFLTCON_IRFLT_LEN)) | ((val) << POS_REG_IRFLTCON_IRFLT_LEN))
#define GET_REG_IRFLTCON_IRFLT_LEN()                 ((ADDR_REG_IRFLTCON & BIT_REG_IRFLTCON_IRFLT_LEN) >> POS_REG_IRFLTCON_IRFLT_LEN)
// IR Filter Input Source Select
#define POS_REG_IRFLTCON_IRFLT_SRCS                  1
#define BIT_REG_IRFLTCON_IRFLT_SRCS                  0x2
#define SETF_REG_IRFLTCON_IRFLT_SRCS()               ADDR_REG_IRFLTCON |= BIT_REG_IRFLTCON_IRFLT_SRCS
#define CLRF_REG_IRFLTCON_IRFLT_SRCS()               ADDR_REG_IRFLTCON &= ~BIT_REG_IRFLTCON_IRFLT_SRCS
#define GETF_REG_IRFLTCON_IRFLT_SRCS()               ((ADDR_REG_IRFLTCON & BIT_REG_IRFLTCON_IRFLT_SRCS) >> POS_REG_IRFLTCON_IRFLT_SRCS)
// IR Filter Enable
#define POS_REG_IRFLTCON_IRFLT_EN                    0
#define BIT_REG_IRFLTCON_IRFLT_EN                    0x1
#define SETF_REG_IRFLTCON_IRFLT_EN()                 ADDR_REG_IRFLTCON |= BIT_REG_IRFLTCON_IRFLT_EN
#define CLRF_REG_IRFLTCON_IRFLT_EN()                 ADDR_REG_IRFLTCON &= ~BIT_REG_IRFLTCON_IRFLT_EN
#define GETF_REG_IRFLTCON_IRFLT_EN()                 ((ADDR_REG_IRFLTCON & BIT_REG_IRFLTCON_IRFLT_EN) >> POS_REG_IRFLTCON_IRFLT_EN)

//1m时钟下的解码timing
#define RPTERR_CNT              1000
#define DATERR_CNT              1000
#define ONEERR_CNT              250
#define ZEROERR_CNT             125
#define TOPR_CNT                1718
#define RPTPR_CNT               11249
#define DATPR_CNT               13499
#define DATA_0_CNT              1119
#define DATA_1_CNT              2249


/**
 * @brief Flag Enumeration.
 */
typedef enum {
    IRTX_FLAG_WDMA_HPND         = (1 << 0),
    IRTX_FLAG_WDMA_APND         = (1 << 1),
    IRTX_FLAG_RDMA_HPND         = (1 << 2),
    IRTX_FLAG_RDMA_APND         = (1 << 3),
} IRTX_FLAG_TYPEDEF;

// irrx
typedef enum
{
    IRRX_WAKUP_SLEEP_DISABLE,
    IRRX_WAKUP_SLEEP_ENABLE,
}irrx_wakup_en_t;

typedef enum
{
    IRRX_DATA_SELECT_32BIT,
    IRRX_DATA_SELECT_16BIT,
}irrx_data_select_t;

typedef enum
{
    IRRX_INT_DISABLE,
    IRRX_INT_ENABLE,
}irrx_interrupt_en_t;

typedef enum
{
    IRRX_DISABLE,
    IRRX_ENABLE,
}irrx_en_t;

typedef struct
{
    uint8_t clk_sel;
    irrx_wakup_en_t wakup_en;
    irrx_data_select_t data_format;
    irrx_interrupt_en_t int_en;
    irrx_en_t irrx_en;
}irrx_param_t;

// irtx
typedef enum
{
    IRTX_ENCODE_FORMAT_NEC,
    IRTX_ENCODE_FORMAT_TC9012,
    IRTX_ENCODE_FORMAT_RC5,
    IRTX_ENCODE_FORMAT_RC6,
}irtx_encode_format_t;

typedef enum
{
    // irtx repeat
    IRTX_KEY_RELEASED,
    IRTX_KEY_PRESSED,
}irtx_key_t;

typedef enum
{
    IRTX_CAP_EDGE_FALLING,
    IRTX_CAP_EDGE_RISING,
}irtx_cap_edge_t;

typedef enum
{
    IRTX_CARRIER_DISABLE,
    IRTX_CARRIER_ENABLE,
}irtx_carrier_en_t;

/*if tc9012 indicated as sync code
    0: sync code is "0"
    1: sync code is "1"
  if rc6 indicated as trailer code
    0: trailer code is "0"
    1: trailer code is "1"
*/
typedef enum
{
    IRTX_SYNC_OR_TRAILER_CODE_0,
    IRTX_SYNC_OR_TRAILER_CODE_1,
}irtx_sync_code_t;

typedef enum
{
    IRTX_INVERT_OUTPUT_DISABLE,
    IRTX_INVERT_OUTPUT_ENABLE,
}irtx_invert_en_t;

typedef enum
{
    IRTX_FILTER_DISABLE,
    IRTX_FILTER_ENABLE,
}irtx_filter_en_t;

typedef enum
{
    IRTX_MODE_CLKSEL_4M,
    IRTX_MODE_CLKSEL_3M,
}irtx_mod_clksel_t;

typedef enum
{
    IRTX_DMA_MODE_32BIT,
    IRTX_DMA_MODE_16BIT,
}irtx_dma_mode_t;

//时钟选择
typedef enum {
    IR_CLK_X24M32K,
    IR_CLK_X24MDIV,
    IR_CLK_OSC32K,
    IR_CLK_RC32K,
}ir_clk_t;

typedef struct
{
    irtx_filter_en_t filter_en;         // IR Filter Enable(In Capture Mode Or IR_RX Mode)
    uint8_t filter_len;
}irtx_filter_config_t;

typedef struct
{
    irtx_carrier_en_t carrier_en;
    irtx_mod_clksel_t mod_clksel;       // Tx Modulation Clock Select. 1-4M; 0-3M
    uint8_t ircw_duty;                  // Tx Carrier Duty = (ircw_duty) / (ircw_length + 1))
    uint8_t ircw_length;                // Tx Carrier Freq = (3M or 4M) / (ircw_length + 1))
}irtx_carrier_config_t;

typedef struct
{
    irtx_dma_mode_t wdma_mode;          //IR write DMA mode
    irtx_dma_mode_t rdma_mode;          //IR read DMA mode
}irtx_dma_config_t;

typedef struct
{
    ir_clk_t clk_sel;
    uint32_t tx_data;
    irtx_encode_format_t encode_format;
    irtx_key_t  key;
    irtx_cap_edge_t cap_edge;
    irtx_carrier_config_t carrier_config;
    irtx_filter_config_t filter_config;
    irtx_sync_code_t sync_code;
    irtx_invert_en_t invert_en;
    irtx_dma_config_t dma_config;
}irtx_param_t;

//TIMING信息结构体
typedef struct
{
    // irtx select encoding format in modulatin mode. 0-nec;  1-tc9012;  2-rc5;  3-rc6.
    uint16_t em;
    // irtx code "1" mode. 0-mark + space;   1-space + mark.
    uint16_t o;
    // irtx code "0" mode. 0-mark + space;   1-space + mark.
    uint16_t z;
    // irtx code "1" mark time = (m1 + 1) * (1M or 32K).
    uint16_t m1;
    // irtx code "1" space time = (s1 + 1) * (1M or 32K).
    uint16_t s1;
    // irtx code "0" mark time = (m0 + 1) * (1M or 32K).
    uint16_t m0;
    // irtx code "0" space time = (s0 + 1) * (1M or 32K).
    uint16_t s0;
    // irtx start code "1" time = (t1 + 1) * (1M or 32K).
    uint16_t t1;
    // irtx start code "0" time = (t0 + 1) * (1M or 32K).
    uint16_t t0;
    //irtx repeat code "1" time = (r1 + 1) * (1M or 32K).
    uint16_t r1;
    // irtx repeat code "0" time = (r0 + 1) * (1M or 32K).
    uint16_t r0;
    // irtx the interval between repeat time = (rt + 1) * (1M or 32K).
    uint32_t rt;
    // irtx end pulse time = (et + 1) * (1M or 32K).
    uint16_t et;
    // irtx sync code in tc9012 or trailer code in rc6.
    uint16_t sy;
    // irtx repeat mode.   0-indicates duplicate code(nec or tc9012); 1-indicates no duplicate code(rc5 or rc6)
    uint16_t rm;
    // irtx length
    uint16_t l;
} IRTX_timingTypeDef;

// #if IRTXCLK_SEL
//                                                                         //em,   o,z, m1,  s1, m0, s0,  t1,  t0,  r1,  r0,    rt,et,sy,rm,l
//     #define IRTX_TIMING_NEC    (const uint32_t[])IRTX_TIMING(IRTX_ENCMOD_NEC,   0,0,559,1689,559,559,8999,4499,8999,2249,108000,559,0,0,32)
//     #define IRTX_TIMING_TC9012 (const uint32_t[])IRTX_TIMING(IRTX_ENCMOD_TC9012,0,0,559,1689,559,559,4499,4499,4499,2249,108000,559,1,0,32)
//     #define IRTX_TIMING_RC5    (const uint32_t[])IRTX_TIMING(IRTX_ENCMOD_RC5,   1,0,844, 844,844,844,1687,1687,   0,   0,108000, 17,0,1,12)
//     #define IRTX_TIMING_RC6    (const uint32_t[])IRTX_TIMING(IRTX_ENCMOD_RC6,   0,1,443, 443,443,443,2665, 888,   0,   0,106699, 17,0,1,16)
// #else
//     #define IRTX_TIMING_NEC    (const uint32_t[])IRTX_TIMING(IRTX_ENCMOD_NEC,   0,0, 18,  54, 18, 17, 293, 147, 294,  73,  3537, 18,0,0,32)
//     #define IRTX_TIMING_TC9012 (const uint32_t[])IRTX_TIMING(IRTX_ENCMOD_TC9012,0,0, 18,  54, 18, 17, 147, 146, 147,  74,  3537, 18,1,0,32)
//     #define IRTX_TIMING_RC5    (const uint32_t[])IRTX_TIMING(IRTX_ENCMOD_RC5,   1,0, 28,  26, 28, 26,  55,  55,   0,   0,  3537,  1,0,1,12)
//     #define IRTX_TIMING_RC6    (const uint32_t[])IRTX_TIMING(IRTX_ENCMOD_RC6,   0,1, 14,  13, 14, 13,  87,  28,   0,   0,  3494,  1,0,1,16)
// #endif

//1m时钟下TIMING数据
                                       //em,   o,z, m1,  s1, m0, s0,  t1,  t0,  r1,  r0,              rt,et,sy,rm,l
#define IRTX_TIMING_NEC    {IRTX_ENCODE_FORMAT_NEC,   0,0,559,1689,559,559,8999,4499,8999,2249,(uint32_t)108000,559,0,0,32}
#define IRTX_TIMING_TC9012 {IRTX_ENCODE_FORMAT_TC9012,0,0,559,1689,559,559,4499,4499,4499,2249,(uint32_t)108000,559,1,0,32}
#define IRTX_TIMING_RC5    {IRTX_ENCODE_FORMAT_RC5,   1,0,844, 844,844,844,1687,1687,   0,   0,(uint32_t)108000, 17,0,1,12}
#define IRTX_TIMING_RC6    {IRTX_ENCODE_FORMAT_RC6,   0,1,443, 443,443,443,2665, 888,   0,   0,(uint32_t)106699, 17,0,1,16}

//以1m时钟的timing信息为基准缩放其他时钟下的timing信息
//缩放使用浮点数防止精度丢失或直接为0
static uint32_t timing_freq[4] = {32760, 1000000, 31250, 32812};

static uint32_t inline ir_timing_freq_get(uint8_t clk_sel)
{
    return timing_freq[clk_sel];
}

void irtx_kick_wait(void);
void irtx_clk_init (uint8_t clksel);
void ir_clk_init(uint8_t clksel);

void irtx_io_init (void);
void irrx_io_init (void);

void irrx_done_clr(void);
void irtx_cap_done_clr(void);
void irrx_done_set(void);
void irtx_cap_done_set(void);
uint8_t irrx_done_get(void);
uint8_t irtx_cap_done_get(void);

void irrx_decode_config(irrx_typedef *irrx_reg, const irrx_param_t *irrx_param);
void irrx_decode_init(irrx_typedef *irrx_reg, const irrx_param_t *irrx_param);
void irrx_init(irrx_typedef *irrx_reg, const irrx_param_t *irrx_param);
void irrx_wake_init(irrx_typedef *irrx_reg, const irrx_param_t *irrx_param);

void irtx_init(irtx_typedef *irtx_reg, const irtx_param_t *irtx_param);
void irtx_timing_init(irtx_typedef *irtx_reg, const irtx_param_t *irtx_param);
void irtx_capture_buf_init(irtx_typedef *irtx_reg, void *buf, uint32_t len);
void irtx_learn_buf_init(irtx_typedef *irtx_reg, void *buf, uint32_t len);
void irtx_capture_init(irtx_typedef *irtx_reg, const irtx_param_t *irtx_param);
void irtx_learn_init(irtx_typedef *irtx_reg, const irtx_param_t *irtx_param);
void irtx_base_init(irtx_typedef *irtx_reg, const irtx_param_t *irtx_param);
void irtx_data_set (irtx_typedef *irtx_reg, uint32_t code);

void irtx_send(void);
void irtx_repeat_send(void);
void irtx_pic_config(isr_t isr, int pr, IRTX_FLAG_TYPEDEF flag_type, FUNCTIONAL_STATE state);

/*function name of redefining*/
#define irtx_softmodulation_init(x,y)           irtx_learn_init(x,y)
#define irtx_softmodulation_buf_init(x,y,z)     irtx_learn_buf_init(x,y,z)

#endif
