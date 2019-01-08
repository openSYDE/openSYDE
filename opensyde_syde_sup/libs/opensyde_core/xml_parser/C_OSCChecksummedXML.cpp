//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Parse / write XML file with CRC protection

   see .h file header for details

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     14.07.2016  STW/A.Stangl (refactored from existing CDLSecureXML)
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h" //pre-compiled headers
#include "stwtypes.h"
#include "stwerrors.h"
#include "C_OSCChecksummedXML.h"

#include "CSCLString.h"
#include "CSCLChecksums.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_errors;
using namespace stw_scl;
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */
static const C_SCLString mc_NAME_CRC_ATTRIBUTE = "file_crc";

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief  Constructor

   Set up class

   \created     19.09.2014  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_OSCChecksummedXML::C_OSCChecksummedXML(void) :
   C_OSCXMLParser(),
   mu16_CRCDepth(0U)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief  Open XML data from file

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

   \created     25.01.2018  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_OSCChecksummedXML::LoadFromFile(const C_SCLString & orc_FileName)
{
   sint32 s32_Return;
   C_SCLString c_Text;
   uint16 u16_CRCCalc;
   uint16 u16_CRCFromFile;

   s32_Return = C_OSCXMLParser::LoadFromFile(orc_FileName);
   if (s32_Return == C_NO_ERR)
   {
      c_Text = this->SelectRoot();
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
            u16_CRCFromFile = static_cast<uint16>(this->GetAttributeUint32(mc_NAME_CRC_ATTRIBUTE));
            u16_CRCCalc = this->m_CalcXMLCRC();

            this->SelectRoot(); //be defensive: set defined start state
            s32_Return = (u16_CRCCalc == u16_CRCFromFile) ? C_NO_ERR : C_CHECKSUM;
         }
      }
   }

   return s32_Return;
}

//-----------------------------------------------------------------------------
/*!
   \brief  Write XML data to file

   Update CRC and write data to file.
   After calculation the checksum value is written to the attribute "c_NAME_CRC_ATTRIBUTE" of the root name.
   A pre-existing file will be replaced.

   Will set the active node to "root".

   \param[in]   orc_FileName   path to XML file to write to

   \return
   C_NO_ERR   data was written to file
   C_NOACT    could not write data from file

   \created     07.09.2016  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
sint32 C_OSCChecksummedXML::SaveToFile(const C_SCLString & orc_FileName)
{
   sint32 s32_Return;
   const uint16 u16_CRCCalc = this->m_CalcXMLCRC();

   if (this->SelectRoot() == "")
   {
      s32_Return = C_NOACT;
   }
   else
   {
      this->SetAttributeString(mc_NAME_CRC_ATTRIBUTE, "0x" + C_SCLString::IntToHex(u16_CRCCalc, 4));

      s32_Return = C_OSCXMLParser::SaveToFile(orc_FileName);
   }
   return s32_Return;
}

//-----------------------------------------------------------------------------

void C_OSCChecksummedXML::m_CalcXMLCRCNode(uint16 & oru16_CRC)
{
   std::vector<C_OSCXMLAttribute> c_Attributes;
   C_SCLString c_NodeLv1;
   C_SCLString c_Text;
   c_NodeLv1 = this->SelectNodeChild();

   mu16_CRCDepth++;
   C_SCLChecksums::CalcCRC16(&mu16_CRCDepth, 2U, oru16_CRC);
   while (c_NodeLv1 != "")
   {
      C_SCLChecksums::CalcCRC16(c_NodeLv1.c_str(), c_NodeLv1.Length(), oru16_CRC);
      c_Attributes = this->GetAttributes();
      for (uint32 u32_Index = 0U; u32_Index < c_Attributes.size(); u32_Index++)
      {
         C_SCLChecksums::CalcCRC16(c_Attributes[u32_Index].c_Name.c_str(),
                                   c_Attributes[u32_Index].c_Name.Length(), oru16_CRC);
         C_SCLChecksums::CalcCRC16(c_Attributes[u32_Index].c_Value.c_str(),
                                   c_Attributes[u32_Index].c_Value.Length(), oru16_CRC);
      }
      c_Text = this->GetNodeContent();
      C_SCLChecksums::CalcCRC16(c_Text.c_str(), c_Text.Length(), oru16_CRC);

      //sub-nodes ?
      m_CalcXMLCRCNode(oru16_CRC);

      c_NodeLv1 = this->SelectNodeNext();
      if (c_NodeLv1 == "")
      {
         //go back to parent, we are finished here ...
         this->SelectNodeParent();
      }
   }
}

//-----------------------------------------------------------------------------

uint16 C_OSCChecksummedXML::m_CalcXMLCRC(void)
{
   std::vector<C_OSCXMLAttribute> c_Attributes;
   C_SCLString c_Text;
   uint16 u16_CRC = 0x1D0FU; //set CCITT25 start value

   c_Text = this->SelectRoot();

   C_SCLChecksums::CalcCRC16(c_Text.c_str(), c_Text.Length(), u16_CRC);
   c_Attributes = this->GetAttributes();
   for (uint32 u32_Index = 0U; u32_Index < c_Attributes.size(); u32_Index++)
   {
      if (c_Attributes[u32_Index].c_Name != mc_NAME_CRC_ATTRIBUTE) //skip CRC value
      {
         C_SCLChecksums::CalcCRC16(c_Attributes[u32_Index].c_Name.c_str(),
                                   c_Attributes[u32_Index].c_Name.Length(), u16_CRC);
         C_SCLChecksums::CalcCRC16(c_Attributes[u32_Index].c_Value.c_str(),
                                   c_Attributes[u32_Index].c_Value.Length(), u16_CRC);
      }
   }

   mu16_CRCDepth = 1U;
   c_Text = this->GetNodeContent();
   C_SCLChecksums::CalcCRC16(c_Text.c_str(), c_Text.Length(), u16_CRC);
   C_SCLChecksums::CalcCRC16(&mu16_CRCDepth, 2U, u16_CRC);

   //subnodes:
   m_CalcXMLCRCNode(u16_CRC);

   return u16_CRC;
}

//-----------------------------------------------------------------------------
