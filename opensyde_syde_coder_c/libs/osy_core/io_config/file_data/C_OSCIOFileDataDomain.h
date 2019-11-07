//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       IO description domain
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCIOFILEDATADOMAIN_H
#define C_OSCIOFILEDATADOMAIN_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "CSCLString.h"
#include "C_OSCIOFileDataChannelUseCase.h"
#include "C_OSCIOFileDataStruct.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCIOFileDataDomain
{
public:
   C_OSCIOFileDataDomain(void);
   virtual ~C_OSCIOFileDataDomain(void);

   stw_scl::C_SCLString c_Id;                                    ///< Unique ID for later reference of this domain
   stw_scl::C_SCLString c_Name;                                  ///< Displayable plural name of domain
   stw_scl::C_SCLString c_SingularName;                          ///< Displayable singular name of domain
   std::vector<stw_scl::C_SCLString> c_Channels;                 ///< All available channels for this domain
   std::vector<C_OSCIOFileDataChannelUseCase> c_ChannelUseCases; ///< All available channel use-cases or this domain
   std::vector<C_OSCIOFileDataStruct> c_Parameters;              ///< All available parameters for this domain
   std::vector<C_OSCIOFileDataStruct> c_InputValues;             ///< All available inputs for this domain
   std::vector<C_OSCIOFileDataStruct> c_OutputValues;            ///< All available outputs for this domain
   std::vector<C_OSCIOFileDataStruct> c_StatusValues;            ///< All available status values for this domain
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
