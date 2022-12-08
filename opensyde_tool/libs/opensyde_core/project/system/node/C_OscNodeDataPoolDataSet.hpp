//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for data set related, additional information (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODEDATAPOOLDATASET_H
#define C_OSCNODEDATAPOOLDATASET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "stwtypes.hpp"

#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscNodeDataPoolDataSet
{
public:
   C_OscNodeDataPoolDataSet(void);

   void CalcHash(uint32_t & oru32_HashValue) const;

   stw::scl::C_SclString c_Name;    ///< User data set name
   stw::scl::C_SclString c_Comment; ///< User data set comment
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
