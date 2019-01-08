//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Manager for all device descriptions (header)

   Manager for all device descriptions (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     19.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OSCDEVICEMANAGERH
#define C_OSCDEVICEMANAGERH

/* -- Includes ------------------------------------------------------------- */

#include "CSCLString.h"
#include "C_OSCDeviceGroup.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCDeviceManager
{
public:
   const C_OSCDeviceDefinition * LookForDevice(const stw_scl::C_SCLString & orc_Name) const;

   std::vector<C_OSCDeviceGroup> GetDeviceGroups(void) const;

   bool WasLoaded(void) const;
   stw_types::sint32 LoadFromFile(const stw_scl::C_SCLString & orc_File);

   C_OSCDeviceManager(void);

private:
   bool mq_WasLoaded;
   std::vector<C_OSCDeviceGroup> mc_DeviceGroups; ///< Array of all known device groups
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
