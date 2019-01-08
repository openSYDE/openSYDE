//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Manager for model and view instances to avoid invalid pointers (implementation)

   Manager for model and view instances to avoid invalid pointers

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     17.03.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SdNdeDataPoolListModelViewManager.h"
#include "C_SdNdeDataPoolListDataSetView.h"
#include "C_SdNdeDataPoolListArrayEditView.h"
#include "C_SdNdeDataPoolListTableView.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in,out] opc_Parent Optional pointer to parent

   \created     17.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeDataPoolListModelViewManager::C_SdNdeDataPoolListModelViewManager(QObject * const opc_Parent) :
   QObject(opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     17.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeDataPoolListModelViewManager::~C_SdNdeDataPoolListModelViewManager(void)
{
   Clear();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Clear known models

   \created     23.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListModelViewManager::Clear(void)
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get data set view

   \param[in] oru32_NodeIndex     Node index
   \param[in] oru32_DataPoolIndex Node data pool index
   \param[in] oru32_ListIndex     Node data pool list index

   \return
   NULL Not registered
   Else Registered view

   \created     17.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeDataPoolListDataSetView * C_SdNdeDataPoolListModelViewManager::GetDataSetView(const uint32 & oru32_NodeIndex,
                                                                                     const uint32 & oru32_DataPoolIndex,
                                                                                     const uint32 & oru32_ListIndex)
{
   C_SdNdeDataPoolListDataSetView * pc_Retval = NULL;

   for (uint32 u32_ItView = 0; u32_ItView < this->mc_DataSetViews.size(); ++u32_ItView)
   {
      C_SdNdeDataPoolListDataSetView * const pc_Tmp = this->mc_DataSetViews[u32_ItView];
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get data set model

   \param[in] oru32_NodeIndex     Node index
   \param[in] oru32_DataPoolIndex Node data pool index
   \param[in] oru32_ListIndex     Node data pool list index

   \return
   Data set model (garanteed)

   \created     17.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeDataPoolListDataSetModel * C_SdNdeDataPoolListModelViewManager::GetDataSetModel(const uint32 & oru32_NodeIndex,
                                                                                       const uint32 & oru32_DataPoolIndex,
                                                                                       const uint32 & oru32_ListIndex)
{
   C_SdNdeDataPoolListDataSetModel * pc_Retval = NULL;

   for (uint32 u32_ItView = 0; u32_ItView < this->mc_DataSetModels.size(); ++u32_ItView)
   {
      C_SdNdeDataPoolListDataSetModel * const pc_Tmp = this->mc_DataSetModels[u32_ItView];
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
      this->mc_DataSetModels.push_back(new C_SdNdeDataPoolListDataSetModel());
      pc_Retval = this->mc_DataSetModels[this->mc_DataSetModels.size() - 1];
      //Init
      pc_Retval->SetList(oru32_NodeIndex, oru32_DataPoolIndex, oru32_ListIndex);
      //Connects
      connect(pc_Retval, &C_SdNdeDataPoolListDataSetModel::SigErrorChangePossible, this,
              &C_SdNdeDataPoolListModelViewManager::m_OnDataSetErrorChangePossible);
      connect(pc_Retval, &C_SdNdeDataPoolListDataSetModel::SigDataChange, this,
              &C_SdNdeDataPoolListModelViewManager::m_OnDataSetDataChange);
      connect(pc_Retval, &C_SdNdeDataPoolListDataSetModel::SigColumnCountChange, this,
              &C_SdNdeDataPoolListModelViewManager::m_OnDataSetColumnCountChange);
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get array edit view

   \param[in] oru32_NodeIndex     Node index
   \param[in] oru32_DataPoolIndex Node data pool index
   \param[in] oru32_ListIndex     Node data pool list index
   \param[in] oru32_ElementIndex  Node data pool list element index
   \param[in] ore_ArrayEditType   Enum for node data pool list element variable
   \param[in] oru32_DataSetIndex  If min or max use 0
                                  Else use data set index

   \return
   NULL Not registered
   Else Registered view

   \created     17.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeDataPoolListArrayEditView * C_SdNdeDataPoolListModelViewManager::GetArrayEditView(const uint32 & oru32_NodeIndex,
                                                                                         const uint32 & oru32_DataPoolIndex, const uint32 & oru32_ListIndex, const uint32 & oru32_ElementIndex, const C_SdNdeDataPoolUtil::E_ArrayEditType & ore_ArrayEditType,
                                                                                         const uint32 & oru32_DataSetIndex)
{
   C_SdNdeDataPoolListArrayEditView * pc_Retval = NULL;

   for (uint32 u32_ItView = 0; u32_ItView < this->mc_ArrayEditViews.size(); ++u32_ItView)
   {
      C_SdNdeDataPoolListArrayEditView * const pc_Tmp = this->mc_ArrayEditViews[u32_ItView];
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get array edit model

   \param[in] oru32_NodeIndex     Node index
   \param[in] oru32_DataPoolIndex Node data pool index
   \param[in] oru32_ListIndex     Node data pool list index
   \param[in] oru32_ElementIndex  Node data pool list element index
   \param[in] ore_ArrayEditType   Enum for node data pool list element variable
   \param[in] oru32_DataSetIndex  If min or max use 0
                                  Else use data set index

   \return
   Array edit model (garanteed)

   \created     17.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeDataPoolListArrayEditModel * C_SdNdeDataPoolListModelViewManager::GetArrayEditModel(
   const uint32 & oru32_NodeIndex, const uint32 & oru32_DataPoolIndex, const uint32 & oru32_ListIndex,
   const uint32 & oru32_ElementIndex, const C_SdNdeDataPoolUtil::E_ArrayEditType & ore_ArrayEditType,
   const uint32 & oru32_DataSetIndex)
{
   C_SdNdeDataPoolListArrayEditModel * pc_Retval = NULL;

   for (uint32 u32_ItView = 0; u32_ItView < this->mc_ArrayEditModels.size(); ++u32_ItView)
   {
      C_SdNdeDataPoolListArrayEditModel * const pc_Tmp = this->mc_ArrayEditModels[u32_ItView];
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
      this->mc_ArrayEditModels.push_back(new C_SdNdeDataPoolListArrayEditModel());
      pc_Retval = this->mc_ArrayEditModels[this->mc_ArrayEditModels.size() - 1];
      //Init
      pc_Retval->SetElement(oru32_NodeIndex, oru32_DataPoolIndex, oru32_ListIndex, oru32_ElementIndex,
                            ore_ArrayEditType, oru32_DataSetIndex);
      //Connects
      connect(pc_Retval, &C_SdNdeDataPoolListArrayEditModel::SigErrorChangePossible, this,
              &C_SdNdeDataPoolListModelViewManager::m_OnArrayEditErrorChangePossible);
      //lint -e{64,918,1025,1703} Qt interface
      connect(pc_Retval, &C_SdNdeDataPoolListArrayEditModel::SigDataChange, this,
              &C_SdNdeDataPoolListModelViewManager::m_OnArrayEditDataChange);
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get element view

   \param[in] oru32_NodeIndex     Node index
   \param[in] oru32_DataPoolIndex Node data pool index
   \param[in] oru32_ListIndex     Node data pool list index

   \return
   NULL Not registered
   Else Registered view

   \created     17.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeDataPoolListTableView * C_SdNdeDataPoolListModelViewManager::GetElementView(const uint32 & oru32_NodeIndex,
                                                                                   const uint32 & oru32_DataPoolIndex,
                                                                                   const uint32 & oru32_ListIndex)
{
   C_SdNdeDataPoolListTableView * pc_Retval = NULL;

   for (uint32 u32_ItView = 0; u32_ItView < this->mc_ElementViews.size(); ++u32_ItView)
   {
      C_SdNdeDataPoolListTableView * const pc_Tmp = this->mc_ElementViews[u32_ItView];
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

//-----------------------------------------------------------------------------
/*!
   \brief   Get element model

   \param[in] oru32_NodeIndex     Node index
   \param[in] oru32_DataPoolIndex Node data pool index
   \param[in] oru32_ListIndex     Node data pool list index

   \return
   Element model (garanteed)

   \created     17.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeDataPoolListTableModel * C_SdNdeDataPoolListModelViewManager::GetElementModel(const uint32 & oru32_NodeIndex,
                                                                                     const uint32 & oru32_DataPoolIndex,
                                                                                     const uint32 & oru32_ListIndex)
{
   C_SdNdeDataPoolListTableModel * pc_Retval = NULL;

   for (uint32 u32_ItView = 0; u32_ItView < this->mc_ElementModels.size(); ++u32_ItView)
   {
      C_SdNdeDataPoolListTableModel * const pc_Tmp = this->mc_ElementModels[u32_ItView];
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
      this->mc_ElementModels.push_back(new C_SdNdeDataPoolListTableModel());
      pc_Retval = this->mc_ElementModels[this->mc_ElementModels.size() - 1];
      //Init
      pc_Retval->SetList(oru32_NodeIndex, oru32_DataPoolIndex, oru32_ListIndex);
      //Connects
      connect(pc_Retval, &C_SdNdeDataPoolListTableModel::SigErrorChangePossible, this,
              &C_SdNdeDataPoolListModelViewManager::m_OnElementErrorChangePossible);
      connect(pc_Retval, &C_SdNdeDataPoolListTableModel::SigSizeChangePossible, this,
              &C_SdNdeDataPoolListModelViewManager::m_OnElementSizeChangePossible);
      //lint -e{64,918,1025,1703} Qt interface
      connect(pc_Retval, &C_SdNdeDataPoolListTableModel::SigDataChange, this,
              &C_SdNdeDataPoolListModelViewManager::m_OnElementDataChange);
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Register data set view

   \param[in]     oru32_NodeIndex     Node index
   \param[in]     oru32_DataPoolIndex Node data pool index
   \param[in]     oru32_ListIndex     Node data pool list index
   \param[in,out] opc_View            Data set view

   \created     17.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListModelViewManager::RegisterDataSetView(const uint32 & oru32_NodeIndex,
                                                              const uint32 & oru32_DataPoolIndex,
                                                              const uint32 & oru32_ListIndex,
                                                              C_SdNdeDataPoolListDataSetView * const opc_View)
{
   C_SdNdeDataPoolListDataSetView * pc_Found = NULL;

   for (uint32 u32_ItView = 0; u32_ItView < this->mc_DataSetViews.size(); ++u32_ItView)
   {
      C_SdNdeDataPoolListDataSetView * const pc_Tmp = this->mc_DataSetViews[u32_ItView];
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

//-----------------------------------------------------------------------------
/*!
   \brief   Unregister data set view

   \param[in]     oru32_NodeIndex     Node index
   \param[in]     oru32_DataPoolIndex Node data pool index
   \param[in]     oru32_ListIndex     Node data pool list index
   \param[in,out] opc_View            Data set view

   \created     17.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListModelViewManager::UnRegisterDataSetView(const uint32 & oru32_NodeIndex,
                                                                const uint32 & oru32_DataPoolIndex,
                                                                const uint32 & oru32_ListIndex,
                                                                const C_SdNdeDataPoolListDataSetView * const opc_View)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_DataSetViews.size(); ++u32_ItView)
   {
      if (this->mc_DataSetViews[u32_ItView] == opc_View)
      {
         C_SdNdeDataPoolListDataSetView * const pc_Tmp = this->mc_DataSetViews[u32_ItView];
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

//-----------------------------------------------------------------------------
/*!
   \brief   Register array edit view

   \param[in]     oru32_NodeIndex     Node index
   \param[in]     oru32_DataPoolIndex Node data pool index
   \param[in]     oru32_ListIndex     Node data pool list index
   \param[in]     oru32_ElementIndex  Node data pool list element index
   \param[in]     ore_ArrayEditType   Enum for node data pool list element variable
   \param[in]     oru32_DataSetIndex  If min or max use 0
                                      Else use data set index
   \param[in,out] opc_View            Array edit view

   \created     17.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListModelViewManager::RegisterArrayEditView(const uint32 & oru32_NodeIndex,
                                                                const uint32 & oru32_DataPoolIndex,
                                                                const uint32 & oru32_ListIndex,
                                                                const uint32 & oru32_ElementIndex,
                                                                const C_SdNdeDataPoolUtil::E_ArrayEditType & ore_ArrayEditType, const uint32 & oru32_DataSetIndex,
                                                                C_SdNdeDataPoolListArrayEditView * const opc_View)
{
   C_SdNdeDataPoolListArrayEditView * pc_Found = NULL;

   for (uint32 u32_ItView = 0; u32_ItView < this->mc_ArrayEditViews.size(); ++u32_ItView)
   {
      C_SdNdeDataPoolListArrayEditView * const pc_Tmp = this->mc_ArrayEditViews[u32_ItView];
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

//-----------------------------------------------------------------------------
/*!
   \brief   Unregister array edit view

   \param[in]     oru32_NodeIndex     Node index
   \param[in]     oru32_DataPoolIndex Node data pool index
   \param[in]     oru32_ListIndex     Node data pool list index
   \param[in]     oru32_ElementIndex  Node data pool list element index
   \param[in]     ore_ArrayEditType   Enum for node data pool list element variable
   \param[in]     oru32_DataSetIndex  If min or max use 0
                                      Else use data set index
   \param[in,out] opc_View            Array edit view

   \created     17.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListModelViewManager::UnRegisterArrayEditView(const uint32 & oru32_NodeIndex,
                                                                  const uint32 & oru32_DataPoolIndex,
                                                                  const uint32 & oru32_ListIndex,
                                                                  const uint32 & oru32_ElementIndex,
                                                                  const C_SdNdeDataPoolUtil::E_ArrayEditType & ore_ArrayEditType, const uint32 & oru32_DataSetIndex,
                                                                  const C_SdNdeDataPoolListArrayEditView * const opc_View)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_ArrayEditViews.size(); ++u32_ItView)
   {
      if (this->mc_ArrayEditViews[u32_ItView] == opc_View)
      {
         C_SdNdeDataPoolListArrayEditView * const pc_Tmp = this->mc_ArrayEditViews[u32_ItView];
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

//-----------------------------------------------------------------------------
/*!
   \brief   Register element view

   \param[in]     oru32_NodeIndex     Node index
   \param[in]     oru32_DataPoolIndex Node data pool index
   \param[in]     oru32_ListIndex     Node data pool list index
   \param[in,out] opc_View            Element view

   \created     17.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListModelViewManager::RegisterElementView(const uint32 & oru32_NodeIndex,
                                                              const uint32 & oru32_DataPoolIndex,
                                                              const uint32 & oru32_ListIndex,
                                                              C_SdNdeDataPoolListTableView * const opc_View)
{
   C_SdNdeDataPoolListTableView  * pc_Found = NULL;

   for (uint32 u32_ItView = 0; u32_ItView < this->mc_ElementViews.size(); ++u32_ItView)
   {
      C_SdNdeDataPoolListTableView * const pc_Tmp = this->mc_ElementViews[u32_ItView];
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

//-----------------------------------------------------------------------------
/*!
   \brief   Unregister element view

   \param[in]     oru32_NodeIndex     Node index
   \param[in]     oru32_DataPoolIndex Node data pool index
   \param[in]     oru32_ListIndex     Node data pool list index
   \param[in,out] opc_View            Element view

   \created     17.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListModelViewManager::UnRegisterElementView(const uint32 & oru32_NodeIndex,
                                                                const uint32 & oru32_DataPoolIndex,
                                                                const uint32 & oru32_ListIndex,
                                                                const C_SdNdeDataPoolListTableView * const opc_View)
{
   for (uint32 u32_ItView = 0; u32_ItView < this->mc_ElementViews.size(); ++u32_ItView)
   {
      if (this->mc_ElementViews[u32_ItView] == opc_View)
      {
         C_SdNdeDataPoolListTableView * const pc_Tmp = this->mc_ElementViews[u32_ItView];
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

//-----------------------------------------------------------------------------
/*!
   \brief   Forward signal

   \param[in] oru32_NodeIndex         Node index
   \param[in] oru32_DataPoolIndex     Data pool index
   \param[in] oru32_DataPoolListIndex List index

   \created     20.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListModelViewManager::RegisterDataSetCountChange(const uint32 & oru32_NodeIndex,
                                                                     const uint32 & oru32_DataPoolIndex,
                                                                     const uint32 & oru32_ListIndex)
{
   Q_EMIT this->SigDataSetCountChange(oru32_NodeIndex, oru32_DataPoolIndex, oru32_ListIndex);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Register model change

   \created     18.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListModelViewManager::UpdateModels(void)
{
   for (uint32 u32_ItModel = 0; u32_ItModel < this->mc_DataSetModels.size(); ++u32_ItModel)
   {
      C_SdNdeDataPoolListDataSetModel * const pc_Model = this->mc_DataSetModels[u32_ItModel];
      pc_Model->Reset();
   }
   for (uint32 u32_ItModel = 0; u32_ItModel < this->mc_ArrayEditModels.size(); ++u32_ItModel)
   {
      C_SdNdeDataPoolListArrayEditModel * const pc_Model = this->mc_ArrayEditModels[u32_ItModel];
      pc_Model->Reset();
   }
   for (uint32 u32_ItModel = 0; u32_ItModel < this->mc_ElementModels.size(); ++u32_ItModel)
   {
      C_SdNdeDataPoolListTableModel * const pc_Model = this->mc_ElementModels[u32_ItModel];
      pc_Model->Reset();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Forward signal

   \param[in] oru32_NodeIndex         Node index
   \param[in] oru32_DataPoolIndex     Data pool index
   \param[in] oru32_DataPoolListIndex List index

   \created     27.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListModelViewManager::m_OnDataSetErrorChangePossible(const uint32 & oru32_NodeIndex,
                                                                         const uint32 & oru32_DataPoolIndex,
                                                                         const uint32 & oru32_DataPoolListIndex)
{
   Q_EMIT this->SigDataSetErrorChange(oru32_NodeIndex, oru32_DataPoolIndex, oru32_DataPoolListIndex);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Do data change

   \param[in]     oru32_DataPoolListDataSetIndex Node data pool list data set index
   \param[in]     orc_NewData                    New data
   \param[in]     ore_DataChangeType             Data change type

   \created     17.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListModelViewManager::m_OnDataSetDataChange(const uint32 & oru32_NodeIndex,
                                                                const uint32 & oru32_DataPoolIndex,
                                                                const uint32 & oru32_DataPoolListIndex,
                                                                const uint32 & oru32_DataPoolListDataSetIndex,
                                                                const QVariant & orc_NewData,
                                                                const C_SdNdeDataPoolUtil::E_DataSetDataChangeType & ore_DataChangeType)
{
   C_SdNdeDataPoolListDataSetView  * const pc_View = this->GetDataSetView(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                          oru32_DataPoolListIndex);

   if (pc_View != NULL)
   {
      pc_View->OnDataChange(oru32_DataPoolListDataSetIndex, orc_NewData, ore_DataChangeType);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle data set count change

   \param[in] oru32_NodeIndex         Node index
   \param[in] oru32_DataPoolIndex     Node data pool index
   \param[in] oru32_DataPoolListIndex Node data pool list index
   \param[in] ors32_NewColumnCount    New column count

   \created     20.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListModelViewManager::m_OnDataSetColumnCountChange(const uint32 & oru32_NodeIndex,
                                                                       const uint32 & oru32_DataPoolIndex,
                                                                       const uint32 & oru32_DataPoolListIndex,
                                                                       const sint32 & ors32_NewColumnCount)
{
   C_SdNdeDataPoolListDataSetView  * const pc_View = this->GetDataSetView(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                          oru32_DataPoolListIndex);

   if (pc_View != NULL)
   {
      pc_View->OnColumnCountChange(ors32_NewColumnCount);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Forward signal

   \param[in] oru32_NodeIndex         Node index
   \param[in] oru32_DataPoolIndex     Node data pool index
   \param[in] oru32_DataPoolListIndex Node data pool list index

   \created     27.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListModelViewManager::m_OnElementErrorChangePossible(const uint32 & oru32_NodeIndex,
                                                                         const uint32 & oru32_DataPoolIndex,
                                                                         const uint32 & oru32_DataPoolListIndex)
{
   C_SdNdeDataPoolListTableView  * const pc_View = this->GetElementView(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                        oru32_DataPoolListIndex);

   if (pc_View != NULL)
   {
      pc_View->OnErrorChangePossible();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Forward signal

   \param[in] oru32_NodeIndex         Node index
   \param[in] oru32_DataPoolIndex     Node data pool index
   \param[in] oru32_DataPoolListIndex Node data pool list index

   \created     24.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListModelViewManager::m_OnElementSizeChangePossible(const uint32 & oru32_NodeIndex,
                                                                        const uint32 & oru32_DataPoolIndex,
                                                                        const uint32 & oru32_DataPoolListIndex)
{
   C_SdNdeDataPoolListTableView  * const pc_View = this->GetElementView(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                        oru32_DataPoolListIndex);

   if (pc_View != NULL)
   {
      pc_View->OnSizeChangePossible();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Do data change

   \param[in]     oru32_NodeIndex                Node index
   \param[in]     oru32_DataPoolIndex            Node data pool index
   \param[in]     oru32_DataPoolListIndex        Node data pool list index
   \param[in]     oru32_DataPoolListElementIndex Node data pool list element index
   \param[in]     orc_NewData                    New data
   \param[in]     ore_DataChangeType             Data change type
   \param[in]     oru32_ArrayIndex               Optional array index
   \param[in]     ors32_DataSetIndex             Optional data set index

   \created     17.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListModelViewManager::m_OnElementDataChange(const uint32 & oru32_NodeIndex,
                                                                const uint32 & oru32_DataPoolIndex,
                                                                const uint32 & oru32_DataPoolListIndex,
                                                                const uint32 & oru32_DataPoolListElementIndex,
                                                                const QVariant & orc_NewData,
                                                                const C_SdNdeDataPoolUtil::E_ElementDataChangeType & ore_DataChangeType, const uint32 & oru32_ArrayIndex,
                                                                const sint32 & ors32_DataSetIndex)
{
   C_SdNdeDataPoolListTableView  * const pc_View = this->GetElementView(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                        oru32_DataPoolListIndex);

   if (pc_View != NULL)
   {
      pc_View->OnDataChangeElements(oru32_DataPoolListElementIndex, orc_NewData, ore_DataChangeType, oru32_ArrayIndex,
                                    ors32_DataSetIndex);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Forward signal

   \param[in]     oru32_NodeIndex     Node index
   \param[in]     oru32_DataPoolIndex Node data pool index
   \param[in]     oru32_ListIndex     Node data pool list index
   \param[in]     oru32_ElementIndex  Node data pool list element index
   \param[in]     ore_ArrayEditType   Enum for node data pool list element variable
   \param[in]     oru32_DataSetIndex  If min or max use 0
                                      Else use data set index

   \created     21.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListModelViewManager::m_OnArrayEditErrorChangePossible(const uint32 & oru32_NodeIndex,
                                                                           const uint32 & oru32_DataPoolIndex,
                                                                           const uint32 & oru32_ListIndex,
                                                                           const uint32 & oru32_ElementIndex,
                                                                           const C_SdNdeDataPoolUtil::E_ArrayEditType & ore_ArrayEditType,
                                                                           const uint32 & oru32_DataSetIndex)
{
   C_SdNdeDataPoolListArrayEditView  * const pc_View = this->GetArrayEditView(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                              oru32_ListIndex, oru32_ElementIndex,
                                                                              ore_ArrayEditType, oru32_DataSetIndex);

   if (pc_View != NULL)
   {
      pc_View->OnErrorChangePossible();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Do data change

   \param[in]     oru32_NodeIndex         Node index
   \param[in]     oru32_DataPoolIndex     Node data pool index
   \param[in]     oru32_ListIndex         Node data pool list index
   \param[in]     oru32_ElementIndex      Node data pool list element index
   \param[in]     ore_ArrayEditType       Enum for node data pool list element variable
   \param[in]     oru32_DataSetIndex      If min or max use 0
                                          Else use data set index
   \param[in]     oru32_ArrayElementIndex Array index
   \param[in]     orc_NewData             New data

   \created     21.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListModelViewManager::m_OnArrayEditDataChange(const uint32 & oru32_NodeIndex,
                                                                  const uint32 & oru32_DataPoolIndex,
                                                                  const uint32 & oru32_ListIndex,
                                                                  const uint32 & oru32_ElementIndex,
                                                                  const C_SdNdeDataPoolUtil::E_ArrayEditType & ore_ArrayEditType, const uint32 & oru32_DataSetIndex, const uint32 & oru32_ArrayElementIndex,
                                                                  const QVariant & orc_NewData)
{
   C_SdNdeDataPoolListArrayEditView  * const pc_View = this->GetArrayEditView(oru32_NodeIndex, oru32_DataPoolIndex,
                                                                              oru32_ListIndex, oru32_ElementIndex,
                                                                              ore_ArrayEditType, oru32_DataSetIndex);

   if (pc_View != NULL)
   {
      pc_View->OnDataChange(oru32_ArrayElementIndex, orc_NewData);
   }
}
