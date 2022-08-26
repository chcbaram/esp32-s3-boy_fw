/*
 * mcp2515.h
 *
 *  Created on: 2020. 12. 30.
 *      Author: baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_MCP2515_H_
#define SRC_COMMON_HW_INCLUDE_MCP2515_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"


#ifdef _USE_HW_MCP2515


#define MCP_INST_RESET              0xC0
#define MCP_INST_READ               0x03
#define MCP_INST_READ_RXBUFFER(x)   (0x90+(x<<1))
#define MCP_INST_WRITE              0x02
#define MCP_INST_LOAD_TXBUFFER(x)   (0x40+(x<<0))
#define MCP_INST_RTS(x)             (0x80+(x<<0))
#define MCP_INST_READ_STATUS        0xA0
#define MCP_INST_RX_STATUS          0xB0
#define MCP_INST_BIT_MODIFY         0x05

#define MCP_BFPCTRL                 0x0C
#define MCP_TXRTSCTRL               0x0D
#define MCP_CANSTAT                 0x0E
#define MCP_CANCTRL                 0x0F
#define MCP_TEC                     0x1C
#define MCP_REC                     0x1D
#define MCP_CNF3                    0x28
#define MCP_CNF2                    0x29
#define MCP_CNF1                    0x2A
#define MCP_CANINTE                 0x2B
#define MCP_CANINTF                 0x2C
#define MCP_EFLG                    0x2D
#define MCP_TXB0CTRL                0x30
#define MCP_TXB1CTRL                0x40
#define MCP_TXB2CTRL                0x50
#define MCP_RXB0CTRL                0x60
#define MCP_RXB1CTRL                0x70

#define MCP_TXBCTRL(x)              (0x30 + ((x)<<4))
#define MCP_RXBCTRL(x)              (0x60 + ((x)<<4))
#define MCP_TXBSIDH(x)              (0x31 + ((x)<<4))
#define MCP_RXBSIDH(x)              (0x61 + ((x)<<4))


#define MCP_RXMSIDH(x)              (0x20 + ((x)*4))

#define MCP_MASK_MAX                2
#define MCP_FILTER_MAX              6


enum CANINTF
{
  CANINTF_RX0IF = 0x01,
  CANINTF_RX1IF = 0x02,
  CANINTF_TX0IF = 0x04,
  CANINTF_TX1IF = 0x08,
  CANINTF_TX2IF = 0x10,
  CANINTF_ERRIF = 0x20,
  CANINTF_WAKIF = 0x40,
  CANINTF_MERRF = 0x80
};


typedef enum
{
  MCP_MODE_NORMAL,
  MCP_MODE_SLEEP,
  MCP_MODE_LOOPBACK,
  MCP_MODE_LISTEN,
  MCP_MODE_CONFIG
} McpMode;

typedef enum
{
  MCP_BAUD_125K,
  MCP_BAUD_250K,
  MCP_BAUD_500K,
  MCP_BAUD_1000K
} McpBaud;

typedef struct
{
  uint16_t id;
  bool     ext;
  uint8_t  dlc;
  uint8_t  data[8];
} mcp_msg_t;


bool mcp2515Init(void);
bool mcp2515Reset(void);
bool mcp2515SetMode(McpMode mode);
bool mcp2515SetBaud(McpBaud baud);
bool mcp2515SetFilterMask(uint8_t index, const bool ext, const uint32_t data);
bool mcp2515SetFilter(uint8_t index, const bool ext, const uint32_t data);
McpMode mcp2515GetMode(void);
McpBaud mcp2515GetBaud(void);
uint8_t mcp2515ReadStatus(void);
uint8_t mcp2515ReadErrorFlags(void);

#endif

#ifdef __cplusplus
}
#endif

#endif /* SRC_COMMON_HW_INCLUDE_MCP2515_H_ */
