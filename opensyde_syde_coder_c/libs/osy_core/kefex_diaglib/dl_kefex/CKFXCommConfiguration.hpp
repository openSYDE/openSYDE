#ifndef CKFXCOMMCONFIGURATIONH
#define CKFXCOMMCONFIGURATIONH
//---------------------------------------------------------------------------

#include "stwtypes.hpp"
#include "C_SclDynamicArray.hpp"
#include "C_SclIniFile.hpp"
#include "C_SclString.hpp"
#include "DiagLib_config.hpp"

//---------------------------------------------------------------------------

namespace stw
{
namespace diag_lib
{
//---------------------------------------------------------------------------

///holds one communication protocol parameter
class C_KFXCommParameter
{
public:
   stw::scl::C_SclString c_Name;         ///< name of parameter
   int64_t s64_MinValue;                 ///< valid minimum value
   int64_t s64_MaxValue;                 ///< valid maximum value
   int64_t s64_DefaultValue;             ///< default value
   stw::scl::C_SclString c_DisplayName;  ///< text to display in configuration dialog
   stw::scl::C_SclString c_Comment;      ///< can for example be used for "hint"
   stw::scl::C_SclString c_INIDirective; ///< directive for storing parameter value in an INI file

   C_KFXCommParameter(void);
};

//---------------------------------------------------------------------------

///engine for handling communication protocol specific parameters
class STW_DIAGLIB_EXPORT C_KFXCommConfigurationBase
{
public:
   C_KFXCommConfigurationBase(void);
   C_KFXCommConfigurationBase(const C_KFXCommConfigurationBase & orc_Source);
   C_KFXCommConfigurationBase(const stw::scl::C_SclDynamicArray<C_KFXCommParameter> & orc_Parameters);
   virtual ~C_KFXCommConfigurationBase(void);

   C_KFXCommConfigurationBase & operator =(const C_KFXCommConfigurationBase & orc_Source);

   stw::scl::C_SclDynamicArray<C_KFXCommParameter> mc_Parameters;
   stw::scl::C_SclDynamicArray<int64_t> mc_Values;
   stw::scl::C_SclString mc_ProtocolName;

   void SetParameterList(const stw::scl::C_SclDynamicArray<C_KFXCommParameter> & orc_Parameters);
   int32_t GetParameterValue(const stw::scl::C_SclString & orc_ParameterName, int64_t & ors64_Value) const;
   int32_t SetParameterValue(const stw::scl::C_SclString & orc_ParameterName, const int64_t os64_Value);
   void SetAllDefaults(void);

   int32_t LoadConfigFromINI(stw::scl::C_SclIniFile & orc_File, const stw::scl::C_SclString & orc_Section);
   int32_t SaveConfigToINI(stw::scl::C_SclIniFile & orc_File, const stw::scl::C_SclString & orc_Section) const;
};

//---------------------------------------------------------------------------
///higher-level class (more comfortable functions for KEFEX protocol)
class STW_DIAGLIB_EXPORT C_KFXCommConfiguration :
   public C_KFXCommConfigurationBase
{
private:
public:
   C_KFXCommConfiguration & operator =(const C_KFXCommConfiguration & orc_Source);

   //getters and setters:
   uint8_t  GetBaseID(void) const;
   uint8_t  GetClientAddress(void) const;
   uint8_t  GetServerAddress(void) const;
   uint16_t GetTimeout(void) const;
   uint8_t  GetSTMin(void) const;
   uint8_t  GetBSMax(void) const;

   void SetBaseID       (const uint8_t ou8_Value);
   void SetClientAddress(const uint8_t ou8_Value);
   void SetServerAddress(const uint8_t ou8_Value);
   void SetTimeout      (const uint16_t ou16_Value);
   void SetSTMin        (const uint8_t ou8_Value);
   void SetBSMax        (const uint8_t ou8_Value);
};

//---------------------------------------------------------------------------

//stay compatible for "old" VisLib applications:
typedef C_KFXCommConfiguration t_KFXProtocolInfo;

//---------------------------------------------------------------------------
}
}
//---------------------------------------------------------------------------

#endif
