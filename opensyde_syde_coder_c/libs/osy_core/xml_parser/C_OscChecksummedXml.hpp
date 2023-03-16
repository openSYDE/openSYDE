//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief      Parse / write XML file with CRC protection

   \class      stw::opensyde_core::C_OscChecksummedXML
   \brief      Parse / write XML file with CRC protection

   Class to parse and write XML files with added CRC protection.

   Use-case: Readable files that can not be accidentally changed manually.

   The following elements are calculated into the CRC:
   - names and values of all nodes
   - names and values of all attributes
   - absolute index of the node (to prevent a potential situation where moving a node in the hierarchy might not be
                                 detected)

   The following elements are not calculated into the CRC:
   - XML header tag
   - value of the CRC attribute and name

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCHECKSUMMEDXMLHPP
#define C_OSCCHECKSUMMEDXMLHPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_OscXmlParser.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscChecksummedXml :
   public C_OscXmlParser
{
private:
   uint16_t mu16_CrcDepth;

   void m_CalcXmlCrcNode(uint16_t & oru16_Crc);
   uint16_t m_CalcXmlCrc(void);

public:
   C_OscChecksummedXml(void);

   virtual int32_t LoadFromFile(const stw::scl::C_SclString & orc_FileName);
   virtual int32_t SaveToFile(const stw::scl::C_SclString & orc_FileName);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
