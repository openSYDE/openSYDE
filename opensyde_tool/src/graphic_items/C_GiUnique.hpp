//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class for all graphics items which require unique identification (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_GIUNIQUE_HPP
#define C_GIUNIQUE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_GiUnique
{
public:
   C_GiUnique(const uint64_t & oru64_Id = 0);
   virtual ~C_GiUnique(void);
   bool CheckMatch(const uint64_t & oru64_Id) const;

   uint64_t GetId() const;

private:
   const uint64_t mu64_Id;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
