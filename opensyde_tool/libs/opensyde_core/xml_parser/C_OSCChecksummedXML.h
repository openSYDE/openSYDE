//-----------------------------------------------------------------------------
/*!
   \file
   \brief      Parse / write XML file with CRC protection

   \class      stw_opensyde_core::C_OSCChecksummedXML
   \brief      Parse / write XML file with CRC protection

   Class to parse and write XML files with added CRC protection.

   Use-case: Readable files that can not be accidentally changed manually.

   The following elements are calculated into the CRC:
   - names and values of all nodes
   - names and values of all attributes
   - absolute index of the node (to prevent a potentially theoretical constellation where moving a node in the hierarchy
                                 might not be detected)

   The following elements are not calculated into the CRC:
   - XML header tag
   - value of the CRC attribute and name

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     14.07.2016  STW/A.Stangl (refactored from existing CDLSecureXML)
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSCCHECKSUMMEDXMLH
#define C_OSCCHECKSUMMEDXMLH

/* -- Includes ------------------------------------------------------------- */
#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCXMLParser.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCChecksummedXML :
   public C_OSCXMLParser
{
private:
   stw_types::uint16 mu16_CRCDepth;

   void m_CalcXMLCRCNode(stw_types::uint16 & oru16_CRC);
   stw_types::uint16 m_CalcXMLCRC(void);

public:
   C_OSCChecksummedXML(void);

   virtual stw_types::sint32 LoadFromFile(const stw_scl::C_SCLString & orc_FileName);
   virtual stw_types::sint32 SaveToFile(const stw_scl::C_SCLString & orc_FileName);
};

/* -- Extern Global Variables ---------------------------------------------- */
}

#endif
