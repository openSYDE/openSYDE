//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Checksummed INI file class

   \class       stw::opensyde_core::C_OscChecksummedIniFile
   \brief       Checksummed INI file class

   Expands the C_SclIniFile class by a checksum functionality to prevent manual changes in the INI file.
   Note: this is not a very secure method as this class can be used by another programmer to manually update the
         checksum and then use the file. So it's just intended to prevent accidental manual changes.

   Checking and updating the checksum is optional for an application.
   The contents of the INI file can be accessed at any time using the base class Read/Write functions.

   Typical application workflow:
   ~~~{.cpp}
   C_OscChecksummedIniFile c_File("myfile.ini"); //open file
   bool q_Return = c_File.CheckCheckSum(); //is the checksum correct ...
   [...] work with ini file using the base classes functions
   c_File.UpdateCheckSum();  //recalculate checksum (will be written back to file when life time of c_File ends)
   ~~~

   The class supports two "versions" of the checksum.
   Reason:
    In "V1" of the checksum it was always calculated over not only one section, but also all the previous sections
     again. This causes rather long parse times.
    We can't change this approach as all old files would become invalid.
   Solution:
    When writing the checksum we always write 2 versions:
    - old (slow) + new (fast) + version-number
    So newer application can use the faster reading method and still be compatible with old files.
    Older applications still can handle the files.
    Writing will stay slow, but this is usually not done very often.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCCHECKSUMMEDINIFILE_HPP
#define C_OSCCHECKSUMMEDINIFILE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclIniFile.hpp"
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscChecksummedIniFile :
   public stw::scl::C_SclIniFile
{
private:
   uint16_t m_CalcCheckSum(const bool oq_Fast);

public:
   bool CheckCheckSum(void);
   void UpdateCheckSum(void);

   C_OscChecksummedIniFile(const stw::scl::C_SclString & orc_FileName);
   virtual ~C_OscChecksummedIniFile(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
