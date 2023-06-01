//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       CAN message to text conversion base class

   Provides basic functionality useful for all kinds of protocol interpreters.

   \copyright   Copyright 2010 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#include "precomp_headers.hpp" //pre-compiled headers

#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_CanMonProtocolBase.hpp"

#include "C_SclString.hpp"

//---------------------------------------------------------------------------

using namespace stw::errors;
using namespace stw::cmon_protocol;
using namespace stw::scl;

//-----------------------------------------------------------------------------
/*!
   \brief    save protocol specific parameters to INI file

   Nothing to save by default.
   Overload if we have something to save (use protocol abbreviation as prefix for directives).

   \param[in,out]   orc_IniFile  opened INI file to write to
   \param[in]       orc_Section  section in INI file to write to

   \return
   C_NO_ERR  -> data added to INI file (or: nothing to save)
   else      -> error writing data
*/
//-----------------------------------------------------------------------------
int32_t C_CanMonProtocolBase::SaveParamsToIni(C_SclIniFile & orc_IniFile, const C_SclString & orc_Section)
{
   (void)orc_IniFile;
   (void)orc_Section;
   return C_NO_ERR; //nothing to save by default
}

//-----------------------------------------------------------------------------
/*!
   \brief    load protocol specific parameters from INI file

   Nothing to load by default.
   Overload if we have something to load (use protocol abbreviation as prefix for directives).

   \param[in,out]   orc_IniFile  opened INI file to read from
   \param[in]       orc_Section  section in INI file to read from

   \return
   C_NO_ERR  -> data read from INI file (or: nothing to read)
   else      -> error reading data
*/
//-----------------------------------------------------------------------------
int32_t C_CanMonProtocolBase::LoadParamsFromIni(C_SclIniFile & orc_IniFile, const C_SclString & orc_Section)
{
   (void)orc_IniFile;
   (void)orc_Section;
   return C_NO_ERR; //nothing to load by default
}

//-----------------------------------------------------------------------------
/*!
   \brief    convert 2 bytes to uint16 (interprets data in little endian order)

   \param[in]       oau8_Bytes   data to convert

   \return
   extracted value
*/
//-----------------------------------------------------------------------------
uint16_t C_CanMonProtocolBase::mh_BytesToWordLowHigh(const uint8_t oau8_Bytes[2])
{
   return (oau8_Bytes[0] + (static_cast<uint16_t>((static_cast<uint16_t>(oau8_Bytes[1])) << 8U)));
}

//-----------------------------------------------------------------------------
/*!
   \brief    convert 2 bytes to uint16 (interprets data in big endian order)

   \param[in]       oau8_Bytes   data to convert

   \return
   extracted value
*/
//-----------------------------------------------------------------------------
uint16_t C_CanMonProtocolBase::mh_BytesToWordHighLow(const uint8_t oau8_Bytes[2])
{
   return (oau8_Bytes[1] + (static_cast<uint16_t>(static_cast<uint16_t>(oau8_Bytes[0]) << 8U)));
}

//-----------------------------------------------------------------------------
/*!
   \brief    convert 4 bytes to uint32 (interprets data in little endian order)

   \param[in]       oau8_Bytes   data to convert

   \return
   extracted value
*/
//-----------------------------------------------------------------------------
uint32_t C_CanMonProtocolBase::mh_BytesToDwordLowHigh(const uint8_t oau8_Bytes[4])
{
   return static_cast<uint32_t>(oau8_Bytes[0] +
                                ((static_cast<uint32_t>(oau8_Bytes[1])) << 8U) +
                                ((static_cast<uint32_t>(oau8_Bytes[2])) << 16U) +
                                ((static_cast<uint32_t>(oau8_Bytes[3])) << 24U));
}

//-----------------------------------------------------------------------------
/*!
   \brief    convert 4 bytes to uint32 (interprets data in big endian order)

   \param[in]       oau8_Bytes   data to convert

   \return
   extracted value
*/
//-----------------------------------------------------------------------------
uint32_t C_CanMonProtocolBase::mh_BytesToDwordHighLow(const uint8_t oau8_Bytes[4])
{
   return static_cast<uint32_t>(oau8_Bytes[3] +
                                ((static_cast<uint32_t>(oau8_Bytes[2])) << 8U) +
                                ((static_cast<uint32_t>(oau8_Bytes[1])) << 16U) +
                                ((static_cast<uint32_t>(oau8_Bytes[0])) << 24U));
}

//-----------------------------------------------------------------------------
/*!
   \brief    convert value to string

   Converts passed value to a string.
   If "mq_Decimal" is true the result will be a decimal string representation of the value.
   Otherwise it will be a hex string representation of the value.
   No leading zeros will be inserted.

   \param[in]       ou32_Value   value to convert

   \return
   value in string format
*/
//-----------------------------------------------------------------------------
C_SclString C_CanMonProtocolBase::m_GetValueDecHex(const uint32_t ou32_Value) const
{
   C_SclString c_Text;

   if (mq_Decimal == true)
   {
      (void)c_Text.PrintFormatted("%u", ou32_Value);
   }
   else
   {
      (void)c_Text.PrintFormatted("%X", ou32_Value);
   }
   return c_Text;
}

//-----------------------------------------------------------------------------
/*!
   \brief    convert value to string

   Converts passed value to a string.
   If "mq_Decimal" is true the result will be a decimal string representation of the value.
   Otherwise it will be a hex string representation of the value.

   Decimal values will be filled with leading zeros to have a total length of 5 characters.
   Hex values will be prepended by a blank, then be filled with leading zeros to have a total length of 5 characters.

   \param[in]       ou16_Value   value to convert

   \return
   value in string format
*/
//-----------------------------------------------------------------------------
C_SclString C_CanMonProtocolBase::m_GetWordAsStringFormat(const uint16_t ou16_Value) const
{
   C_SclString c_Help;

   if (mq_Decimal == true)
   {
      (void)c_Help.PrintFormatted("%05d", ou16_Value);
   }
   else
   {
      (void)c_Help.PrintFormatted(" %04X", ou16_Value);
   }
   return c_Help;
}

//-----------------------------------------------------------------------------
/*!
   \brief    convert value to string

   Converts passed value to a string.
   If "mq_Decimal" is true the result will be a decimal string representation of the value.
   Otherwise it will be a hex string representation of the value.

   Decimal values will be filled with leading zeros to have a total length of 3 characters.
   Hex values will be prepended by a blank, then be filled with leading zeros to have a total length of 3 characters.

   \param[in]       ou8_Value   value to convert

   \return
   value in string format
*/
//-----------------------------------------------------------------------------
C_SclString C_CanMonProtocolBase::m_GetByteAsStringFormat(const uint8_t ou8_Value) const
{
   C_SclString c_Help;

   if (mq_Decimal == true)
   {
      (void)c_Help.PrintFormatted("%03d", ou8_Value);
   }
   else
   {
      (void)c_Help.PrintFormatted(" %02X", ou8_Value);
   }
   return c_Help;
}

//---------------------------------------------------------------------------

void C_CanMonProtocolBase::SetDecimal(const bool oq_Decimal)
{
   mq_Decimal = oq_Decimal;
}

//---------------------------------------------------------------------------

C_CanMonProtocolBase::C_CanMonProtocolBase(void) :
   mq_Decimal(false)
{
}

//---------------------------------------------------------------------------

C_CanMonProtocolBase::~C_CanMonProtocolBase(void)
{
   //nothing to do by default
}

//---------------------------------------------------------------------------

C_CanMonProtocolKefexIva::C_CanMonProtocolKefexIva(void) :
   C_CanMonProtocolBase()
{
#ifdef CMONPROTOCOL_ALLOW_RAMVIEW_PROJECT_MAPPING
   mpc_KfxLists = NULL;
#endif
   mu16_KfxListOffset = 0;
}

//---------------------------------------------------------------------------

#ifdef CMONPROTOCOL_ALLOW_RAMVIEW_PROJECT_MAPPING
void C_CMONProtocolKEFEX_IVA::SetVariableInfo(const stw::diag_lib::C_KFXVariableLists * const opc_Lists,
                                              const uint16 ou16_ListOffset)
{
   mpc_KfxLists       = opc_Lists;
   mu16_KfxListOffset = ou16_ListOffset;
}
#endif

//---------------------------------------------------------------------------

C_SclString C_CanMonProtocolKefexIva::m_KfxTextAndIndexToString(const char_t * const opcn_Text,
                                                                const uint16_t ou16_Index) const
{
   return (static_cast<C_SclString>(opcn_Text) + " " + m_KfxIndexToString(ou16_Index));
}

//---------------------------------------------------------------------------

C_SclString C_CanMonProtocolKefexIva::m_KfxIndexToString(const uint16_t ou16_Index, const bool oq_IsKefexVarIndex) const
{
   C_SclString c_Help = "";

#ifdef CMONPROTOCOL_ALLOW_RAMVIEW_PROJECT_MAPPING
   uint16 u16_List;
   uint16 u16_Variable;
   if (oq_IsKefexVarIndex == true)
   {
      if (mpc_KfxLists != NULL)
      {
         stw::diag_lib::C_KFXVariableLists::UnpackIndex(ou16_Index, &u16_List, &u16_Variable);
         u16_List -= mu16_KfxListOffset;
         if (mpc_KfxLists->VariableExists(u16_List, u16_Variable) == true)
         {
            c_Help = "VAR " + mpc_KfxLists->operator [](u16_List).c_ListName + "." +
                     mpc_KfxLists->operator [](u16_List).VariableList[u16_Variable].c_Name;
         }
      }
   }
#else
   (void)oq_IsKefexVarIndex;
#endif

   if (c_Help == "")
   {
      if (mq_Decimal == true)
      {
         (void)c_Help.PrintFormatted("INDEX %d", ou16_Index);
      }
      else
      {
         (void)c_Help.PrintFormatted("INDEX %X", ou16_Index);
      }
   }
   return c_Help;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Convert serial number array to string

   Support of two serial number formats:
      1: format up to and including 2019. E.g: 05.123456.1001
      2: format from 2020. E.g: 200012345678

   \param[in]       opu8_SerialNumber     Pointer to first of six serial number array elements

   \return
   serial number string
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_CanMonProtocolBase::mh_SerialNumberToString(const uint8_t * const opu8_SerialNumber)
{
   C_SclString c_Result;

   if (opu8_SerialNumber != NULL)
   {
      if (opu8_SerialNumber[0] < static_cast<uint8_t>(0x20))
      {
         //format up to and including 2019. E.g: 05.123456.1001
         c_Result.PrintFormatted("%02X.%02X%02X%02X.%02X%02X",
                                 opu8_SerialNumber[0], opu8_SerialNumber[1], opu8_SerialNumber[2], opu8_SerialNumber[3],
                                 opu8_SerialNumber[4], opu8_SerialNumber[5]);
      }
      else
      {
         //format from 2020. E.g: 200012345678
         c_Result.PrintFormatted("%02X%02X%02X%02X%02X%02X",
                                 opu8_SerialNumber[0], opu8_SerialNumber[1], opu8_SerialNumber[2], opu8_SerialNumber[3],
                                 opu8_SerialNumber[4], opu8_SerialNumber[5]);
      }
   }

   return c_Result;
}

//---------------------------------------------------------------------------
