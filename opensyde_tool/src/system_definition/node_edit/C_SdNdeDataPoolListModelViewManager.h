//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Manager for model and view instances to avoid invalid pointers (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDATAPOOLLISTMODELVIEWMANAGER_H
#define C_SDNDEDATAPOOLLISTMODELVIEWMANAGER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QObject>
#include "stwtypes.h"
#include "C_SdNdeDataPoolListDataSetModel.h"
#include "C_SdNdeDataPoolListArrayEditModel.h"
#include "C_SdNdeDataPoolListTableModel.h"
#include "C_SdNdeDataPoolUtil.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
class C_SdNdeDataPoolListDataSetView;
class C_SdNdeDataPoolListArrayEditView;
class C_SdNdeDataPoolListTableView;
}
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDataPoolListModelViewManager :
   public QObject
{
   Q_OBJECT

public:
   //Avoid call
   C_SdNdeDataPoolListModelViewManager(QObject * const opc_Parent = NULL);
   virtual ~C_SdNdeDataPoolListModelViewManager(void);
   void Clear(void);

   stw_opensyde_gui::C_SdNdeDataPoolListDataSetView * GetDataSetView(const stw_types::uint32 & oru32_NodeIndex,
                                                                     const stw_types::uint32 & oru32_DataPoolIndex,
                                                                     const stw_types::uint32 & oru32_ListIndex);
   C_SdNdeDataPoolListDataSetModel * GetDataSetModel(const stw_types::uint32 & oru32_NodeIndex,
                                                     const stw_types::uint32 & oru32_DataPoolIndex,
                                                     const stw_types::uint32 & oru32_ListIndex);
   stw_opensyde_gui::C_SdNdeDataPoolListArrayEditView * GetArrayEditView(const stw_types::uint32 & oru32_NodeIndex,
                                                                         const stw_types::uint32 & oru32_DataPoolIndex,
                                                                         const stw_types::uint32 & oru32_ListIndex,
                                                                         const stw_types::uint32 & oru32_ElementIndex,
                                                                         const C_SdNdeDataPoolUtil::E_ArrayEditType & ore_ArrayEditType, const stw_types::uint32 & oru32_DataSetIndex);
   C_SdNdeDataPoolListArrayEditModel * GetArrayEditModel(const stw_types::uint32 & oru32_NodeIndex,
                                                         const stw_types::uint32 & oru32_DataPoolIndex,
                                                         const stw_types::uint32 & oru32_ListIndex,
                                                         const stw_types::uint32 & oru32_ElementIndex,
                                                         const C_SdNdeDataPoolUtil::E_ArrayEditType & ore_ArrayEditType,
                                                         const stw_types::uint32 & oru32_DataSetIndex);
   stw_opensyde_gui::C_SdNdeDataPoolListTableView * GetElementView(const stw_types::uint32 & oru32_NodeIndex,
                                                                   const stw_types::uint32 & oru32_DataPoolIndex,
                                                                   const stw_types::uint32 & oru32_ListIndex);
   C_SdNdeDataPoolListTableModel * GetElementModel(const stw_types::uint32 & oru32_NodeIndex,
                                                   const stw_types::uint32 & oru32_DataPoolIndex,
                                                   const stw_types::uint32 & oru32_ListIndex);

   void RegisterDataSetView(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                            const stw_types::uint32 & oru32_ListIndex,
                            stw_opensyde_gui::C_SdNdeDataPoolListDataSetView * const opc_View);
   void UnRegisterDataSetView(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                              const stw_types::uint32 & oru32_ListIndex,
                              const stw_opensyde_gui::C_SdNdeDataPoolListDataSetView * const opc_View);
   void RegisterArrayEditView(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                              const stw_types::uint32 & oru32_ListIndex, const stw_types::uint32 & oru32_ElementIndex,
                              const C_SdNdeDataPoolUtil::E_ArrayEditType & ore_ArrayEditType,
                              const stw_types::uint32 & oru32_DataSetIndex,
                              stw_opensyde_gui::C_SdNdeDataPoolListArrayEditView * const opc_View);
   void UnRegisterArrayEditView(const stw_types::uint32 & oru32_NodeIndex,
                                const stw_types::uint32 & oru32_DataPoolIndex,
                                const stw_types::uint32 & oru32_ListIndex, const stw_types::uint32 & oru32_ElementIndex,
                                const C_SdNdeDataPoolUtil::E_ArrayEditType & ore_ArrayEditType,
                                const stw_types::uint32 & oru32_DataSetIndex,
                                const stw_opensyde_gui::C_SdNdeDataPoolListArrayEditView * const opc_View);
   void RegisterElementView(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                            const stw_types::uint32 & oru32_ListIndex,
                            stw_opensyde_gui::C_SdNdeDataPoolListTableView * const opc_View);
   void UnRegisterElementView(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                              const stw_types::uint32 & oru32_ListIndex,
                              const stw_opensyde_gui::C_SdNdeDataPoolListTableView * const opc_View);
   void RegisterDataSetCountChange(const stw_types::uint32 & oru32_NodeIndex,
                                   const stw_types::uint32 & oru32_DataPoolIndex,
                                   const stw_types::uint32 & oru32_ListIndex);
   void UpdateModels(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigDataSetErrorChange(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                              const stw_types::uint32 & oru32_ListIndex);
   void SigDataSetCountChange(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                              const stw_types::uint32 & oru32_ListIndex);

private:
   std::vector<stw_opensyde_gui::C_SdNdeDataPoolListDataSetView *> mc_DataSetViews;
   std::vector<C_SdNdeDataPoolListDataSetModel *> mc_DataSetModels;
   std::vector<stw_opensyde_gui::C_SdNdeDataPoolListArrayEditView *> mc_ArrayEditViews;
   std::vector<C_SdNdeDataPoolListArrayEditModel *> mc_ArrayEditModels;
   std::vector<stw_opensyde_gui::C_SdNdeDataPoolListTableView *> mc_ElementViews;
   std::vector<C_SdNdeDataPoolListTableModel *> mc_ElementModels;

   void m_OnDataSetErrorChangePossible(const stw_types::uint32 & oru32_NodeIndex,
                                       const stw_types::uint32 & oru32_DataPoolIndex,
                                       const stw_types::uint32 & oru32_DataPoolListIndex);
   void m_OnDataSetDataChange(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                              const stw_types::uint32 & oru32_DataPoolListIndex,
                              const stw_types::uint32 & oru32_DataPoolListDataSetIndex, const QVariant & orc_NewData,
                              const stw_opensyde_gui_logic::C_SdNdeDataPoolUtil::E_DataSetDataChangeType & ore_DataChangeType);
   void m_OnDataSetColumnCountChange(const stw_types::uint32 & oru32_NodeIndex,
                                     const stw_types::uint32 & oru32_DataPoolIndex,
                                     const stw_types::uint32 & oru32_DataPoolListIndex,
                                     const stw_types::sint32 & ors32_NewColumnCount);
   void m_OnElementErrorChangePossible(const stw_types::uint32 & oru32_NodeIndex,
                                       const stw_types::uint32 & oru32_DataPoolIndex,
                                       const stw_types::uint32 & oru32_DataPoolListIndex);
   void m_OnElementSizeChangePossible(const stw_types::uint32 & oru32_NodeIndex,
                                      const stw_types::uint32 & oru32_DataPoolIndex,
                                      const stw_types::uint32 & oru32_DataPoolListIndex);
   void m_OnElementDataChange(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                              const stw_types::uint32 & oru32_DataPoolListIndex,
                              const stw_types::uint32 & oru32_DataPoolListElementIndex, const QVariant & orc_NewData,
                              const stw_opensyde_gui_logic::C_SdNdeDataPoolUtil::E_ElementDataChangeType & ore_DataChangeType, const stw_types::uint32 & oru32_ArrayIndex, const stw_types::sint32 & ors32_DataSetIndex);
   void m_OnArrayEditErrorChangePossible(const stw_types::uint32 & oru32_NodeIndex,
                                         const stw_types::uint32 & oru32_DataPoolIndex,
                                         const stw_types::uint32 & oru32_ListIndex,
                                         const stw_types::uint32 & oru32_ElementIndex,
                                         const C_SdNdeDataPoolUtil::E_ArrayEditType & ore_ArrayEditType,
                                         const stw_types::uint32 & oru32_DataSetIndex);
   void m_OnArrayEditDataChange(const stw_types::uint32 & oru32_NodeIndex,
                                const stw_types::uint32 & oru32_DataPoolIndex,
                                const stw_types::uint32 & oru32_ListIndex, const stw_types::uint32 & oru32_ElementIndex,
                                const C_SdNdeDataPoolUtil::E_ArrayEditType & ore_ArrayEditType,
                                const stw_types::uint32 & oru32_DataSetIndex,
                                const stw_types::uint32 & oru32_ArrayElementIndex, const QVariant & orc_NewData);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
