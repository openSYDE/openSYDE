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
#include "C_SclString.hpp"
#include "C_OscHalcDefChannelDef.hpp"
#include "C_OscHalcDefChannelValues.hpp"
#include "C_OscHalcDefChannelUseCase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscHalcDefDomain
{
public:
   enum E_VariableSelector
   {
      eVA_PARAM,
      eVA_INPUT,
      eVA_OUTPUT,
      eVA_STATUS
   };

   enum E_Category
   {
      eCA_INPUT,
      eCA_OUTPUT,
      eCA_OTHER
   };

   C_OscHalcDefDomain(void);
   virtual ~C_OscHalcDefDomain(void);

   virtual void CalcHash(uint32_t & oru32_HashValue) const;

   stw::scl::C_SclString c_Id;                                ///< Unique ID for later reference of this domain
   stw::scl::C_SclString c_Name;                              ///< Displayable plural name of domain
   stw::scl::C_SclString c_SingularName;                      ///< Displayable singular name of domain
   stw::scl::C_SclString c_Comment;                           ///< Description to display to user
   std::vector<C_OscHalcDefChannelDef> c_Channels;            ///< All available channels for this domain
   std::vector<C_OscHalcDefChannelUseCase> c_ChannelUseCases; ///< All available channel use-cases for this domain
   C_OscHalcDefChannelValues c_DomainValues;                  ///< All available values for the domain
   C_OscHalcDefChannelValues c_ChannelValues;                 ///< All available values for each channel
   E_Category e_Category;                                     ///< Optional category
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
