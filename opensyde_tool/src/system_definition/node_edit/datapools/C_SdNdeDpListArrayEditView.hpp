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
#include "stwtypes.hpp"
#include "C_TblViewScroll.hpp"
#include "C_PuiSdNodeDataPoolList.hpp"
#include "C_OscNodeDataPoolList.hpp"
#include "C_SdNdeUnoAedDataPoolListManager.hpp"
#include "C_SdNdeDpListArrayEditDelegate.hpp"
#include "C_OgeContextMenu.hpp"
#include "C_SdNdeDpUtil.hpp"
#include "C_SdNdeDpListModelViewManager.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpListArrayEditView :
   public C_TblViewScroll
{
   Q_OBJECT

public:
   C_SdNdeDpListArrayEditView(QWidget * const opc_Parent = NULL);
   virtual ~C_SdNdeDpListArrayEditView(void);
   void SetElement(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                   const uint32_t & oru32_ListIndex, const uint32_t & oru32_ElementIndex,
                   const stw::opensyde_gui_logic::C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                   const uint32_t & oru32_DataSetIndex);
   bool Equals(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex, const uint32_t & oru32_ListIndex,
               const uint32_t & oru32_ElementIndex,
               const stw::opensyde_gui_logic::C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
               const uint32_t & oru32_DataSetIndex) const;
   QUndoCommand * TakeUndoCommand(void);
   void OnErrorChangePossible(void);
   void OnDataChange(const uint32_t & oru32_ArrayIndex, const QVariant & orc_NewData);
   void SetModelViewManager(stw::opensyde_gui_logic::C_SdNdeDpListModelViewManager * const opc_Value);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChangePossible(void);
   void SigSelectionChanged(const uint32_t & oru32_Count);

private:
   uint32_t mu32_NodeIndex;
   uint32_t mu32_DataPoolIndex;
   uint32_t mu32_ListIndex;
   uint32_t mu32_ElementIndex;
   stw::opensyde_gui_logic::C_SdNdeDpUtil::E_ArrayEditType me_ArrayEditType;
   uint32_t mu32_DataSetIndex;
   stw::opensyde_gui_logic::C_SdNdeUnoAedDataPoolListManager mc_UndoManager;
   stw::opensyde_gui_logic::C_SdNdeDpListArrayEditDelegate mc_Delegate;
   stw::opensyde_gui_logic::C_SdNdeDpListModelViewManager * mpc_ModelViewManager;
   QLabel * mpc_LabelCorner;

   std::vector<uint32_t> m_GetSelectedIndices(void) const;
   void m_UpdateModelView(void);
   void m_UpdateCornerButton(void);
   //Avoid call
   C_SdNdeDpListArrayEditView(const C_SdNdeDpListArrayEditView &);
   C_SdNdeDpListArrayEditView & operator =(const C_SdNdeDpListArrayEditView &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
