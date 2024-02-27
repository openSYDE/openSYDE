//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       SydeFlash variant of path variables push button widget
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_FLAOGEPUBPATHVARIABLES_HPP
#define C_FLAOGEPUBPATHVARIABLES_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_CamOgePubPathVariablesBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_FlaOgePubPathVariables :
   public C_CamOgePubPathVariablesBase
{
   Q_OBJECT

public:
   explicit C_FlaOgePubPathVariables(QWidget * const opc_Parent = NULL);

private:
   //Avoid call
   C_FlaOgePubPathVariables(const C_FlaOgePubPathVariables &);
   C_FlaOgePubPathVariables & operator =(const C_FlaOgePubPathVariables &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
