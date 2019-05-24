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

/* -- Includes ------------------------------------------------------------- */
#include "C_OSCParamSetInterpretedNode.h"
#include "C_OSCParamSetInterpretedFileInfoData.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_core
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OSCParamSetInterpretedData
{
public:
   C_OSCParamSetInterpretedData(void);

   void Clear();
   void AddInterpretedFileData(const C_OSCParamSetInterpretedFileInfoData & orc_FileInfo);
   const C_OSCParamSetInterpretedNode * GetInterpretedDataForNode(const stw_scl::C_SCLString & orc_NodeName) const;

   C_OSCParamSetInterpretedFileInfoData c_FileInfo;
   //Interpreted file content
   std::vector<C_OSCParamSetInterpretedNode> c_InterpretedNodes;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
