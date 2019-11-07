//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list array edit table view (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPLISTARRAYEDITVIEW_H
#define C_SDNDEDPLISTARRAYEDITVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QUndoStack>
#include <QLabel>
#include "stwtypes.h"
#include "C_TblViewScroll.h"
#include "C_PuiSdNodeDataPoolList.h"
#include "C_OSCNodeDataPoolList.h"
#include "C_SdNdeUnoAedDataPoolListManager.h"
#include "C_SdNdeDpListArrayEditDelegate.h"
#include "C_OgeContextMenu.h"
#include "C_SdNdeDpUtil.h"
#include "C_SdNdeDpListModelViewManager.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

//lint -e{1960} Forward declarations necessary for automatic registration
class C_SdNdeDpListArrayEditView :
   public C_TblViewScroll
{
   Q_OBJECT

public:
   C_SdNdeDpListArrayEditView(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDpListArrayEditView(void);
   void SetElement(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                   const stw_types::uint32 & oru32_ListIndex, const stw_types::uint32 & oru32_ElementIndex,
                   const stw_opensyde_gui_logic::C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                   const stw_types::uint32 & oru32_DataSetIndex);
   bool Equals(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
               const stw_types::uint32 & oru32_ListIndex, const stw_types::uint32 & oru32_ElementIndex,
               const stw_opensyde_gui_logic::C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
               const stw_types::uint32 & oru32_DataSetIndex) const;
   QUndoCommand * TakeUndoCommand(void);
   void OnErrorChangePossible(void);
   void OnDataChange(const stw_types::uint32 & oru32_ArrayIndex, const QVariant & orc_NewData);
   void SetModelViewManager(stw_opensyde_gui_logic::C_SdNdeDpListModelViewManager * const opc_Value);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChangePossible(void);
   void SigSelectionChanged(const stw_types::uint32 & oru32_Count);

private:
   stw_types::uint32 mu32_NodeIndex;
   stw_types::uint32 mu32_DataPoolIndex;
   stw_types::uint32 mu32_ListIndex;
   stw_types::uint32 mu32_ElementIndex;
   stw_opensyde_gui_logic::C_SdNdeDpUtil::E_ArrayEditType me_ArrayEditType;
   stw_types::uint32 mu32_DataSetIndex;
   stw_opensyde_gui_logic::C_SdNdeUnoAedDataPoolListManager mc_UndoManager;
   stw_opensyde_gui_logic::C_SdNdeDpListArrayEditDelegate mc_Delegate;
   stw_opensyde_gui_logic::C_SdNdeDpListModelViewManager * mpc_ModelViewManager;
   QLabel * mpc_LabelCorner;

   std::vector<stw_types::uint32> m_GetSelectedIndices(void) const;
   void m_UpdateModelView(void);
   void m_UpdateCornerButton(void);
   //Avoid call
   C_SdNdeDpListArrayEditView(const C_SdNdeDpListArrayEditView &);
   C_SdNdeDpListArrayEditView & operator =(const C_SdNdeDpListArrayEditView &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
