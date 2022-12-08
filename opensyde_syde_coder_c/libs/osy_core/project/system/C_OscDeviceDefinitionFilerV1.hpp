//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       File handler for device definition file data (V1)
   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCDEVICEDEFINITIONFILERV1_H
#define C_OSCDEVICEDEFINITIONFILERV1_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_OscDeviceDefinition.hpp"
#include "C_OscXmlParser.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscDeviceDefinitionFilerV1
{
private:
   static const uint16_t mhu16_FILE_VERSION; ///< file version this class can read / write

   static void mh_ParseOpenSydeAvailability(const C_OscXmlParser & orc_Parser, bool & orq_ProtocolSupportedCan,
                                            bool & orq_ProtocolSupportedEthernet);
   static void mh_ParseOpenSydeFlashloaderParameter(const C_OscXmlParser & orc_Parser,
                                                    uint32_t & oru32_RequestDownloadTimeout,
                                                    uint32_t & oru32_TransferDataTimeout, bool & orq_IsFileBased);
   static void mh_ParseStwFlashloaderAvailability(const C_OscXmlParser & orc_Parser, bool & orq_ProtocolSupportedCan);

public:
   static int32_t h_Load(C_OscDeviceDefinition & orc_DeviceDefinition, C_OscXmlParser & orc_Parser,
                         const stw::scl::C_SclString & orc_Path);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
