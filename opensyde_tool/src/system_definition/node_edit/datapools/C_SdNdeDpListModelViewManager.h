//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Manager for model and view instances to avoid invalid pointers (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPLISTMODELVIEWMANAGER_H
#define C_SDNDEDPLISTMODELVIEWMANAGER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QObject>
#include "stwtypes.h"
#include "C_SdNdeDpListDataSetModel.h"
#include "C_SdNdeDpListArrayEditModel.h"
#include "C_SdNdeDpListTableModel.h"
#include "C_SdNdeDpUtil.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
class C_SdNdeDpListDataSetView;
class C_SdNdeDpListArrayEditView;
class C_SdNdeDpListTableView;
}
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpListModelViewManager :
   public QObject
{
   Q_OBJECT

public:
   //Avoid call
   C_SdNdeDpListModelViewManager(QObject * const opc_Parent = NULL);
   virtual ~C_SdNdeDpListModelViewManager(void);
   void Clear(void);

   stw_opensyde_gui::C_SdNdeDpListDataSetView * GetDataSetView(const stw_types::uint32 & oru32_NodeIndex,
                                                                     const stw_types::uint32 & oru32_DataPoolIndex,
                                                                     const stw_types::uint32 & oru32_ListIndex);
   C_SdNdeDpListDataSetModel * GetDataSetModel(const stw_types::uint32 & oru32_NodeIndex,
                                                     const stw_types::uint32 & oru32_DataPoolIndex,
                                                     const stw_types::uint32 & oru32_ListIndex);
   stw_opensyde_gui::C_SdNdeDpListArrayEditView * GetArrayEditView(const stw_types::uint32 & oru32_NodeIndex,
                                                                         const stw_types::uint32 & oru32_DataPoolIndex,
                                                                         const stw_types::uint32 & oru32_ListIndex,
                                                                         const stw_types::uint32 & oru32_ElementIndex,
                                                                         const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType, const stw_types::uint32 & oru32_DataSetIndex);
   C_SdNdeDpListArrayEditModel * GetArrayEditModel(const stw_types::uint32 & oru32_NodeIndex,
                                                         const stw_types::uint32 & oru32_DataPoolIndex,
                                                         const stw_types::uint32 & oru32_ListIndex,
                                                         const stw_types::uint32 & oru32_ElementIndex,
                                                         const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                                                         const stw_types::uint32 & oru32_DataSetIndex);
   stw_opensyde_gui::C_SdNdeDpListTableView * GetElementView(const stw_types::uint32 & oru32_NodeIndex,
                                                                   const stw_types::uint32 & oru32_DataPoolIndex,
                                                                   const stw_types::uint32 & oru32_ListIndex);
   C_SdNdeDpListTableModel * GetElementModel(const stw_types::uint32 & oru32_NodeIndex,
                                                   const stw_types::uint32 & oru32_DataPoolIndex,
                                                   const stw_types::uint32 & oru32_ListIndex);

   void RegisterDataSetView(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                            const stw_types::uint32 & oru32_ListIndex,
                            stw_opensyde_gui::C_SdNdeDpListDataSetView * const opc_View);
   void UnRegisterDataSetView(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                              const stw_types::uint32 & oru32_ListIndex,
                              const stw_opensyde_gui::C_SdNdeDpListDataSetView * const opc_View);
   void RegisterArrayEditView(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                              const stw_types::uint32 & oru32_ListIndex, const stw_types::uint32 & oru32_ElementIndex,
                              const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                              const stw_types::uint32 & oru32_DataSetIndex,
                              stw_opensyde_gui::C_SdNdeDpListArrayEditView * const opc_View);
   void UnRegisterArrayEditView(const stw_types::uint32 & oru32_NodeIndex,
                                const stw_types::uint32 & oru32_DataPoolIndex,
                                const stw_types::uint32 & oru32_ListIndex, const stw_types::uint32 & oru32_ElementIndex,
                                const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                                const stw_types::uint32 & oru32_DataSetIndex,
                                const stw_opensyde_gui::C_SdNdeDpListArrayEditView * const opc_View);
   void RegisterElementView(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                            const stw_types::uint32 & oru32_ListIndex,
                            stw_opensyde_gui::C_SdNdeDpListTableView * const opc_View);
   void UnRegisterElementView(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                              const stw_types::uint32 & oru32_ListIndex,
                              const stw_opensyde_gui::C_SdNdeDpListTableView * const opc_View);
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
   std::vector<stw_opensyde_gui::C_SdNdeDpListDataSetView *> mc_DataSetViews;
   std::vector<C_SdNdeDpListDataSetModel *> mc_DataSetModels;
   std::vector<stw_opensyde_gui::C_SdNdeDpListArrayEditView *> mc_ArrayEditViews;
   std::vector<C_SdNdeDpListArrayEditModel *> mc_ArrayEditModels;
   std::vector<stw_opensyde_gui::C_SdNdeDpListTableView *> mc_ElementViews;
   std::vector<C_SdNdeDpListTableModel *> mc_ElementModels;

   void m_OnDataSetErrorChangePossible(const stw_types::uint32 & oru32_NodeIndex,
                                       const stw_types::uint32 & oru32_DataPoolIndex,
                                       const stw_types::uint32 & oru32_DataPoolListIndex);
   void m_OnDataSetDataChange(const stw_types::uint32 & oru32_NodeIndex, const stw_types::uint32 & oru32_DataPoolIndex,
                              const stw_types::uint32 & oru32_DataPoolListIndex,
                              const stw_types::uint32 & oru32_DataPoolListDataSetIndex, const QVariant & orc_NewData,
                              const stw_opensyde_gui_logic::C_SdNdeDpUtil::E_DataSetDataChangeType & ore_DataChangeType);
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
                              const stw_opensyde_gui_logic::C_SdNdeDpUtil::E_ElementDataChangeType & ore_DataChangeType, const stw_types::uint32 & oru32_ArrayIndex, const stw_types::sint32 & ors32_DataSetIndex);
   void m_OnArrayEditErrorChangePossible(const stw_types::uint32 & oru32_NodeIndex,
                                         const stw_types::uint32 & oru32_DataPoolIndex,
                                         const stw_types::uint32 & oru32_ListIndex,
                                         const stw_types::uint32 & oru32_ElementIndex,
                                         const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                                         const stw_types::uint32 & oru32_DataSetIndex);
   void m_OnArrayEditDataChange(const stw_types::uint32 & oru32_NodeIndex,
                                const stw_types::uint32 & oru32_DataPoolIndex,
                                const stw_types::uint32 & oru32_ListIndex, const stw_types::uint32 & oru32_ElementIndex,
                                const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                                const stw_types::uint32 & oru32_DataSetIndex,
                                const stw_types::uint32 & oru32_ArrayElementIndex, const QVariant & orc_NewData);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
