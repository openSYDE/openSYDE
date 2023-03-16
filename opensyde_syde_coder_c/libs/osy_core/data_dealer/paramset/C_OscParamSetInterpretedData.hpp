//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Storage for interpreted param set data (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCPARAMSETINTERPRETEDDATA_H
#define C_OSCPARAMSETINTERPRETEDDATA_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscParamSetInterpretedNode.hpp"
#include "C_OscParamSetInterpretedFileInfoData.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscParamSetInterpretedData
{
public:
   C_OscParamSetInterpretedData(void);

   void Clear();
   void AddInterpretedFileData(const C_OscParamSetInterpretedFileInfoData & orc_FileInfo);
   const C_OscParamSetInterpretedNode * GetInterpretedDataForNode(const stw::scl::C_SclString & orc_NodeName) const;

   C_OscParamSetInterpretedFileInfoData c_FileInfo;
   //Interpreted file content
   std::vector<C_OscParamSetInterpretedNode> c_InterpretedNodes;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
