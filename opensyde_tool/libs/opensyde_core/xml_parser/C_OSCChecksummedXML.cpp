//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Parse / write XML file with CRC protection

   see .h file header for details

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp" //pre-compiled headers
#include "stwtypes.hpp"
#include "stwerrors.hpp"
#include "C_OscChecksummedXml.hpp"

#include "C_SclString.hpp"
#include "C_SclChecksums.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::scl;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
static const C_SclString mc_NAME_CRC_ATTRIBUTE = "file_crc";

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Constructor

   Set up class
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscChecksummedXml::C_OscChecksummedXml(void) :
   C_OscXmlParser(),
   mu16_CrcDepth(0U)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Open XML data from file

   Open XML file.
   If the file could not be opened the function will return an error and prepare an empty XML structure.
   After the data was opened the function will check the CRC

   The checksum value is expected in the attribute "c_NAME_CRC_ATTRIBUTE" of the root name.

   \param[in]   orc_FileName   path to XML file to open

   \return
   C_NO_ERR    data was read from file
   C_NOACT     could not load from file; invalid XML
   C_RD_WR     file was read but checksum entry not found at defined position
   C_CHECKSUM  data was read but CRC is not correct
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscChecksummedXml::LoadFromFile(const C_SclString & orc_FileName)
{
   int32_t s32_Return;

   s32_Return = C_OscXmlParser::LoadFromFile(orc_FileName);
   if (s32_Return == C_NO_ERR)
   {
      const C_SclString c_Text = this->SelectRoot();
      if (c_Text == "")
      {
         s32_Return = C_RD_WR;
      }
      else
      {
         if (this->AttributeExists(mc_NAME_CRC_ATTRIBUTE) == false)
         {
            s32_Return = C_RD_WR;
         }
         else
         {
            const uint16_t u16_CrcFromFile = static_cast<uint16_t>(this->GetAttributeUint32(mc_NAME_CRC_ATTRIBUTE));
            const uint16_t u16_CrcCalc = this->m_CalcXmlCrc();

            this->SelectRoot(); //be defensive: set defined start state
            s32_Return = (u16_CrcCalc == u16_CrcFromFile) ? C_NO_ERR : C_CHECKSUM;
         }
      }
   }

   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Write XML data to file

   Update CRC and write data to file.
   After calculation the checksum value is written to the attribute "c_NAME_CRC_ATTRIBUTE" of the root name.
   A pre-existing file will be replaced.

   Will set the active node to "root".

   \param[in]   orc_FileName   path to XML file to write to

   \return
   C_NO_ERR   data was written to file
   C_NOACT    could not write data from file
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_OscChecksummedXml::SaveToFile(const C_SclString & orc_FileName)
{
   int32_t s32_Return;
   const uint16_t u16_CrcCalc = this->m_CalcXmlCrc();

   if (this->SelectRoot() == "")
   {
      s32_Return = C_NOACT;
   }
   else
   {
      this->SetAttributeString(mc_NAME_CRC_ATTRIBUTE, "0x" + C_SclString::IntToHex(u16_CrcCalc, 4));

      s32_Return = C_OscXmlParser::SaveToFile(orc_FileName);
   }
   return s32_Return;
}

//----------------------------------------------------------------------------------------------------------------------

void C_OscChecksummedXml::m_CalcXmlCrcNode(uint16_t & oru16_Crc)
{
   std::vector<C_OscXmlAttribute> c_Attributes;
   C_SclString c_NodeLv1;
   C_SclString c_Text;
   c_NodeLv1 = this->SelectNodeChild();

   mu16_CrcDepth++;
   C_SclChecksums::CalcCRC16(&mu16_CrcDepth, 2U, oru16_Crc);
   while (c_NodeLv1 != "")
   {
      C_SclChecksums::CalcCRC16(c_NodeLv1.c_str(), c_NodeLv1.Length(), oru16_Crc);
      c_Attributes = this->GetAttributes();
      for (uint32_t u32_Index = 0U; u32_Index < c_Attributes.size(); u32_Index++)
      {
         C_SclChecksums::CalcCRC16(c_Attributes[u32_Index].c_Name.c_str(),
                                   c_Attributes[u32_Index].c_Name.Length(), oru16_Crc);
         C_SclChecksums::CalcCRC16(c_Attributes[u32_Index].c_Value.c_str(),
                                   c_Attributes[u32_Index].c_Value.Length(), oru16_Crc);
      }
      c_Text = this->GetNodeContent();
      C_SclChecksums::CalcCRC16(c_Text.c_str(), c_Text.Length(), oru16_Crc);

      //sub-nodes ?
      m_CalcXmlCrcNode(oru16_Crc);

      c_NodeLv1 = this->SelectNodeNext();
      if (c_NodeLv1 == "")
      {
         //go back to parent, we are finished here ...
         this->SelectNodeParent();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------

uint16_t C_OscChecksummedXml::m_CalcXmlCrc(void)
{
   std::vector<C_OscXmlAttribute> c_Attributes;
   C_SclString c_Text;
   uint16_t u16_Crc = 0x1D0FU; //set CCITT25 start value

   c_Text = this->SelectRoot();

   C_SclChecksums::CalcCRC16(c_Text.c_str(), c_Text.Length(), u16_Crc);
   c_Attributes = this->GetAttributes();
   for (uint32_t u32_Index = 0U; u32_Index < c_Attributes.size(); u32_Index++)
   {
      if (c_Attributes[u32_Index].c_Name != mc_NAME_CRC_ATTRIBUTE) //skip CRC value
      {
         C_SclChecksums::CalcCRC16(c_Attributes[u32_Index].c_Name.c_str(),
                                   c_Attributes[u32_Index].c_Name.Length(), u16_Crc);
         C_SclChecksums::CalcCRC16(c_Attributes[u32_Index].c_Value.c_str(),
                                   c_Attributes[u32_Index].c_Value.Length(), u16_Crc);
      }
   }

   mu16_CrcDepth = 1U;
   c_Text = this->GetNodeContent();
   C_SclChecksums::CalcCRC16(c_Text.c_str(), c_Text.Length(), u16_Crc);
   C_SclChecksums::CalcCRC16(&mu16_CrcDepth, 2U, u16_Crc);

   //subnodes:
   m_CalcXmlCrcNode(u16_Crc);

   return u16_Crc;
}

//----------------------------------------------------------------------------------------------------------------------
