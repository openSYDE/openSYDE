//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       openSYDE target support package data handling class
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCTARGETSUPPORTPACKAGE_HPP
#define C_OSCTARGETSUPPORTPACKAGE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants -----------------------------------------------------------  -----------------------------------
   */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscTargetSupportPackage
{
public:
   C_OscTargetSupportPackage();
   stw::scl::C_SclString c_DeviceName; ///< name of device this package it intended for
   stw::scl::C_SclString c_Comment;    ///< comment about this package

   stw::scl::C_SclString c_TemplatePath; ///< path to template package archive

   void Clear(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
