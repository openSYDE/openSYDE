//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Utility class to handle imports of KEFEX RAMView .def style projects

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCIMPORTRAMVIEW_HPP
#define C_OSCIMPORTRAMVIEW_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_SclStringList.hpp"
#include "C_OscNodeDataPool.hpp"
#include "CKFXVariableLists.hpp"
#include "CKFXProjectOptions.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscImportRamView
{
public:
   static int32_t h_ImportDataPoolFromRamViewDefProject(const stw::scl::C_SclString & orc_ProjectPath,
                                                        C_OscNodeDataPool & orc_DataPool,
                                                        stw::scl::C_SclStringList & orc_ImportInformation);

private:
   static int32_t mh_ImportElementType(const stw::diag_lib::C_KFXVariableBase & orc_RamViewElement,
                                       C_OscNodeDataPoolListElement & orc_Element);
   static void mh_ImportElementMinMax(const stw::diag_lib::C_KFXVariableBase & orc_RamViewElement,
                                      C_OscNodeDataPoolListElement & orc_Element);
   static void mh_ImportElementDataSetValues(const stw::diag_lib::C_KFXVariableBase & orc_RamViewElement,
                                             C_OscNodeDataPoolListElement & orc_Element);
   static void mh_AdaptName(stw::scl::C_SclString & orc_Name, stw::scl::C_SclString & orc_Comment,
                            const stw::scl::C_SclString & orc_ElementDesignator,
                            stw::scl::C_SclStringList & orc_ImportInformation);

   static int32_t mh_LoadRamViewDefProject(const stw::scl::C_SclString & orc_ProjectPath,
                                           stw::diag_lib::C_KFXProjectOptions & orc_ProjectOptions,
                                           stw::diag_lib::C_KFXVariableLists & orc_VariableLists);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
