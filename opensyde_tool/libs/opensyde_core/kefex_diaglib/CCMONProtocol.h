#ifndef CCMONProtocolH
#define CCMONProtocolH

/* -- Includes ------------------------------------------------------------ */
#include "stwtypes.h"
#include "CCMONProtocolTarget.h"
#include "CCMONProtocolBase.h"
#include "CCMONProtocolCANopen.h"
#include "CCMONProtocolKEFEX.h"
#include "CCMONProtocolSHIPIPIVA.h"
#include "CCMONProtocolGD.h"
#include "CCMONProtocolSTWFF.h"
#include "CCMONProtocolXFL.h"
#include "CCMONProtocolL2.h"
#include "CCMONProtocolJ1939.h"
#include "CCMONProtocolOpenSYDE.h"
#include "CSCLString.h"

#ifndef CMONPROTOCOL_PACKAGE
#ifdef __BORLANDC__
#define CMONPROTOCOL_PACKAGE __declspec(package)
#else
#define CMONPROTOCOL_PACKAGE
#endif
#endif

//---------------------------------------------------------------------------

namespace stw_cmon_protocol
{

//---------------------------------------------------------------------------

const stw_types::sint32 gs32_KFX_CMON_NUM_PROTOCOLS = 9;

//keep original name for compatibility (type and values are part of VisLib's API)
enum e_CMONL7Protocols
{
   CMONL7ProtocolNone = 0,  //L2 only
   CMONL7ProtocolCANopen,
   CMONL7ProtocolKEFEX,
   CMONL7ProtocolXFL,
   CMONL7ProtocolSTWFF,
   CMONL7ProtocolGD,
   CMONL7Protocol_SHIP_IP_IVA,
   CMONL7ProtocolJ1939,
   CMONL7ProtocolOpenSYDE
};

//---------------------------------------------------------------------------
///Aggregator for all possible protocol interpretation types
class CMONPROTOCOL_PACKAGE C_CMONProtocols
{
private:
   bool mq_Decimal;

   C_CMONProtocolL2      mc_ProtocolL2;
   C_CMONProtocolCANopen mc_ProtocolCANopen;
   C_CMONProtocolKEFEX   mc_ProtocolKEFEX;
   C_CMONProtocolXFL     mc_ProtocolXFL;
   C_CMONProtocolSTWFF   mc_ProtocolSTWFF;
   C_CMONProtocolGD      mc_ProtocolGD;
   C_CMONProtocolSHIP_IP_IVA mc_ProtocolSHIP_IP_IVA;
   C_CMONProtocolJ1939   mc_ProtocolJ1939;
   C_CMONProtocolOpenSYDE mc_ProtocolOpenSYDE;

protected:
   C_CMONProtocolBase * mapc_Protocols[gs32_KFX_CMON_NUM_PROTOCOLS];

   e_CMONL7Protocols me_ActiveProtocol;

public:
   //general:
   C_CMONProtocols(void);
   virtual ~C_CMONProtocols(void);

   C_CMONProtocols(const C_CMONProtocols & orc_Source);               //copying will cause compiler error
   C_CMONProtocols & operator = (const C_CMONProtocols & orc_Source); //assignment will cause compiler error

   stw_types::sint32 SetProtocolMode(const e_CMONL7Protocols oe_L7Protocol);
   e_CMONL7Protocols GetProtocolMode(void) const;

   stw_types::sint32 GetProtocolName(const e_CMONL7Protocols oe_L7Protocol,
                                     stw_scl::C_SCLString & orc_Description) const;
   bool GetProtocolHasParameters(const e_CMONL7Protocols oe_L7Protocol) const;

   stw_types::sint32 SetDecimalMode(const bool oq_Decimal);
   bool GetDecimalMode(void) const;

   static stw_scl::C_SCLString FormatTimeStamp(const stw_types::uint64 ou64_TimeStampUs,
                                               const bool oq_LeftFillBlanks = false);

   //for displaying on screen (will consider configured L7 protocol and decimal/hex setting):
   stw_scl::C_SCLString MessageToString(const stw_can::T_STWCAN_Msg_RX & orc_Msg) const;
   stw_scl::C_SCLString MessageToString(const stw_can::T_STWCAN_Msg_TX & orc_Msg) const;
   stw_scl::C_SCLString MessageToString(const stw_can::T_STWCAN_Msg_RX & orc_Message,
                                        const stw_types::uint32 ou32_Count) const;

   //for protocolling (e.g. to file; individual fields separated by semicolon; L2 AND L7 interpretation; always hex)
   stw_scl::C_SCLString MessageToStringLog(const stw_can::T_STWCAN_Msg_RX & orc_Msg) const;
   stw_scl::C_SCLString MessageToStringLog(const stw_can::T_STWCAN_Msg_TX & orc_Msg) const;

   stw_types::sint32 SaveProtocolParametersToINI(const stw_scl::C_SCLString & orc_FileName,
                                                 const stw_scl::C_SCLString & orc_Section) const;
   stw_types::sint32 LoadProtocolParametersFromINI(const stw_scl::C_SCLString & orc_FileName,
                                                   const stw_scl::C_SCLString & orc_Section) const;

   //KEFEX protocol parameters:
#ifdef CMONPROTOCOL_ALLOW_RAMVIEW_PROJECT_MAPPING
   stw_types::sint32 KFXSetVariableInfo(const stw_diag_lib::C_KFXVariableLists * const opc_Lists,
                                        const stw_types::uint16 ou16_ListOffset);
#endif
   stw_types::uint16 KFXGetBaseID(void) const;
   void KFXSetBaseID(const stw_types::uint16 ou16_BaseID);

   //STW Flashloader ("XFL") protocol parameters:
   stw_types::uint32 XFLGetSendID(void) const;
   void XFLSetSendID(const stw_types::uint32 ou32_SendID);
};

//---------------------------------------------------------------------------

}

#endif
