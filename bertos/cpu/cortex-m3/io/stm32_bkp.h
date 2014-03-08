/**
 * \file
 * <!--
 * This file is part of BeRTOS.
 *
 * Bertos is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * Copyright 2014 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 *
 * \brief STM32Fxxx BKP definition.
 *
 * \author Daniele Basile <asterix@develer.com>
 */

#ifndef STM32_BKP_H
#define STM32_BKP_H

#include <cpu/detect.h>
#include <cfg/compiler.h>

#if CPU_CM3_STM32F1

/******************  Bit definition for BKP_RTCCR register  *******************/
#define  BKP_RTCCR_CAL                       ((uint16_t)0x007F)     /*!< Calibration value */
#define  BKP_RTCCR_CCO                       ((uint16_t)0x0080)     /*!< Calibration Clock Output */
#define  BKP_RTCCR_ASOE                      ((uint16_t)0x0100)     /*!< Alarm or Second Output Enable */
#define  BKP_RTCCR_ASOS                      ((uint16_t)0x0200)     /*!< Alarm or Second Output Selection */

/********************  Bit definition for BKP_CR register  ********************/
#define  BKP_CR_TPE                          ((uint8_t)0x01)        /*!< TAMPER pin enable */
#define  BKP_CR_TPAL                         ((uint8_t)0x02)        /*!< TAMPER pin active level */

/*******************  Bit definition for BKP_CSR register  ********************/
#define  BKP_CSR_CTE                         ((uint16_t)0x0001)     /*!< Clear Tamper event */
#define  BKP_CSR_CTI                         ((uint16_t)0x0002)     /*!< Clear Tamper Interrupt */
#define  BKP_CSR_TPIE                        ((uint16_t)0x0004)     /*!< TAMPER Pin interrupt enable */
#define  BKP_CSR_TEF                         ((uint16_t)0x0100)     /*!< Tamper Event Flag */
#define  BKP_CSR_TIF                         ((uint16_t)0x0200)     /*!< Tamper Interrupt Flag */

/**
  *  Backup Registers
  */
struct stm32_bkp
{
  uint32_t  RESERVED0;
  reg16_t DR1;
  uint16_t  RESERVED1;
  reg16_t DR2;
  uint16_t  RESERVED2;
  reg16_t DR3;
  uint16_t  RESERVED3;
  reg16_t DR4;
  uint16_t  RESERVED4;
  reg16_t DR5;
  uint16_t  RESERVED5;
  reg16_t DR6;
  uint16_t  RESERVED6;
  reg16_t DR7;
  uint16_t  RESERVED7;
  reg16_t DR8;
  uint16_t  RESERVED8;
  reg16_t DR9;
  uint16_t  RESERVED9;
  reg16_t DR10;
  uint16_t  RESERVED10;
  reg16_t RTCCR;
  uint16_t  RESERVED11;
  reg16_t CR;
  uint16_t  RESERVED12;
  reg16_t CSR;
  uint16_t  RESERVED13[5];
  reg16_t DR11;
  uint16_t  RESERVED14;
  reg16_t DR12;
  uint16_t  RESERVED15;
  reg16_t DR13;
  uint16_t  RESERVED16;
  reg16_t DR14;
  uint16_t  RESERVED17;
  reg16_t DR15;
  uint16_t  RESERVED18;
  reg16_t DR16;
  uint16_t  RESERVED19;
  reg16_t DR17;
  uint16_t  RESERVED20;
  reg16_t DR18;
  uint16_t  RESERVED21;
  reg16_t DR19;
  uint16_t  RESERVED22;
  reg16_t DR20;
  uint16_t  RESERVED23;
  reg16_t DR21;
  uint16_t  RESERVED24;
  reg16_t DR22;
  uint16_t  RESERVED25;
  reg16_t DR23;
  uint16_t  RESERVED26;
  reg16_t DR24;
  uint16_t  RESERVED27;
  reg16_t DR25;
  uint16_t  RESERVED28;
  reg16_t DR26;
  uint16_t  RESERVED29;
  reg16_t DR27;
  uint16_t  RESERVED30;
  reg16_t DR28;
  uint16_t  RESERVED31;
  reg16_t DR29;
  uint16_t  RESERVED32;
  reg16_t DR30;
  uint16_t  RESERVED33;
  reg16_t DR31;
  uint16_t  RESERVED34;
  reg16_t DR32;
  uint16_t  RESERVED35;
  reg16_t DR33;
  uint16_t  RESERVED36;
  reg16_t DR34;
  uint16_t  RESERVED37;
  reg16_t DR35;
  uint16_t  RESERVED38;
  reg16_t DR36;
  uint16_t  RESERVED39;
  reg16_t DR37;
  uint16_t  RESERVED40;
  reg16_t DR38;
  uint16_t  RESERVED41;
  reg16_t DR39;
  uint16_t  RESERVED42;
  reg16_t DR40;
  uint16_t  RESERVED43;
  reg16_t DR41;
  uint16_t  RESERVED44;
  reg16_t DR42;
  uint16_t  RESERVED45;
};

#else
#error Unknown CPU
#endif

#endif /* STM32_BKP_H */
