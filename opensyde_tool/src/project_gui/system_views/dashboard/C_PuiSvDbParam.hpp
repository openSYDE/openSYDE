//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI class for dashboard parameterization table data (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDBPARAM_HPP
#define C_PUISVDBPARAM_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include "C_PuiSvDbWidgetBase.hpp"
#include "C_OscNodeDataPoolListId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_PuiSvDbExpandedTreeIndex
{
public:
   C_PuiSvDbNodeDataPoolListElementId c_ExpandedId;
   uint32_t u32_Layer;
};

class C_PuiSvDbParam :
   public C_PuiSvDbWidgetBase
{
public:
   C_PuiSvDbParam(void);

   void CalcHash(uint32_t & oru32_HashValue) const override;
   bool IsReadElement(void) const override;
   int32_t CheckError(const stw::opensyde_core::C_OscNodeDataPoolListId & orc_ListId, bool & orq_Error) const;
   static int32_t h_CheckMinMax(const stw::opensyde_core::C_OscNodeDataPoolListElement & orc_Elem,
                                const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Value, bool & orq_Ok);
   int32_t RemoveElement(const uint32_t & oru32_Index) override;

   std::vector<int32_t> c_DataSetSelectionIndices; ///< Index of selected data set (-1 if
   ///< invalid)
   ///< Size of list values expected
   std::vector<stw::opensyde_core::C_OscNodeDataPoolContent> c_ListValues; ///< Values for each included list item,
   ///< needs to be synchronised
   ///< with system definition
   ///< Size of elements expected
   std::vector<std::vector<int32_t> > c_ColWidth; ///< Column width of table
   ///< Outer vector: Size of 1 expected
   ///< Inner vector: Size of exactly 7 expected
   std::vector<C_PuiSvDbExpandedTreeIndex> c_ExpandedItems; ///< Expanded items
   std::vector<int32_t> c_ColPosIndices;                    ///< Column position indices,
   ///<  e.g. [0]=2 -< model column 0 is at
   ///<   visible index 2
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
