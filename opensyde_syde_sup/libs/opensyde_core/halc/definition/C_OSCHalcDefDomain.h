//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC definition domain
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCHALCDEFDOMAIN_H
#define C_OSCHALCDEFDOMAIN_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "CSCLString.h"
#include "C_OSCHalcDefChannelUseCase.h"
#include "C_OSCHalcDefStruct.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCHalcDefDomain
{
public:
   enum E_VariableSelector
   {
      eVA_PARAM,
      eVA_INPUT,
      eVA_OUTPUT,
      eVA_STATUS
   };

   C_OSCHalcDefDomain(void);
   virtual ~C_OSCHalcDefDomain(void);

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   stw_scl::C_SCLString c_Id;                                 ///< Unique ID for later reference of this domain
   stw_scl::C_SCLString c_Name;                               ///< Displayable plural name of domain
   stw_scl::C_SCLString c_SingularName;                       ///< Displayable singular name of domain
   std::vector<stw_scl::C_SCLString> c_Channels;              ///< All available channels for this domain
   std::vector<C_OSCHalcDefChannelUseCase> c_ChannelUseCases; ///< All available channel use-cases for this domain
   std::vector<C_OSCHalcDefStruct> c_Parameters;              ///< All available parameters for this domain
   std::vector<C_OSCHalcDefStruct> c_InputValues;             ///< All available inputs for this domain
   std::vector<C_OSCHalcDefStruct> c_OutputValues;            ///< All available outputs for this domain
   std::vector<C_OSCHalcDefStruct> c_StatusValues;            ///< All available status values for this domain
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
