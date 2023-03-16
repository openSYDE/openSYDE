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
#include "stwtypes.hpp"
#include "C_SdNdeDpListDataSetModel.hpp"
#include "C_SdNdeDpListArrayEditModel.hpp"
#include "C_SdNdeDpListTableModel.hpp"
#include "C_SdNdeDpUtil.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
class C_SdNdeDpListDataSetView;
class C_SdNdeDpListArrayEditView;
class C_SdNdeDpListTableView;
}

namespace opensyde_gui_logic
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
   ~C_SdNdeDpListModelViewManager(void) override;
   void Clear(void);

   stw::opensyde_gui::C_SdNdeDpListDataSetView * GetDataSetView(const uint32_t & oru32_NodeIndex,
                                                                const uint32_t & oru32_DataPoolIndex,
                                                                const uint32_t & oru32_ListIndex);
   C_SdNdeDpListDataSetModel * GetDataSetModel(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                               const uint32_t & oru32_ListIndex);
   stw::opensyde_gui::C_SdNdeDpListArrayEditView * GetArrayEditView(const uint32_t & oru32_NodeIndex,
                                                                    const uint32_t & oru32_DataPoolIndex,
                                                                    const uint32_t & oru32_ListIndex,
                                                                    const uint32_t & oru32_ElementIndex,
                                                                    const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType, const uint32_t & oru32_DataSetIndex);
   C_SdNdeDpListArrayEditModel * GetArrayEditModel(const uint32_t & oru32_NodeIndex,
                                                   const uint32_t & oru32_DataPoolIndex,
                                                   const uint32_t & oru32_ListIndex,
                                                   const uint32_t & oru32_ElementIndex,
                                                   const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                                                   const uint32_t & oru32_DataSetIndex);
   stw::opensyde_gui::C_SdNdeDpListTableView * GetElementView(const uint32_t & oru32_NodeIndex,
                                                              const uint32_t & oru32_DataPoolIndex,
                                                              const uint32_t & oru32_ListIndex);
   C_SdNdeDpListTableModel * GetElementModel(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                             const uint32_t & oru32_ListIndex);

   void RegisterDataSetView(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                            const uint32_t & oru32_ListIndex,
                            stw::opensyde_gui::C_SdNdeDpListDataSetView * const opc_View);
   void UnRegisterDataSetView(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                              const uint32_t & oru32_ListIndex,
                              const stw::opensyde_gui::C_SdNdeDpListDataSetView * const opc_View);
   void RegisterArrayEditView(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                              const uint32_t & oru32_ListIndex, const uint32_t & oru32_ElementIndex,
                              const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                              const uint32_t & oru32_DataSetIndex,
                              stw::opensyde_gui::C_SdNdeDpListArrayEditView * const opc_View);
   void UnRegisterArrayEditView(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                const uint32_t & oru32_ListIndex, const uint32_t & oru32_ElementIndex,
                                const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                                const uint32_t & oru32_DataSetIndex,
                                const stw::opensyde_gui::C_SdNdeDpListArrayEditView * const opc_View);
   void RegisterElementView(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                            const uint32_t & oru32_ListIndex,
                            stw::opensyde_gui::C_SdNdeDpListTableView * const opc_View);
   void UnRegisterElementView(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                              const uint32_t & oru32_ListIndex,
                              const stw::opensyde_gui::C_SdNdeDpListTableView * const opc_View);
   void RegisterDataSetCountChange(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                   const uint32_t & oru32_ListIndex);
   void UpdateModels(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigDataSetErrorChange(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                              const uint32_t & oru32_ListIndex);
   void SigDataSetCountChange(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                              const uint32_t & oru32_ListIndex);

private:
   std::vector<stw::opensyde_gui::C_SdNdeDpListDataSetView *> mc_DataSetViews;
   std::vector<C_SdNdeDpListDataSetModel *> mc_DataSetModels;
   std::vector<stw::opensyde_gui::C_SdNdeDpListArrayEditView *> mc_ArrayEditViews;
   std::vector<C_SdNdeDpListArrayEditModel *> mc_ArrayEditModels;
   std::vector<stw::opensyde_gui::C_SdNdeDpListTableView *> mc_ElementViews;
   std::vector<C_SdNdeDpListTableModel *> mc_ElementModels;

   void m_OnDataSetErrorChangePossible(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                       const uint32_t & oru32_DataPoolListIndex);
   void m_OnDataSetDataChange(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                              const uint32_t & oru32_DataPoolListIndex, const uint32_t & oru32_DataPoolListDataSetIndex,
                              const QVariant & orc_NewData,
                              const stw::opensyde_gui_logic::C_SdNdeDpUtil::E_DataSetDataChangeType & ore_DataChangeType);
   void m_OnDataSetColumnCountChange(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                     const uint32_t & oru32_DataPoolListIndex, const int32_t & ors32_NewColumnCount);
   void m_OnElementErrorChangePossible(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                       const uint32_t & oru32_DataPoolListIndex);
   void m_OnElementSizeChangePossible(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                      const uint32_t & oru32_DataPoolListIndex);
   void m_OnElementDataChange(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                              const uint32_t & oru32_DataPoolListIndex, const uint32_t & oru32_DataPoolListElementIndex,
                              const QVariant & orc_NewData,
                              const stw::opensyde_gui_logic::C_SdNdeDpUtil::E_ElementDataChangeType & ore_DataChangeType, const uint32_t & oru32_ArrayIndex, const int32_t & ors32_DataSetIndex);
   void m_OnArrayEditErrorChangePossible(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                         const uint32_t & oru32_ListIndex, const uint32_t & oru32_ElementIndex,
                                         const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                                         const uint32_t & oru32_DataSetIndex);
   void m_OnArrayEditDataChange(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                const uint32_t & oru32_ListIndex, const uint32_t & oru32_ElementIndex,
                                const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                                const uint32_t & oru32_DataSetIndex, const uint32_t & oru32_ArrayElementIndex,
                                const QVariant & orc_NewData);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
