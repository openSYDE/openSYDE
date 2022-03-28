//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Parameter set file comparison description storage
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACPARAMSETFILEINFOCOMPARISONDESCRIPTION_H
#define C_SYVUPPACPARAMSETFILEINFOCOMPARISONDESCRIPTION_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacParamSetFileInfoComparisonDescription
{
public:
   C_SyvUpPacParamSetFileInfoComparisonDescription(void);

   QString c_Description;
   QString c_FileValue;
   QString c_SDValue;
   stw_types::uint32 u32_LayerNum;

   enum E_ResultType
   {
      eRT_MATCH,
      eRT_NO_MATCH,
      eRT_NOT_FOUND
   };

   E_ResultType e_ResultType;

   QString GetResultText(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
