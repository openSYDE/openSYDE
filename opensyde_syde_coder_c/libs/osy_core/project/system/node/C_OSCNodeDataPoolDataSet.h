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

/* -- Includes ------------------------------------------------------------- */

#include "stwtypes.h"

#include "CSCLString.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCNodeDataPoolDataSet
{
public:
   C_OSCNodeDataPoolDataSet(void);

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   stw_scl::C_SCLString c_Name;    ///< User data set name
   stw_scl::C_SCLString c_Comment; ///< User data set comment
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
