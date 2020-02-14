//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Manager for model and view instances to avoid invalid pointers (implementation)

   Manager for model and view instances to avoid invalid pointers

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SdNdeDpListModelViewManager.h"
#include "C_SdNdeDpListDataSetView.h"
#include "C_SdNdeDpListArrayEditView.h"
#include "C_SdNdeDpListTableView.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListModelViewManager::C_SdNdeDpListModelViewManager(QObject * const opc_Parent) :
   QObject(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListModelViewManager::~C_SdNdeDpListModelViewManager(void)
{
   Clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear known models
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListModelViewManager::Clear(void)
{
   for (uint32 u32_ItModel = 0; u32_ItModel < this->mc_DataSetModels.size(); ++u32_ItModel)
   {
      delete (this->mc_DataSetModels[u32_ItModel]);
      this->mc_DataSetModels[u32_ItModel] = NULL;
   }
   this->mc_DataSetModels.clear();
   for (uint32 u32_ItModel = 0; u32_ItModel < this->mc_ArrayEditModels.size(); ++u32_ItModel)
   {
      delete (this->mc_ArrayEditModels[u32_ItModel]);
      this->mc_ArrayEditModels[u32_ItModel] = NULL;
   }
   this->mc_ArrayEditModels.clear();
   for (uint32 u32_ItModel = 0; u32_ItModel < this->mc_ElementModels.size(); ++u32_ItModel)
   {
      delete (this->mc_ElementModels[u32_ItModel]);
      this->mc_ElementModels[u32_ItModel] = NULL;
   }
   this->mc_ElementModels.clear();
   this->mc_ArrayEditViews.clear();
   this->mc_DataSetViews.clear();
   this->mc_ElementViews.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data set view

   \param[in]  oru32_NodeIndex      Node index
   \param[in]  oru32_DataPoolIndex  Node data pool index
   \param[in]  oru32_ListIndex      Node data pool list index

   \return
   NULL Not registered
   Else Registered view
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListDataSetView * C_SdNdeDpListModelViewManager::GetDataSetView(const uint32 & oru32_NodeIndex,
                                                                         const uint32 & oru32_DataPoolIndex,
                                                                         const uint32 & oru32_ListIndex)
{
   C_SdNdeDpListDataSetView * pc_Retval = NULL;

   for (uint32 u32_ItView = 0; u32_ItView < this->mc_DataSetViews.size(); ++u32_ItView)
   {
      C_SdNdeDpListDataSetView * const pc_Tmp = this->mc_DataSetViews[u32_ItView];
      if (pc_Tmp != NULL)
      {
         if (pc_Tmp->Equals(oru32_NodeIndex, oru32_DataPoolIndex, oru32_ListIndex))
         {
            pc_Retval = pc_Tmp;
         }
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get data set model

   \param[in]  oru32_NodeIndex      Node index
   \param[in]  oru32_DataPoolIndex  Node data pool index
   \param[in]  oru32_ListIndex      Node data pool list index

   \return
   Data set model (garanteed)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListDataSetModel * C_SdNdeDpListModelViewManager::GetDataSetModel(const uint32 & oru32_NodeIndex,
                                                                           const uint32 & oru32_DataPoolIndex,
                                                                           const uint32 & oru32_ListIndex)
{
   C_SdNdeDpListDataSetModel * pc_Retval = NULL;

   for (uint32 u32_ItView = 0; u32_ItView < this->mc_DataSetModels.size(); ++u32_ItView)
   {
      C_SdNdeDpListDataSetModel * const pc_Tmp = this->mc_DataSetModels[u32_ItView];
      if (pc_Tmp != NULL)
      {
         if (pc_Tmp->Equals(oru32_NodeIndex, oru32_DataPoolIndex, oru32_ListIndex))
         {
            pc_Retval = pc_Tmp;
         }
      }
   }
   //Create if necessary
   if (pc_Retval == NULL)
   {
      this->mc_DataSetModels.push_back(new C_SdNdeDpListDataSetModel());
      pc_Retval = this->mc_DataSetModels[this->mc_DataSetModels.size() - 1];
      //Init
      pc_Retval->SetList(oru32_NodeIndex, oru32_DataPoolIndex, oru32_ListIndex);
      //Connects
      connect(pc_Retval, &C_SdNdeDpListDataSetModel::SigErrorChangePossible, this,
              &C_SdNdeDpListModelViewManager::m_OnDataSetErrorChangePossible);
      connect(pc_Retval, &C_SdNdeDpListDataSetModel::SigDataChange, this,
              &C_SdNdeDpListModelViewManager::m_OnDataSetDataChange);
      connect(pc_Retval, &C_SdNdeDpListDataSetModel::SigColumnCountChange, this,
              &C_SdNdeDpListModelViewManager::m_OnDataSetColumnCountChange);
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get array edit view

   \param[in]  oru32_NodeIndex      Node index
   \param[in]  oru32_DataPoolIndex  Node data pool index
   \param[in]  oru32_ListIndex      Node data pool list index
   \param[in]  oru32_ElementIndex   Node data pool list element index
   \param[in]  ore_ArrayEditType    Enum for node data pool list element variable
   \param[in]  oru32_DataSetIndex   If min or max use 0
                                    Else use data set index

   \return
   NULL Not registered
   Else Registered view
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListArrayEditView * C_SdNdeDpListModelViewManager::GetArrayEditView(const uint32 & oru32_NodeIndex,
                                                                             const uint32 & oru32_DataPoolIndex,
                                                                             const uint32 & oru32_ListIndex,
                                                                             const uint32 & oru32_ElementIndex,
                                                                             const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                                                                             const uint32 & oru32_DataSetIndex)
{
   C_SdNdeDpListArrayEditView * pc_Retval = NULL;

   for (uint32 u32_ItView = 0; u32_ItView < this->mc_ArrayEditViews.size(); ++u32_ItView)
   {
      C_SdNdeDpListArrayEditView * const pc_Tmp = this->mc_ArrayEditViews[u32_ItView];
      if (pc_Tmp != NULL)
      {
         if (pc_Tmp->Equals(oru32_NodeIndex, oru32_DataPoolIndex, oru32_ListIndex, oru32_ElementIndex,
                            ore_ArrayEditType, oru32_DataSetIndex))
         {
            pc_Retval = pc_Tmp;
         }
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get array edit model

   \param[in]  oru32_NodeIndex      Node index
   \param[in]  oru32_DataPoolIndex  Node data pool index
   \param[in]  oru32_ListIndex      Node data pool list index
   \param[in]  oru32_ElementIndex   Node data pool list element index
   \param[in]  ore_ArrayEditType    Enum for node data pool list element variable
   \param[in]  oru32_DataSetIndex   If min or max use 0
                                    Else use data set index

   \return
   Array edit model (garanteed)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListArrayEditModel * C_SdNdeDpListModelViewManager::GetArrayEditModel(const uint32 & oru32_NodeIndex,
                                                                               const uint32 & oru32_DataPoolIndex,
                                                                               const uint32 & oru32_ListIndex,
                                                                               const uint32 & oru32_ElementIndex,
                                                                               const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                                                                               const uint32 & oru32_DataSetIndex)
{
   C_SdNdeDpListArrayEditModel * pc_Retval = NULL;

   for (uint32 u32_ItView = 0; u32_ItView < this->mc_ArrayEditModels.size(); ++u32_ItView)
   {
      C_SdNdeDpListArrayEditModel * const pc_Tmp = this->mc_ArrayEditModels[u32_ItView];
      if (pc_Tmp != NULL)
      {
         if (pc_Tmp->Equals(oru32_NodeIndex, oru32_DataPoolIndex, oru32_ListIndex, oru32_ElementIndex,
                            ore_ArrayEditType, oru32_DataSetIndex))
         {
            pc_Retval = pc_Tmp;
         }
      }
   }
   //Create if necessary
   if (pc_Retval == NULL)
   {
      this->mc_ArrayEditModels.push_back(new C_SdNdeDpListArrayEditModel());
      pc_Retval = this->mc_ArrayEditModels[this->mc_ArrayEditModels.size() - 1];
      //Init
      pc_Retval->SetElement(oru32_NodeIndex, oru32_DataPoolIndex, oru32_ListIndex, oru32_ElementIndex,
                            ore_ArrayEditType, oru32_DataSetIndex);
      //Connects
      connect(pc_Retval, &C_SdNdeDpListArrayEditModel::SigErrorChangePossible, this,
              &C_SdNdeDpListModelViewManager::m_OnArrayEditErrorChangePossible);
      //lint -e{64,918,1025,1703} Qt interface
      connect(pc_Retval, &C_SdNdeDpListArrayEditModel::SigDataChange, this,
              &C_SdNdeDpListModelViewManager::m_OnArrayEditDataChange);
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get element view

   \param[in]  oru32_NodeIndex      Node index
   \param[in]  oru32_DataPoolIndex  Node data pool index
   \param[in]  oru32_ListIndex      Node data pool list index

   \return
   NULL Not registered
   Else Registered view
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListTableView * C_SdNdeDpListModelViewManager::GetElementView(const uint32 & oru32_NodeIndex,
                                                                       const uint32 & oru32_DataPoolIndex,
                                                                       const uint32 & oru32_ListIndex)
{
   C_SdNdeDpListTableView * pc_Retval = NULL;

   for (uint32 u32_ItView = 0; u32_ItView < this->mc_ElementViews.size(); ++u32_ItView)
   {
      C_SdNdeDpListTableView * const pc_Tmp = this->mc_ElementViews[u32_ItView];
      if (pc_Tmp != NULL)
      {
         if (pc_Tmp->Equals(oru32_NodeIndex, oru32_DataPoolIndex, oru32_ListIndex))
         {
            pc_Retval = pc_Tmp;
         }
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get element model

   \param[in]  oru32_NodeIndex      Node index
   \param[in]  oru32_DataPoolIndex  Node data pool index
   \param[in]  oru32_ListIndex      Node data pool list index

   \return
   Element model (garanteed)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListTableModel * C_SdNdeDpListModelViewManager::GetElementModel(const uint32 & oru32_NodeIndex,
                                                                         const uint32 & oru32_DataPoolIndex,
                                                                         const uint32 & oru32_ListIndex)
{
   C_SdNdeDpListTableModel * pc_Retval = NULL;

   for (uint32 u32_ItView = 0; u32_ItView < this->mc_ElementModels.size(); ++u32_ItView)
   {
      C_SdNdeDpListTableModel * const pc_Tmp = this->mc_ElementModels[u32_ItView];
      if (pc_Tmp != NULL)
      {
         if (pc_Tmp->Equals(oru32_NodeIndex, oru32_DataPoolIndex, oru32_ListIndex))
         {
            pc_Retval = pc_Tmp;
         }
      }
   }
   //Create if necessary
   if (pc_Retval == NULL)
   {
      this->mc_ElementModels.push_back(new C_SdNdeDpListTableModel());
      pc_Retval = this->mc_ElementModels[this->mc_ElementModels.size() - 1];
      //Init
      pc_Retval->SetList(oru32_NodeIndex, oru32_DataPoolIndex, oru32_ListIndex);
      //Connects
      connect(pc_Retval, &C_SdNdeDpListTableModel::SigErrorChangePossible, this,
              &C_SdNdeDpListModelViewManager::m_OnElementErrorChangePossible);
      connect(pc_Retval, &C_SdNdeDpListTableModel::SigSizeChangePossible, this,
              &C_SdNdeDpListModelViewManager::m_OnElementSizeChangePossible);
      //lint -e{64,918,1025,1703} Qt interface
      connect(pc_Retval, &C_SdNdeDpListTableModel::SigDataChange, this,
              &C_SdNdeDpListModelViewManager::m_OnElementDataChange);
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register data set view

   \param[in]      oru32_NodeIndex        Node index
   \param[in]      oru32_DataPoolIndex    Node data pool index
   \param[in]      oru32_ListIndex        Node data pool list index
   \param[in,out]  opc_View               Data set view
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListModelViewManager::RegisterDataSetView(const uint32 & oru32_NodeIndex,
                                                        const uint32 & oru32_DataPoolIndex,
                                                        const uint32 & oru32_ListIndex,
                                                        C_SdNdeDpListDataSetView * const opc_View)
{
   C_SdNdeDpListDataSetView * pc_Found = NULL;

   for (uint32 u32_ItView = 0; u32_ItView < this->mc_DataSetViews.size(); ++u32_ItView)
   {
      C_SdNdeDpListDataSetView * const pc_Tmp = this->mc_DataSetViews[u32_ItView];
      if (pc_Tmp != NULL)
      {
         if (pc_Tmp->Equals(oru32_NodeIndex, oru32_DataPoolIndex, oru32_ListIndex))
         {
            pc_Found = pc_Tmp;
            this->mc_DataSetViews[u32_ItView] = opc_View;
         }
      }
   }
   if (pc_Found == NULL)
   {
      this->mc_DataSetViews.push_back(opc_View);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Unregister data set view

   \param[in]      oru32_NodeIndex        Node index
   \param[in]      oru32_DataPoolIndex    Node data pool index
   \param[in]      oru32_ListIndex        Node data pool list index
   \param[in,out]  opc_View               Data set view
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListModelViewManager::UnRegisterDataSetView(const uint32 & oru32_NodeIndex,
                                                          const uint32 & oru32_DataPoolIndex,
                                                          const uint32 & oru32_ListIndex,
                                                          const C_SdNdeDpListDataSetView * const opc_View)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_DataSetViews.size(); ++u32_ItView)
   {
      if (this->mc_DataSetViews[u32_ItView] == opc_View)
      {
         C_SdNdeDpListDataSetView * const pc_Tmp = this->mc_DataSetViews[u32_ItView];
         if (pc_Tmp != NULL)
         {
            if (pc_Tmp->Equals(oru32_NodeIndex, oru32_DataPoolIndex, oru32_ListIndex))
            {
               this->mc_DataSetViews[u32_ItView] = NULL;
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register array edit view

   \param[in]      oru32_NodeIndex        Node index
   \param[in]      oru32_DataPoolIndex    Node data pool index
   \param[in]      oru32_ListIndex        Node data pool list index
   \param[in]      oru32_ElementIndex     Node data pool list element index
   \param[in]      ore_ArrayEditType      Enum for node data pool list element variable
   \param[in]      oru32_DataSetIndex     If min or max use 0
                                          Else use data set index
   \param[in,out]  opc_View               Array edit view
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListModelViewManager::RegisterArrayEditView(const uint32 & oru32_NodeIndex,
                                                          const uint32 & oru32_DataPoolIndex,
                                                          const uint32 & oru32_ListIndex,
                                                          const uint32 & oru32_ElementIndex,
                                                          const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                                                          const uint32 & oru32_DataSetIndex,
                                                          C_SdNdeDpListArrayEditView * const opc_View)
{
   C_SdNdeDpListArrayEditView * pc_Found = NULL;

   for (uint32 u32_ItView = 0; u32_ItView < this->mc_ArrayEditViews.size(); ++u32_ItView)
   {
      C_SdNdeDpListArrayEditView * const pc_Tmp = this->mc_ArrayEditViews[u32_ItView];
      if (pc_Tmp != NULL)
      {
         if (pc_Tmp->Equals(oru32_NodeIndex, oru32_DataPoolIndex, oru32_ListIndex, oru32_ElementIndex,
                            ore_ArrayEditType, oru32_DataSetIndex))
         {
            pc_Found = pc_Tmp;
            this->mc_ArrayEditViews[u32_ItView] = opc_View;
         }
      }
   }
   if (pc_Found == NULL)
   {
      this->mc_ArrayEditViews.push_back(opc_View);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Unregister array edit view

   \param[in]      oru32_NodeIndex        Node index
   \param[in]      oru32_DataPoolIndex    Node data pool index
   \param[in]      oru32_ListIndex        Node data pool list index
   \param[in]      oru32_ElementIndex     Node data pool list element index
   \param[in]      ore_ArrayEditType      Enum for node data pool list element variable
   \param[in]      oru32_DataSetIndex     If min or max use 0
                                          Else use data set index
   \param[in,out]  opc_View               Array edit view
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListModelViewManager::UnRegisterArrayEditView(const uint32 & oru32_NodeIndex,
                                                            const uint32 & oru32_DataPoolIndex,
                                                            const uint32 & oru32_ListIndex,
                                                            const uint32 & oru32_ElementIndex,
                                                            const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                                                            const uint32 & oru32_DataSetIndex,
                                                            const C_SdNdeDpListArrayEditView * const opc_View)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_ArrayEditViews.size(); ++u32_ItView)
   {
      if (this->mc_ArrayEditViews[u32_ItView] == opc_View)
      {
         C_SdNdeDpListArrayEditView * const pc_Tmp = this->mc_ArrayEditViews[u32_ItView];
         if (pc_Tmp != NULL)
         {
            if (pc_Tmp->Equals(oru32_NodeIndex, oru32_DataPoolIndex, oru32_ListIndex, oru32_ElementIndex,
                               ore_ArrayEditType, oru32_DataSetIndex))
            {
               this->mc_ArrayEditViews[u32_ItView] = NULL;
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register element view

   \param[in]      oru32_NodeIndex        Node index
   \param[in]      oru32_DataPoolIndex    Node data pool index
   \param[in]      oru32_ListIndex        Node data pool list index
   \param[in,out]  opc_View               Element view
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListModelViewManager::RegisterElementView(const uint32 & oru32_NodeIndex,
                                                        const uint32 & oru32_DataPoolIndex,
                                                        const uint32 & oru32_ListIndex,
                                                        C_SdNdeDpListTableView * const opc_View)
{
   C_SdNdeDpListTableView  * pc_Found = NULL;

   for (uint32 u32_ItView = 0; u32_ItView < this->mc_ElementViews.size(); ++u32_ItView)
   {
      C_SdNdeDpListTableView * const pc_Tmp = this->mc_ElementViews[u32_ItView];
      if (pc_Tmp != NULL)
      {
         if (pc_Tmp->Equals(oru32_NodeIndex, oru32_DataPoolIndex, oru32_ListIndex))
         {
            pc_Found = pc_Tmp;
            this->mc_ElementViews[u32_ItView] = opc_View;
         }
      }
   }
   if (pc_Found == NULL)
   {
      this->mc_ElementViews.push_back(opc_View);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Unregister element view

   \param[in]      oru32_NodeIndex        Node index
   \param[in]      oru32_DataPoolIndex    Node data pool index
   \param[in]      oru32_ListIndex        Node data pool list index
   \param[in,out]  opc_View               Element view
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListModelViewManager::UnRegisterElementView(const uint32 & oru32_NodeIndex,
                                                          const uint32 & oru32_DataPoolIndex,
                                                          const uint32 & oru32_ListIndex,
                                                          const C_SdNdeDpListTableView * const opc_View)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_ElementViews.size(); ++u32_ItView)
   {
      if (this->mc_ElementViews[u32_ItView] == opc_View)
      {
         C_SdNdeDpListTableView * const pc_Tmp = this->mc_ElementViews[u32_ItView];
         if (pc_Tmp != NULL)
         {
            if (pc_Tmp->Equals(oru32_NodeIndex, oru32_DataPoolIndex, oru32_ListIndex))
            {
               this->mc_ElementViews[u32_ItView] = NULL;
            }
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward signal

   \param[in]  oru32_NodeIndex      Node index
   \param[in]  oru32_DataPoolIndex  Data pool index
   \param[in]  oru32_ListIndex      List index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListModelViewManager::RegisterDataSetCountChange(const uint32 & oru32_NodeIndex,
                                                               const uint32 & oru32_DataPoolIndex,
                                                               const uint32 & oru32_ListIndex)
{
   Q_EMIT this->SigDataSetCountChange(oru32_NodeIndex, oru32_DataPoolIndex, oru32_ListIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register model change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListModelViewManager::UpdateModels(void)
{
   for (uint32 u32_ItModel = 0; u32_ItModel < this->mc_DataSetModels.size(); ++u32_ItModel)
   {
      C_SdNdeDpListDataSetModel * const pc_Model = this->mc_DataSetModels[u32_ItModel];
      pc_Model->Reset();
   }
   for (uint32 u32_ItModel = 0; u32_ItModel < this->mc_ArrayEditModels.size(); ++u32_ItModel)
   {
      C_SdNdeDpListArrayEditModel * const pc_Model = this->mc_ArrayEditModels[u32_ItModel];
      pc_Model->Reset();
   }
   for (uint32 u32_ItModel = 0; u32_ItModel < this->mc_ElementModels.size(); ++u32_ItModel)
   {
      C_SdNdeDpListTableModel * const pc_Model = this->mc_ElementModels[u32_ItModel];
      pc_Model->Reset();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward signal

   \param[in]  oru32_NodeIndex            Node index
   \param[in]  oru32_DataPoolIndex        Data pool index
   \param[in]  oru32_DataPoolListIndex    List index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListModelViewManager::m_OnDataSetErrorChangePossible(const uint32 & oru32_NodeIndex,
                                                                   const uint32 & oru32_DataPoolIndex,
                                                                   const uint32 & oru32_DataPoolListIndex)
{
   Q_EMIT this->SigDataSetErrorChange(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do data change

   \param[in]  oru32_NodeIndex                  Node index
   \param[in]  oru32_DataPoolIndex              Data pool index
   \param[in]  oru32_DataPoolListIndex          List index
   \param[in]  oru32_DataPoolListDataSetIndex   Node data pool list data set index
   \param[in]  orc_NewData                      New data
   \param[in]  ore_DataChangeType               Data change type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListModelViewManager::m_OnDataSetDataChange(const uint32 & oru32_NodeIndex,
                                                          const uint32 & oru32_DataPoolIndex,
                                                          const uint32 & oru32_DataPoolListIndex,
                                                          const uint32 & oru32_DataPoolListDataSetIndex,
                                                          const QVariant & orc_NewData,
                                                          const C_SdNdeDpUtil::E_DataSetDataChangeType & ore_DataChangeType)
{
   C_SdNdeDpListDataSetView  * const pc_View = this->GetDataSetView(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                    oru32_DataPoolListIndex);

   if (pc_View != NULL)
   {
      pc_View->OnDataChange(oru32_DataPoolListDataSetIndex, orc_NewData, ore_DataChangeType);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle data set count change

   \param[in]  oru32_NodeIndex            Node index
   \param[in]  oru32_DataPoolIndex        Node data pool index
   \param[in]  oru32_DataPoolListIndex    Node data pool list index
   \param[in]  ors32_NewColumnCount       New column count
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListModelViewManager::m_OnDataSetColumnCountChange(const uint32 & oru32_NodeIndex,
                                                                 const uint32 & oru32_DataPoolIndex,
                                                                 const uint32 & oru32_DataPoolListIndex,
                                                                 const sint32 & ors32_NewColumnCount)
{
   C_SdNdeDpListDataSetView  * const pc_View = this->GetDataSetView(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                    oru32_DataPoolListIndex);

   if (pc_View != NULL)
   {
      pc_View->OnColumnCountChange(ors32_NewColumnCount);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward signal

   \param[in]  oru32_NodeIndex            Node index
   \param[in]  oru32_DataPoolIndex        Node data pool index
   \param[in]  oru32_DataPoolListIndex    Node data pool list index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListModelViewManager::m_OnElementErrorChangePossible(const uint32 & oru32_NodeIndex,
                                                                   const uint32 & oru32_DataPoolIndex,
                                                                   const uint32 & oru32_DataPoolListIndex)
{
   C_SdNdeDpListTableView  * const pc_View = this->GetElementView(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                  oru32_DataPoolListIndex);

   if (pc_View != NULL)
   {
      pc_View->OnErrorChangePossible();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward signal

   \param[in]  oru32_NodeIndex            Node index
   \param[in]  oru32_DataPoolIndex        Node data pool index
   \param[in]  oru32_DataPoolListIndex    Node data pool list index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListModelViewManager::m_OnElementSizeChangePossible(const uint32 & oru32_NodeIndex,
                                                                  const uint32 & oru32_DataPoolIndex,
                                                                  const uint32 & oru32_DataPoolListIndex)
{
   C_SdNdeDpListTableView  * const pc_View = this->GetElementView(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                  oru32_DataPoolListIndex);

   if (pc_View != NULL)
   {
      pc_View->OnSizeChangePossible();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do data change

   \param[in]  oru32_NodeIndex                  Node index
   \param[in]  oru32_DataPoolIndex              Node data pool index
   \param[in]  oru32_DataPoolListIndex          Node data pool list index
   \param[in]  oru32_DataPoolListElementIndex   Node data pool list element index
   \param[in]  orc_NewData                      New data
   \param[in]  ore_DataChangeType               Data change type
   \param[in]  oru32_ArrayIndex                 Optional array index
   \param[in]  ors32_DataSetIndex               Optional data set index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListModelViewManager::m_OnElementDataChange(const uint32 & oru32_NodeIndex,
                                                          const uint32 & oru32_DataPoolIndex,
                                                          const uint32 & oru32_DataPoolListIndex,
                                                          const uint32 & oru32_DataPoolListElementIndex,
                                                          const QVariant & orc_NewData,
                                                          const C_SdNdeDpUtil::E_ElementDataChangeType & ore_DataChangeType, const uint32 & oru32_ArrayIndex,
                                                          const sint32 & ors32_DataSetIndex)
{
   C_SdNdeDpListTableView  * const pc_View = this->GetElementView(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                  oru32_DataPoolListIndex);

   if (pc_View != NULL)
   {
      pc_View->OnDataChangeElements(oru32_DataPoolListElementIndex, orc_NewData, ore_DataChangeType, oru32_ArrayIndex,
                                    ors32_DataSetIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward signal

   \param[in]  oru32_NodeIndex      Node index
   \param[in]  oru32_DataPoolIndex  Node data pool index
   \param[in]  oru32_ListIndex      Node data pool list index
   \param[in]  oru32_ElementIndex   Node data pool list element index
   \param[in]  ore_ArrayEditType    Enum for node data pool list element variable
   \param[in]  oru32_DataSetIndex   If min or max use 0
                                    Else use data set index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListModelViewManager::m_OnArrayEditErrorChangePossible(const uint32 & oru32_NodeIndex,
                                                                     const uint32 & oru32_DataPoolIndex,
                                                                     const uint32 & oru32_ListIndex,
                                                                     const uint32 & oru32_ElementIndex,
                                                                     const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                                                                     const uint32 & oru32_DataSetIndex)
{
   C_SdNdeDpListArrayEditView  * const pc_View = this->GetArrayEditView(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                        oru32_ListIndex, oru32_ElementIndex,
                                                                        ore_ArrayEditType, oru32_DataSetIndex);

   if (pc_View != NULL)
   {
      pc_View->OnErrorChangePossible();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do data change

   \param[in]  oru32_NodeIndex            Node index
   \param[in]  oru32_DataPoolIndex        Node data pool index
   \param[in]  oru32_ListIndex            Node data pool list index
   \param[in]  oru32_ElementIndex         Node data pool list element index
   \param[in]  ore_ArrayEditType          Enum for node data pool list element variable
   \param[in]  oru32_DataSetIndex         If min or max use 0
                                          Else use data set index
   \param[in]  oru32_ArrayElementIndex    Array index
   \param[in]  orc_NewData                New data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListModelViewManager::m_OnArrayEditDataChange(const uint32 & oru32_NodeIndex,
                                                            const uint32 & oru32_DataPoolIndex,
                                                            const uint32 & oru32_ListIndex,
                                                            const uint32 & oru32_ElementIndex,
                                                            const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                                                            const uint32 & oru32_DataSetIndex,
                                                            const uint32 & oru32_ArrayElementIndex,
                                                            const QVariant & orc_NewData)
{
   C_SdNdeDpListArrayEditView  * const pc_View = this->GetArrayEditView(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                        oru32_ListIndex, oru32_ElementIndex,
                                                                        ore_ArrayEditType, oru32_DataSetIndex);

   if (pc_View != NULL)
   {
      pc_View->OnDataChange(oru32_ArrayElementIndex, orc_NewData);
   }
}
