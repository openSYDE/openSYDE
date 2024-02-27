//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Button with menu for selecting path variables
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMOGEPUBPATHVARIABLES_HPP
#define C_CAMOGEPUBPATHVARIABLES_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_CamOgePubPathVariablesBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamOgePubPathVariables :
   public C_CamOgePubPathVariablesBase
{
   Q_OBJECT

public:
   explicit C_CamOgePubPathVariables(QWidget * const opc_Parent = NULL);

private:
   //Avoid call
   C_CamOgePubPathVariables(const C_CamOgePubPathVariables &);
   C_CamOgePubPathVariables & operator =(const C_CamOgePubPathVariables &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
