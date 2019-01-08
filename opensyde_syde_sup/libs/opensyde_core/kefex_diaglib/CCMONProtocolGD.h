#ifndef CCMONProtocolGDH
#define CCMONProtocolGDH

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

///Interpretation of Generic Driver protocol
class CMONPROTOCOL_PACKAGE C_CMONProtocolGD : public C_CMONProtocolBase
{
public:
   virtual stw_scl::C_SCLString MessageToString(const stw_can::T_STWCAN_Msg_RX & orc_Msg) const;
   virtual stw_scl::C_SCLString GetProtocolName(void) const;
};

//---------------------------------------------------------------------------

}

#endif
