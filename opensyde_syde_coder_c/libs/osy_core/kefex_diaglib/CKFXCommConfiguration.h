#ifndef CKFXCOMMCONFIGURATIONH
#define CKFXCOMMCONFIGURATIONH
//---------------------------------------------------------------------------

#include "stwtypes.h"
#include "SCLDynamicArray.h"
#include "CSCLIniFile.h"
#include "CSCLString.h"
#include "DiagLib_config.h"

//---------------------------------------------------------------------------

namespace stw_diag_lib
{

//---------------------------------------------------------------------------

///holds one communication protocol parameter
class C_KFXCommParameter
{
public:
   stw_scl::C_SCLString c_Name;          ///< name of parameter
   stw_types::sint64   s64_MinValue;     ///< valid minimum value
   stw_types::sint64   s64_MaxValue;     ///< valid maximum value
   stw_types::sint64   s64_DefaultValue; ///< default value
   stw_scl::C_SCLString c_DisplayName;   ///< text to display in configuration dialog
   stw_scl::C_SCLString c_Comment;       ///< can for example be used for "hint"
   stw_scl::C_SCLString c_INIDirective;  ///< directive for storing parameter value in an INI file

   C_KFXCommParameter(void);
};

//---------------------------------------------------------------------------

///engine for handling communication protocol specific parameters
class STW_DIAGLIB_EXPORT C_KFXCommConfigurationBase
{
public:
   C_KFXCommConfigurationBase(void);
   C_KFXCommConfigurationBase(const C_KFXCommConfigurationBase & orc_Source);
   C_KFXCommConfigurationBase(const stw_scl::SCLDynamicArray<C_KFXCommParameter> & orc_Parameters);
   virtual ~C_KFXCommConfigurationBase(void);

   C_KFXCommConfigurationBase& operator =(const C_KFXCommConfigurationBase & orc_Source);

   stw_scl::SCLDynamicArray<C_KFXCommParameter>mc_Parameters;
   stw_scl::SCLDynamicArray<stw_types::sint64>mc_Values;
   stw_scl::C_SCLString mc_ProtocolName;

   void SetParameterList(const stw_scl::SCLDynamicArray<C_KFXCommParameter> & orc_Parameters);
   stw_types::sint32 GetParameterValue(const stw_scl::C_SCLString & orc_ParameterName,
                                       stw_types::sint64 & ors64_Value) const;
   stw_types::sint32 SetParameterValue(const stw_scl::C_SCLString & orc_ParameterName,
                                       const stw_types::sint64 os64_Value);
   void SetAllDefaults(void);

   stw_types::sint32 LoadConfigFromINI(stw_scl::C_SCLIniFile & orc_File, const stw_scl::C_SCLString & orc_Section);
   stw_types::sint32 SaveConfigToINI(stw_scl::C_SCLIniFile & orc_File, const stw_scl::C_SCLString & orc_Section) const;
};

//---------------------------------------------------------------------------
///higher-level class (more comfortable functions for KEFEX protocol)
class STW_DIAGLIB_EXPORT C_KFXCommConfiguration : public C_KFXCommConfigurationBase
{
private:
public:
   C_KFXCommConfiguration& operator =(const C_KFXCommConfiguration & orc_Source);

   //getters and setters:
   stw_types::uint8  GetBaseID(void) const;
   stw_types::uint8  GetClientAddress(void) const;
   stw_types::uint8  GetServerAddress(void) const;
   stw_types::uint16 GetTimeout(void) const;
   stw_types::uint8  GetSTMin(void) const;
   stw_types::uint8  GetBSMax(void) const;

   void SetBaseID       (const stw_types::uint8 ou8_Value);
   void SetClientAddress(const stw_types::uint8 ou8_Value);
   void SetServerAddress(const stw_types::uint8 ou8_Value);
   void SetTimeout      (const stw_types::uint16 ou16_Value);
   void SetSTMin        (const stw_types::uint8 ou8_Value);
   void SetBSMax        (const stw_types::uint8 ou8_Value);
};

//---------------------------------------------------------------------------

//stay compatible for "old" VisLib applications:
typedef C_KFXCommConfiguration t_KFXProtocolInfo;

//---------------------------------------------------------------------------

}

//---------------------------------------------------------------------------

#endif
