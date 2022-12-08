//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Translate L2 CAN message to textual interpretation

   \copyright   Copyright 2002 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef CCMONProtocolH
#define CCMONProtocolH

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_CanMonProtocolTarget.hpp"
#include "C_CanMonProtocolBase.hpp"
#include "C_CanMonProtocolCanOpen.hpp"
#include "C_CanMonProtocolKefex.hpp"
#include "C_CanMonProtocolShipIpIva.hpp"
#include "C_CanMonProtocolGd.hpp"
#include "C_CanMonProtocolStwFf.hpp"
#include "C_CanMonProtocolXfl.hpp"
#include "C_CanMonProtocolL2.hpp"
#include "C_CanMonProtocolJ1939.hpp"
#include "C_CanMonProtocolOpenSyde.hpp"
#include "C_SclString.hpp"

#ifndef CMONPROTOCOL_PACKAGE
#ifdef __BORLANDC__
#define CMONPROTOCOL_PACKAGE __declspec(package)
#else
#define CMONPROTOCOL_PACKAGE
#endif
#endif

//----------------------------------------------------------------------------------------------------------------------

namespace stw
{
namespace cmon_protocol
{
//----------------------------------------------------------------------------------------------------------------------

const int32_t gs32_KFX_CMON_NUM_PROTOCOLS = 9;

//keep original name for compatibility (type and values are part of VisLib's API)
enum e_CanMonL7Protocols
{
   CMONL7ProtocolNone = 0, //L2 only
   CMONL7ProtocolCANopen,
   CMONL7ProtocolKEFEX,
   CMONL7ProtocolXFL,
   CMONL7ProtocolSTWFF,
   CMONL7ProtocolGD,
   CMONL7Protocol_SHIP_IP_IVA,
   CMONL7ProtocolJ1939,
   CMONL7ProtocolOpenSYDE
};

//----------------------------------------------------------------------------------------------------------------------
///Aggregator for all possible protocol interpretation types
class CMONPROTOCOL_PACKAGE C_CanMonProtocols
{
private:
   bool mq_Decimal;

   C_CanMonProtocolL2 mc_ProtocolL2;
   C_CanMonProtocolCanOpen mc_ProtocolCANopen;
   C_CanMonProtocolKefex mc_ProtocolKEFEX;
   C_CanMonProtocolXfl mc_ProtocolXFL;
   C_CanMonProtocolStwFf mc_ProtocolSTWFF;
   C_CanMonProtocolGd mc_ProtocolGD;
   C_CanMonProtocolShipIpIva mc_ProtocolSHIP_IP_IVA;
   C_CanMonProtocolJ1939 mc_ProtocolJ1939;
   C_CanMonProtocolOpenSyde mc_ProtocolOpenSYDE;

protected:
   C_CanMonProtocolBase * mapc_Protocols[gs32_KFX_CMON_NUM_PROTOCOLS];

   e_CanMonL7Protocols me_ActiveProtocol;

public:
   //general:
   C_CanMonProtocols(void);
   virtual ~C_CanMonProtocols(void);

   C_CanMonProtocols(const C_CanMonProtocols & orc_Source);               //copying will cause compiler error
   C_CanMonProtocols & operator = (const C_CanMonProtocols & orc_Source); //assignment will cause compiler error

   int32_t SetProtocolMode(const e_CanMonL7Protocols oe_L7Protocol);
   e_CanMonL7Protocols GetProtocolMode(void) const;

   int32_t GetProtocolName(const e_CanMonL7Protocols oe_L7Protocol, stw::scl::C_SclString & orc_Description) const;
   bool GetProtocolHasParameters(const e_CanMonL7Protocols oe_L7Protocol) const;

   int32_t SetDecimalMode(const bool oq_Decimal);
   bool GetDecimalMode(void) const;

   static stw::scl::C_SclString FormatTimeStamp(const uint64_t ou64_TimeStampUs, const bool oq_LeftFillBlanks = false);

   //for displaying on screen (will consider configured L7 protocol and decimal/hex setting):
   stw::scl::C_SclString MessageToString(const stw::can::T_STWCAN_Msg_RX & orc_Msg) const;
   stw::scl::C_SclString MessageToString(const stw::can::T_STWCAN_Msg_TX & orc_Msg) const;
   stw::scl::C_SclString MessageToString(const stw::can::T_STWCAN_Msg_RX & orc_Message,
                                         const uint32_t ou32_Count) const;

   //for protocolling (e.g. to file; individual fields separated by semicolon; L2 AND L7 interpretation; always hex)
   stw::scl::C_SclString MessageToStringLog(const stw::can::T_STWCAN_Msg_RX & orc_Msg) const;
   stw::scl::C_SclString MessageToStringLog(const stw::can::T_STWCAN_Msg_TX & orc_Msg) const;

   int32_t SaveProtocolParametersToINI(const stw::scl::C_SclString & orc_FileName,
                                       const stw::scl::C_SclString & orc_Section) const;
   int32_t LoadProtocolParametersFromINI(const stw::scl::C_SclString & orc_FileName,
                                         const stw::scl::C_SclString & orc_Section) const;

   //KEFEX protocol parameters:
#ifdef CMONPROTOCOL_ALLOW_RAMVIEW_PROJECT_MAPPING
   int32_t KFXSetVariableInfo(const stw::diag_lib::C_KFXVariableLists * const opc_Lists,
                              const uint16_t ou16_ListOffset);
#endif
   uint16_t KFXGetBaseID(void) const;
   void KFXSetBaseID(const uint16_t ou16_BaseID);

   //STW Flashloader ("XFL") protocol parameters:
   uint32_t XFLGetSendID(void) const;
   void XFLSetSendID(const uint32_t ou32_SendID);
};

//----------------------------------------------------------------------------------------------------------------------
}
}

#endif
