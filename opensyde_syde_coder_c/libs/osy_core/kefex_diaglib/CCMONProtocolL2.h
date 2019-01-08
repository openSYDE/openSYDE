#ifndef CCMONProtocolL2H
#define CCMONProtocolL2H

#include "stwtypes.h"
#include "CCMONProtocolBase.h"
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

///Interpretation as layer two protocol (no L7 interpretation)
class CMONPROTOCOL_PACKAGE C_CMONProtocolL2 : public C_CMONProtocolBase
{
public:
   virtual stw_scl::C_SCLString MessageToString(const stw_can::T_STWCAN_Msg_RX & orc_Msg) const;
   virtual stw_scl::C_SCLString GetProtocolName(void) const;
};

//---------------------------------------------------------------------------

}

#endif
