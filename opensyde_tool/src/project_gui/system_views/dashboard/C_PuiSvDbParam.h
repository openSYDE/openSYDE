//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       UI class for dashboard parameterization table data (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_PUISVDBPARAM_H
#define C_PUISVDBPARAM_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <vector>
#include "C_PuiSvDbWidgetBase.h"
#include "C_OSCNodeDataPoolListId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_PuiSvDbExpandedTreeIndex
{
public:
   C_PuiSvDbNodeDataPoolListElementId c_ExpandedId;
   stw_types::uint32 u32_Layer;
};

class C_PuiSvDbParam :
   public C_PuiSvDbWidgetBase
{
public:
   C_PuiSvDbParam(void);

   virtual void CalcHash(stw_types::uint32 & oru32_HashValue) const override;
   virtual bool IsReadElement(void) const override;
   stw_types::sint32 CheckError(const stw_opensyde_core::C_OSCNodeDataPoolListId & orc_ListId, bool & orq_Error) const;
   static stw_types::sint32 h_CheckMinMax(const stw_opensyde_core::C_OSCNodeDataPoolListElement & orc_Elem,
                                          const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Value, bool & orq_Ok);
   virtual stw_types::sint32 RemoveElement(const stw_types::uint32 & oru32_Index) override;

   std::vector<stw_types::sint32> c_DataSetSelectionIndices; ///< Index of selected data set (-1 if
   ///< invalid)
   ///< Size of list values expected
   std::vector<stw_opensyde_core::C_OSCNodeDataPoolContent> c_ListValues; ///< Values for each included list item,
   ///< needs to be synchronised
   ///< with system definition
   ///< Size of elements expected
   std::vector<std::vector<stw_types::sint32> > c_ColWidth; ///< Column width of table
   ///< Outer vector: Size of 1 expected
   ///< Inner vector: Size of exactly 7 expected
   std::vector<C_PuiSvDbExpandedTreeIndex> c_ExpandedItems; ///< Expanded items
   std::vector<stw_types::sint32> c_ColPosIndices;          ///< Column position indices,
   ///<  e.g. [0]=2 -< model column 0 is at
   ///<   visible index 2
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
