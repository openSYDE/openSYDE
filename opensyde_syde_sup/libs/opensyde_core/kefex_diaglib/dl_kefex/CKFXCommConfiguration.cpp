//**************************************************************.DA*
//   .MODULE [ CKFXCommConfiguration ]
//--------------------------------------------------------------.D1-
//   .FILE [ CKFXCommConfiguration.cpp ]
//   .GROUP [ KEFEX Library ]
//--------------------------------------------------------------.F1-
//   .DESCRIPTION
//   Class encapsulation communication protocol parameters.
//--------------------------------------------------------------
//   .SPEC
//   SWKE KEFEX
//**************************************************************.DE*
#include "precomp_headers.hpp"  //pre-compiled headers
#ifdef __BORLANDC__   //putting the pragmas in the config-header will not work
#pragma hdrstop
#pragma package(smart_init)
#endif

#include "DiagLib_config.hpp"  //diaglib configuration


#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "CKFXCommConfiguration.hpp"

//---------------------------------------------------------------------------


using namespace stw::errors;
using namespace stw::scl;
using namespace stw::diag_lib;

//---------------------------------------------------------------------------

C_KFXCommParameter::C_KFXCommParameter(void) : c_Name(""), s64_MinValue(0), s64_MaxValue(0), s64_DefaultValue(0),
                                               c_DisplayName(""), c_Comment(""), c_INIDirective("")
{
}

//---------------------------------------------------------------------------

uint8_t C_KFXCommConfiguration::GetBaseID(void) const
{
   int64_t s64_Value;
   int32_t s32_Return;
   s32_Return = GetParameterValue("BASEID", s64_Value);
   if (s32_Return != C_NO_ERR)
   {
      //complain really hard ...
      throw "Error in usage of C_KFXCommConfiguration:\nTrying to use deprecated access to bBaseID for a protocol implementation that does not have this parameter !";
   }
   return static_cast<uint8_t>(s64_Value);
}

//---------------------------------------------------------------------------

void C_KFXCommConfiguration::SetBaseID(const uint8_t ou8_Value)
{
   int32_t s32_Return;
   s32_Return = SetParameterValue("BASEID", static_cast<int64_t>(ou8_Value));
   if (s32_Return != C_NO_ERR)
   {
      //complain really hard ...
      throw "Error in usage of C_KFXCommConfiguration:\nTrying to use deprecated access to bBaseID for a protocol implementation that does not have this parameter !";
   }
}

//---------------------------------------------------------------------------

uint8_t C_KFXCommConfiguration::GetClientAddress(void) const
{
   int64_t s64_Value;
   int32_t s32_Return;
   s32_Return = GetParameterValue("CLIENTADDRESS", s64_Value);
   if (s32_Return != C_NO_ERR)
   {
      //complain really hard ...
      throw "Error in usage of C_KFXCommConfiguration:\nTrying to use deprecated access to bClientAddress for a protocol implementation that does not have this parameter !";
   }
   return static_cast<uint8_t>(s64_Value);
}

//---------------------------------------------------------------------------

void C_KFXCommConfiguration::SetClientAddress(const uint8_t ou8_Value)
{
   int32_t s32_Return;
   s32_Return = SetParameterValue("CLIENTADDRESS", static_cast<int64_t>(ou8_Value));
   if (s32_Return != C_NO_ERR)
   {
      //complain really hard ...
      throw "Error in usage of C_KFXCommConfiguration:\nTrying to use deprecated access to bClientAddress for a protocol implementation that does not have this parameter !";
   }
}

//---------------------------------------------------------------------------

uint8_t C_KFXCommConfiguration::GetServerAddress(void) const
{
   int64_t s64_Value;
   int32_t s32_Return;
   s32_Return = GetParameterValue("SERVERADDRESS", s64_Value);
   if (s32_Return != C_NO_ERR)
   {
      //complain really hard ...
      throw "Error in usage of C_KFXCommConfiguration:\nTrying to use deprecated access to bClientAddress for a protocol implementation that does not have this parameter !";
   }
   return static_cast<uint8_t>(s64_Value);
}

//---------------------------------------------------------------------------

void C_KFXCommConfiguration::SetServerAddress(const uint8_t ou8_Value)
{
   int32_t s32_Return;
   s32_Return = SetParameterValue("SERVERADDRESS", static_cast<int64_t>(ou8_Value));
   if (s32_Return != C_NO_ERR)
   {
      //complain really hard ...
      throw "Error in usage of C_KFXCommConfiguration:\nTrying to use deprecated access to bServerAddress for a protocol implementation that does not have this parameter !";
   }
}

//---------------------------------------------------------------------------

uint16_t C_KFXCommConfiguration::GetTimeout(void) const
{
   int64_t s64_Value;
   int32_t s32_Return;
   s32_Return = GetParameterValue("TIMEOUT", s64_Value);
   if (s32_Return != C_NO_ERR)
   {
      //complain really hard ...
      throw "Error in usage of C_KFXCommConfiguration:\nTrying to use deprecated access to wTimeout for a protocol implementation that does not have this parameter !";
   }
   return static_cast<uint16_t>(s64_Value);
}

//---------------------------------------------------------------------------

void C_KFXCommConfiguration::SetTimeout(const uint16_t ou16_Value)
{
   int32_t s32_Return;
   s32_Return = SetParameterValue("TIMEOUT", static_cast<int64_t>(ou16_Value));
   if (s32_Return != C_NO_ERR)
   {
      //complain really hard ...
      throw "Error in usage of C_KFXCommConfiguration:\nTrying to use deprecated access to wTimeout for a protocol implementation that does not have this parameter !";
   }
}

//---------------------------------------------------------------------------

uint8_t C_KFXCommConfiguration::GetSTMin(void) const
{
   int64_t s64_Value;
   int32_t s32_Return;
   s32_Return = GetParameterValue("STMIN", s64_Value);
   if (s32_Return != C_NO_ERR)
   {
      //complain really hard ...
      throw "Error in usage of C_KFXCommConfiguration:\nTrying to use deprecated access to bSTMin for a protocol implementation that does not have this parameter !";
   }
   return static_cast<uint8_t>(s64_Value);
}

//---------------------------------------------------------------------------

void C_KFXCommConfiguration::SetSTMin(const uint8_t ou8_Value)
{
   int32_t s32_Return;
   s32_Return = SetParameterValue("STMIN", static_cast<int64_t>(ou8_Value));
   if (s32_Return != C_NO_ERR)
   {
      //complain really hard ...
      throw "Error in usage of C_KFXCommConfiguration:\nTrying to use deprecated access to bSTMin for a protocol implementation that does not have this parameter !";
   }
}

//---------------------------------------------------------------------------

uint8_t C_KFXCommConfiguration::GetBSMax(void) const
{
   int64_t s64_Value;
   int32_t s32_Return;
   s32_Return = GetParameterValue("BSMAX", s64_Value);
   if (s32_Return != C_NO_ERR)
   {
      //complain really hard ...
      throw "Error in usage of C_KFXCommConfiguration:\nTrying to use deprecated access to bBSMax for a protocol implementation that does not have this parameter !";
   }
   return static_cast<uint8_t>(s64_Value);
}

//---------------------------------------------------------------------------

void C_KFXCommConfiguration::SetBSMax(const uint8_t ou8_Value)
{
   int32_t s32_Return;
   s32_Return = SetParameterValue("BSMAX", static_cast<int64_t>(ou8_Value));
   if (s32_Return != C_NO_ERR)
   {
      //complain really hard ...
      throw "Error in usage of C_KFXCommConfiguration:\nTrying to use deprecated access to bBSMax for a protocol implementation that does not have this parameter !";
   }
}

//---------------------------------------------------------------------------

C_KFXCommConfigurationBase::C_KFXCommConfigurationBase(void)
{
   //default: stay compatible with existing applications -> use standard KEFEX protocol parameters:
   mc_Parameters.SetLength(6);
   mc_Parameters[0].c_Name          = "BASEID";
   mc_Parameters[0].s64_MinValue    = 0;
   mc_Parameters[0].s64_MaxValue    = 6;
   mc_Parameters[0].s64_DefaultValue= 6;
   mc_Parameters[0].c_DisplayName   = "Base ID";
   mc_Parameters[0].c_Comment       = "Base ID (0,2,4,6)";
   mc_Parameters[0].c_INIDirective  = "BASEID";

   mc_Parameters[1].c_Name          = "CLIENTADDRESS";
   mc_Parameters[1].s64_MinValue    = 0;
   mc_Parameters[1].s64_MaxValue    = 255;
   mc_Parameters[1].s64_DefaultValue= 100;
   mc_Parameters[1].c_DisplayName   = "Client address";
   mc_Parameters[1].c_Comment       = "Client address (Node ID of client)";
   mc_Parameters[1].c_INIDirective  = "CLIENTADDRESS";

   mc_Parameters[2].c_Name          = "SERVERADDRESS";
   mc_Parameters[2].s64_MinValue    = 0;
   mc_Parameters[2].s64_MaxValue    = 255;
   mc_Parameters[2].s64_DefaultValue= 0;
   mc_Parameters[2].c_DisplayName   = "Server address";
   mc_Parameters[2].c_Comment       = "Server address (Node ID of server)";
   mc_Parameters[2].c_INIDirective  = "SERVERADDRESS";

   mc_Parameters[3].c_Name          = "TIMEOUT";
   mc_Parameters[3].s64_MinValue    = 0;
   mc_Parameters[3].s64_MaxValue    = 65535;
   mc_Parameters[3].s64_DefaultValue= 500;
   mc_Parameters[3].c_DisplayName   = "Timeout (ms)";
   mc_Parameters[3].c_Comment       = "Timeout in milli-seconds";
   mc_Parameters[3].c_INIDirective  = "TIMEOUT";

   mc_Parameters[4].c_Name          = "STMIN";
   mc_Parameters[4].s64_MinValue    = 0;
   mc_Parameters[4].s64_MaxValue    = 255;
   mc_Parameters[4].s64_DefaultValue= 0;
   mc_Parameters[4].c_DisplayName   = "STMin (ms)";
   mc_Parameters[4].c_Comment       = "Minimum distance between 2 RX messages in milli-seconds";
   mc_Parameters[4].c_INIDirective  = "STMIN";

   mc_Parameters[5].c_Name          = "BSMAX";
   mc_Parameters[5].s64_MinValue    = 1;
   mc_Parameters[5].s64_MaxValue    = 255;
   mc_Parameters[5].s64_DefaultValue= 20;
   mc_Parameters[5].c_DisplayName   = "BSMax";
   mc_Parameters[5].c_Comment       = "Maximum number of RX messages without a handshake";
   mc_Parameters[5].c_INIDirective  = "BSMAX";

   mc_Values.SetLength(mc_Parameters.GetLength());
}

//---------------------------------------------------------------------------

C_KFXCommConfigurationBase::~C_KFXCommConfigurationBase(void)
{
}

//---------------------------------------------------------------------------

C_KFXCommConfigurationBase::C_KFXCommConfigurationBase(const C_KFXCommConfigurationBase & orc_Source)
{
   this->operator=(orc_Source);
}

//---------------------------------------------------------------------------

C_KFXCommConfigurationBase& C_KFXCommConfigurationBase::operator =(const C_KFXCommConfigurationBase & orc_Source)
{
   mc_Parameters   = orc_Source.mc_Parameters;
   mc_Values       = orc_Source.mc_Values;
   mc_ProtocolName = orc_Source.mc_ProtocolName;
   return (*this);
}

//---------------------------------------------------------------------------

C_KFXCommConfigurationBase::C_KFXCommConfigurationBase(const C_SclDynamicArray<C_KFXCommParameter> & orc_Parameters)
{
   this->SetParameterList(orc_Parameters);
}

//---------------------------------------------------------------------------

void C_KFXCommConfigurationBase::SetParameterList(const C_SclDynamicArray<C_KFXCommParameter> & orc_Parameters)
{
   int32_t s32_Index;
   mc_Parameters = orc_Parameters;
   mc_Values.SetLength(orc_Parameters.GetLength());
   for (s32_Index = 0; s32_Index < mc_Values.GetLength(); s32_Index++)
   {
      mc_Values[s32_Index] = 0L;
      //make uppercase as we always compare uppercase
      mc_Parameters[s32_Index].c_Name = mc_Parameters[s32_Index].c_Name.UpperCase();
   }
}

//---------------------------------------------------------------------------

int32_t C_KFXCommConfigurationBase::GetParameterValue(const C_SclString & orc_ParameterName,
                                                     int64_t & ors64_Value) const
{
   int32_t s32_Index;
   for (s32_Index = 0; s32_Index < mc_Parameters.GetLength(); s32_Index++)
   {
      if (mc_Parameters[s32_Index].c_Name == orc_ParameterName.UpperCase())
      {
         ors64_Value = mc_Values[s32_Index];
         return C_NO_ERR;
      }
   }
   return C_RANGE;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommConfigurationBase::SetParameterValue(const C_SclString & orc_ParameterName, const int64_t os64_Value)
{
   int32_t s32_Index;
   for (s32_Index = 0; s32_Index < mc_Parameters.GetLength(); s32_Index++)
   {
      if (mc_Parameters[s32_Index].c_Name == orc_ParameterName.UpperCase())
      {
         mc_Values[s32_Index] = os64_Value;
         return C_NO_ERR;
      }
   }
   return C_RANGE;
}

//---------------------------------------------------------------------------

void C_KFXCommConfigurationBase::SetAllDefaults(void)
{
   int32_t s32_Index;
   for (s32_Index = 0; s32_Index < mc_Parameters.GetLength(); s32_Index++)
   {
      mc_Values[s32_Index] = mc_Parameters[s32_Index].s64_DefaultValue;
   }
}

//---------------------------------------------------------------------------

int32_t C_KFXCommConfigurationBase::LoadConfigFromINI(C_SclIniFile & orc_File, const C_SclString & orc_Section)
{
   int32_t s32_Index;

   //Protocol Name will be set from the outside
   //Procedure:
   //- application checks: which protocol is configured in .def or .ksp file
   //- init corresponding protocol class
   //- load protocol parameters into class
   for (s32_Index = 0; s32_Index < mc_Parameters.GetLength(); s32_Index++)
   {
      mc_Values[s32_Index] = static_cast<int64_t>(orc_File.ReadInteger(orc_Section, mc_Parameters[s32_Index].c_INIDirective,
                                                       static_cast<int32_t>(mc_Parameters[s32_Index].s64_DefaultValue)));
   }
   return C_NO_ERR;
}

//---------------------------------------------------------------------------

int32_t C_KFXCommConfigurationBase::SaveConfigToINI(C_SclIniFile & orc_File, const C_SclString & orc_Section) const
{
   int32_t s32_Index;

   try
   {
      orc_File.WriteString(orc_Section, "COMM_PROTOCOL_TYPE", mc_ProtocolName);
      for (s32_Index = 0; s32_Index < mc_Parameters.GetLength(); s32_Index++)
      {
         orc_File.WriteInteger(orc_Section, mc_Parameters[s32_Index].c_INIDirective,
                               static_cast<int32_t>(mc_Values[s32_Index]));
      }
   }
   catch (...)
   {
      return C_NOACT;
   }
   return C_NO_ERR;
}

//--------------------------------------------------------------

C_KFXCommConfiguration& C_KFXCommConfiguration::operator =(const C_KFXCommConfiguration & orc_Source)
{
   C_KFXCommConfigurationBase::operator =(orc_Source);
   return (*this);
}

//--------------------------------------------------------------


