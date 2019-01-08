#ifndef CCMONProtocolXFLH
#define CCMONProtocolXFLH

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

///Interpretation of STW Flashloader protocol
class CMONPROTOCOL_PACKAGE C_CMONProtocolXFL : public C_CMONProtocolBase
{
private:
   stw_types::uint32 mu32_XFLSendID;

public:
   C_CMONProtocolXFL(void);

   virtual stw_scl::C_SCLString MessageToString(const stw_can::T_STWCAN_Msg_RX & orc_Msg) const;
   virtual stw_scl::C_SCLString GetProtocolName(void) const;

   virtual stw_types::sint32 SaveParamsToIni(stw_scl::C_SCLIniFile & orc_IniFile,
                                             const stw_scl::C_SCLString & orc_Section);
   virtual stw_types::sint32 LoadParamsFromIni(stw_scl::C_SCLIniFile & orc_IniFile,
                                               const stw_scl::C_SCLString & orc_Section);

   stw_types::uint32 GetSendID(void) const;
   void SetSendID(const stw_types::uint32 ou32_Send);
};

//---------------------------------------------------------------------------

}

#endif
